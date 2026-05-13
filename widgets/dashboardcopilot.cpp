#include "dashboardcopilot.h"
#include "../aiclient.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextBrowser>
#include <QTextEdit>
#include <QPushButton>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QScrollBar>
#include <QKeyEvent>

static constexpr const char *COPILOT_SYSTEM_PROMPT = R"(你是一个 Qt 串口仪表盘 UI 助手。用户会用自然语言描述需要的控件，你需要严格输出对应的 JSON 配置，不附带任何解释文本。

【重要规则】关于按钮的 message 和 hexMode 字段：
- hexMode: false 表示 ASCII 文本模式。message 字段直接写用户想要发送的原始文本字符。
  例如：用户说"发 s" → {"message":"s","hexMode":false}
  例如：用户说"发 dsfa" → {"message":"dsfa","hexMode":false}
  例如：用户说"发数字1"或"发字符1" → {"message":"1","hexMode":false}
  例如：用户说"发 HELLO" → {"message":"HELLO","hexMode":false}
- hexMode: true 表示十六进制模式。message 字段写报文的十六进制字符串（不含空格和0x前缀）。
  例如：用户说"发十六进制 01" → {"message":"01","hexMode":true}
  例如：用户说"发HEX FF00" → {"message":"FF00","hexMode":true}
- 除非用户明确说"十六进制""hex""HEX"，否则默认使用 hexMode:false（ASCII文本模式）。

支持的控件类型和 JSON Schema：

1. 数据解析框 (display) — 从串口报文中提取并显示解析值：
{"type":"display","label":"标签","formatStr":"Torque:%d Nm","style":0}
- formatStr: 解析格式，%d=整数, %f=浮点, %s=字符串
- style: 0=默认, 1=深色, 2=极简

2. 逻辑指示灯 (indicator) — 条件表达式控制颜色：
{"type":"indicator","name":"名称","expression":"PWM > 45","formatStr":"PWM:%d","trueColor":"#22c55e","falseColor":"#ef4444","style":0}
- expression: 支持 >, <, >=, <=, ==, !=, &&, ||
- style: 0=默认, 1=扁平, 2=仿真

3. 发送按钮 (button) — 点击向串口发送数据：
{"type":"button","name":"名称","message":"内容","hexMode":false,"style":0}
- style: 0=默认, 1=圆角, 2=线框, 3=方形, 4=胶囊, 5=文字, 6=危险

可以返回单个 JSON 对象或 JSON 数组。只输出 JSON，不要输出其他文字。)";

// Intercept Enter key in the input QTextEdit to send, Shift+Enter for newline
class ChatInputTextEdit : public QTextEdit
{
public:
    using QTextEdit::QTextEdit;
    std::function<void()> onSend;

protected:
    void keyPressEvent(QKeyEvent *ke) override
    {
        if ((ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter)
            && !(ke->modifiers() & Qt::ShiftModifier)) {
            if (onSend) onSend();
            return;
        }
        QTextEdit::keyPressEvent(ke);
    }
};

DashboardCopilot::DashboardCopilot(AiClient *aiClient, QWidget *parent)
    : QWidget(parent)
    , m_aiClient(aiClient)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Chat history browser — no inline colors, let the global theme handle text/bg
    m_chatBrowser = new QTextBrowser(this);
    m_chatBrowser->setOpenExternalLinks(false);
    layout->addWidget(m_chatBrowser, 1);

    // Input area — styled via objectName so the global theme can reach it
    auto *inputArea = new QWidget(this);
    inputArea->setObjectName("copilotInputArea");
    auto *inputLayout = new QHBoxLayout(inputArea);
    inputLayout->setContentsMargins(10, 8, 10, 8);
    inputLayout->setSpacing(8);

    m_inputEdit = new ChatInputTextEdit(inputArea);
    m_inputEdit->setPlaceholderText("描述你想要的控件... (Enter 发送, Shift+Enter 换行)");
    m_inputEdit->setAcceptRichText(false);
    m_inputEdit->setMaximumHeight(100);
    m_inputEdit->setMinimumHeight(72);
    inputLayout->addWidget(m_inputEdit, 1);

    m_sendBtn = new QPushButton("发送", inputArea);
    m_sendBtn->setObjectName("copilotSendBtn");
    m_sendBtn->setFixedWidth(64);
    m_sendBtn->setMinimumHeight(72);
    inputLayout->addWidget(m_sendBtn);

    layout->addWidget(inputArea);

    // Enter sends, Shift+Enter newline
    static_cast<ChatInputTextEdit *>(m_inputEdit)->onSend = [this]() { onSendClicked(); };
    connect(m_sendBtn, &QPushButton::clicked, this, &DashboardCopilot::onSendClicked);
    connect(m_aiClient, &AiClient::chatResultReady, this, &DashboardCopilot::onAiReply);
    connect(m_aiClient, &AiClient::errorOccurred, this, &DashboardCopilot::onAiError);

    // Show welcome hint
    showWelcomeHint();
}

void DashboardCopilot::showWelcomeHint()
{
    m_chatBrowser->setHtml(
        "<div style='text-align:center; padding:40px 20px; color:#9ca3af;'>"
        "<p style='font-size:15px; font-weight:600;'>仪表盘副驾</p>"
        "<p style='font-size:12px;'>用自然语言描述你需要的控件，AI 会自动在仪表盘上创建</p>"
        "<p style='font-size:12px; margin-top:12px;'>例如：</p>"
        "<p style='font-size:12px;'>\"帮我加一个解析框，格式是 Torque:%d Nm\"</p>"
        "<p style='font-size:12px;'>\"加一个红色的逻辑灯，条件是 PWM > 45\"</p>"
        "<p style='font-size:12px;'>\"加三个按钮，分别发 s、hello、dsfa\"</p>"
        "</div>");
}

void DashboardCopilot::onSendClicked()
{
    const QString text = m_inputEdit->toPlainText().trimmed();
    if (text.isEmpty())
        return;

    if (m_aiClient->apiKey().isEmpty()) {
        appendBubble("请先通过菜单 AI -> AI 设置 配置 API Key", false, true);
        return;
    }

    // Clear welcome hint on first message
    if (m_history.isEmpty())
        m_chatBrowser->clear();

    appendBubble(text, true);
    m_inputEdit->clear();

    QJsonObject userMsg;
    userMsg["role"] = "user";
    userMsg["content"] = text;
    m_history.append(userMsg);

    m_sendBtn->setEnabled(false);
    m_inputEdit->setEnabled(false);

    m_aiClient->chat(COPILOT_SYSTEM_PROMPT, m_history, "copilot");
}

void DashboardCopilot::onAiReply(const QString &result, const QString &requestId)
{
    if (requestId != "copilot")
        return;

    m_sendBtn->setEnabled(true);
    m_inputEdit->setEnabled(true);

    QJsonObject assistantMsg;
    assistantMsg["role"] = "assistant";
    assistantMsg["content"] = result;
    m_history.append(assistantMsg);

    // Try to parse as widget config JSON
    const QJsonObject singleObj = tryParseJson(result);
    if (!singleObj.isEmpty() && singleObj.contains("type")) {
        const QString json = QString::fromUtf8(QJsonDocument(singleObj).toJson(QJsonDocument::Compact));
        appendBubble("已生成控件配置：\n" + json, false);
        emit aiControlGenerated(singleObj);
        return;
    }

    const QJsonArray arr = tryParseJsonArray(result);
    if (!arr.isEmpty()) {
        QString summary = "已生成 " + QString::number(arr.size()) + " 个控件：\n";
        for (const auto &item : arr) {
            const QJsonObject obj = item.toObject();
            if (obj.contains("type")) {
                emit aiControlGenerated(obj);
                summary += QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)) + "\n";
            }
        }
        appendBubble(summary, false);
        return;
    }

    // Not JSON — display as plain AI reply
    appendBubble(result, false);
}

void DashboardCopilot::onAiError(const QString &errorMsg)
{
    m_sendBtn->setEnabled(true);
    m_inputEdit->setEnabled(true);
    appendBubble("错误: " + errorMsg, false, true);
}

void DashboardCopilot::appendBubble(const QString &text, bool isUser, bool isError)
{
    const QString escaped = text.toHtmlEscaped().replace("\n", "<br>");

    QString html;
    if (isUser) {
        html =
            "<table width='100%' style='margin:4px 0;'><tr>"
            "<td></td>"
            "<td style='background-color:#2563eb; color:#ffffff; border-radius:12px; "
            "padding:10px 14px; font-size:13px; line-height:1.6; "
            "max-width:600px; margin-left:auto;'>"
            "<div style='font-size:13px; color:#93c5fd; margin-bottom:3px; font-weight:600;'>用户： </div>"
            "<span style='color:#ffffff;'>" + escaped + "</span>"
            "</td></tr></table>";
    } else if (isError) {
        html =
            "<div style='margin:6px 60px 6px 0;'>"
            "<div style='display:inline-block; max-width:80%; "
            "background-color:#fef2f2; color:#991b1b; border:1px solid #fecaca; border-radius:12px; "
            "padding:10px 14px; font-size:13px; line-height:1.6;'>"
            "<div style='font-size:13px; color:#dc2626; margin-bottom:3px; font-weight:600;'>系统： </div>"
            + escaped +
            "</div></div>";
    } else {
        html =
            "<div style='margin:6px 60px 6px 0;'>"
            "<div style='display:inline-block; max-width:80%; "
            "background-color:#f3f4f6; color:#1f2937; border-radius:12px; "
            "padding:10px 14px; font-size:13px; line-height:1.6;'>"
            "<div style='font-size:11px; color:#6b7280; margin-bottom:3px; font-weight:600;'>AI 助手</div>"
            + escaped +
            "</div></div>";
    }

    m_chatBrowser->append(html);

    QScrollBar *sb = m_chatBrowser->verticalScrollBar();
    sb->setValue(sb->maximum());
}

QJsonObject DashboardCopilot::tryParseJson(const QString &text) const
{
    const QString cleaned = extractJsonFromMarkdown(text);
    const QJsonDocument doc = QJsonDocument::fromJson(cleaned.toUtf8());
    if (doc.isObject())
        return doc.object();
    return {};
}

QJsonArray DashboardCopilot::tryParseJsonArray(const QString &text) const
{
    const QString cleaned = extractJsonFromMarkdown(text);
    const QJsonDocument doc = QJsonDocument::fromJson(cleaned.toUtf8());
    if (doc.isArray())
        return doc.array();
    return {};
}

QString DashboardCopilot::extractJsonFromMarkdown(const QString &text) const
{
    QString s = text.trimmed();
    if (s.startsWith("```json"))
        s = s.mid(7);
    else if (s.startsWith("```"))
        s = s.mid(3);
    if (s.endsWith("```"))
        s.chop(3);
    return s.trimmed();
}

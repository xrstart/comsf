#ifndef DASHBOARDCOPILOT_H
#define DASHBOARDCOPILOT_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>

class QTextBrowser;
class QTextEdit;
class QPushButton;
class AiClient;

class DashboardCopilot : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardCopilot(AiClient *aiClient, QWidget *parent = nullptr);

signals:
    void aiControlGenerated(const QJsonObject &config);

private slots:
    void onSendClicked();
    void onAiReply(const QString &result, const QString &requestId);
    void onAiError(const QString &errorMsg);

private:
    void showWelcomeHint();
    void appendBubble(const QString &text, bool isUser, bool isError = false);
    QJsonObject tryParseJson(const QString &text) const;
    QJsonArray tryParseJsonArray(const QString &text) const;
    QString extractJsonFromMarkdown(const QString &text) const;

    AiClient *m_aiClient;
    QTextBrowser *m_chatBrowser;
    QTextEdit *m_inputEdit;
    QPushButton *m_sendBtn;
    QJsonArray m_history;
};

#endif // DASHBOARDCOPILOT_H

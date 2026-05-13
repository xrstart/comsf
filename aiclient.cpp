#include "aiclient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>

static constexpr const char *DEFAULT_BASE_URL = "https://api.deepseek.com";
static constexpr const char *DEFAULT_MODEL = "deepseek-chat";

AiClient::AiClient(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_baseUrl(DEFAULT_BASE_URL)
    , m_model(DEFAULT_MODEL)
    , m_busy(false)
{
}

void AiClient::setApiKey(const QString &key) { m_apiKey = key; }
void AiClient::setBaseUrl(const QString &url) { m_baseUrl = url; }
void AiClient::setModel(const QString &model) { m_model = model; }

QString AiClient::apiKey() const { return m_apiKey; }
QString AiClient::baseUrl() const { return m_baseUrl; }
QString AiClient::model() const { return m_model; }

bool AiClient::isBusy() const { return m_busy; }

void AiClient::analyze(const QString &data)
{
    if (m_apiKey.isEmpty()) {
        emit errorOccurred("请先配置 API Key");
        return;
    }
    if (m_busy) {
        emit errorOccurred("请求正在进行中，请稍候...");
        return;
    }

    const QString prompt =
        "你现在是一个资深嵌入式 MCU 工程师。"
        "请帮我分析以下这段截获的串口报文数据，检查格式、寻找异常或猜测数据规律：\n\n"
        + data;

    m_busy = true;
    emit busyChanged(true);

    QNetworkReply *reply = sendRequest(prompt);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        m_busy = false;
        emit busyChanged(false);

        if (reply->error() != QNetworkReply::NoError) {
            emit errorOccurred("网络错误: " + reply->errorString());
            return;
        }

        const QByteArray raw = reply->readAll();
        const QJsonDocument doc = QJsonDocument::fromJson(raw);
        if (doc.isNull()) {
            emit errorOccurred("JSON 解析失败");
            return;
        }

        const QJsonObject root = doc.object();

        if (root.contains("error")) {
            const QJsonObject err = root.value("error").toObject();
            emit errorOccurred("API 错误: " + err.value("message").toString());
            return;
        }

        const QJsonArray choices = root.value("choices").toArray();
        if (choices.isEmpty()) {
            emit errorOccurred("API 返回为空");
            return;
        }

        const QJsonObject message = choices.first().toObject().value("message").toObject();
        const QString content = message.value("content").toString();
        emit resultReady(content);
    });
}

void AiClient::chat(const QString &systemPrompt, const QJsonArray &messages, const QString &requestId)
{
    if (m_apiKey.isEmpty()) {
        emit errorOccurred("请先配置 API Key");
        return;
    }
    if (m_busy) {
        emit errorOccurred("请求正在进行中，请稍候...");
        return;
    }

    m_busy = true;
    emit busyChanged(true);

    QNetworkReply *reply = sendChatRequest(systemPrompt, messages, 0.7);
    connect(reply, &QNetworkReply::finished, this, [this, reply, requestId]() {
        reply->deleteLater();
        m_busy = false;
        emit busyChanged(false);

        if (reply->error() != QNetworkReply::NoError) {
            emit errorOccurred("网络错误: " + reply->errorString());
            return;
        }

        const QByteArray raw = reply->readAll();
        const QJsonDocument doc = QJsonDocument::fromJson(raw);
        if (doc.isNull()) {
            emit errorOccurred("JSON 解析失败");
            return;
        }

        const QJsonObject root = doc.object();
        if (root.contains("error")) {
            const QJsonObject err = root.value("error").toObject();
            emit errorOccurred("API 错误: " + err.value("message").toString());
            return;
        }

        const QJsonArray choices = root.value("choices").toArray();
        if (choices.isEmpty()) {
            emit errorOccurred("API 返回为空");
            return;
        }

        const QJsonObject message = choices.first().toObject().value("message").toObject();
        const QString content = message.value("content").toString();
        emit chatResultReady(content, requestId);
    });
}

QNetworkReply *AiClient::sendRequest(const QString &userMessage)
{
    QJsonArray messages;
    QJsonObject userMsg;
    userMsg["role"] = "user";
    userMsg["content"] = userMessage;
    messages.append(userMsg);

    return sendChatRequest(
        "你是一个专业的嵌入式系统数据分析师，擅长解读串口通信协议和MCU报文数据。",
        messages, 0.3);
}

QNetworkReply *AiClient::sendChatRequest(const QString &systemPrompt, const QJsonArray &messages, double temperature)
{
    const QUrl url(m_baseUrl + "/v1/chat/completions");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + m_apiKey).toUtf8());

    QJsonObject body;
    body["model"] = m_model;
    body["temperature"] = temperature;

    QJsonArray allMessages;
    QJsonObject systemMsg;
    systemMsg["role"] = "system";
    systemMsg["content"] = systemPrompt;
    allMessages.append(systemMsg);

    for (const auto &msg : messages)
        allMessages.append(msg.toObject());

    body["messages"] = allMessages;

    return m_networkManager->post(request, QJsonDocument(body).toJson());
}

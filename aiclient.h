#ifndef AICLIENT_H
#define AICLIENT_H

#include <QObject>
#include <QString>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class AiClient : public QObject
{
    Q_OBJECT

public:
    explicit AiClient(QObject *parent = nullptr);

    void setApiKey(const QString &key);
    void setBaseUrl(const QString &url);
    void setModel(const QString &model);

    QString apiKey() const;
    QString baseUrl() const;
    QString model() const;

    void analyze(const QString &data);
    void chat(const QString &systemPrompt, const QJsonArray &messages, const QString &requestId = {});
    bool isBusy() const;

signals:
    void resultReady(const QString &result);
    void chatResultReady(const QString &result, const QString &requestId);
    void errorOccurred(const QString &errorMsg);
    void busyChanged(bool busy);

private:
    QNetworkReply *sendRequest(const QString &userMessage);
    QNetworkReply *sendChatRequest(const QString &systemPrompt, const QJsonArray &messages, double temperature);

    QNetworkAccessManager *m_networkManager;
    QString m_apiKey;
    QString m_baseUrl;
    QString m_model;
    bool m_busy;
};

#endif // AICLIENT_H

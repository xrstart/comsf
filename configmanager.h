#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QString>

class ConfigManager : public QObject
{
    Q_OBJECT

public:
    explicit ConfigManager(QObject *parent = nullptr);

    // Save all dashboard widgets config to JSON file
    bool saveToFile(const QString &filePath, const QJsonArray &widgets);

    // Load widgets config from JSON file
    QJsonArray loadFromFile(const QString &filePath);

    // Build JSON for each widget type
    static QJsonObject buttonToJson(const QString &name, const QString &message,
                                     bool hexMode, int row, int col);
    static QJsonObject displayToJson(const QString &label, const QString &formatStr,
                                      int row, int col);
    static QJsonObject indicatorToJson(const QString &name, const QString &expression,
                                        const QString &trueColor, const QString &falseColor,
                                        int row, int col);

signals:
    void configError(const QString &errorMsg);
};

#endif // CONFIGMANAGER_H

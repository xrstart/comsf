#include "configmanager.h"
#include <QFile>
#include <QJsonParseError>

ConfigManager::ConfigManager(QObject *parent)
    : QObject(parent)
{
}

bool ConfigManager::saveToFile(const QString &filePath, const QJsonArray &widgets)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        emit configError(QString("Cannot open file for writing: %1").arg(filePath));
        return false;
    }

    QJsonObject root;
    root["version"] = "1.0";
    root["widgets"] = widgets;

    QJsonDocument doc(root);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

QJsonArray ConfigManager::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit configError(QString("Cannot open file for reading: %1").arg(filePath));
        return QJsonArray();
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    file.close();

    if (error.error != QJsonParseError::NoError) {
        emit configError(QString("JSON parse error: %1").arg(error.errorString()));
        return QJsonArray();
    }

    QJsonObject root = doc.object();
    return root["widgets"].toArray();
}

QJsonObject ConfigManager::buttonToJson(const QString &name, const QString &message,
                                          bool hexMode, int row, int col)
{
    QJsonObject obj;
    obj["type"] = "button";
    obj["name"] = name;
    obj["message"] = message;
    obj["hexMode"] = hexMode;
    obj["row"] = row;
    obj["col"] = col;
    return obj;
}

QJsonObject ConfigManager::displayToJson(const QString &label, const QString &formatStr,
                                           int row, int col)
{
    QJsonObject obj;
    obj["type"] = "display";
    obj["label"] = label;
    obj["formatStr"] = formatStr;
    obj["row"] = row;
    obj["col"] = col;
    return obj;
}

QJsonObject ConfigManager::indicatorToJson(const QString &name, const QString &expression,
                                             const QString &trueColor, const QString &falseColor,
                                             int row, int col)
{
    QJsonObject obj;
    obj["type"] = "indicator";
    obj["name"] = name;
    obj["expression"] = expression;
    obj["trueColor"] = trueColor;
    obj["falseColor"] = falseColor;
    obj["row"] = row;
    obj["col"] = col;
    return obj;
}

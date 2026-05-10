#ifndef PARSER_H
#define PARSER_H

#include <QObject>
#include <QMap>
#include <QVariant>
#include <QRegularExpression>
#include <QString>

class Parser : public QObject
{
    Q_OBJECT

public:
    explicit Parser(QObject *parent = nullptr);

    // Parse data using a format string like "Torque:%d Nm"
    // Returns true if parsing succeeded, stores extracted values in variables
    bool parse(const QString &formatStr, const QString &rawData,
               QMap<QString, QVariant> &variables);

    // Build a regex from a format string
    // %d -> (?<name>\\-?\\d+), %f -> (?<name>\\-?\\d+\\.?\\d*), %s -> (?<name>\\S+), %c -> (?<name>.)
    QRegularExpression buildRegex(const QString &formatStr) const;

    // Get variable names from format string
    QStringList extractVariableNames(const QString &formatStr) const;

signals:
    void parseError(const QString &errorMsg);

private:
    QString m_lastError;
};

#endif // PARSER_H

#include "parser.h"

Parser::Parser(QObject *parent)
    : QObject(parent)
{
}

bool Parser::parse(const QString &formatStr, const QString &rawData,
                   QMap<QString, QVariant> &variables)
{
    if (formatStr.isEmpty() || rawData.isEmpty())
        return false;

    QRegularExpression regex = buildRegex(formatStr);
    if (!regex.isValid()) {
        emit parseError(QString("Invalid format: %1").arg(formatStr));
        return false;
    }

    QRegularExpressionMatch match = regex.match(rawData);
    if (!match.hasMatch())
        return false;

    QStringList names = extractVariableNames(formatStr);
    int varIndex = 1; // regex capture groups start at 1
    for (const QString &name : names) {
        QString captured = match.captured(varIndex);
        if (captured.isEmpty()) {
            varIndex++;
            continue;
        }

        // Try to convert to number
        bool ok;
        double numVal = captured.toDouble(&ok);
        if (ok)
            variables[name] = numVal;
        else
            variables[name] = captured;

        varIndex++;
    }
    return true;
}

QRegularExpression Parser::buildRegex(const QString &formatStr) const
{
    // Variable naming: auto-generate names based on position
    // We'll use positional names: var0, var1, ...
    // But if user uses format like "Torque:%d Nm", the regex captures the number

    QString pattern;
    int varCount = 0;

    for (int i = 0; i < formatStr.length(); ) {
        if (formatStr[i] == '%' && i + 1 < formatStr.length()) {
            QChar spec = formatStr[i + 1];
            QString groupName = QString("var%1").arg(varCount);

            switch (spec.unicode()) {
            case 'd':
                pattern += QString("(?<%1>\\-?\\d+)").arg(groupName);
                break;
            case 'f':
                pattern += QString("(?<%1>\\-?\\d+\\.?\\d*(?:[eE][+\\-]?\\d+)?)").arg(groupName);
                break;
            case 's':
                pattern += QString("(?<%1>\\S+)").arg(groupName);
                break;
            case 'c':
                pattern += QString("(?<%1>.)").arg(groupName);
                break;
            default:
                // Not a format specifier, treat literally
                pattern += QRegularExpression::escape(QString(formatStr[i]));
                i++;
                continue;
            }
            varCount++;
            i += 2;
        } else {
            pattern += QRegularExpression::escape(QString(formatStr[i]));
            i++;
        }
    }

    return QRegularExpression(pattern);
}

QStringList Parser::extractVariableNames(const QString &formatStr) const
{
    QStringList names;
    int varCount = 0;

    for (int i = 0; i < formatStr.length(); ) {
        if (formatStr[i] == '%' && i + 1 < formatStr.length()) {
            QChar spec = formatStr[i + 1];
            switch (spec.unicode()) {
            case 'd': case 'f': case 's': case 'c':
                names << QString("var%1").arg(varCount++);
                i += 2;
                break;
            default:
                i++;
                break;
            }
        } else {
            i++;
        }
    }
    return names;
}

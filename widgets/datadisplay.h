#ifndef DATADISPLAY_H
#define DATADISPLAY_H

#include <QFrame>
#include <QLabel>
#include <QMap>
#include <QVariant>
#include <QString>
#include "parser.h"

class DataDisplay : public QFrame
{
    Q_OBJECT

public:
    enum Style { DefaultStyle, DarkStyle, MinimalStyle };

    explicit DataDisplay(const QString &label, const QString &formatStr,
                         QWidget *parent = nullptr);

    QString displayLabel() const { return m_label; }
    QString formatString() const { return m_formatStr; }
    QString lastValue() const { return m_lastValue; }
    int styleIndex() const { return m_style; }

    void updateConfig(const QString &label, const QString &formatStr);
    void setStyle(int styleIndex);
    QMap<QString, QVariant> parseAndExtract(const QString &rawData);

public slots:
    void processData(const QString &rawData);

private:
    void applyStyle();

    QString m_label;
    QString m_formatStr;
    QString m_lastValue;
    QLabel *m_titleLabel;
    QLabel *m_valueLabel;
    Parser *m_parser;
    int m_style;
};

#endif // DATADISPLAY_H

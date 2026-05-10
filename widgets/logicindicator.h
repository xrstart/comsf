#ifndef LOGICINDICATOR_H
#define LOGICINDICATOR_H

#include <QFrame>
#include <QLabel>
#include <QMap>
#include <QVariant>
#include <QString>
#include "expressionevaluator.h"

class Parser;

class LogicIndicator : public QFrame
{
    Q_OBJECT

public:
    enum Style { DefaultStyle, FlatStyle, RealisticStyle };

    explicit LogicIndicator(const QString &name, const QString &expression,
                            const QString &formatStr,
                            const QString &trueColor, const QString &falseColor,
                            QWidget *parent = nullptr);

    QString indicatorName() const { return m_name; }
    QString expression() const { return m_expression; }
    QString formatString() const { return m_formatStr; }
    QString trueColor() const { return m_trueColor; }
    QString falseColor() const { return m_falseColor; }
    int styleIndex() const { return m_style; }

    void updateConfig(const QString &name, const QString &expression,
                      const QString &formatStr,
                      const QString &trueColor, const QString &falseColor);
    void setStyle(int styleIndex);

public slots:
    void processData(const QString &rawData);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_name;
    QString m_expression;
    QString m_formatStr;
    QString m_trueColor;
    QString m_falseColor;
    QColor m_currentColor;
    QLabel *m_nameLabel;
    Parser *m_parser;
    ExpressionEvaluator *m_evaluator;
    bool m_hasResult;
    Style m_style;
};

#endif // LOGICINDICATOR_H

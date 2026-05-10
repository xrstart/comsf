#ifndef EXPRESSIONEVALUATOR_H
#define EXPRESSIONEVALUATOR_H

#include <QObject>
#include <QMap>
#include <QVariant>
#include <QString>

class ExpressionEvaluator : public QObject
{
    Q_OBJECT

public:
    explicit ExpressionEvaluator(QObject *parent = nullptr);

    // Evaluate a logical expression with given variables
    // Returns true/false, ok indicates if evaluation succeeded
    bool evaluate(const QString &expression, const QMap<QString, QVariant> &variables, bool &ok);

signals:
    void evalError(const QString &errorMsg);

private:
    // Tokenizer
    enum TokenType { TokNumber, TokVariable, TokOperator, TokLParen, TokRParen, TokEnd };

    struct Token {
        TokenType type;
        QString value;
    };

    QList<Token> tokenize(const QString &expr);
    bool evalExpression(const QList<Token> &tokens, int &pos,
                        const QMap<QString, QVariant> &variables, double &result);
    bool evalComparison(const QList<Token> &tokens, int &pos,
                        const QMap<QString, QVariant> &variables, double &result);
    bool evalTerm(const QList<Token> &tokens, int &pos,
                  const QMap<QString, QVariant> &variables, double &result);
    bool evalFactor(const QList<Token> &tokens, int &pos,
                    const QMap<QString, QVariant> &variables, double &result);
    bool evalPrimary(const QList<Token> &tokens, int &pos,
                     const QMap<QString, QVariant> &variables, double &result);

    double getVariableValue(const QString &name, const QMap<QString, QVariant> &variables, bool &ok) const;
};

#endif // EXPRESSIONEVALUATOR_H

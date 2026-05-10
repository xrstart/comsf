#include "expressionevaluator.h"
#include <QRegularExpression>
#include <cmath>

ExpressionEvaluator::ExpressionEvaluator(QObject *parent)
    : QObject(parent)
{
}

bool ExpressionEvaluator::evaluate(const QString &expression,
                                    const QMap<QString, QVariant> &variables,
                                    bool &ok)
{
    ok = false;
    if (expression.trimmed().isEmpty())
        return false;

    QList<Token> tokens = tokenize(expression);
    if (tokens.isEmpty())
        return false;

    int pos = 0;
    double result = 0;
    if (!evalExpression(tokens, pos, variables, result))
        return false;

    if (pos < tokens.size() && tokens[pos].type != TokEnd) {
        emit evalError(QString("Unexpected token: %1").arg(tokens[pos].value));
        return false;
    }

    ok = true;
    return result != 0.0;
}

QList<ExpressionEvaluator::Token> ExpressionEvaluator::tokenize(const QString &expr)
{
    QList<Token> tokens;
    int i = 0;
    QString cleanExpr = expr.simplified();

    while (i < cleanExpr.length()) {
        QChar ch = cleanExpr[i];

        if (ch.isSpace()) {
            i++;
            continue;
        }

        // Number
        if (ch.isDigit() || (ch == '.' && i + 1 < cleanExpr.length() && cleanExpr[i + 1].isDigit())) {
            QString num;
            while (i < cleanExpr.length() && (cleanExpr[i].isDigit() || cleanExpr[i] == '.' || cleanExpr[i] == 'e' || cleanExpr[i] == 'E' || cleanExpr[i] == '-' || cleanExpr[i] == '+')) {
                if ((cleanExpr[i] == '-' || cleanExpr[i] == '+') && !num.isEmpty() && num.back() != 'e' && num.back() != 'E')
                    break;
                num += cleanExpr[i];
                i++;
            }
            tokens.append({TokNumber, num});
            continue;
        }

        // Variable name (letters, digits, underscores)
        if (ch.isLetter() || ch == '_') {
            QString name;
            while (i < cleanExpr.length() && (cleanExpr[i].isLetterOrNumber() || cleanExpr[i] == '_')) {
                name += cleanExpr[i];
                i++;
            }
            // Check for AND, OR, NOT keywords
            QString upper = name.toUpper();
            if (upper == "AND") {
                tokens.append({TokOperator, "AND"});
            } else if (upper == "OR") {
                tokens.append({TokOperator, "OR"});
            } else if (upper == "NOT") {
                tokens.append({TokOperator, "NOT"});
            } else {
                tokens.append({TokVariable, name});
            }
            continue;
        }

        // Operators
        if (ch == '>' || ch == '<' || ch == '=' || ch == '!') {
            QString op;
            op += ch;
            if (i + 1 < cleanExpr.length() && cleanExpr[i + 1] == '=') {
                op += '=';
                i += 2;
            } else {
                i++;
            }
            tokens.append({TokOperator, op});
            continue;
        }

        if (ch == '&') {
            if (i + 1 < cleanExpr.length() && cleanExpr[i + 1] == '&') {
                tokens.append({TokOperator, "AND"});
                i += 2;
            } else {
                i++;
            }
            continue;
        }

        if (ch == '|') {
            if (i + 1 < cleanExpr.length() && cleanExpr[i + 1] == '|') {
                tokens.append({TokOperator, "OR"});
                i += 2;
            } else {
                i++;
            }
            continue;
        }

        if (ch == '!') {
            if (i + 1 < cleanExpr.length() && cleanExpr[i + 1] == '=') {
                tokens.append({TokOperator, "!="});
                i += 2;
            } else {
                tokens.append({TokOperator, "NOT"});
                i++;
            }
            continue;
        }

        if (ch == '(') {
            tokens.append({TokLParen, "("});
            i++;
            continue;
        }

        if (ch == ')') {
            tokens.append({TokRParen, ")"});
            i++;
            continue;
        }

        // Skip unknown characters
        i++;
    }

    tokens.append({TokEnd, ""});
    return tokens;
}

bool ExpressionEvaluator::evalExpression(const QList<Token> &tokens, int &pos,
                                          const QMap<QString, QVariant> &variables,
                                          double &result)
{
    // expression = comparison ( (AND|OR) comparison )*
    if (!evalComparison(tokens, pos, variables, result))
        return false;

    while (pos < tokens.size() && tokens[pos].type == TokOperator &&
           (tokens[pos].value == "AND" || tokens[pos].value == "OR")) {
        QString op = tokens[pos].value;
        pos++;
        double right = 0;
        if (!evalComparison(tokens, pos, variables, right))
            return false;

        if (op == "AND")
            result = (result != 0.0 && right != 0.0) ? 1.0 : 0.0;
        else // OR
            result = (result != 0.0 || right != 0.0) ? 1.0 : 0.0;
    }
    return true;
}

bool ExpressionEvaluator::evalComparison(const QList<Token> &tokens, int &pos,
                                          const QMap<QString, QVariant> &variables,
                                          double &result)
{
    // comparison = term ( (>|<|==|!=|>=|<=) term )*
    if (!evalTerm(tokens, pos, variables, result))
        return false;

    while (pos < tokens.size() && tokens[pos].type == TokOperator &&
           (tokens[pos].value == ">" || tokens[pos].value == "<" ||
            tokens[pos].value == "==" || tokens[pos].value == "!=" ||
            tokens[pos].value == ">=" || tokens[pos].value == "<=")) {
        QString op = tokens[pos].value;
        pos++;
        double right = 0;
        if (!evalTerm(tokens, pos, variables, right))
            return false;

        if (op == ">")
            result = (result > right) ? 1.0 : 0.0;
        else if (op == "<")
            result = (result < right) ? 1.0 : 0.0;
        else if (op == "==")
            result = (std::abs(result - right) < 1e-9) ? 1.0 : 0.0;
        else if (op == "!=")
            result = (std::abs(result - right) >= 1e-9) ? 1.0 : 0.0;
        else if (op == ">=")
            result = (result >= right) ? 1.0 : 0.0;
        else if (op == "<=")
            result = (result <= right) ? 1.0 : 0.0;
    }
    return true;
}

bool ExpressionEvaluator::evalTerm(const QList<Token> &tokens, int &pos,
                                    const QMap<QString, QVariant> &variables,
                                    double &result)
{
    // term = factor ( (+|-) factor )*
    if (!evalFactor(tokens, pos, variables, result))
        return false;

    while (pos < tokens.size() && tokens[pos].type == TokOperator &&
           (tokens[pos].value == "+" || tokens[pos].value == "-")) {
        QString op = tokens[pos].value;
        pos++;
        double right = 0;
        if (!evalFactor(tokens, pos, variables, right))
            return false;

        if (op == "+")
            result += right;
        else
            result -= right;
    }
    return true;
}

bool ExpressionEvaluator::evalFactor(const QList<Token> &tokens, int &pos,
                                      const QMap<QString, QVariant> &variables,
                                      double &result)
{
    // factor = primary ( (*|/) primary )*
    if (!evalPrimary(tokens, pos, variables, result))
        return false;

    while (pos < tokens.size() && tokens[pos].type == TokOperator &&
           (tokens[pos].value == "*" || tokens[pos].value == "/")) {
        QString op = tokens[pos].value;
        pos++;
        double right = 0;
        if (!evalPrimary(tokens, pos, variables, right))
            return false;

        if (op == "*")
            result *= right;
        else {
            if (std::abs(right) < 1e-15) {
                emit evalError("Division by zero");
                return false;
            }
            result /= right;
        }
    }
    return true;
}

bool ExpressionEvaluator::evalPrimary(const QList<Token> &tokens, int &pos,
                                       const QMap<QString, QVariant> &variables,
                                       double &result)
{
    if (pos >= tokens.size())
        return false;

    // NOT
    if (tokens[pos].type == TokOperator && tokens[pos].value == "NOT") {
        pos++;
        if (!evalPrimary(tokens, pos, variables, result))
            return false;
        result = (result == 0.0) ? 1.0 : 0.0;
        return true;
    }

    // Unary minus
    if (tokens[pos].type == TokOperator && tokens[pos].value == "-") {
        pos++;
        if (!evalPrimary(tokens, pos, variables, result))
            return false;
        result = -result;
        return true;
    }

    // Parenthesized expression
    if (tokens[pos].type == TokLParen) {
        pos++;
        if (!evalExpression(tokens, pos, variables, result))
            return false;
        if (pos >= tokens.size() || tokens[pos].type != TokRParen) {
            emit evalError("Missing closing parenthesis");
            return false;
        }
        pos++;
        return true;
    }

    // Number
    if (tokens[pos].type == TokNumber) {
        bool ok;
        result = tokens[pos].value.toDouble(&ok);
        if (!ok) {
            emit evalError(QString("Invalid number: %1").arg(tokens[pos].value));
            return false;
        }
        pos++;
        return true;
    }

    // Variable
    if (tokens[pos].type == TokVariable) {
        bool ok;
        result = getVariableValue(tokens[pos].value, variables, ok);
        if (!ok) {
            emit evalError(QString("Variable not found: %1").arg(tokens[pos].value));
            return false;
        }
        pos++;
        return true;
    }

    emit evalError(QString("Unexpected token: %1").arg(tokens[pos].value));
    return false;
}

double ExpressionEvaluator::getVariableValue(const QString &name,
                                              const QMap<QString, QVariant> &variables,
                                              bool &ok) const
{
    if (!variables.contains(name)) {
        ok = false;
        return 0.0;
    }
    ok = true;
    return variables[name].toDouble();
}

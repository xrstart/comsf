#include "logicindicator.h"
#include "parser.h"
#include <QVBoxLayout>
#include <QPainter>

LogicIndicator::LogicIndicator(const QString &name, const QString &expression,
                               const QString &formatStr,
                               const QString &trueColor, const QString &falseColor,
                               QWidget *parent)
    : QFrame(parent)
    , m_name(name)
    , m_expression(expression)
    , m_formatStr(formatStr)
    , m_trueColor(trueColor)
    , m_falseColor(falseColor)
    , m_currentColor(Qt::gray)
    , m_parser(new Parser(this))
    , m_evaluator(new ExpressionEvaluator(this))
    , m_hasResult(false)
    , m_style(DefaultStyle)
{
    setFrameStyle(QFrame::NoFrame);
    setMinimumSize(80, 60);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 2, 4, 2);
    layout->setSpacing(2);

    m_nameLabel = new QLabel(m_name, this);
    m_nameLabel->setAlignment(Qt::AlignCenter);
    m_nameLabel->setStyleSheet("font-weight: 500; font-size: 11px; color: #6b7280; background: transparent;");
    layout->addWidget(m_nameLabel, 0, Qt::AlignTop | Qt::AlignHCenter);
    layout->addStretch(1);
}

void LogicIndicator::updateConfig(const QString &name, const QString &expression,
                                   const QString &formatStr,
                                   const QString &trueColor, const QString &falseColor)
{
    m_name = name;
    m_expression = expression;
    m_formatStr = formatStr;
    m_trueColor = trueColor;
    m_falseColor = falseColor;
    m_nameLabel->setText(m_name);
    m_hasResult = false;
    m_currentColor = Qt::gray;
    update();
}

void LogicIndicator::setStyle(int styleIndex)
{
    m_style = static_cast<Style>(styleIndex);
    update();
}

void LogicIndicator::processData(const QString &rawData)
{
    if (m_formatStr.isEmpty() || m_expression.isEmpty())
        return;

    QMap<QString, QVariant> variables;
    if (!m_parser->parse(m_formatStr, rawData, variables))
        return;

    bool ok = false;
    bool result = m_evaluator->evaluate(m_expression, variables, ok);

    if (ok) {
        m_hasResult = true;
        m_currentColor = QColor(result ? m_trueColor : m_falseColor);
    } else {
        m_hasResult = false;
        m_currentColor = Qt::gray;
    }
    update();
}

void LogicIndicator::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Circle area: below the name label
    int topMargin = m_nameLabel->height() + 4;
    int availW = width() - 8;
    int availH = height() - topMargin - 4;
    int diameter = qMin(availW, availH);
    if (diameter < 16)
        diameter = 16;

    int cx = width() / 2;
    int cy = topMargin + availH / 2;

    switch (m_style) {
    case FlatStyle: {
        painter.setBrush(m_currentColor);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(cx - diameter / 2, cy - diameter / 2, diameter, diameter);
        break;
    }
    case RealisticStyle: {
        QRadialGradient gradient(cx - diameter * 0.15, cy - diameter * 0.15, diameter * 0.55);
        gradient.setColorAt(0, m_currentColor.lighter(140));
        gradient.setColorAt(0.6, m_currentColor);
        gradient.setColorAt(1, m_currentColor.darker(120));
        painter.setBrush(gradient);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(cx - diameter / 2, cy - diameter / 2, diameter, diameter);
        // Subtle highlight
        painter.setBrush(QColor(255, 255, 255, 40));
        painter.drawEllipse(cx - diameter / 4, cy - diameter / 3, diameter / 3, diameter / 4);
        break;
    }
    default: { // DefaultStyle - clean flat circle with subtle border
        painter.setBrush(m_currentColor);
        painter.setPen(QPen(m_currentColor.darker(110), 1));
        painter.drawEllipse(cx - diameter / 2, cy - diameter / 2, diameter, diameter);
        break;
    }
    }
}

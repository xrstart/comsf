#include "datadisplay.h"
#include <QVBoxLayout>

DataDisplay::DataDisplay(const QString &label, const QString &formatStr,
                         QWidget *parent)
    : QFrame(parent)
    , m_label(label)
    , m_formatStr(formatStr)
    , m_parser(new Parser(this))
    , m_style(DefaultStyle)
{
    setFrameStyle(QFrame::Box | QFrame::Raised);
    setLineWidth(2);
    setMinimumSize(120, 60);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(6, 4, 6, 4);
    layout->setSpacing(2);

    m_titleLabel = new QLabel(m_label, this);
    m_titleLabel->setAlignment(Qt::AlignCenter);

    m_valueLabel = new QLabel("--", this);
    m_valueLabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(m_titleLabel);
    layout->addWidget(m_valueLabel);

    applyStyle();
}

void DataDisplay::updateConfig(const QString &label, const QString &formatStr)
{
    m_label = label;
    m_formatStr = formatStr;
    m_titleLabel->setText(m_label);
}

void DataDisplay::setStyle(int styleIndex)
{
    m_style = styleIndex;
    applyStyle();
}

void DataDisplay::applyStyle()
{
    switch (m_style) {
    case DarkStyle:
        setStyleSheet("DataDisplay { background-color: #2b2b2b; border: 1px solid #555; border-radius: 6px; }");
        m_titleLabel->setStyleSheet("font-weight: bold; font-size: 10px; color: #aaa;");
        m_valueLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #00cc88;");
        break;
    case MinimalStyle:
        setStyleSheet("DataDisplay { background-color: transparent; border: none; border-bottom: 2px solid #4a90d9; }");
        m_titleLabel->setStyleSheet("font-size: 10px; color: #888;");
        m_valueLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #333;");
        break;
    default:
        setStyleSheet("DataDisplay { background-color: #f0f0f0; border: 1px solid #ccc; border-radius: 4px; }");
        m_titleLabel->setStyleSheet("font-weight: bold; font-size: 10px; color: #333;");
        m_valueLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #0066cc;");
        break;
    }
}

QMap<QString, QVariant> DataDisplay::parseAndExtract(const QString &rawData)
{
    QMap<QString, QVariant> variables;
    if (m_formatStr.isEmpty())
        return variables;

    if (m_parser->parse(m_formatStr, rawData, variables)) {
        if (!variables.isEmpty()) {
            QVariant val = variables.first();
            if (val.canConvert<double>())
                m_valueLabel->setText(QString::number(val.toDouble(), 'g', 6));
            else
                m_valueLabel->setText(val.toString());
            m_lastValue = m_valueLabel->text();
        }
    }
    return variables;
}

void DataDisplay::processData(const QString &rawData)
{
    parseAndExtract(rawData);
}

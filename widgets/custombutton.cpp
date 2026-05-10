#include "custombutton.h"
#include "serialmanager.h"

CustomButton::CustomButton(const QString &name, const QString &message,
                           bool hexMode, SerialManager *serialManager,
                           QWidget *parent)
    : QPushButton(parent)
    , m_name(name)
    , m_message(message)
    , m_hexMode(hexMode)
    , m_style(DefaultStyle)
    , m_serialManager(serialManager)
{
    setText(m_name);
    setMinimumSize(80, 36);
    applyStyle();
    connect(this, &QPushButton::clicked, this, &CustomButton::onClicked);
}

void CustomButton::updateConfig(const QString &name, const QString &message, bool hexMode)
{
    m_name = name;
    m_message = message;
    m_hexMode = hexMode;
    setText(m_name);
}

void CustomButton::setStyle(int styleIndex)
{
    m_style = styleIndex;
    applyStyle();
}

void CustomButton::applyStyle()
{
    switch (m_style) {
    case RoundedStyle:
        setStyleSheet(
            "QPushButton { background-color: #4a90d9; color: white; border: none; "
            "border-radius: 12px; padding: 6px 16px; font-weight: bold; }"
            "QPushButton:hover { background-color: #357abd; }"
            "QPushButton:pressed { background-color: #2a6099; }");
        break;
    case OutlineStyle:
        setStyleSheet(
            "QPushButton { background-color: transparent; color: #4a90d9; "
            "border: 2px solid #4a90d9; border-radius: 4px; padding: 6px 16px; font-weight: bold; }"
            "QPushButton:hover { background-color: #4a90d9; color: white; }"
            "QPushButton:pressed { background-color: #357abd; color: white; }");
        break;
    default:
        setStyleSheet(
            "QPushButton { font-weight: bold; padding: 6px 12px; }");
        break;
    }
}

void CustomButton::onClicked()
{
    if (!m_serialManager || !m_serialManager->isPortOpen())
        return;

    if (m_hexMode)
        m_serialManager->sendHex(m_message);
    else
        m_serialManager->sendAscii(m_message);
}

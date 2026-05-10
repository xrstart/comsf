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
            "QPushButton { background-color: #2563eb; color: white; border: none; "
            "border-radius: 6px; padding: 6px 16px; font-weight: 500; }"
            "QPushButton:hover { background-color: #1d4ed8; }"
            "QPushButton:pressed { background-color: #1e40af; }");
        break;
    case OutlineStyle:
        setStyleSheet(
            "QPushButton { background-color: transparent; color: #374151; "
            "border: 1px solid #d1d5db; border-radius: 6px; padding: 6px 16px; font-weight: 500; }"
            "QPushButton:hover { background-color: #f9fafb; border-color: #9ca3af; }"
            "QPushButton:pressed { background-color: #f3f4f6; }");
        break;
    case SquareStyle:
        setStyleSheet(
            "QPushButton { background-color: #111827; color: #ffffff; border: none; "
            "border-radius: 0px; padding: 6px 16px; font-weight: 500; }"
            "QPushButton:hover { background-color: #1f2937; }"
            "QPushButton:pressed { background-color: #000000; }");
        break;
    case PillStyle:
        setStyleSheet(
            "QPushButton { background-color: #111827; color: #ffffff; border: none; "
            "border-radius: 16px; padding: 6px 20px; font-weight: 500; }"
            "QPushButton:hover { background-color: #1f2937; }"
            "QPushButton:pressed { background-color: #000000; }");
        break;
    case TextStyle:
        setStyleSheet(
            "QPushButton { background-color: transparent; color: #111827; border: none; "
            "border-radius: 4px; padding: 6px 12px; font-weight: 500; "
            "text-decoration: underline; }"
            "QPushButton:hover { color: #2563eb; }"
            "QPushButton:pressed { color: #1e40af; }");
        break;
    case DangerStyle:
        setStyleSheet(
            "QPushButton { background-color: #dc2626; color: #ffffff; border: none; "
            "border-radius: 6px; padding: 6px 16px; font-weight: 500; }"
            "QPushButton:hover { background-color: #b91c1c; }"
            "QPushButton:pressed { background-color: #991b1b; }");
        break;
    default:
        setStyleSheet(
            "QPushButton { background-color: #111827; color: #ffffff; border: none; "
            "border-radius: 6px; padding: 6px 16px; font-weight: 500; }"
            "QPushButton:hover { background-color: #1f2937; }"
            "QPushButton:pressed { background-color: #000000; }");
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

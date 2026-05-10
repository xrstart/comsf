#ifndef CUSTOMBUTTON_H
#define CUSTOMBUTTON_H

#include <QPushButton>
#include <QString>

class SerialManager;

class CustomButton : public QPushButton
{
    Q_OBJECT

public:
    enum Style { DefaultStyle, RoundedStyle, OutlineStyle };

    explicit CustomButton(const QString &name, const QString &message,
                          bool hexMode, SerialManager *serialManager,
                          QWidget *parent = nullptr);

    QString buttonName() const { return m_name; }
    QString message() const { return m_message; }
    bool hexMode() const { return m_hexMode; }
    int styleIndex() const { return m_style; }

    void updateConfig(const QString &name, const QString &message, bool hexMode);
    void setStyle(int styleIndex);

private slots:
    void onClicked();

private:
    void applyStyle();

    QString m_name;
    QString m_message;
    bool m_hexMode;
    int m_style;
    SerialManager *m_serialManager;
};

#endif // CUSTOMBUTTON_H

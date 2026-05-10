#ifndef BUTTONCONFIGDIALOG_H
#define BUTTONCONFIGDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>

class ButtonConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ButtonConfigDialog(QWidget *parent = nullptr);
    ButtonConfigDialog(const QString &name, const QString &message, bool hexMode,
                       int styleIndex, QWidget *parent = nullptr);

    QString name() const;
    QString message() const;
    bool hexMode() const;
    int styleIndex() const;

private:
    void setupUI();
    QLineEdit *m_nameEdit;
    QLineEdit *m_messageEdit;
    QCheckBox *m_hexCheck;
    QComboBox *m_styleCombo;
};

#endif // BUTTONCONFIGDIALOG_H

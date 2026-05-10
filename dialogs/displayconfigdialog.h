#ifndef DISPLAYCONFIGDIALOG_H
#define DISPLAYCONFIGDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>

class DisplayConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DisplayConfigDialog(QWidget *parent = nullptr);
    DisplayConfigDialog(const QString &label, const QString &formatStr,
                        int styleIndex, QWidget *parent = nullptr);

    QString label() const;
    QString formatString() const;
    int styleIndex() const;

private:
    void setupUI();
    QLineEdit *m_labelEdit;
    QLineEdit *m_formatEdit;
    QComboBox *m_styleCombo;
};

#endif // DISPLAYCONFIGDIALOG_H

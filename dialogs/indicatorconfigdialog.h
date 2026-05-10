#ifndef INDICATORCONFIGDIALOG_H
#define INDICATORCONFIGDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>

class IndicatorConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IndicatorConfigDialog(QWidget *parent = nullptr);
    IndicatorConfigDialog(const QString &name, const QString &expression,
                          const QString &formatStr,
                          const QString &trueColor, const QString &falseColor,
                          int styleIndex, QWidget *parent = nullptr);

    QString name() const;
    QString expression() const;
    QString formatString() const;
    QString trueColor() const;
    QString falseColor() const;
    int styleIndex() const;

private slots:
    void onSelectTrueColor();
    void onSelectFalseColor();

private:
    void setupUI();
    QLineEdit *m_nameEdit;
    QLineEdit *m_formatEdit;
    QLineEdit *m_expressionEdit;
    QPushButton *m_trueColorBtn;
    QPushButton *m_falseColorBtn;
    QComboBox *m_styleCombo;
    QString m_trueColor;
    QString m_falseColor;
};

#endif // INDICATORCONFIGDIALOG_H

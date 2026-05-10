#include "buttonconfigdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

ButtonConfigDialog::ButtonConfigDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
}

ButtonConfigDialog::ButtonConfigDialog(const QString &name, const QString &message,
                                       bool hexMode, int styleIndex, QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    m_nameEdit->setText(name);
    m_messageEdit->setText(message);
    m_hexCheck->setChecked(hexMode);
    m_styleCombo->setCurrentIndex(styleIndex);
}

QString ButtonConfigDialog::name() const { return m_nameEdit->text(); }
QString ButtonConfigDialog::message() const { return m_messageEdit->text(); }
bool ButtonConfigDialog::hexMode() const { return m_hexCheck->isChecked(); }
int ButtonConfigDialog::styleIndex() const { return m_styleCombo->currentIndex(); }

void ButtonConfigDialog::setupUI()
{
    setWindowTitle("配置发送按钮");
    setMinimumWidth(350);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    mainLayout->addWidget(new QLabel("按钮名称:"));
    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText("如：启动电机");
    mainLayout->addWidget(m_nameEdit);

    mainLayout->addWidget(new QLabel("预设报文:"));
    m_messageEdit = new QLineEdit(this);
    m_messageEdit->setPlaceholderText("如：START 或 AA 55 01 02");
    mainLayout->addWidget(m_messageEdit);

    m_hexCheck = new QCheckBox("HEX 格式发送", this);
    mainLayout->addWidget(m_hexCheck);

    mainLayout->addWidget(new QLabel("按钮样式:"));
    m_styleCombo = new QComboBox(this);
    m_styleCombo->addItems({"默认", "圆角蓝色", "线框"});
    mainLayout->addWidget(m_styleCombo);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *okBtn = new QPushButton("确定", this);
    QPushButton *cancelBtn = new QPushButton("取消", this);
    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    btnLayout->addStretch();
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);
    mainLayout->addLayout(btnLayout);
}

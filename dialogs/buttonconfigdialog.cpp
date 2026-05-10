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
    setMinimumWidth(360);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(20, 20, 20, 16);

    QLabel *nameLbl = new QLabel("按钮名称", this);
    nameLbl->setStyleSheet("color: #6b7280; font-size: 12px;");
    mainLayout->addWidget(nameLbl);
    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText("如：启动电机");
    mainLayout->addWidget(m_nameEdit);

    QLabel *msgLbl = new QLabel("预设报文", this);
    msgLbl->setStyleSheet("color: #6b7280; font-size: 12px;");
    mainLayout->addWidget(msgLbl);
    m_messageEdit = new QLineEdit(this);
    m_messageEdit->setPlaceholderText("如：START 或 AA 55 01 02");
    mainLayout->addWidget(m_messageEdit);

    m_hexCheck = new QCheckBox("HEX 格式发送", this);
    mainLayout->addWidget(m_hexCheck);

    QLabel *styleLbl = new QLabel("按钮样式", this);
    styleLbl->setStyleSheet("color: #6b7280; font-size: 12px;");
    mainLayout->addWidget(styleLbl);
    m_styleCombo = new QComboBox(this);
    m_styleCombo->addItems({"黑色主按钮", "蓝色强调", "幽灵线框", "方形直角", "胶囊圆角", "纯文字", "红色警告"});
    mainLayout->addWidget(m_styleCombo);

    mainLayout->addSpacing(8);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(8);
    QPushButton *cancelBtn = new QPushButton("取消", this);
    cancelBtn->setStyleSheet(
        "QPushButton { background-color: transparent; color: #374151; border: 1px solid #d1d5db; "
        "border-radius: 4px; padding: 6px 16px; }"
        "QPushButton:hover { background-color: #f3f4f6; border-color: #9ca3af; }");
    QPushButton *okBtn = new QPushButton("确定", this);
    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    btnLayout->addStretch();
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(okBtn);
    mainLayout->addLayout(btnLayout);
}

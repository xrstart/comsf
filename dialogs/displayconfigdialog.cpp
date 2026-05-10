#include "displayconfigdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

DisplayConfigDialog::DisplayConfigDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
}

DisplayConfigDialog::DisplayConfigDialog(const QString &label, const QString &formatStr,
                                         int styleIndex, QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    m_labelEdit->setText(label);
    m_formatEdit->setText(formatStr);
    m_styleCombo->setCurrentIndex(styleIndex);
}

QString DisplayConfigDialog::label() const { return m_labelEdit->text(); }
QString DisplayConfigDialog::formatString() const { return m_formatEdit->text(); }
int DisplayConfigDialog::styleIndex() const { return m_styleCombo->currentIndex(); }

void DisplayConfigDialog::setupUI()
{
    setWindowTitle("配置数据显示框");
    setMinimumWidth(400);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(20, 20, 20, 16);

    QLabel *labelLbl = new QLabel("数据标签", this);
    labelLbl->setStyleSheet("color: #6b7280; font-size: 12px;");
    mainLayout->addWidget(labelLbl);
    m_labelEdit = new QLineEdit(this);
    m_labelEdit->setPlaceholderText("如：实时速度");
    mainLayout->addWidget(m_labelEdit);

    QLabel *fmtLbl = new QLabel("解析格式化字符串", this);
    fmtLbl->setStyleSheet("color: #6b7280; font-size: 12px;");
    mainLayout->addWidget(fmtLbl);
    m_formatEdit = new QLineEdit(this);
    m_formatEdit->setPlaceholderText("如：Speed:%f m/s 或 Torque:%d Nm");
    mainLayout->addWidget(m_formatEdit);

    QLabel *hint = new QLabel("支持 %d(整数) %f(浮点) %s(字符串) %c(字符)", this);
    hint->setStyleSheet("color: #9ca3af; font-size: 11px;");
    mainLayout->addWidget(hint);

    QLabel *styleLbl = new QLabel("显示样式", this);
    styleLbl->setStyleSheet("color: #6b7280; font-size: 12px;");
    mainLayout->addWidget(styleLbl);
    m_styleCombo = new QComboBox(this);
    m_styleCombo->addItems({"默认", "深色", "极简"});
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

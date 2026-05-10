#include "indicatorconfigdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QColorDialog>

IndicatorConfigDialog::IndicatorConfigDialog(QWidget *parent)
    : QDialog(parent)
    , m_trueColor("green")
    , m_falseColor("red")
{
    setupUI();
}

IndicatorConfigDialog::IndicatorConfigDialog(const QString &name, const QString &expression,
                                              const QString &formatStr,
                                              const QString &trueColor, const QString &falseColor,
                                              int styleIndex, QWidget *parent)
    : QDialog(parent)
    , m_trueColor(trueColor)
    , m_falseColor(falseColor)
{
    setupUI();
    m_nameEdit->setText(name);
    m_formatEdit->setText(formatStr);
    m_expressionEdit->setText(expression);
    m_trueColorBtn->setStyleSheet(QString("background-color: %1; border: 1px solid #999;").arg(m_trueColor));
    m_falseColorBtn->setStyleSheet(QString("background-color: %1; border: 1px solid #999;").arg(m_falseColor));
    m_styleCombo->setCurrentIndex(styleIndex);
}

QString IndicatorConfigDialog::name() const { return m_nameEdit->text(); }
QString IndicatorConfigDialog::expression() const { return m_expressionEdit->text(); }
QString IndicatorConfigDialog::formatString() const { return m_formatEdit->text(); }
QString IndicatorConfigDialog::trueColor() const { return m_trueColor; }
QString IndicatorConfigDialog::falseColor() const { return m_falseColor; }
int IndicatorConfigDialog::styleIndex() const { return m_styleCombo->currentIndex(); }

void IndicatorConfigDialog::setupUI()
{
    setWindowTitle("配置逻辑信号灯");
    setMinimumWidth(420);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    mainLayout->addWidget(new QLabel("信号灯名称:"));
    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText("如：力矩达标指示");
    mainLayout->addWidget(m_nameEdit);

    mainLayout->addWidget(new QLabel("解析格式化字符串:"));
    m_formatEdit = new QLineEdit(this);
    m_formatEdit->setPlaceholderText("如：Torque:%f,PWM:%d");
    mainLayout->addWidget(m_formatEdit);

    QLabel *hint1 = new QLabel("支持 %d(整数) %f(浮点) %s(字符串) %c(字符)，变量按顺序命名为 var0, var1...");
    hint1->setStyleSheet("color: gray; font-size: 11px;");
    hint1->setWordWrap(true);
    mainLayout->addWidget(hint1);

    mainLayout->addWidget(new QLabel("逻辑表达式:"));
    m_expressionEdit = new QLineEdit(this);
    m_expressionEdit->setPlaceholderText("如：var0 > 2.8 AND var1 > 45");
    mainLayout->addWidget(m_expressionEdit);

    QLabel *hint2 = new QLabel("支持 > < == != AND/OR/NOT 运算符");
    hint2->setStyleSheet("color: gray; font-size: 11px;");
    hint2->setWordWrap(true);
    mainLayout->addWidget(hint2);

    mainLayout->addWidget(new QLabel("信号灯样式:"));
    m_styleCombo = new QComboBox(this);
    m_styleCombo->addItems({"默认", "扁平", "拟物"});
    mainLayout->addWidget(m_styleCombo);

    QHBoxLayout *trueColorLayout = new QHBoxLayout();
    trueColorLayout->addWidget(new QLabel("条件满足颜色:"));
    m_trueColorBtn = new QPushButton(this);
    m_trueColorBtn->setFixedSize(60, 26);
    m_trueColorBtn->setStyleSheet(QString("background-color: %1; border: 1px solid #999;").arg(m_trueColor));
    connect(m_trueColorBtn, &QPushButton::clicked, this, &IndicatorConfigDialog::onSelectTrueColor);
    trueColorLayout->addWidget(m_trueColorBtn);
    trueColorLayout->addStretch();
    mainLayout->addLayout(trueColorLayout);

    QHBoxLayout *falseColorLayout = new QHBoxLayout();
    falseColorLayout->addWidget(new QLabel("条件不满足颜色:"));
    m_falseColorBtn = new QPushButton(this);
    m_falseColorBtn->setFixedSize(60, 26);
    m_falseColorBtn->setStyleSheet(QString("background-color: %1; border: 1px solid #999;").arg(m_falseColor));
    connect(m_falseColorBtn, &QPushButton::clicked, this, &IndicatorConfigDialog::onSelectFalseColor);
    falseColorLayout->addWidget(m_falseColorBtn);
    falseColorLayout->addStretch();
    mainLayout->addLayout(falseColorLayout);

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

void IndicatorConfigDialog::onSelectTrueColor()
{
    QColor color = QColorDialog::getColor(QColor(m_trueColor), this, "选择满足条件颜色");
    if (color.isValid()) {
        m_trueColor = color.name();
        m_trueColorBtn->setStyleSheet(QString("background-color: %1; border: 1px solid #999;").arg(m_trueColor));
    }
}

void IndicatorConfigDialog::onSelectFalseColor()
{
    QColor color = QColorDialog::getColor(QColor(m_falseColor), this, "选择不满足条件颜色");
    if (color.isValid()) {
        m_falseColor = color.name();
        m_falseColorBtn->setStyleSheet(QString("background-color: %1; border: 1px solid #999;").arg(m_falseColor));
    }
}

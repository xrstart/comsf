#include "mainwindow.h"
#include "serialmanager.h"
#include "configmanager.h"
#include "widgets/dashboardarea.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QMenuBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollBar>
#include <QJsonArray>
#include <QDateTime>
#include <QWidgetAction>
#include <QApplication>

static const char *THEMES[] = {
    // 0: Default (light)
    "",
    // 1: Dark
    R"(
        * { background-color: #2b2b2b; color: #e0e0e0; border-color: #555; }
        QMainWindow, QWidget { background-color: #2b2b2b; }
        QGroupBox { border: 1px solid #555; border-radius: 4px; margin-top: 8px; padding-top: 16px; }
        QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 4px; color: #aaa; }
        QComboBox, QLineEdit, QTextEdit { background-color: #3c3c3c; color: #e0e0e0; border: 1px solid #555; border-radius: 3px; padding: 3px; }
        QComboBox::drop-down { border: none; }
        QComboBox QAbstractItemView { background-color: #3c3c3c; color: #e0e0e0; selection-background-color: #4a90d9; }
        QPushButton { background-color: #4a4a4a; color: #e0e0e0; border: 1px solid #666; border-radius: 4px; padding: 4px 10px; }
        QPushButton:hover { background-color: #5a5a5a; }
        QPushButton:pressed { background-color: #3a3a3a; }
        QCheckBox { color: #e0e0e0; }
        QLabel { color: #e0e0e0; }
        QMenuBar { background-color: #333; color: #e0e0e0; }
        QMenuBar::item:selected { background-color: #4a90d9; }
        QMenu { background-color: #3c3c3c; color: #e0e0e0; border: 1px solid #555; }
        QMenu::item:selected { background-color: #4a90d9; }
        QStatusBar { background-color: #333; color: #aaa; }
        QSplitter::handle { background-color: #444; }
        QScrollBar:vertical { background: #2b2b2b; width: 10px; }
        QScrollBar::handle:vertical { background: #555; border-radius: 5px; min-height: 20px; }
        QScrollArea { border: 1px solid #555; }
    )",
    // 2: Blue
    R"(
        QGroupBox { border: 1px solid #a0c0e0; border-radius: 4px; margin-top: 8px; padding-top: 16px; background-color: #f0f6ff; }
        QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 4px; color: #2060a0; }
        QComboBox, QLineEdit, QTextEdit { background-color: white; border: 1px solid #a0c0e0; border-radius: 3px; padding: 3px; }
        QPushButton { background-color: #4a90d9; color: white; border: none; border-radius: 4px; padding: 4px 10px; }
        QPushButton:hover { background-color: #357abd; }
        QPushButton:pressed { background-color: #2a6099; }
        QMenuBar { background-color: #2060a0; color: white; }
        QMenuBar::item:selected { background-color: #357abd; }
        QMenu { background-color: white; border: 1px solid #a0c0e0; }
        QMenu::item:selected { background-color: #4a90d9; color: white; }
        QStatusBar { background-color: #2060a0; color: white; }
    )",
    // 3: Green
    R"(
        QGroupBox { border: 1px solid #80c080; border-radius: 4px; margin-top: 8px; padding-top: 16px; background-color: #f0fff0; }
        QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 4px; color: #208020; }
        QComboBox, QLineEdit, QTextEdit { background-color: white; border: 1px solid #80c080; border-radius: 3px; padding: 3px; }
        QPushButton { background-color: #5cb85c; color: white; border: none; border-radius: 4px; padding: 4px 10px; }
        QPushButton:hover { background-color: #449d44; }
        QPushButton:pressed { background-color: #398439; }
        QMenuBar { background-color: #208020; color: white; }
        QMenuBar::item:selected { background-color: #449d44; }
        QMenu { background-color: white; border: 1px solid #80c080; }
        QMenu::item:selected { background-color: #5cb85c; color: white; }
        QStatusBar { background-color: #208020; color: white; }
    )",
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_serialManager(new SerialManager(this))
    , m_configManager(new ConfigManager(this))
    , m_logPaused(false)
{
    setupUI();
    setupMenuBar();

    m_logFlushTimer = new QTimer(this);
    m_logFlushTimer->setInterval(50);
    connect(m_logFlushTimer, &QTimer::timeout, this, [this]() {
        if (!m_recvBuffer.isEmpty() && !m_logPaused) {
            m_logText->moveCursor(QTextCursor::End);
            m_logText->insertPlainText(QString::fromUtf8(m_recvBuffer));
            m_recvBuffer.clear();
            QScrollBar *sb = m_logText->verticalScrollBar();
            sb->setValue(sb->maximum());
        }
    });
    m_logFlushTimer->start();

    connect(m_serialManager, &SerialManager::dataReceived, this, &MainWindow::onSerialDataReceived);
    connect(m_serialManager, &SerialManager::dataSent, this, &MainWindow::onSerialDataSent);
    connect(m_serialManager, &SerialManager::portError, this, &MainWindow::onSerialError);
    connect(m_serialManager, &SerialManager::portOpened, this, &MainWindow::onPortOpened);
    connect(m_serialManager, &SerialManager::portClosed, this, &MainWindow::onPortClosed);

    onRefreshPorts();
    statusBar()->showMessage("就绪");
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI()
{
    setWindowTitle("串口上位机助手");
    setMinimumSize(1100, 750);
    resize(1200, 800);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QHBoxLayout *mainLayout = new QHBoxLayout(central);

    // === Left panel ===
    QGroupBox *configGroup = new QGroupBox("串口配置", this);
    configGroup->setFixedWidth(210);
    QFormLayout *configLayout = new QFormLayout(configGroup);
    configLayout->setLabelAlignment(Qt::AlignRight);
    configLayout->setFormAlignment(Qt::AlignTop | Qt::AlignLeft);
    configLayout->setSpacing(5);

    setupSerialConfigPanel();
    configLayout->addRow("端口:", m_portCombo);
    configLayout->addRow("波特率:", m_baudRateCombo);
    configLayout->addRow("数据位:", m_dataBitsCombo);
    configLayout->addRow("停止位:", m_stopBitsCombo);
    configLayout->addRow("校验位:", m_parityCombo);
    configLayout->addRow("流控:", m_flowControlCombo);

    QHBoxLayout *portBtnLayout = new QHBoxLayout();
    portBtnLayout->addWidget(m_refreshBtn);
    portBtnLayout->addWidget(m_openBtn);
    portBtnLayout->addWidget(m_closeBtn);
    configLayout->addRow("", portBtnLayout);

    // Send area under serial config
    configLayout->addItem(new QSpacerItem(0, 10));
    configLayout->addRow("模式:", m_sendModeCombo);
    configLayout->addRow("数据:", m_sendEdit);
    configLayout->addRow("", m_sendBtn);

    // === Right panel ===
    QVBoxLayout *rightLayout = new QVBoxLayout();

    QGroupBox *logGroup = new QGroupBox("原始数据日志", this);
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    m_logText = new QTextEdit(this);
    m_logText->setReadOnly(true);
    m_logText->setFont(QFont("Consolas", 9));
    logLayout->addWidget(m_logText);

    QHBoxLayout *logControlLayout = new QHBoxLayout();
    m_pauseLogCheck = new QCheckBox("暂停显示", this);
    m_clearLogBtn = new QPushButton("清空", this);
    connect(m_clearLogBtn, &QPushButton::clicked, this, &MainWindow::onClearLog);
    connect(m_pauseLogCheck, &QCheckBox::toggled, this, [this](bool checked) { m_logPaused = checked; });
    logControlLayout->addWidget(m_pauseLogCheck);
    logControlLayout->addStretch();
    logControlLayout->addWidget(m_clearLogBtn);
    logLayout->addLayout(logControlLayout);

    QGroupBox *dashboardGroup = new QGroupBox("动态仪表盘", this);
    QVBoxLayout *dashLayout = new QVBoxLayout(dashboardGroup);
    m_dashboard = new DashboardArea(m_serialManager, this);
    dashLayout->addWidget(m_dashboard);

    // Auto-save config when dashboard save is clicked
    connect(m_dashboard, &DashboardArea::configSaved, this, [this](const QJsonArray &config) {
        m_configManager->saveToFile("autosave.json", config);
        statusBar()->showMessage("配置已自动保存", 3000);
    });

    QSplitter *splitter = new QSplitter(Qt::Vertical, this);
    splitter->addWidget(logGroup);
    splitter->addWidget(dashboardGroup);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 2);

    rightLayout->addWidget(splitter, 1);
    mainLayout->addWidget(configGroup);
    mainLayout->addLayout(rightLayout, 1);
}

void MainWindow::setupSerialConfigPanel()
{
    m_portCombo = new QComboBox(this);
    m_baudRateCombo = new QComboBox(this);
    m_baudRateCombo->setEditable(true);
    m_baudRateCombo->lineEdit()->setStyleSheet("padding-left: 0px; margin: 0px;");
    m_baudRateCombo->addItems({"9600", "19200", "38400", "57600", "115200",
                                "230400", "460800", "921600", "1000000", "2000000"});
    m_baudRateCombo->setCurrentText("115200");
    m_dataBitsCombo = new QComboBox(this);
    m_dataBitsCombo->addItems({"5", "6", "7", "8"});
    m_dataBitsCombo->setCurrentIndex(3);
    m_stopBitsCombo = new QComboBox(this);
    m_stopBitsCombo->addItems({"1", "1.5", "2"});
    m_parityCombo = new QComboBox(this);
    m_parityCombo->addItems({"None", "Odd", "Even", "Mark", "Space"});
    m_flowControlCombo = new QComboBox(this);
    m_flowControlCombo->addItems({"无", "硬件流控", "软件流控"});

    // Force left alignment for all combo boxes
    auto alignLeft = [](QComboBox *combo) {
        for (int i = 0; i < combo->count(); ++i)
            combo->setItemData(i, static_cast<int>(Qt::AlignLeft | Qt::AlignVCenter), Qt::TextAlignmentRole);
        if (combo->isEditable() && combo->lineEdit())
            combo->lineEdit()->setAlignment(Qt::AlignLeft);
    };
    alignLeft(m_baudRateCombo);
    alignLeft(m_dataBitsCombo);
    alignLeft(m_stopBitsCombo);
    alignLeft(m_parityCombo);
    alignLeft(m_flowControlCombo);

    m_refreshBtn = new QPushButton("刷新", this);
    m_openBtn = new QPushButton("打开", this);
    m_closeBtn = new QPushButton("关闭", this);
    m_closeBtn->setEnabled(false);
    connect(m_refreshBtn, &QPushButton::clicked, this, &MainWindow::onRefreshPorts);
    connect(m_openBtn, &QPushButton::clicked, this, &MainWindow::onOpenPort);
    connect(m_closeBtn, &QPushButton::clicked, this, &MainWindow::onClosePort);

    m_sendModeCombo = new QComboBox(this);
    m_sendModeCombo->addItems({"ASCII", "HEX"});
    m_sendEdit = new QLineEdit(this);
    m_sendEdit->setPlaceholderText("输入要发送的数据...");
    m_sendBtn = new QPushButton("发送", this);
    connect(m_sendBtn, &QPushButton::clicked, this, &MainWindow::onSendData);
    connect(m_sendEdit, &QLineEdit::returnPressed, this, &MainWindow::onSendData);
}

void MainWindow::setupMenuBar()
{
    QMenuBar *menuBar = this->menuBar();

    QMenu *fileMenu = menuBar->addMenu("文件(&F)");
    fileMenu->addAction("导出配置(&E)", this, &MainWindow::onExportConfig);
    fileMenu->addAction("导入配置(&I)", this, &MainWindow::onImportConfig);
    fileMenu->addSeparator();
    fileMenu->addAction("退出(&Q)", this, &QWidget::close);

    // Theme menu
    QMenu *themeMenu = menuBar->addMenu("主题(&T)");
    m_themeCombo = new QComboBox(this);
    m_themeCombo->addItems({"默认浅色", "深色", "蓝色", "绿色"});
    m_themeCombo->setFixedWidth(100);
    connect(m_themeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onThemeChanged);
    // Add combobox as a widget action in the menu
    QWidgetAction *wa = new QWidgetAction(this);
    wa->setDefaultWidget(m_themeCombo);
    themeMenu->addAction(wa);
}

void MainWindow::onRefreshPorts() { m_portCombo->clear(); m_portCombo->addItems(m_serialManager->availablePorts()); }

void MainWindow::onOpenPort()
{
    if (m_portCombo->currentText().isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择串口号");
        return;
    }

    QSerialPort::DataBits dataBits;
    switch (m_dataBitsCombo->currentIndex()) {
    case 0: dataBits = QSerialPort::Data5; break;
    case 1: dataBits = QSerialPort::Data6; break;
    case 2: dataBits = QSerialPort::Data7; break;
    default: dataBits = QSerialPort::Data8; break;
    }

    QSerialPort::StopBits stopBits;
    switch (m_stopBitsCombo->currentIndex()) {
    case 0: stopBits = QSerialPort::OneStop; break;
    case 1: stopBits = QSerialPort::OneAndHalfStop; break;
    default: stopBits = QSerialPort::TwoStop; break;
    }

    QSerialPort::Parity parity;
    switch (m_parityCombo->currentIndex()) {
    case 0: parity = QSerialPort::NoParity; break;
    case 1: parity = QSerialPort::OddParity; break;
    case 2: parity = QSerialPort::EvenParity; break;
    case 3: parity = QSerialPort::MarkParity; break;
    default: parity = QSerialPort::SpaceParity; break;
    }

    QSerialPort::FlowControl flowControl;
    switch (m_flowControlCombo->currentIndex()) {
    case 0: flowControl = QSerialPort::NoFlowControl; break;
    case 1: flowControl = QSerialPort::HardwareControl; break;
    default: flowControl = QSerialPort::SoftwareControl; break;
    }

    m_serialManager->openPort(m_portCombo->currentText(), m_baudRateCombo->currentText().toInt(),
                               dataBits, stopBits, parity, flowControl);
}

void MainWindow::onClosePort() { m_serialManager->closePort(); }

void MainWindow::onSendData()
{
    QString text = m_sendEdit->text().trimmed();
    if (text.isEmpty()) return;
    if (m_sendModeCombo->currentIndex() == 0)
        m_serialManager->sendAscii(text);
    else
        m_serialManager->sendHex(text);
}

void MainWindow::onClearLog() { m_logText->clear(); }

void MainWindow::onSerialDataReceived(const QByteArray &data)
{
    QString timestamp = QDateTime::currentDateTime().toString("[HH:mm:ss.zzz] ");
    QString display;
    if (m_sendModeCombo->currentIndex() == 1)
        display = timestamp + "[RX] " + SerialManager::bytesToHexString(data) + "\n";
    else
        display = timestamp + "[RX] " + QString::fromUtf8(data) + "\n";
    m_recvBuffer.append(display.toUtf8());
}

void MainWindow::onSerialDataSent(const QByteArray &data)
{
    if (m_logPaused) return;
    QString timestamp = QDateTime::currentDateTime().toString("[HH:mm:ss.zzz] ");
    QString display;
    if (m_sendModeCombo->currentIndex() == 1)
        display = timestamp + "[TX] " + SerialManager::bytesToHexString(data) + "\n";
    else
        display = timestamp + "[TX] " + QString::fromUtf8(data) + "\n";
    m_logText->moveCursor(QTextCursor::End);
    m_logText->insertPlainText(display);
}

void MainWindow::onSerialError(const QString &errorMsg)
{
    statusBar()->showMessage("错误: " + errorMsg, 5000);
    m_logText->moveCursor(QTextCursor::End);
    m_logText->insertPlainText("[ERROR] " + errorMsg + "\n");
}

void MainWindow::onPortOpened()
{
    updatePortStatus(true);
    statusBar()->showMessage("串口已打开", 3000);
    m_logText->moveCursor(QTextCursor::End);
    m_logText->insertPlainText("[INFO] 串口已打开: " + m_portCombo->currentText() + "\n");
}

void MainWindow::onPortClosed()
{
    updatePortStatus(false);
    statusBar()->showMessage("串口已关闭", 3000);
    m_logText->moveCursor(QTextCursor::End);
    m_logText->insertPlainText("[INFO] 串口已关闭\n");
}

void MainWindow::updatePortStatus(bool open)
{
    m_openBtn->setEnabled(!open);
    m_closeBtn->setEnabled(open);
    m_portCombo->setEnabled(!open);
    m_baudRateCombo->setEnabled(!open);
    m_dataBitsCombo->setEnabled(!open);
    m_stopBitsCombo->setEnabled(!open);
    m_parityCombo->setEnabled(!open);
    m_flowControlCombo->setEnabled(!open);
}

void MainWindow::onExportConfig()
{
    QString filePath = QFileDialog::getSaveFileName(this, "导出配置", "", "JSON Files (*.json)");
    if (filePath.isEmpty()) return;
    if (m_configManager->saveToFile(filePath, m_dashboard->exportConfig()))
        statusBar()->showMessage("配置已导出", 3000);
}

void MainWindow::onImportConfig()
{
    QString filePath = QFileDialog::getOpenFileName(this, "导入配置", "", "JSON Files (*.json)");
    if (filePath.isEmpty()) return;
    QJsonArray config = m_configManager->loadFromFile(filePath);
    if (config.isEmpty()) return;
    m_dashboard->importConfig(config);
    statusBar()->showMessage("配置已导入", 3000);
}

void MainWindow::onThemeChanged(int index) { applyTheme(index); }

void MainWindow::applyTheme(int index)
{
    if (index >= 0 && index < 4)
        qobject_cast<QApplication *>(qApp)->setStyleSheet(THEMES[index]);
}

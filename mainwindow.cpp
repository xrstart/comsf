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

// Theme 0: 浅色 (Light)
static const char *THEME_LIGHT = R"(
    * { font-family: "Microsoft YaHei UI", "Segoe UI", sans-serif; font-size: 13px; color: #1f2937; }
    QMainWindow, QWidget { background-color: #f5f5f5; }
    QMenuBar { background-color: #ffffff; border-bottom: 1px solid #e5e7eb; padding: 0 4px; spacing: 2px; }
    QMenuBar::item { padding: 6px 12px; border-radius: 4px; color: #374151; }
    QMenuBar::item:selected { background-color: #f3f4f6; color: #111827; }
    QMenu { background-color: #ffffff; border: 1px solid #e5e7eb; border-radius: 6px; padding: 4px 0; }
    QMenu::item { padding: 6px 24px 6px 12px; color: #374151; }
    QMenu::item:selected { background-color: #eff6ff; color: #2563eb; }
    QMenu::separator { height: 1px; background-color: #e5e7eb; margin: 4px 8px; }
    QGroupBox { background-color: #ffffff; border: 1px solid #e5e7eb; border-radius: 6px; margin-top: 10px; padding: 16px 12px 12px 12px; font-weight: 600; }
    QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; left: 12px; top: 2px; padding: 0 6px; color: #6b7280; font-size: 12px; font-weight: 500; }
    QLineEdit, QTextEdit, QPlainTextEdit { background-color: #ffffff; border: 1px solid #d1d5db; border-radius: 4px; padding: 4px 8px; color: #1f2937; selection-background-color: #2563eb; selection-color: #ffffff; }
    QComboBox { background-color: #ffffff; border: 1px solid #d1d5db; border-radius: 4px; padding: 2px 8px; color: #1f2937; min-height: 22px; selection-background-color: #2563eb; selection-color: #ffffff; }
    QComboBox:focus, QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus { border-color: #2563eb; }
    QComboBox:hover, QLineEdit:hover, QTextEdit:hover { border-color: #9ca3af; }
    QComboBox::drop-down { border: none; width: 24px; padding: 0px; }
    QComboBox::down-arrow { image: none; border-left: 4px solid transparent; border-right: 4px solid transparent; border-top: 5px solid #6b7280; margin-right: 8px; }
    QComboBox QAbstractItemView { background-color: #ffffff; border: 1px solid #e5e7eb; border-radius: 4px; padding: 2px; outline: none; }
    QComboBox QAbstractItemView::item { padding: 5px 8px; border-radius: 3px; min-height: 20px; }
    QComboBox QAbstractItemView::item:selected { background-color: #eff6ff; color: #2563eb; }
    QPushButton { background-color: #111827; color: #ffffff; border: none; border-radius: 4px; padding: 4px 14px; min-height: 24px; font-weight: 500; }
    QPushButton:hover { background-color: #1f2937; }
    QPushButton:pressed { background-color: #000000; }
    QPushButton:disabled { background-color: #e5e7eb; color: #9ca3af; }
    QCheckBox { spacing: 6px; color: #374151; }
    QCheckBox::indicator { width: 16px; height: 16px; border: 1.5px solid #d1d5db; border-radius: 3px; background-color: #ffffff; }
    QCheckBox::indicator:hover { border-color: #2563eb; }
    QCheckBox::indicator:checked { background-color: #2563eb; border-color: #2563eb; }
    QCheckBox::indicator:disabled { background-color: #f3f4f6; border-color: #e5e7eb; }
    QLabel { color: #374151; background: transparent; }
    QSplitter::handle { background-color: #e5e7eb; }
    QSplitter::handle:horizontal { width: 1px; }
    QSplitter::handle:vertical { height: 1px; }
    QScrollBar:vertical { background: transparent; width: 8px; margin: 0; }
    QScrollBar::handle:vertical { background: #d1d5db; border-radius: 4px; min-height: 32px; }
    QScrollBar::handle:vertical:hover { background: #9ca3af; }
    QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
    QScrollBar:horizontal { background: transparent; height: 8px; margin: 0; }
    QScrollBar::handle:horizontal { background: #d1d5db; border-radius: 4px; min-width: 32px; }
    QScrollBar::handle:horizontal:hover { background: #9ca3af; }
    QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }
    QScrollArea { border: none; background-color: transparent; }
    QStatusBar { background-color: #ffffff; border-top: 1px solid #e5e7eb; color: #6b7280; font-size: 12px; padding: 0 8px; }
    QToolTip { background-color: #111827; color: #ffffff; border: none; border-radius: 4px; padding: 4px 8px; font-size: 12px; }
    QDialog { background-color: #f5f5f5; }
)";

// Theme 1: 深色 (Dark)
static const char *THEME_DARK = R"(
    * { font-family: "Microsoft YaHei UI", "Segoe UI", sans-serif; font-size: 13px; color: #e5e7eb; }
    QMainWindow, QWidget { background-color: #111827; }
    QMenuBar { background-color: #1f2937; border-bottom: 1px solid #374151; padding: 0 4px; spacing: 2px; }
    QMenuBar::item { padding: 6px 12px; border-radius: 4px; color: #d1d5db; }
    QMenuBar::item:selected { background-color: #374151; color: #f9fafb; }
    QMenu { background-color: #1f2937; border: 1px solid #374151; border-radius: 6px; padding: 4px 0; }
    QMenu::item { padding: 6px 24px 6px 12px; color: #d1d5db; }
    QMenu::item:selected { background-color: #1e3a5f; color: #60a5fa; }
    QMenu::separator { height: 1px; background-color: #374151; margin: 4px 8px; }
    QGroupBox { background-color: #1f2937; border: 1px solid #374151; border-radius: 6px; margin-top: 10px; padding: 16px 12px 12px 12px; font-weight: 600; }
    QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; left: 12px; top: 2px; padding: 0 6px; color: #9ca3af; font-size: 12px; font-weight: 500; }
    QLineEdit, QTextEdit, QPlainTextEdit { background-color: #111827; border: 1px solid #4b5563; border-radius: 4px; padding: 4px 8px; color: #e5e7eb; selection-background-color: #2563eb; selection-color: #ffffff; }
    QComboBox { background-color: #111827; border: 1px solid #4b5563; border-radius: 4px; padding: 2px 8px; color: #e5e7eb; min-height: 22px; selection-background-color: #2563eb; selection-color: #ffffff; }
    QComboBox:focus, QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus { border-color: #60a5fa; }
    QComboBox:hover, QLineEdit:hover, QTextEdit:hover { border-color: #6b7280; }
    QComboBox::drop-down { border: none; width: 24px; padding: 0px; }
    QComboBox::down-arrow { image: none; border-left: 4px solid transparent; border-right: 4px solid transparent; border-top: 5px solid #9ca3af; margin-right: 8px; }
    QComboBox QAbstractItemView { background-color: #1f2937; border: 1px solid #4b5563; border-radius: 4px; padding: 2px; outline: none; color: #e5e7eb; }
    QComboBox QAbstractItemView::item { padding: 5px 8px; border-radius: 3px; min-height: 20px; color: #e5e7eb; }
    QComboBox QAbstractItemView::item:selected { background-color: #1e3a5f; color: #60a5fa; }
    QPushButton { background-color: #e5e7eb; color: #111827; border: none; border-radius: 4px; padding: 4px 14px; min-height: 24px; font-weight: 500; }
    QPushButton:hover { background-color: #f3f4f6; }
    QPushButton:pressed { background-color: #d1d5db; }
    QPushButton:disabled { background-color: #374151; color: #6b7280; }
    QCheckBox { spacing: 6px; color: #d1d5db; }
    QCheckBox::indicator { width: 16px; height: 16px; border: 1.5px solid #4b5563; border-radius: 3px; background-color: #111827; }
    QCheckBox::indicator:hover { border-color: #60a5fa; }
    QCheckBox::indicator:checked { background-color: #2563eb; border-color: #2563eb; }
    QCheckBox::indicator:disabled { background-color: #1f2937; border-color: #374151; }
    QLabel { color: #d1d5db; background: transparent; }
    QSplitter::handle { background-color: #374151; }
    QSplitter::handle:horizontal { width: 1px; }
    QSplitter::handle:vertical { height: 1px; }
    QScrollBar:vertical { background: transparent; width: 8px; margin: 0; }
    QScrollBar::handle:vertical { background: #4b5563; border-radius: 4px; min-height: 32px; }
    QScrollBar::handle:vertical:hover { background: #6b7280; }
    QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
    QScrollBar:horizontal { background: transparent; height: 8px; margin: 0; }
    QScrollBar::handle:horizontal { background: #4b5563; border-radius: 4px; min-width: 32px; }
    QScrollBar::handle:horizontal:hover { background: #6b7280; }
    QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }
    QScrollArea { border: none; background-color: transparent; }
    QStatusBar { background-color: #1f2937; border-top: 1px solid #374151; color: #9ca3af; font-size: 12px; padding: 0 8px; }
    QToolTip { background-color: #f9fafb; color: #111827; border: none; border-radius: 4px; padding: 4px 8px; font-size: 12px; }
    QDialog { background-color: #111827; }
)";

// Theme 2: 蓝色 (Blue)
static const char *THEME_BLUE = R"(
    * { font-family: "Microsoft YaHei UI", "Segoe UI", sans-serif; font-size: 13px; color: #1e293b; }
    QMainWindow, QWidget { background-color: #f0f4ff; }
    QMenuBar { background-color: #ffffff; border-bottom: 1px solid #c7d2fe; padding: 0 4px; spacing: 2px; }
    QMenuBar::item { padding: 6px 12px; border-radius: 4px; color: #334155; }
    QMenuBar::item:selected { background-color: #e0e7ff; color: #1e3a8a; }
    QMenu { background-color: #ffffff; border: 1px solid #c7d2fe; border-radius: 6px; padding: 4px 0; }
    QMenu::item { padding: 6px 24px 6px 12px; color: #334155; }
    QMenu::item:selected { background-color: #dbeafe; color: #1d4ed8; }
    QMenu::separator { height: 1px; background-color: #c7d2fe; margin: 4px 8px; }
    QGroupBox { background-color: #ffffff; border: 1px solid #c7d2fe; border-radius: 6px; margin-top: 10px; padding: 16px 12px 12px 12px; font-weight: 600; }
    QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; left: 12px; top: 2px; padding: 0 6px; color: #6366f1; font-size: 12px; font-weight: 500; }
    QLineEdit, QTextEdit, QPlainTextEdit { background-color: #ffffff; border: 1px solid #c7d2fe; border-radius: 4px; padding: 4px 8px; color: #1e293b; selection-background-color: #4f46e5; selection-color: #ffffff; }
    QComboBox { background-color: #ffffff; border: 1px solid #c7d2fe; border-radius: 4px; padding: 2px 8px; color: #1e293b; min-height: 22px; selection-background-color: #4f46e5; selection-color: #ffffff; }
    QComboBox:focus, QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus { border-color: #4f46e5; }
    QComboBox:hover, QLineEdit:hover, QTextEdit:hover { border-color: #a5b4fc; }
    QComboBox::drop-down { border: none; width: 24px; padding: 0px; }
    QComboBox::down-arrow { image: none; border-left: 4px solid transparent; border-right: 4px solid transparent; border-top: 5px solid #6366f1; margin-right: 8px; }
    QComboBox QAbstractItemView { background-color: #ffffff; border: 1px solid #c7d2fe; border-radius: 4px; padding: 2px; outline: none; }
    QComboBox QAbstractItemView::item { padding: 5px 8px; border-radius: 3px; min-height: 20px; }
    QComboBox QAbstractItemView::item:selected { background-color: #dbeafe; color: #1d4ed8; }
    QPushButton { background-color: #4f46e5; color: #ffffff; border: none; border-radius: 4px; padding: 4px 14px; min-height: 24px; font-weight: 500; }
    QPushButton:hover { background-color: #4338ca; }
    QPushButton:pressed { background-color: #3730a3; }
    QPushButton:disabled { background-color: #c7d2fe; color: #a5b4fc; }
    QCheckBox { spacing: 6px; color: #334155; }
    QCheckBox::indicator { width: 16px; height: 16px; border: 1.5px solid #c7d2fe; border-radius: 3px; background-color: #ffffff; }
    QCheckBox::indicator:hover { border-color: #4f46e5; }
    QCheckBox::indicator:checked { background-color: #4f46e5; border-color: #4f46e5; }
    QCheckBox::indicator:disabled { background-color: #e0e7ff; border-color: #c7d2fe; }
    QLabel { color: #334155; background: transparent; }
    QSplitter::handle { background-color: #c7d2fe; }
    QSplitter::handle:horizontal { width: 1px; }
    QSplitter::handle:vertical { height: 1px; }
    QScrollBar:vertical { background: transparent; width: 8px; margin: 0; }
    QScrollBar::handle:vertical { background: #a5b4fc; border-radius: 4px; min-height: 32px; }
    QScrollBar::handle:vertical:hover { background: #818cf8; }
    QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
    QScrollBar:horizontal { background: transparent; height: 8px; margin: 0; }
    QScrollBar::handle:horizontal { background: #a5b4fc; border-radius: 4px; min-width: 32px; }
    QScrollBar::handle:horizontal:hover { background: #818cf8; }
    QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }
    QScrollArea { border: none; background-color: transparent; }
    QStatusBar { background-color: #ffffff; border-top: 1px solid #c7d2fe; color: #6366f1; font-size: 12px; padding: 0 8px; }
    QToolTip { background-color: #1e3a8a; color: #ffffff; border: none; border-radius: 4px; padding: 4px 8px; font-size: 12px; }
    QDialog { background-color: #f0f4ff; }
)";

// Theme 3: 绿色 (Green)
static const char *THEME_GREEN = R"(
    * { font-family: "Microsoft YaHei UI", "Segoe UI", sans-serif; font-size: 13px; color: #1a2e1a; }
    QMainWindow, QWidget { background-color: #f0faf0; }
    QMenuBar { background-color: #ffffff; border-bottom: 1px solid #a7f3d0; padding: 0 4px; spacing: 2px; }
    QMenuBar::item { padding: 6px 12px; border-radius: 4px; color: #166534; }
    QMenuBar::item:selected { background-color: #d1fae5; color: #14532d; }
    QMenu { background-color: #ffffff; border: 1px solid #a7f3d0; border-radius: 6px; padding: 4px 0; }
    QMenu::item { padding: 6px 24px 6px 12px; color: #166534; }
    QMenu::item:selected { background-color: #dcfce7; color: #15803d; }
    QMenu::separator { height: 1px; background-color: #a7f3d0; margin: 4px 8px; }
    QGroupBox { background-color: #ffffff; border: 1px solid #a7f3d0; border-radius: 6px; margin-top: 10px; padding: 16px 12px 12px 12px; font-weight: 600; }
    QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; left: 12px; top: 2px; padding: 0 6px; color: #059669; font-size: 12px; font-weight: 500; }
    QLineEdit, QTextEdit, QPlainTextEdit { background-color: #ffffff; border: 1px solid #a7f3d0; border-radius: 4px; padding: 4px 8px; color: #1a2e1a; selection-background-color: #059669; selection-color: #ffffff; }
    QComboBox { background-color: #ffffff; border: 1px solid #a7f3d0; border-radius: 4px; padding: 2px 8px; color: #1a2e1a; min-height: 22px; selection-background-color: #059669; selection-color: #ffffff; }
    QComboBox:focus, QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus { border-color: #059669; }
    QComboBox:hover, QLineEdit:hover, QTextEdit:hover { border-color: #6ee7b7; }
    QComboBox::drop-down { border: none; width: 24px; padding: 0px; }
    QComboBox::down-arrow { image: none; border-left: 4px solid transparent; border-right: 4px solid transparent; border-top: 5px solid #059669; margin-right: 8px; }
    QComboBox QAbstractItemView { background-color: #ffffff; border: 1px solid #a7f3d0; border-radius: 4px; padding: 2px; outline: none; }
    QComboBox QAbstractItemView::item { padding: 5px 8px; border-radius: 3px; min-height: 20px; }
    QComboBox QAbstractItemView::item:selected { background-color: #dcfce7; color: #15803d; }
    QPushButton { background-color: #059669; color: #ffffff; border: none; border-radius: 4px; padding: 4px 14px; min-height: 24px; font-weight: 500; }
    QPushButton:hover { background-color: #047857; }
    QPushButton:pressed { background-color: #065f46; }
    QPushButton:disabled { background-color: #a7f3d0; color: #6ee7b7; }
    QCheckBox { spacing: 6px; color: #166534; }
    QCheckBox::indicator { width: 16px; height: 16px; border: 1.5px solid #a7f3d0; border-radius: 3px; background-color: #ffffff; }
    QCheckBox::indicator:hover { border-color: #059669; }
    QCheckBox::indicator:checked { background-color: #059669; border-color: #059669; }
    QCheckBox::indicator:disabled { background-color: #dcfce7; border-color: #a7f3d0; }
    QLabel { color: #166534; background: transparent; }
    QSplitter::handle { background-color: #a7f3d0; }
    QSplitter::handle:horizontal { width: 1px; }
    QSplitter::handle:vertical { height: 1px; }
    QScrollBar:vertical { background: transparent; width: 8px; margin: 0; }
    QScrollBar::handle:vertical { background: #6ee7b7; border-radius: 4px; min-height: 32px; }
    QScrollBar::handle:vertical:hover { background: #34d399; }
    QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
    QScrollBar:horizontal { background: transparent; height: 8px; margin: 0; }
    QScrollBar::handle:horizontal { background: #6ee7b7; border-radius: 4px; min-width: 32px; }
    QScrollBar::handle:horizontal:hover { background: #34d399; }
    QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }
    QScrollArea { border: none; background-color: transparent; }
    QStatusBar { background-color: #ffffff; border-top: 1px solid #a7f3d0; color: #059669; font-size: 12px; padding: 0 8px; }
    QToolTip { background-color: #14532d; color: #ffffff; border: none; border-radius: 4px; padding: 4px 8px; font-size: 12px; }
    QDialog { background-color: #f0faf0; }
)";

static const char *THEMES[] = { THEME_LIGHT, THEME_DARK, THEME_BLUE, THEME_GREEN };
static const int THEME_COUNT = 4;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_serialManager(new SerialManager(this))
    , m_configManager(new ConfigManager(this))
    , m_logPaused(false)
{
    qobject_cast<QApplication *>(qApp)->setStyleSheet(THEMES[0]);
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
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(12);

    // === Left panel (serial config card) ===
    QGroupBox *configGroup = new QGroupBox("串口配置", this);
    configGroup->setFixedWidth(240);
    QFormLayout *configLayout = new QFormLayout(configGroup);
    configLayout->setLabelAlignment(Qt::AlignLeft);
    configLayout->setFormAlignment(Qt::AlignTop | Qt::AlignLeft);
    configLayout->setHorizontalSpacing(8);
    configLayout->setVerticalSpacing(6);
    configLayout->setContentsMargins(10, 18, 10, 10);

    setupSerialConfigPanel();
    configLayout->addRow("端口", m_portCombo);
    configLayout->addRow("波特率", m_baudRateCombo);
    configLayout->addRow("数据位", m_dataBitsCombo);
    configLayout->addRow("停止位", m_stopBitsCombo);
    configLayout->addRow("校验位", m_parityCombo);
    configLayout->addRow("流控", m_flowControlCombo);

    QVBoxLayout *portBtnLayout = new QVBoxLayout();
    portBtnLayout->setSpacing(4);
    portBtnLayout->addWidget(m_refreshBtn);
    QHBoxLayout *openCloseLayout = new QHBoxLayout();
    openCloseLayout->setSpacing(6);
    openCloseLayout->addWidget(m_openBtn);
    openCloseLayout->addWidget(m_closeBtn);
    portBtnLayout->addLayout(openCloseLayout);
    configLayout->addRow("", portBtnLayout);

    // Send area with visual separator
    QFrame *separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Plain);
    separator->setStyleSheet("background-color: #e5e7eb; max-height: 1px; margin: 8px 0;");
    configLayout->addRow(separator);

    configLayout->addRow("模式", m_sendModeCombo);
    configLayout->addRow("数据", m_sendEdit);
    configLayout->addRow("", m_sendBtn);

    // === Right panel ===
    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(12);

    QGroupBox *logGroup = new QGroupBox("原始数据日志", this);
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    logLayout->setContentsMargins(8, 12, 8, 8);
    logLayout->setSpacing(8);
    m_logText = new QTextEdit(this);
    m_logText->setReadOnly(true);
    m_logText->setFont(QFont("Cascadia Code", 9));
    logLayout->addWidget(m_logText);

    QHBoxLayout *logControlLayout = new QHBoxLayout();
    logControlLayout->setSpacing(8);
    m_pauseLogCheck = new QCheckBox("暂停显示", this);
    m_clearLogBtn = new QPushButton("清空", this);
    m_clearLogBtn->setStyleSheet(
        "QPushButton { background-color: transparent; color: #6b7280; border: 1px solid #d1d5db; "
        "border-radius: 4px; padding: 4px 12px; font-size: 12px; }"
        "QPushButton:hover { background-color: #f3f4f6; color: #374151; border-color: #9ca3af; }");
    connect(m_clearLogBtn, &QPushButton::clicked, this, &MainWindow::onClearLog);
    connect(m_pauseLogCheck, &QCheckBox::toggled, this, [this](bool checked) { m_logPaused = checked; });
    logControlLayout->addWidget(m_pauseLogCheck);
    logControlLayout->addStretch();
    logControlLayout->addWidget(m_clearLogBtn);
    logLayout->addLayout(logControlLayout);

    QGroupBox *dashboardGroup = new QGroupBox("动态仪表盘", this);
    QVBoxLayout *dashLayout = new QVBoxLayout(dashboardGroup);
    dashLayout->setContentsMargins(0, 12, 0, 0);
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
    splitter->setHandleWidth(1);

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

    // Ghost button style
    static const char *ghostBtnStyle =
        "QPushButton { background-color: transparent; color: #374151; border: 1px solid #d1d5db; "
        "border-radius: 4px; padding: 4px 8px; min-height: 24px; font-size: 12px; }"
        "QPushButton:hover { background-color: #f3f4f6; border-color: #9ca3af; }"
        "QPushButton:pressed { background-color: #e5e7eb; }"
        "QPushButton:disabled { color: #d1d5db; border-color: #e5e7eb; }";

    m_refreshBtn = new QPushButton("刷新", this);
    m_refreshBtn->setStyleSheet(ghostBtnStyle);
    m_openBtn = new QPushButton("打开", this);
    m_closeBtn = new QPushButton("关闭", this);
    m_closeBtn->setStyleSheet(ghostBtnStyle);
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
    m_themeCombo->addItems({"浅色", "深色", "蓝色", "绿色"});
    m_themeCombo->setFixedWidth(100);
    connect(m_themeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onThemeChanged);
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
    if (index >= 0 && index < THEME_COUNT)
        qobject_cast<QApplication *>(qApp)->setStyleSheet(THEMES[index]);
}

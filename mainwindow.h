#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QTimer>
#include <QByteArray>

class SerialManager;
class DashboardArea;
class ConfigManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onOpenPort();
    void onClosePort();
    void onRefreshPorts();
    void onSendData();
    void onClearLog();
    void onSerialDataReceived(const QByteArray &data);
    void onSerialDataSent(const QByteArray &data);
    void onSerialError(const QString &errorMsg);
    void onPortOpened();
    void onPortClosed();
    void onExportConfig();
    void onImportConfig();
    void onThemeChanged(int index);

private:
    void setupUI();
    void setupSerialConfigPanel();
    void setupMenuBar();
    void updatePortStatus(bool open);
    void applyTheme(int index);

    SerialManager *m_serialManager;
    DashboardArea *m_dashboard;
    ConfigManager *m_configManager;

    // Serial config
    QComboBox *m_portCombo;
    QComboBox *m_baudRateCombo;
    QComboBox *m_dataBitsCombo;
    QComboBox *m_stopBitsCombo;
    QComboBox *m_parityCombo;
    QComboBox *m_flowControlCombo;
    QPushButton *m_openBtn;
    QPushButton *m_closeBtn;
    QPushButton *m_refreshBtn;

    // Send
    QComboBox *m_sendModeCombo;
    QLineEdit *m_sendEdit;
    QPushButton *m_sendBtn;

    // Log
    QTextEdit *m_logText;
    QCheckBox *m_pauseLogCheck;
    QPushButton *m_clearLogBtn;
    QLabel *m_statusLabel;

    // Theme
    QComboBox *m_themeCombo;

    QByteArray m_recvBuffer;
    QTimer *m_logFlushTimer;
    bool m_logPaused;
};

#endif // MAINWINDOW_H

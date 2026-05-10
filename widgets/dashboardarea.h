#ifndef DASHBOARDAREA_H
#define DASHBOARDAREA_H

#include <QScrollArea>
#include <QContextMenuEvent>
#include <QMenu>
#include <QList>
#include <QJsonArray>
#include <QJsonObject>

class QPushButton;
class GridCanvas;
class SerialManager;
class CustomButton;
class DataDisplay;
class LogicIndicator;

class DashboardArea : public QScrollArea
{
    Q_OBJECT

public:
    explicit DashboardArea(SerialManager *serialManager, QWidget *parent = nullptr);

    CustomButton *addButton(const QString &name, const QString &message, bool hexMode,
                             int x = -1, int y = -1, int w = -1, int h = -1, int styleIdx = 0);
    DataDisplay *addDisplay(const QString &label, const QString &formatStr,
                             int x = -1, int y = -1, int w = -1, int h = -1, int styleIdx = 0);
    LogicIndicator *addIndicator(const QString &name, const QString &expression,
                                  const QString &formatStr,
                                  const QString &trueColor, const QString &falseColor,
                                  int x = -1, int y = -1, int w = -1, int h = -1, int styleIdx = 0);

    void clearAll();
    QJsonArray exportConfig() const;
    void importConfig(const QJsonArray &config);
    int widgetCount() const;
    bool isEditMode() const;

signals:
    void configSaved(const QJsonArray &config);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void onAddButton();
    void onAddDisplay();
    void onAddIndicator();
    void onWidgetContextMenu(const QPoint &pos);
    void onSerialDataReceived(const QByteArray &data);
    void onEditClicked();
    void onSaveClicked();

private:
    QPoint nextPosition() const;

    SerialManager *m_serialManager;
    GridCanvas *m_canvas;
    QPushButton *m_editBtn;
    QPushButton *m_saveBtn;

    struct WidgetInfo {
        enum Type { Button, Display, Indicator } type;
        QWidget *widget;
    };

    QList<WidgetInfo> m_widgets;
};

#endif // DASHBOARDAREA_H

#include "dashboardarea.h"
#include "gridcanvas.h"
#include "custombutton.h"
#include "datadisplay.h"
#include "logicindicator.h"
#include "serialmanager.h"
#include "dialogs/buttonconfigdialog.h"
#include "dialogs/displayconfigdialog.h"
#include "dialogs/indicatorconfigdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

DashboardArea::DashboardArea(SerialManager *serialManager, QWidget *parent)
    : QScrollArea(parent)
    , m_serialManager(serialManager)
{
    setWidgetResizable(true);
    setFrameStyle(QFrame::NoFrame);

    // Main container with toolbar + canvas
    QWidget *container = new QWidget(this);
    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(0);

    // Toolbar
    QWidget *toolbar = new QWidget(container);
    toolbar->setFixedHeight(36);
    toolbar->setStyleSheet("background-color: #fafafa; border-bottom: 1px solid #e5e7eb;");
    QHBoxLayout *toolbarLayout = new QHBoxLayout(toolbar);
    toolbarLayout->setContentsMargins(10, 4, 10, 4);
    toolbarLayout->setSpacing(8);

    m_editBtn = new QPushButton("编辑", toolbar);
    m_editBtn->setFixedHeight(26);
    m_editBtn->setStyleSheet(
        "QPushButton { background-color: transparent; color: #374151; border: 1px solid #d1d5db; "
        "border-radius: 4px; padding: 0 14px; font-size: 12px; }"
        "QPushButton:hover { background-color: #f3f4f6; border-color: #9ca3af; }"
        "QPushButton:pressed { background-color: #e5e7eb; }");
    connect(m_editBtn, &QPushButton::clicked, this, &DashboardArea::onEditClicked);

    m_saveBtn = new QPushButton("保存", toolbar);
    m_saveBtn->setFixedHeight(26);
    m_saveBtn->setEnabled(false);
    m_saveBtn->setStyleSheet(
        "QPushButton { background-color: #111827; color: #ffffff; border: none; "
        "border-radius: 4px; padding: 0 14px; font-size: 12px; }"
        "QPushButton:hover { background-color: #1f2937; }"
        "QPushButton:pressed { background-color: #000000; }"
        "QPushButton:disabled { background-color: #e5e7eb; color: #9ca3af; }");
    connect(m_saveBtn, &QPushButton::clicked, this, &DashboardArea::onSaveClicked);

    toolbarLayout->addWidget(m_editBtn);
    toolbarLayout->addWidget(m_saveBtn);
    toolbarLayout->addStretch();

    containerLayout->addWidget(toolbar);

    m_canvas = new GridCanvas(container);
    containerLayout->addWidget(m_canvas, 1);

    setWidget(container);

    connect(m_serialManager, &SerialManager::dataReceived,
            this, &DashboardArea::onSerialDataReceived);
}

CustomButton *DashboardArea::addButton(const QString &name, const QString &message,
                                        bool hexMode, int x, int y, int w, int h, int styleIdx)
{
    QPoint pos = (x < 0) ? nextPosition() : QPoint(x, y);
    QSize size = (w < 0) ? QSize(120, 40) : QSize(w, h);

    CustomButton *btn = new CustomButton(name, message, hexMode, m_serialManager, m_canvas);
    btn->setGeometry(QRect(pos, size));
    btn->setStyle(styleIdx);
    btn->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(btn, &QWidget::customContextMenuRequested, this, &DashboardArea::onWidgetContextMenu);
    m_canvas->childWasAdded(btn);
    m_widgets.append({WidgetInfo::Button, btn});
    return btn;
}

DataDisplay *DashboardArea::addDisplay(const QString &label, const QString &formatStr,
                                        int x, int y, int w, int h, int styleIdx)
{
    QPoint pos = (x < 0) ? nextPosition() : QPoint(x, y);
    QSize size = (w < 0) ? QSize(160, 80) : QSize(w, h);

    DataDisplay *disp = new DataDisplay(label, formatStr, m_canvas);
    disp->setGeometry(QRect(pos, size));
    disp->setStyle(styleIdx);
    disp->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(disp, &QWidget::customContextMenuRequested, this, &DashboardArea::onWidgetContextMenu);
    m_canvas->childWasAdded(disp);
    m_widgets.append({WidgetInfo::Display, disp});
    return disp;
}

LogicIndicator *DashboardArea::addIndicator(const QString &name, const QString &expression,
                                             const QString &formatStr,
                                             const QString &trueColor, const QString &falseColor,
                                             int x, int y, int w, int h, int styleIdx)
{
    QPoint pos = (x < 0) ? nextPosition() : QPoint(x, y);
    QSize size = (w < 0) ? QSize(120, 80) : QSize(w, h);

    LogicIndicator *ind = new LogicIndicator(name, expression, formatStr,
                                              trueColor, falseColor, m_canvas);
    ind->setGeometry(QRect(pos, size));
    ind->setStyle(styleIdx);
    ind->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ind, &QWidget::customContextMenuRequested, this, &DashboardArea::onWidgetContextMenu);
    m_canvas->childWasAdded(ind);
    m_widgets.append({WidgetInfo::Indicator, ind});
    return ind;
}

void DashboardArea::clearAll()
{
    for (const WidgetInfo &info : m_widgets)
        info.widget->deleteLater();
    m_widgets.clear();
}

QJsonArray DashboardArea::exportConfig() const
{
    QJsonArray arr;
    for (const WidgetInfo &info : m_widgets) {
        QJsonObject obj;
        QRect g = info.widget->geometry();
        obj["x"] = g.x(); obj["y"] = g.y();
        obj["w"] = g.width(); obj["h"] = g.height();

        switch (info.type) {
        case WidgetInfo::Button: {
            CustomButton *btn = qobject_cast<CustomButton *>(info.widget);
            obj["type"] = "button";
            obj["name"] = btn->buttonName();
            obj["message"] = btn->message();
            obj["hexMode"] = btn->hexMode();
            obj["style"] = btn->styleIndex();
            break;
        }
        case WidgetInfo::Display: {
            DataDisplay *disp = qobject_cast<DataDisplay *>(info.widget);
            obj["type"] = "display";
            obj["label"] = disp->displayLabel();
            obj["formatStr"] = disp->formatString();
            obj["style"] = disp->styleIndex();
            break;
        }
        case WidgetInfo::Indicator: {
            LogicIndicator *ind = qobject_cast<LogicIndicator *>(info.widget);
            obj["type"] = "indicator";
            obj["name"] = ind->indicatorName();
            obj["expression"] = ind->expression();
            obj["formatStr"] = ind->formatString();
            obj["trueColor"] = ind->trueColor();
            obj["falseColor"] = ind->falseColor();
            obj["style"] = ind->styleIndex();
            break;
        }
        }
        arr.append(obj);
    }
    return arr;
}

void DashboardArea::importConfig(const QJsonArray &config)
{
    clearAll();
    for (const QJsonValue &val : config) {
        QJsonObject obj = val.toObject();
        QString type = obj["type"].toString();
        int x = obj["x"].toInt(), y = obj["y"].toInt();
        int w = obj["w"].toInt(), h = obj["h"].toInt();
        int style = obj["style"].toInt(0);

        if (type == "button") {
            addButton(obj["name"].toString(), obj["message"].toString(),
                      obj["hexMode"].toBool(), x, y, w, h, style);
        } else if (type == "display") {
            addDisplay(obj["label"].toString(), obj["formatStr"].toString(), x, y, w, h, style);
        } else if (type == "indicator") {
            addIndicator(obj["name"].toString(), obj["expression"].toString(),
                         obj["formatStr"].toString(),
                         obj["trueColor"].toString(), obj["falseColor"].toString(),
                         x, y, w, h, style);
        }
    }
}

int DashboardArea::widgetCount() const { return m_widgets.size(); }
bool DashboardArea::isEditMode() const { return m_canvas->isEditMode(); }

void DashboardArea::contextMenuEvent(QContextMenuEvent *event)
{
    if (!m_canvas->isEditMode())
        return; // Only allow adding widgets in edit mode

    QMenu menu(this);
    menu.addAction("添加发送按钮", this, &DashboardArea::onAddButton);
    menu.addAction("添加数据显示框", this, &DashboardArea::onAddDisplay);
    menu.addAction("添加逻辑信号灯", this, &DashboardArea::onAddIndicator);
    menu.exec(event->globalPos());
}

void DashboardArea::onEditClicked()
{
    m_canvas->setEditMode(true);
    m_editBtn->setEnabled(false);
    m_saveBtn->setEnabled(true);
}

void DashboardArea::onSaveClicked()
{
    m_canvas->setEditMode(false);
    m_editBtn->setEnabled(true);
    m_saveBtn->setEnabled(false);
    emit configSaved(exportConfig());
}

void DashboardArea::onAddButton()
{
    ButtonConfigDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted)
        addButton(dlg.name(), dlg.message(), dlg.hexMode(), -1, -1, -1, -1, dlg.styleIndex());
}

void DashboardArea::onAddDisplay()
{
    DisplayConfigDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted)
        addDisplay(dlg.label(), dlg.formatString(), -1, -1, -1, -1, dlg.styleIndex());
}

void DashboardArea::onAddIndicator()
{
    IndicatorConfigDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted)
        addIndicator(dlg.name(), dlg.expression(), dlg.formatString(),
                     dlg.trueColor(), dlg.falseColor(), -1, -1, -1, -1, dlg.styleIndex());
}

void DashboardArea::onWidgetContextMenu(const QPoint &pos)
{
    if (!m_canvas->isEditMode())
        return;

    QWidget *sender = qobject_cast<QWidget *>(this->sender());
    if (!sender) return;

    QMenu menu(this);
    QAction *editAction = menu.addAction("编辑配置");
    QAction *deleteAction = menu.addAction("删除控件");

    QAction *selected = menu.exec(sender->mapToGlobal(pos));
    if (!selected) return;

    if (selected == deleteAction) {
        for (int i = 0; i < m_widgets.size(); ++i) {
            if (m_widgets[i].widget == sender) {
                sender->deleteLater();
                m_widgets.removeAt(i);
                break;
            }
        }
    } else if (selected == editAction) {
        for (int i = 0; i < m_widgets.size(); ++i) {
            if (m_widgets[i].widget == sender) {
                switch (m_widgets[i].type) {
                case WidgetInfo::Button: {
                    CustomButton *btn = qobject_cast<CustomButton *>(sender);
                    ButtonConfigDialog dlg(btn->buttonName(), btn->message(),
                                           btn->hexMode(), btn->styleIndex(), this);
                    if (dlg.exec() == QDialog::Accepted) {
                        btn->updateConfig(dlg.name(), dlg.message(), dlg.hexMode());
                        btn->setStyle(dlg.styleIndex());
                    }
                    break;
                }
                case WidgetInfo::Display: {
                    DataDisplay *disp = qobject_cast<DataDisplay *>(sender);
                    DisplayConfigDialog dlg(disp->displayLabel(), disp->formatString(),
                                            disp->styleIndex(), this);
                    if (dlg.exec() == QDialog::Accepted) {
                        disp->updateConfig(dlg.label(), dlg.formatString());
                        disp->setStyle(dlg.styleIndex());
                    }
                    break;
                }
                case WidgetInfo::Indicator: {
                    LogicIndicator *ind = qobject_cast<LogicIndicator *>(sender);
                    IndicatorConfigDialog dlg(ind->indicatorName(), ind->expression(),
                                              ind->formatString(),
                                              ind->trueColor(), ind->falseColor(),
                                              ind->styleIndex(), this);
                    if (dlg.exec() == QDialog::Accepted) {
                        ind->updateConfig(dlg.name(), dlg.expression(), dlg.formatString(),
                                          dlg.trueColor(), dlg.falseColor());
                        ind->setStyle(dlg.styleIndex());
                    }
                    break;
                }
                }
                break;
            }
        }
    }
}

void DashboardArea::onSerialDataReceived(const QByteArray &data)
{
    QString rawData = QString::fromUtf8(data);
    for (const WidgetInfo &info : m_widgets) {
        if (info.type == WidgetInfo::Display) {
            qobject_cast<DataDisplay *>(info.widget)->processData(rawData);
        } else if (info.type == WidgetInfo::Indicator) {
            qobject_cast<LogicIndicator *>(info.widget)->processData(rawData);
        }
    }
}

QPoint DashboardArea::nextPosition() const
{
    if (m_widgets.isEmpty())
        return QPoint(20, 20);
    int maxBottom = 0;
    for (const WidgetInfo &info : m_widgets)
        maxBottom = qMax(maxBottom, info.widget->geometry().bottom());
    return QPoint(20, maxBottom + 20);
}

#ifndef GRIDCANVAS_H
#define GRIDCANVAS_H

#include <QWidget>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QList>
#include <QLine>

class GridCanvas : public QWidget
{
    Q_OBJECT

public:
    explicit GridCanvas(QWidget *parent = nullptr);

    void childWasAdded(QWidget *child);
    void setEditMode(bool editable);
    bool isEditMode() const { return m_editMode; }

    static const int SNAP_THRESHOLD = 8;

protected:
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    enum DragMode { None, Move, ResizeTop, ResizeBottom, ResizeLeft, ResizeRight,
                    ResizeTopLeft, ResizeTopRight, ResizeBottomLeft, ResizeBottomRight };

    struct SnapResult {
        bool snappedX = false;
        bool snappedY = false;
        int snapX = 0;
        int snapY = 0;
        QList<QLine> guides;
    };

    DragMode hitTest(QWidget *w, const QPoint &pos) const;
    void updateCursor(QWidget *w, const QPoint &pos);
    SnapResult calcSnap(QWidget *self, const QRect &geom) const;
    void ensureCanvasSize();

    QWidget *m_dragTarget;
    DragMode m_dragMode;
    QPoint m_dragStartPos;
    QRect m_dragStartGeom;
    bool m_dragging;
    bool m_dragStarted;
    bool m_editMode;
    QList<QLine> m_guideLines;
};

#endif // GRIDCANVAS_H

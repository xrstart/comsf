#include "gridcanvas.h"
#include <QPainter>
#include <QCursor>
#include <cmath>

GridCanvas::GridCanvas(QWidget *parent)
    : QWidget(parent)
    , m_dragTarget(nullptr)
    , m_dragMode(None)
    , m_dragging(false)
    , m_dragStarted(false)
    , m_editMode(false)
{
    setMouseTracking(true);
    setMinimumSize(800, 600);
}

void GridCanvas::childWasAdded(QWidget *child)
{
    child->installEventFilter(this);
    child->setMouseTracking(true);
    child->show();
    if (!m_editMode)
        child->setCursor(Qt::ArrowCursor);
    ensureCanvasSize();
}

void GridCanvas::setEditMode(bool editable)
{
    m_editMode = editable;
    m_dragging = false;
    m_dragStarted = false;
    m_guideLines.clear();
    // Reset cursors for all children
    for (QObject *child : children()) {
        QWidget *w = qobject_cast<QWidget *>(child);
        if (w)
            w->setCursor(editable ? Qt::SizeAllCursor : Qt::ArrowCursor);
    }
    update();
}

void GridCanvas::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    if (!m_editMode)
        return;

    QPainter p(this);

    // Draw subtle grid dots
    p.setPen(QPen(QColor(210, 210, 210), 1));
    for (int x = 0; x < width(); x += 20)
        for (int y = 0; y < height(); y += 20)
            p.drawPoint(x, y);

    // Draw alignment guide lines
    if (!m_guideLines.isEmpty()) {
        QPen guidePen(QColor(255, 0, 0, 60), 0.5, Qt::SolidLine);
        guidePen.setCosmetic(true);
        p.setPen(guidePen);
        for (const QLine &line : m_guideLines)
            p.drawLine(line);
    }
}

bool GridCanvas::eventFilter(QObject *obj, QEvent *event)
{
    QWidget *w = qobject_cast<QWidget *>(obj);
    if (!w || w->parentWidget() != this)
        return QWidget::eventFilter(obj, event);

    // In view mode, pass all events through to widgets (no drag/resize)
    if (!m_editMode)
        return QWidget::eventFilter(obj, event);

    switch (event->type()) {
    case QEvent::MouseButtonPress: {
        QMouseEvent *me = static_cast<QMouseEvent *>(event);
        if (me->button() == Qt::LeftButton) {
            m_dragTarget = w;
            m_dragStartPos = me->globalPosition().toPoint();
            m_dragStartGeom = w->geometry();
            m_dragMode = hitTest(w, me->pos());
            m_dragStarted = false;
            if (m_dragMode != None)
                m_dragging = true;
            return true; // Block press in edit mode
        }
        break;
    }
    case QEvent::MouseMove: {
        QMouseEvent *me = static_cast<QMouseEvent *>(event);
        if (m_dragging && m_dragTarget == w) {
            QPoint delta = me->globalPosition().toPoint() - m_dragStartPos;
            if (!m_dragStarted) {
                if (qAbs(delta.x()) < 5 && qAbs(delta.y()) < 5)
                    return true; // Block small movement in edit mode
                m_dragStarted = true;
            }
            QRect newGeom = m_dragStartGeom;

            switch (m_dragMode) {
            case Move:
                newGeom.moveLeft(m_dragStartGeom.left() + delta.x());
                newGeom.moveTop(m_dragStartGeom.top() + delta.y());
                break;
            case ResizeRight:
                newGeom.setWidth(m_dragStartGeom.width() + delta.x());
                break;
            case ResizeBottom:
                newGeom.setHeight(m_dragStartGeom.height() + delta.y());
                break;
            case ResizeLeft:
                newGeom.setLeft(m_dragStartGeom.left() + delta.x());
                break;
            case ResizeTop:
                newGeom.setTop(m_dragStartGeom.top() + delta.y());
                break;
            case ResizeBottomRight:
                newGeom.setWidth(m_dragStartGeom.width() + delta.x());
                newGeom.setHeight(m_dragStartGeom.height() + delta.y());
                break;
            case ResizeTopLeft:
                newGeom.setTopLeft(m_dragStartGeom.topLeft() + delta);
                break;
            case ResizeTopRight:
                newGeom.setTopRight(m_dragStartGeom.topRight() + QPoint(delta.x(), 0));
                newGeom.setTop(m_dragStartGeom.top() + delta.y());
                break;
            case ResizeBottomLeft:
                newGeom.setLeft(m_dragStartGeom.left() + delta.x());
                newGeom.setHeight(m_dragStartGeom.height() + delta.y());
                break;
            default:
                break;
            }

            if (newGeom.width() < 60) newGeom.setWidth(60);
            if (newGeom.height() < 40) newGeom.setHeight(40);

            if (m_dragMode == Move) {
                SnapResult snap = calcSnap(w, newGeom);
                if (snap.snappedX)
                    newGeom.moveCenter(QPoint(snap.snapX, newGeom.center().y()));
                if (snap.snappedY)
                    newGeom.moveCenter(QPoint(newGeom.center().x(), snap.snapY));
                m_guideLines = snap.guides;
            } else {
                m_guideLines.clear();
            }

            w->setGeometry(newGeom);
            ensureCanvasSize();
            update();
        } else if (m_editMode) {
            updateCursor(w, me->pos());
        }
        break;
    }
    case QEvent::MouseButtonRelease: {
        m_dragging = false;
        m_dragStarted = false;
        m_dragMode = None;
        m_dragTarget = nullptr;
        m_guideLines.clear();
        update();
        return true; // Block release in edit mode
    }
    default:
        break;
    }

    return QWidget::eventFilter(obj, event);
}

GridCanvas::SnapResult GridCanvas::calcSnap(QWidget *self, const QRect &geom) const
{
    SnapResult result;
    int selfCX = geom.center().x();
    int selfCY = geom.center().y();

    int bestDx = SNAP_THRESHOLD + 1;
    int bestDy = SNAP_THRESHOLD + 1;
    int snapCx = selfCX;
    int snapCy = selfCY;

    for (QObject *child : children()) {
        QWidget *other = qobject_cast<QWidget *>(child);
        if (!other || other == self) continue;

        int otherCX = other->geometry().center().x();
        int otherCY = other->geometry().center().y();

        int dx = std::abs(selfCX - otherCX);
        int dy = std::abs(selfCY - otherCY);

        if (dx < bestDx) { bestDx = dx; snapCx = otherCX; }
        if (dy < bestDy) { bestDy = dy; snapCy = otherCY; }
    }

    if (bestDx <= SNAP_THRESHOLD) {
        result.snappedX = true;
        result.snapX = snapCx;
        int minY = qMin(geom.top(), 0);
        int maxY = qMax(geom.bottom(), height());
        for (QObject *child : children()) {
            QWidget *other = qobject_cast<QWidget *>(child);
            if (other && other != self && other->geometry().center().x() == snapCx) {
                minY = qMin(minY, other->geometry().top());
                maxY = qMax(maxY, other->geometry().bottom());
            }
        }
        result.guides.append(QLine(snapCx, minY - 10, snapCx, maxY + 10));
    }

    if (bestDy <= SNAP_THRESHOLD) {
        result.snappedY = true;
        result.snapY = snapCy;
        int minX = qMin(geom.left(), 0);
        int maxX = qMax(geom.right(), width());
        for (QObject *child : children()) {
            QWidget *other = qobject_cast<QWidget *>(child);
            if (other && other != self && other->geometry().center().y() == snapCy) {
                minX = qMin(minX, other->geometry().left());
                maxX = qMax(maxX, other->geometry().right());
            }
        }
        result.guides.append(QLine(minX - 10, snapCy, maxX + 10, snapCy));
    }

    return result;
}

GridCanvas::DragMode GridCanvas::hitTest(QWidget *w, const QPoint &pos) const
{
    const int EDGE = 8;
    QRect r = w->rect();
    bool left = pos.x() < EDGE;
    bool right = pos.x() > r.width() - EDGE;
    bool top = pos.y() < EDGE;
    bool bottom = pos.y() > r.height() - EDGE;

    if (top && left) return ResizeTopLeft;
    if (top && right) return ResizeTopRight;
    if (bottom && left) return ResizeBottomLeft;
    if (bottom && right) return ResizeBottomRight;
    if (left) return ResizeLeft;
    if (right) return ResizeRight;
    if (top) return ResizeTop;
    if (bottom) return ResizeBottom;
    return Move;
}

void GridCanvas::updateCursor(QWidget *w, const QPoint &pos)
{
    DragMode mode = hitTest(w, pos);
    switch (mode) {
    case ResizeTopLeft: case ResizeBottomRight: w->setCursor(Qt::SizeFDiagCursor); break;
    case ResizeTopRight: case ResizeBottomLeft: w->setCursor(Qt::SizeBDiagCursor); break;
    case ResizeLeft: case ResizeRight: w->setCursor(Qt::SizeHorCursor); break;
    case ResizeTop: case ResizeBottom: w->setCursor(Qt::SizeVerCursor); break;
    default: w->setCursor(Qt::SizeAllCursor); break;
    }
}

void GridCanvas::ensureCanvasSize()
{
    int maxRight = 800, maxBottom = 600;
    for (QObject *child : children()) {
        QWidget *w = qobject_cast<QWidget *>(child);
        if (w) {
            maxRight = qMax(maxRight, w->geometry().right() + 40);
            maxBottom = qMax(maxBottom, w->geometry().bottom() + 40);
        }
    }
    if (maxRight != width() || maxBottom != height())
        resize(maxRight, maxBottom);
}

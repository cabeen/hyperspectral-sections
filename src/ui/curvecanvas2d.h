#ifndef CURVECANVAS2D_H
#define CURVECANVAS2D_H

#include <QWidget>
#include "canvas2d.h"

class CurveCanvas2D : public Canvas2D 
{
    Q_OBJECT
public:
    CurveCanvas2D(QList<QPointF*> *points);
    virtual ~CurveCanvas2D();

signals:
    void sample();

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

protected:
    double m_scale;
    QPointF *m_current;
    QList<QPointF*> *m_points;
    bool m_draggingRight;
    bool m_draggingLeft;
};

#endif // CURVECANVAS2D_H

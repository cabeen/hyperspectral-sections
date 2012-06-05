#include "curvecanvas2d.h"
#include <QMouseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QPainter>
#include <QString>
#include <cmath>
#include "settings.h"

CurveCanvas2D::CurveCanvas2D(QList<QPointF*> *points) : Canvas2D()
{
    m_draggingLeft = m_draggingRight = false;
    m_current = NULL;
    m_points = points;
}

CurveCanvas2D::~CurveCanvas2D()
{
}

void CurveCanvas2D::paintEvent(QPaintEvent *event)
{
    Canvas2D::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    if (m_points->size() > 0) {
        QColor color = QColor(settings.curveRed, settings.curveGreen, settings.curveBlue);
        QPen pen(QBrush(color), settings.curveWid);
        painter.setPen(pen);
        
        QPointF *buff = new QPointF();
        buff->setX(2 * settings.curveRad);
        buff->setY(0);
        imageToWindow(buff, buff);
        int d = buff->x();
        buff->setX(settings.curveRad);
        buff->setY(0);
        imageToWindow(buff, buff);
        int r = buff->x();

        for (int i = 0; i < m_points->size(); i++) {
            QPointF *p = (*m_points)[i];
            imageToWindow(p, buff);
            int x = buff->x() - r;
            int y = buff->y() - r;
            if (m_current && p == m_current)
              painter.drawEllipse(x, y, d, d);
            else 
              painter.drawEllipse(x, y, d, d);
        }

        QPointF *prev = new QPointF();
        imageToWindow((*m_points)[0], prev);
        for (int i = 1; i < m_points->size(); i++) {
            QPointF *p = (*m_points)[i];
            imageToWindow(p, buff);
            painter.drawLine(prev->x(), prev->y(), buff->x(), buff->y());
            prev->setX(buff->x());
            prev->setY(buff->y());
        }
        delete prev;
        delete buff;
    }
}

void CurveCanvas2D::mousePressEvent(QMouseEvent *event)
{
    Canvas2D::mousePressEvent(event);
    if (event->button() == Qt::LeftButton)
    {
        QPointF *buff = new QPointF();
        buff->setX(settings.curveRad);
        buff->setY(0);
        imageToWindow(buff, buff);
        double rad = buff->x();
        buff->setX(event->x());
        buff->setY(event->y());
        windowToImage(buff, buff);
        m_current = NULL;

        for (int i = 0; i < m_points->size(); i++) {
            QPointF *p = (*m_points)[i];
            double dx = buff->x() - p->x();
            double dy = buff->y() - p->y();
            double d = sqrt(dx * dx + dy * dy);
            if (d < rad) {
               m_current = p; 
               break;
            }
        }

        if (!m_current) {
            QPointF *np = new QPointF(buff->x(), buff->y());
            m_points->push_back(np);
            m_current = np;
        }

        m_draggingLeft = true;
        update();
        delete buff;

        emit sample();
    }
    else if(event->button() == Qt::RightButton)
    {
    }
}

void CurveCanvas2D::mouseMoveEvent(QMouseEvent *event)
{
    Canvas2D::mouseMoveEvent(event);
    // m_draggingLeft and m_draggingRight are not mutually exclusive
    if (m_draggingLeft)
    {
        if (m_current != NULL) {
            QPointF *buff = new QPointF();
            buff->setX(event->x());
            buff->setY(event->y());
            windowToImage(buff, m_current);

            update();
            delete buff;

            emit sample();
        }
    }
    if (m_draggingRight)
    {
        update();
    }
}

void CurveCanvas2D::mouseReleaseEvent(QMouseEvent *event)
{
    Canvas2D::mouseReleaseEvent(event);
    // Move the mouse again in case this event is at a different position then the last event
    mouseMoveEvent(event);
    if (m_draggingLeft && event->button() == Qt::LeftButton)
    {
        m_draggingLeft = false;
        m_current = NULL;
        update();
    }
    else if (m_draggingRight && event->button() == Qt::RightButton)
    {
        m_draggingRight = false;
        update();
    }
}

#include "canvas2d.h"
#include <QMouseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QPainter>
#include <QString>
#include <cmath>
#include "settings.h"

Canvas2D::Canvas2D()
{
    setBackgroundRole(QPalette::Base);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    m_scale = 1.0;
    m_image = NULL;
    setSize(1, 1);

    m_drawp = false;
}

Canvas2D::~Canvas2D()
{
    delete m_image;
}

void Canvas2D::setPoint(float x, float y)
{
    m_point.setX(x);
    m_point.setY(y);
    m_drawp = true;
    update();
}

void Canvas2D::setPointVisible(bool v)
{
    m_drawp = v;
    update();
}

bool Canvas2D::isPointVisible()
{
    return m_drawp;
}

void Canvas2D::setSize(int width, int height)
{
    // clamp negative sizes so we always have at least one pixel
    if (width < 1) width = 1;
    if (height < 1) height = 1;
    
    if (m_image)
      delete m_image;

    m_image = new QImage(width, height, QImage::Format_RGB32);
    memset(m_image->bits(), 0, width * height * 4);
    notifySizeChanged(m_scale * m_image->width(), m_scale * m_image->height());
}

void Canvas2D::applyScale(double s)
{
    m_scale *= s;
    notifySizeChanged(m_scale * m_image->width(), m_scale * m_image->height());
}

void Canvas2D::resetScale()
{
    m_scale = 1.0;
    notifySizeChanged(m_scale * m_image->width(), m_scale * m_image->height());
}

void Canvas2D::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawImage(rect(), *m_image, m_image->rect());

    if (m_drawp) {
        QColor color = QColor(settings.pointRed, settings.pointGreen, settings.pointBlue);
        QPen pen(color);
        painter.setPen(pen);

        QPointF buff;
        buff.setX(2 * settings.pointRad + 1);
        buff.setY(0);
        imageToWindow(&buff, &buff);
        int d = buff.x();
        buff.setX(settings.pointRad);
        buff.setY(0);
        imageToWindow(&buff, &buff);
        int r = buff.x();

        imageToWindow(&m_point, &buff);
        int x = buff.x() - r;
        int y = buff.y() - r;

        painter.drawEllipse(x, y, d, d);
    }
}

void Canvas2D::windowToImage(QPointF *window, QPointF *image)
{
    float x = window->x();
    float y = window->y();

    x /= m_scale;
    y /= m_scale;

    x = fmax(0, fmin(m_image->width() - 1, x));
    y = fmax(0, fmin(m_image->height() - 1, y));

    image->setX(x);
    image->setY(y);
}

void Canvas2D::imageToWindow(QPointF *image, QPointF *window)
{
    float x = image->x();
    float y = image->y();

    x = fmax(0, fmin(m_image->width() - 1, x));
    y = fmax(0, fmin(m_image->height() - 1, y));

    x *= m_scale;
    y *= m_scale;

    window->setX(x);
    window->setY(y);
}

void Canvas2D::notifySizeChanged(int width, int height)
{
    setFixedSize(width, height);
    update();
}

void Canvas2D::mousePressEvent(QMouseEvent *event)
{
    QPointF buff = QPointF();
    buff.setX(event->x());
    buff.setY(event->y());
    windowToImage(&buff, &buff);
    emit clicked(buff.x(), buff.y());
}

void Canvas2D::mouseMoveEvent(QMouseEvent *event)
{
    QPointF buff = QPointF();
    buff.setX(event->x());
    buff.setY(event->y());
    windowToImage(&buff, &buff);
    emit moved(buff.x(), buff.y());
}

void Canvas2D::mouseReleaseEvent(QMouseEvent *event)
{
}

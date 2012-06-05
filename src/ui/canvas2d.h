#ifndef CANVAS2D_H
#define CANVAS2D_H

#include <QWidget>

class Canvas2D : public QWidget
{
    Q_OBJECT
public:
    Canvas2D();
    virtual ~Canvas2D();

    QImage *image() { return m_image; }

    void setSize(int width, int height);
    void applyScale(double s);
    void resetScale();
    void setPoint(float x, float y);
    void setPointVisible(bool v);
    bool isPointVisible();

signals:
    void moved(float x, float y);
    void clicked(float x, float y);

protected:
    //! This will be called when the size of the canvas has been changed
    virtual void paintEvent(QPaintEvent *);
    virtual void notifySizeChanged(int w, int h);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    void windowToImage(QPointF *window, QPointF *image);
    void imageToWindow(QPointF *image, QPointF *window);

protected:
    QPointF m_point;
    bool m_drawp;
    double m_scale;
    QImage *m_image;
};

#endif

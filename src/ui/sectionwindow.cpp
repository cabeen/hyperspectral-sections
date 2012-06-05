#include <qapplication.h>
#include <qmainwindow.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include "plot.h"
#include "sectionwindow.h"

#include <qprinter.h>
#include <qprintdialog.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_layout.h>
#include <qwt_color_map.h>
#include <qwt_double_range.h>
#include <stdio.h>
#include "settings.h"

class MyZoomer: public QwtPlotZoomer
{
    SectionWindow *m_parent;

public:
    MyZoomer(SectionWindow *parent, QwtPlotCanvas *canvas):
        QwtPlotZoomer(canvas)
    {
        m_parent = parent;
        setTrackerMode(AlwaysOn);
    }

    virtual QwtText trackerText(const QwtDoublePoint &pos) const
    {
        QColor bg(Qt::white);
        bg.setAlpha(200);
        QwtText text = QwtPlotZoomer::trackerText(pos);
        text.setBackgroundBrush( QBrush( bg ));
        return text;
    }

    virtual void updateDisplay()
    {
        QwtPlotZoomer::updateDisplay();
        QwtDoublePoint p = invTransform(this->trackerPosition());
        m_parent->relayMove(p);
    }
};

class RasterData: public QwtRasterData
{
public:
    RasterData():
        QwtRasterData(QwtDoubleRect(0, 0, 1, 1))
    {
        m_raster = NULL;
    }

    RasterData(FloatImage *raster) : QwtRasterData(QwtDoubleRect(0, 0, raster->width(), raster->height()))
    {
        m_raster = raster;
    }

    virtual QwtDoubleRect boundingRect() const
    {
        return QwtDoubleRect(0, 0, m_raster->width(), m_raster->height());
    }

    virtual QwtRasterData *copy() const
    {
        if (m_raster)
            return new RasterData(m_raster);
        else
            return new RasterData();
    }

    virtual QwtDoubleInterval range() const
    {
        if (m_raster) {
            float min, max;
            for (int i = 0; i < m_raster->width(); i++) {
                for (int j = 0; j < m_raster->height(); j++) {
                    float v = m_raster->get(i, j, 0);
                    min = (i == 0) ? v : fmin(min, v);
                    max = (i == 0) ? v : fmax(max, v);
                }
            }

            double dmin = min;
            double dmax = max;
            double del = max == min ? 1.0 : max - min;
            if (!settings.autoRange) {
                double minblend = settings.minSlider / (double) settings.precSlider;
                double maxblend = settings.maxSlider / (double) settings.precSlider;

                dmin = min + minblend * del;
                dmax = min + maxblend * del;
            }

            return QwtDoubleInterval(dmin, dmax);
        } else {
            return QwtDoubleInterval(0, 0);
        }
    }

    virtual double value(double x, double y) const
    {
        if (m_raster) {
            int xi = round(x);
            xi = (xi < 0) ? 0 : (xi >= m_raster->width()) ? m_raster->width() - 1 : xi;

            int yi = round(y);
            yi = (yi < 0) ? 0 : (yi >= m_raster->height()) ? m_raster->height() - 1 : yi;

            return m_raster->get(xi, yi, 0);
        } else {
            return 0;
        }
    }

    void setData(FloatImage *raster) {
        m_raster = raster;
    }

    FloatImage *m_raster;
};

SectionWindow::SectionWindow(QWidget *parent):
    QMainWindow(parent)
{
    m_data = new RasterData();
    m_plot = new QwtPlot(parent);
    m_spect = new QwtPlotSpectrogram();

    m_spect->setColorMap(QwtLinearColorMap(Qt::darkCyan, Qt::red));
    m_spect->setData(*m_data);
    m_spect->attach(m_plot);

    m_plot->axisWidget(QwtPlot::xBottom)->setTitle("Band (index)");
    m_plot->axisWidget(QwtPlot::yLeft)->setTitle("Arclength (pixel)");
    m_plot->setMargin(5);

    // A color bar on the right axis
    QwtScaleWidget *rightAxis = m_plot->axisWidget(QwtPlot::yRight);
    rightAxis->setTitle("Intensity");
    rightAxis->setColorBarEnabled(true);

    m_plot->enableAxis(QwtPlot::yRight);

    m_plot->plotLayout()->setAlignCanvasToScales(true);
    m_plot->replot();

    // LeftButton for the zooming
    // MidButton for the panning
    // RightButton: zoom out by 1
    // Ctrl+RighButton: zoom out to full size

    m_zoomer = new MyZoomer(this, m_plot->canvas());
    m_zoomer->setMousePattern(QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier);
    m_zoomer->setMousePattern(QwtEventPattern::MouseSelect3,
        Qt::RightButton);
    const QColor c(Qt::darkBlue);
    m_zoomer->setRubberBandPen(c);
    m_zoomer->setTrackerPen(c);

    QwtPlotPanner *panner = new QwtPlotPanner(m_plot->canvas());
    panner->setAxisEnabled(QwtPlot::yRight, false);
    panner->setMouseButton(Qt::MidButton);

    // Avoid jumping when labels with more/less digits
    // appear/disappear when scrolling vertically

    const QFontMetrics fm(m_plot->axisWidget(QwtPlot::yLeft)->font());
    QwtScaleDraw *sd = m_plot->axisScaleDraw(QwtPlot::yLeft);
    sd->setMinimumExtent( fm.width("100.00") );

    setCentralWidget(m_plot);
}

void SectionWindow::showData(FloatImage *raster)
{
    m_data->setData(raster);

    int n = settings.numContours;
    double min = m_data->range().minValue();
    double max = m_data->range().maxValue();
    double cdel = max == min ? 1.0 : (max - min) / n;

    m_spect->setData(*m_data);

    QwtValueList contourLevels;
    for ( double level = min; level < max; level += cdel )
        contourLevels += level;
    m_spect->setContourLevels(contourLevels);

    const QwtDoubleInterval range(min, max);

    m_plot->setAxisScale(QwtPlot::yRight, min, max);
    m_plot->enableAxis(QwtPlot::yRight);

    m_plot->setAxisScale(QwtPlot::yRight, min, max);
    m_plot->axisWidget(QwtPlot::yRight)->setColorMap(range, m_spect->colorMap());

    m_zoomer->setZoomBase(m_data->boundingRect());
    m_plot->plotLayout()->setAlignCanvasToScales(true);
    m_plot->updateAxes();
    m_plot->replot();
    m_plot->update();
}

void SectionWindow::relayMove(QwtDoublePoint p)
{
    emit moved(p.x(), p.y());
}

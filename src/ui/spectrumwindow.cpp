#include "spectrumwindow.h"

#include <qregexp.h>
#include <qapplication.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qstatusbar.h>
#include <qprinter.h>
#include <qpicture.h>
#include <qpainter.h>
#include <qfiledialog.h>
#include <qprintdialog.h>
#include <qfileinfo.h>
#include <qwt_counter.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_text.h>
#include <qwt_math.h>
#include <qwt_math.h>
#include <qwt_scale_engine.h>
#include <qwt_symbol.h>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_text.h>

SpectrumWindow::SpectrumWindow(QWidget *parent):
    QMainWindow(parent)
{
    m_plot = new QwtPlot(this);
    m_plot->setMargin(5);
    m_plot->setAutoReplot(false);
    m_plot->setCanvasBackground(QColor(Qt::darkBlue));

    m_plot->setAxisTitle(QwtPlot::xBottom, "Band");
    m_plot->setAxisTitle(QwtPlot::yLeft, "Intensity");

    m_plot->setAxisMaxMajor(QwtPlot::xBottom, 6);
    m_plot->setAxisMaxMinor(QwtPlot::xBottom, 6);
    m_plot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine);

    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->enableXMin(true);
    grid->setMajPen(QPen(Qt::white, 0, Qt::DotLine));
    grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
    grid->attach(m_plot);

    m_crv = new QwtPlotCurve("Amplitude");
    m_crv->setRenderHint(QwtPlotItem::RenderAntialiased);
    m_crv->setPen(QPen(Qt::yellow));
    m_crv->setYAxis(QwtPlot::yLeft);
    m_crv->attach(m_plot);

    m_plot->setAutoReplot(true);

    setContextMenuPolicy(Qt::NoContextMenu);

    m_picker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
        QwtPicker::PointSelection | QwtPicker::DragSelection,
        QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn, m_plot->canvas());
    m_picker->setRubberBandPen(QColor(Qt::green));
    m_picker->setRubberBand(QwtPicker::CrossRubberBand);
    m_picker->setTrackerPen(QColor(Qt::white));

    setCentralWidget(m_plot);

    connect(m_picker, SIGNAL(moved(const QPoint &)), SLOT(moved(const QPoint &)));
}

void SpectrumWindow::moved(const QPoint &pos)
{
    QString info;
    info.sprintf("Band = %g, Intensity = %g",
        m_plot->invTransform(QwtPlot::xBottom, pos.x()),
        m_plot->invTransform(QwtPlot::yLeft, pos.y()));
}

void SpectrumWindow::setData(double *x, double *y, int n)
{
    const bool doReplot = m_plot->autoReplot();
    m_plot->setAutoReplot(false);

    m_crv->setData(x, y, n);

    m_plot->setAutoReplot(doReplot);
    m_plot->replot();
}

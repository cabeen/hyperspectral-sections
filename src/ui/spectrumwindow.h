#ifndef SPECTRUMWINDOW_H
#define SPECTRUMWINDOW_H

#include <qmainwindow.h>
#include <qwt_polygon.h>

class QwtPlotZoomer;
class QwtPlotPicker;
class QwtPlotPanner;
class QwtPlotCurve;
class QwtPlot;

class SpectrumWindow : public QMainWindow
{
    Q_OBJECT

public:
    SpectrumWindow(QWidget *parent = 0);

public slots:
    void setData(double *x, double *y, int n);

private slots:
    void moved(const QPoint &);

private:
    QwtPlotCurve *m_crv;
    QwtPlot *m_plot;
    QwtPlotZoomer *m_zoomer[2];
    QwtPlotPicker *m_picker;
    QwtPlotPanner *m_panner;
};


#endif // SPECTRUMWINDOW_H

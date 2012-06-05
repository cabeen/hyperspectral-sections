#ifndef SECTIONWINDOW_H
#define SECTIONWINDOW_H

#include <qapplication.h>
#include <qmainwindow.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include "plot.h"
#include "floatimage.h"

class RasterData;
class QwtPlotZoomer;

class SectionWindow : public QMainWindow
{
    Q_OBJECT
public:
    SectionWindow(QWidget *parent = 0);

public slots:
    void showData(FloatImage *raster);
    void relayMove(QwtDoublePoint pos);

signals:
    void moved(float x, float y);

private:
    QwtPlotZoomer* m_zoomer;
    RasterData *m_data;
    QwtPlotSpectrogram *m_spect;
    QwtPlot *m_plot;
};

#endif // SECTIONWINDOW_H

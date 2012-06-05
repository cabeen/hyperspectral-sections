#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPrinter>
#include <QPushButton>
#include <QtGui>
#include "sectionwindow.h"
#include "spectrumwindow.h"
#include "curvecanvas2d.h"
#include "hypspecimage.h"
#include "mainoptionwindow.h"
#include "colormap.h"
#include "colormapwindow.h"
#include "floatimage.h"
#include "settings.h"

class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    virtual ~MainWindow();

private slots:
    void open();
    void about();
    void flipSectWin();
    void flipSpecWin();
    void flipMainOpt();
    void flipSectOpt();
    void sample();
    void updateMain();
    void updateSect();
    void zoomIn();
    void zoomOut();
    void normalSize();
    void updateMainOpt();
    void updateSectOpt();
    void popPoint();
    void clearPoints();
    void showMainPixel(float x, float y);
    void mapSect(float x, float y);
    void render(ColorMap cm, FloatImage *raster, QImage *image);

private:
    void createElements();
    void createMainOptions();
    void createSectOptions();
    void adjustScrollBar(QScrollBar *scrollBar, double factor);

    QScrollArea *m_scroll;
    QLabel *m_mainstatus;
    QPushButton *m_mainoptb;
    QSpinBox *m_bandsb;
    QSpinBox *m_radsb;
    QSpinBox *m_widsb;
    QCheckBox *m_arcb;
    QSlider *m_minsl;
    QSlider *m_maxsl;

    CurveCanvas2D *m_maincan;
    HypSpecImage *m_hsi;
    FloatImage *m_sectval;
    FloatImage *m_mainval;
    double *m_specval;
    double *m_waveval;

    ColorMap m_maincm;
    ColorMap m_sectcm;

    QList<QPointF*> m_points;
    QList<QLineF*> m_lines;
    QList<QPointF*> m_samples;

    QMainWindow *m_mainoptswin;
    QMainWindow *m_sectoptswin;
    SectionWindow *m_sectwin;
    SpectrumWindow *m_specwin;
};

#endif

#ifndef COLORMAPWINDOW_H
#define COLORMAPWINDOW_H

#include <QSlider>
#include <QMainWindow>
#include "colormap.h"

class ColorMapWindow : public QMainWindow
{
    Q_OBJECT

public:
    ColorMapWindow(QWidget *parent, ColorMap *cm);

private:
    ColorMap *m_cm;
    QSlider *m_redDark;
    QSlider *m_redBright;
    QSlider *m_greenDark;
    QSlider *m_greenBright;
    QSlider *m_blueDark;
    QSlider *m_blueBright;
};

#endif // COLORMAPWINDOW_H

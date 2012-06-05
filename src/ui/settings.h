#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>

struct Settings
{
    void defaults();

    int curveRad;
    float curveWid;
    int curveRed;
    int curveGreen;
    int curveBlue;

    int pointRad;
    int pointRed;
    int pointGreen;
    int pointBlue;

    int mainWidth;
    int mainHeight;
    int imgWidth;
    int imgHeight;

    int mainBand;
    bool autoRange;
    int minSlider;
    int maxSlider;
    int precSlider;
    int numContours;
};

// The global Settings object, will be initialized by MainWindow
extern Settings settings;

#endif // SETTINGS_H

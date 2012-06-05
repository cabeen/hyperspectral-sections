#include "settings.h"
#include <QFile>
#include <QSettings>

Settings settings;
void Settings::defaults()
{
    curveRad = 5;
    curveWid = 2;
    curveRed = 0;
    curveGreen = 0;
    curveBlue = 255;
    pointRad = 5;
    pointRed = 255;
    pointGreen = 0;
    pointBlue = 0;

    mainWidth = 400;
    mainHeight = 600;
    imgWidth = 400;
    imgHeight = 600;

    mainBand = 1;
    autoRange = true;
    numContours = 0;

    precSlider = 1024;
    minSlider = 0;
    maxSlider = 1024;
}

#ifndef HYPSPECIMAGE_H
#define HYPSPECIMAGE_H

#include "gdal_priv.h"
#include "cpl_conv.h" // for CPLMalloc
#include <QList>
#include <QPointF>
#include "floatimage.h"

class HypSpecImage
{
    public:
    HypSpecImage(const char* filename);

    int bands();
    int width();
    int height();
    void bounds(int band, float *min, float *max);
    void section(QList<QPointF*> samples, FloatImage *section);
    void band(int b, FloatImage *band);
    void spectrum(double x, double y, double *spectrum);

    private:
    GDALDataset *m_data;
};

#endif

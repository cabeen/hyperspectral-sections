#include "hypspecimage.h"
#include <cmath>

HypSpecImage::HypSpecImage(const char *filename)
{
    // Find a tidy way to only call this once
    GDALAllRegister();

    m_data = (GDALDataset *) GDALOpen( filename, GA_ReadOnly );
}

int HypSpecImage::bands()
{
    return m_data->GetRasterCount();
}

int HypSpecImage::width()
{
    return m_data->GetRasterXSize();
}

int HypSpecImage::height()
{
    return m_data->GetRasterYSize();
}

void HypSpecImage::section(QList<QPointF*> samples, FloatImage *raster)
{
//    char** md = m_data->GetMetadata(NULL);
//    if( CSLCount(md) > 0 )
//    {
//        printf( "Metadata:\n" );
//        for(int i = 0; md[i] != NULL; i++ )
//        {
//            printf( "  %s\n", md[i] );
//        }
//    }

    int bands = m_data->GetRasterCount();
    int smps = samples.size();

    float *line = (float *) CPLMalloc(sizeof(float));
    float *data = new float[bands * smps];

    for (int b = 0; b < bands; b++) {
        GDALRasterBand  *rb = m_data->GetRasterBand(b + 1);

        for (int i = 0; i < smps; i++) {
            QPointF *p = samples[i];
            int i0 = floor(p->x());
            int i1 = i0 + 1;
            int j0 = floor(p->y());
            int j1 = j0 + 1;

            rb->RasterIO( GF_Read, i0, j0, 1, 1,
                          line, 1, 1, GDT_Float32,
                          0, 0 );
            float v00 = line[0];

            rb->RasterIO( GF_Read, i1, j0, 1, 1,
                          line, 1, 1, GDT_Float32,
                          0, 0 );
            float v10 = line[0];

            rb->RasterIO( GF_Read, i0, j1, 1, 1,
                          line, 1, 1, GDT_Float32,
                          0, 0 );
            float v01 = line[0];

            rb->RasterIO( GF_Read, i1, j1, 1, 1,
                          line, 1, 1, GDT_Float32,
                          0, 0 );
            float v11 = line[0];

            float id = p->x() - i0;
            float jd = p->y() - j0;

            float vi0 = id * v00 + (1 - id) * v10;
            float vi1 = id * v01 + (1 - id) * v11;

            float v = jd * vi0 + (1 - jd) * vi1;

            data[b * smps + i] = v;
        }
    }

    for (int b = 0; b < bands; b++) {
        for (int i = 0; i < smps; i++) {
            float u = data[b * smps + i];
            raster->set(b, i, 0, u);
        }
    }

    CPLFree(line);
}

void HypSpecImage::bounds(int band, float *min, float *max)
{
    GDALRasterBand *raster = m_data->GetRasterBand(band);
    int bGotMin, bGotMax;
    *min = raster->GetMinimum(&bGotMin);
    *max = raster->GetMaximum(&bGotMax);
    if(!(bGotMin && bGotMax)) {
        double adfMinMax[2];
        GDALComputeRasterMinMax((GDALRasterBandH) raster, TRUE, adfMinMax);
        *min = adfMinMax[0];
        *max = adfMinMax[1];
    }
}


void HypSpecImage::band(int band, FloatImage *raster)
{
    GDALRasterBand *rb = m_data->GetRasterBand(band);

    int width = m_data->GetRasterXSize();
    int height = m_data->GetRasterYSize();

    int lineSize = rb->GetXSize();
    float *line = (float *) CPLMalloc(sizeof(float)*lineSize);

    for (int i = 0; i < height; i++) {
        rb->RasterIO(GF_Read, 0, i, lineSize, 1, line,
                     lineSize, 1, GDT_Float32, 0, 0);
        for (int j = 0; j < width; j++) {
            raster->set(j, i, 0, line[j]);
        }
    }

    CPLFree(line);
}

void HypSpecImage::spectrum(double x, double y, double *spectrum)
{
    int bands = m_data->GetRasterCount();
    float *line = (float *) CPLMalloc(sizeof(float));

    for (int b = 0; b < bands; b++) {
        GDALRasterBand  *rb = m_data->GetRasterBand(b + 1);

        int i0 = floor(x);
        int i1 = i0 + 1;
        int j0 = floor(y);
        int j1 = j0 + 1;

        rb->RasterIO( GF_Read, i0, j0, 1, 1,
                      line, 1, 1, GDT_Float32,
                      0, 0 );
        float v00 = line[0];

        rb->RasterIO( GF_Read, i1, j0, 1, 1,
                      line, 1, 1, GDT_Float32,
                      0, 0 );
        float v10 = line[0];

        rb->RasterIO( GF_Read, i0, j1, 1, 1,
                      line, 1, 1, GDT_Float32,
                      0, 0 );
        float v01 = line[0];

        rb->RasterIO( GF_Read, i1, j1, 1, 1,
                      line, 1, 1, GDT_Float32,
                      0, 0 );
        float v11 = line[0];

        float id = x - i0;
        float jd = y - j0;

        float vi0 = id * v00 + (1 - id) * v10;
        float vi1 = id * v01 + (1 - id) * v11;

        float v = jd * vi0 + (1 - jd) * vi1;

        spectrum[b] = v;
    }

    CPLFree(line);
}

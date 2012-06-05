#include "floatimage.h"

FloatImage::FloatImage(int width, int height, int bands) {
    m_width = width;
    m_height = height;
    m_bands = bands;
    int len = width * height * bands;
    m_data = new float[len];
    for (int i = 0; i < len; i++)
        m_data[i] = 0;
}

FloatImage::~FloatImage() {
    if (m_data)
        delete[] m_data;
    m_data = NULL;
}

float* FloatImage::data() {
    return m_data;
}

int FloatImage::width() const {
    return m_width;
}

int FloatImage::height() const {
    return m_height;
}

int FloatImage::bands() const {
    return m_bands;
}

float FloatImage::get(int i, int j, int k) const {
    return m_data[k * m_width * m_height + j * m_width + i];
}

void FloatImage::set(int i, int j, int k, float value) {
    m_data[k * m_width * m_height + j * m_width + i] = value;
}

int FloatImage::index(int i, int j, int k) const {
    return k * m_width * m_height + j * m_width + i;
}

void FloatImage::setAll(float value) {
    for (int i = 0; i < width() * height() * bands(); i++)
        m_data[i] = value;
}

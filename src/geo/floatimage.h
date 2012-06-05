#ifndef __FLOAT_IMAGE_H__
#define __FLOAT_IMAGE_H__
#include <cstddef>

class FloatImage {
public:
    //! Constructor for filter
    FloatImage(int width, int height, int bands);

    //! Destructor for filter
    virtual ~FloatImage();

    //! Get the data array
    float* data();

    //! Get the width
    int width() const;

    //! Get the height
    int height() const;

    //! Get the number of bands
    int bands() const;

    //! Get the value 
    float get(int i, int j, int k) const;

    //! Set the value 
    void set(int i, int j, int k, float value);

    //! Get the array index
    int index(int i, int j, int k) const;

    //! Set all pixels to a given value 
    void setAll(float value);

private:
    int m_width;
    int m_height;
    int m_bands;
    float* m_data;
};

#endif // __FLOAT_IMAGE_H__

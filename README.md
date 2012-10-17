hyperspectral-section
=====================

A visualization tool for exploring hyperspectral images.  The provides a method
for mapping image intensities along a user-drawn path in spatial coordinates.
This works with all formats supported by GDAL.  This has been successfully
tested on Mac OS 10.6 and Debian-flavored linux.  This depends on Qt 4, GDAL
1.6, and Qwt 5.2.  Other versions of these libraries may work, but they are
untested.  

After downloading the project and changing to the project directory, these 
commands should build and run the project on Debian "squeeze":

sudo apt-get install libgdal1-1.6.0 libgdal1-dev libqwt5-qt4-dev libqt4-dev
qmake
make
./hypsect

This is released under the MIT license.  Any comments can be
directed to cabeen@gmail.com

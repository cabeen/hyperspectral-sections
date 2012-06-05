#include "colormapwindow.h"
#include <QSlider>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>

ColorMapWindow::ColorMapWindow(QWidget *parent, ColorMap *cm) : QMainWindow(parent)
{
    m_cm = cm;

    m_redDark = new QSlider;
    m_redDark->setRange(0.0, 1.0);
    m_redDark->setValue(0.0);

    m_redBright = new QSlider;
    m_redBright->setRange(0.0, 1.0);
    m_redBright->setValue(1.0);

    m_greenDark = new QSlider;
    m_greenDark->setRange(0.0, 1.0);
    m_greenDark->setValue(0.0);

    m_greenBright = new QSlider;
    m_greenBright->setRange(0.0, 1.0);
    m_greenBright->setValue(1.0);

    m_blueDark = new QSlider;
    m_blueDark->setRange(0.0, 1.0);
    m_blueDark->setValue(0.0);

    m_blueBright = new QSlider;
    m_blueBright->setRange(0.0, 1.0);
    m_blueBright->setValue(1.0);

    QPushButton *apply = new QPushButton(tr("&Apply"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Red Dark:")), 0, 0);
    layout->addWidget(m_redDark, 0, 1);
    layout->addWidget(new QLabel(tr("Red Bright:")), 1, 0);
    layout->addWidget(m_redBright, 1, 1);
    layout->addWidget(new QLabel(tr("Green Dark:")), 2, 0);
    layout->addWidget(m_greenDark, 2, 1);
    layout->addWidget(new QLabel(tr("Green Bright:")), 3, 0);
    layout->addWidget(m_greenBright, 3, 1);
    layout->addWidget(new QLabel(tr("Blue Dark:")), 4, 0);
    layout->addWidget(m_blueDark, 4, 1);
    layout->addWidget(new QLabel(tr("Blue Bright:")), 5, 0);
    layout->addWidget(m_blueBright, 5, 1);
    layout->addWidget(apply, 6, 1);

    QWidget *central = new QWidget;
    central->setLayout(layout);

    hide();
    setCentralWidget(central);
}

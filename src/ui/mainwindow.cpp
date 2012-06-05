#include "mainwindow.h"

MainWindow::MainWindow()
{
    settings.defaults();

    m_hsi = NULL;
    m_sectval = NULL;
    m_mainval = NULL;
    m_specval = NULL;
    m_waveval = NULL;

    m_maincan = new CurveCanvas2D(&m_points);
    m_mainstatus = new QLabel;
    m_mainstatus->setText("Ready");

    m_scroll = new QScrollArea;
    m_scroll->setBackgroundRole(QPalette::Dark);
    m_scroll->setWidget(m_maincan);
    setCentralWidget(m_scroll);

    QStatusBar *statusArea = new QStatusBar;
    statusArea->addWidget(m_mainstatus);
    setStatusBar(statusArea);

    m_sectwin = new SectionWindow(this);
    m_sectwin->setWindowTitle("Section Viewer");
    m_sectwin->hide();

    m_specwin = new SpectrumWindow(this);
    m_specwin->setWindowTitle("Spectrum Viewer");
    m_specwin->hide();

    createMainOptions();
    createSectOptions();
    createElements();

    setWindowTitle(tr("Image Viewer"));
    resize(settings.mainWidth, settings.mainHeight);
}

MainWindow::~MainWindow()
{
    if (m_hsi)
        delete m_hsi;
    m_hsi = NULL;

    for (int i = 0; i < m_points.size(); i++) {
        if (m_points[i])
            delete m_points[i];
        m_points[i] = NULL;
    }

    m_points.clear();

    if (m_sectval)
        delete m_sectval;
    m_sectval = NULL;

    if (m_mainval)
        delete m_mainval;
    m_mainval = NULL;
}

void MainWindow::zoomIn()
{
    double scale = 1.25;
    m_maincan->applyScale(scale);

    adjustScrollBar(m_scroll->horizontalScrollBar(), scale);
    adjustScrollBar(m_scroll->verticalScrollBar(), scale);
}

void MainWindow::zoomOut()
{
    double scale = 0.8;
    m_maincan->applyScale(scale);

    adjustScrollBar(m_scroll->horizontalScrollBar(), scale);
    adjustScrollBar(m_scroll->verticalScrollBar(), scale);
}

void MainWindow::normalSize()
{
    m_maincan->resetScale();
}

void MainWindow::flipSectWin()
{
    if (m_sectwin->isVisible())
        m_sectwin->hide();
    else {
        m_sectwin->show();
    }
}

void MainWindow::flipSpecWin()
{
    if (m_specwin->isVisible())
        m_specwin->hide();
    else {
        m_specwin->show();
    }
}

void MainWindow::flipMainOpt()
{
    if (m_mainoptswin->isVisible())
        m_mainoptswin->hide();
    else
        m_mainoptswin->show();
}

void MainWindow::flipSectOpt()
{
    if (m_sectoptswin->isVisible())
        m_sectoptswin->hide();
    else
        m_sectoptswin->show();
}

void MainWindow::updateMainOpt()
{
    int nb = m_bandsb->value();
    if (nb != settings.mainBand) {
        settings.mainBand = nb;
        m_hsi->band(settings.mainBand, m_mainval);
        updateMain();
    }

    int cr = m_radsb->value();
    if (cr != settings.curveRad) {
        settings.curveRad = cr;
        m_maincan->update();
    }

    int cw = m_widsb->value();
    if (cw != settings.curveWid) {
        settings.curveWid = cw;
        m_maincan->update();
    }
}

void MainWindow::updateSectOpt()
{
    settings.autoRange = m_arcb->checkState() == Qt::Checked ? true : false;

    m_minsl->setEnabled(!settings.autoRange);
    m_maxsl->setEnabled(!settings.autoRange);

    settings.minSlider = m_minsl->value();
    settings.maxSlider = m_maxsl->value();

    updateSect();
}

void MainWindow::showMainPixel(float x, float y)
{
    m_mainstatus->setText(QString("Pixel = (%1, %2)").arg((int) round(x)).arg((int) round(y)));
    m_mainstatus->update();

    if (m_hsi) {
        m_hsi->spectrum(x, y, m_specval);
        m_specwin->setData(m_waveval, m_specval, m_hsi->bands());
    }
}

void MainWindow::mapSect(float x, float y)
{
    int iy = round(y);
    if (iy >= 0 && iy < m_samples.size()) {
        QPointF *p = m_samples[round(y)];
        m_maincan->setPoint(p->x(), p->y());
        showMainPixel(p->x(), p->y());
    }
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Open File"), QDir::currentPath());
    if (!fileName.isEmpty()) {
        if (m_hsi)
            delete m_hsi;
        if (m_mainval)
            delete m_mainval;
        if (m_sectval)
            delete m_sectval;
        if (m_specval)
            delete m_specval;
        if (m_waveval)
                delete m_waveval;
        m_sectval = NULL;

        m_hsi = new HypSpecImage(fileName.toAscii().constData());
        m_mainval = new FloatImage(m_hsi->width(),  m_hsi->height(), 1);
        m_specval = new double[m_hsi->bands()];
        m_bandsb->setRange(1, m_hsi->bands());
        m_hsi->band(settings.mainBand, m_mainval);

        m_waveval = new double[m_hsi->bands()];
        for (int i = 0; i < m_hsi->bands(); i++)
            m_waveval[i] = i;

        m_points.clear();
        m_lines.clear();
        m_samples.clear();

        settings.defaults();
        updateMainOpt();
        updateSectOpt();
        clearPoints();
        updateSect();
        updateMain();
    }
}

void MainWindow::sample() {
    if (m_points.size() > 1) {
        for (int i = 0; i < m_lines.size(); i++)
            delete m_lines[i];
        m_lines.clear();

        float length = 0;
        QPointF *prev = m_points.value(0);
        QList<float> lens;
        for (int i = 1; i < m_points.size(); i++) {
            QLineF *line = new QLineF(*prev, *m_points.value(i));
            m_lines.push_back(line);
            lens.push_back(line->length());
            length += line->length();
            prev = m_points.value(i);
        }

        for (int i = 0; i < m_samples.size(); i++)
            delete m_samples[i];
        m_samples.clear();

        float step = 1;
        int which = 0;
        float currdelta = 0;

        while (which < m_lines.size()) {
            float alpha = currdelta / lens[which];
            if (alpha < 0 || alpha > 1)
                printf("Error: alpha is not unit valued");
            m_samples.push_back(new QPointF(m_lines[which]->pointAt(alpha)));

            currdelta += step;

            if (currdelta > lens[which]) {
                currdelta -= lens[which];
                which++;
            }
        }

        if (m_sectval)
            delete m_sectval;

        m_sectval = new FloatImage(m_hsi->bands(), m_samples.size(), 1);
        m_hsi->section(m_samples, m_sectval);
    } else {
        if (m_sectval)
            delete m_sectval;

        m_sectval = new FloatImage(1, 1, 1);
    }

    updateSect();
}

void MainWindow::updateSect()
{
    if (m_hsi && m_sectval) {
        m_sectwin->showData(m_sectval);
    }
}

void MainWindow::updateMain()
{
    if (m_hsi) {
        m_maincan->setSize(m_hsi->width(), m_hsi->height());

        m_hsi->bounds(settings.mainBand, &m_maincm.redDark, &m_maincm.redBright);
        m_hsi->bounds(settings.mainBand, &m_maincm.greenDark, &m_maincm.greenBright);
        m_hsi->bounds(settings.mainBand, &m_maincm.blueDark, &m_maincm.blueBright);
        render(m_maincm, m_mainval, m_maincan->image());
    }
    m_maincan->update();
}
void MainWindow::createSectOptions()
{
    m_arcb = new QCheckBox;
    m_arcb->setCheckState(settings.autoRange ? Qt::Checked : Qt::Unchecked);

    m_minsl = new QSlider(Qt::Horizontal);
    m_minsl->setMinimum(0);
    m_minsl->setMaximum(settings.precSlider);
    m_minsl->setValue(settings.minSlider);
    m_minsl->setEnabled(!settings.autoRange);

    m_maxsl = new QSlider(Qt::Horizontal);
    m_maxsl->setMinimum(0);
    m_maxsl->setMaximum(settings.precSlider);
    m_maxsl->setValue(settings.maxSlider);
    m_maxsl->setEnabled(!settings.autoRange);

    QGridLayout *oml = new QGridLayout;
    oml->addWidget(new QLabel(tr("Auto Range:")), 0, 0);
    oml->addWidget(m_arcb, 0, 1);
    oml->addWidget(new QLabel("Range Minimum:"), 1, 0);
    oml->addWidget(m_minsl, 1, 1);
    oml->addWidget(new QLabel("Range Maximum:"), 2, 0);
    oml->addWidget(m_maxsl, 2, 1);

    QGroupBox *ogb = new QGroupBox("Section Options");
    ogb->setLayout(oml);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(ogb);

    QWidget *central = new QWidget;
    central->setLayout(layout);

    m_sectoptswin = new QMainWindow(this);
    m_sectoptswin->hide();
    m_sectoptswin->setCentralWidget(central);
    m_sectoptswin->setWindowTitle(tr("Section Viewer Options"));

    connect(m_arcb, SIGNAL(released()), this, SLOT(updateSectOpt()));
    connect(m_minsl, SIGNAL(sliderMoved(int)), this, SLOT(updateSectOpt()));
    connect(m_maxsl, SIGNAL(sliderMoved(int)), this, SLOT(updateSectOpt()));
}

void MainWindow::createMainOptions()
{
    m_bandsb = new QSpinBox;
    m_bandsb->setRange(settings.mainBand, settings.mainBand);
    m_bandsb->setValue(settings.mainBand);

    m_radsb = new QSpinBox;
    m_radsb->setRange(1, 50);
    m_radsb->setValue(settings.curveRad);

    m_widsb = new QSpinBox;
    m_widsb->setRange(1, 50);
    m_widsb->setValue(settings.curveWid);

    m_mainoptb = new QPushButton(tr("&Apply"));

    QGridLayout *vml = new QGridLayout;

    vml->addWidget(new QLabel(tr("Render Band:")), 0, 0);
    vml->addWidget(m_bandsb, 0, 1);
    vml->addWidget(new QLabel("Point Radius:"), 1, 0);
    vml->addWidget(m_radsb, 1, 1);
    vml->addWidget(new QLabel("Curve Width:"), 2, 0);
    vml->addWidget(m_widsb, 2, 1);
    vml->addWidget(m_mainoptb, 3, 1);

    QGroupBox *vgb = new QGroupBox("Image Viewer Options");
    vgb->setLayout(vml);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(vgb);

    QWidget *central = new QWidget;
    central->setLayout(layout);

    m_mainoptswin = new QMainWindow(this);
    m_mainoptswin->hide();
    m_mainoptswin->setCentralWidget(central);
    m_mainoptswin->setWindowTitle(tr("Image Viewer Options"));

    connect(m_mainoptb, SIGNAL(clicked()), this, SLOT(updateMainOpt()));
}

void MainWindow::createElements()
{
    QMenu *fileMenu = new QMenu(tr("&File"), this);
    menuBar()->addMenu(fileMenu);

    QMenu *viewMenu = new QMenu(tr("&View"), this);
    menuBar()->addMenu(viewMenu);

    QMenu *selectMenu = new QMenu(tr("&Selection"), this);
    menuBar()->addMenu(selectMenu);

    QMenu *helpMenu = new QMenu(tr("&Help"), this);
    menuBar()->addMenu(helpMenu);

    QAction *openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
    fileMenu->addAction(openAct);

    QAction *exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
    fileMenu->addAction(exitAct);

    QAction *zoomInAct = new QAction(tr("Zoom &In (25%)"), this);
    zoomInAct->setShortcut(tr("Ctrl++"));
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));
    viewMenu->addAction(zoomInAct);

    QAction *zoomOutAct = new QAction(tr("Zoom &Out (25%)"), this);
    zoomOutAct->setShortcut(tr("Ctrl+-"));
    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));
    viewMenu->addAction(zoomOutAct);

    QAction *normalSizeAct = new QAction(tr("&Normal Size"), this);
    normalSizeAct->setShortcut(tr("Ctrl+0"));
    connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));
    viewMenu->addAction(normalSizeAct);

    QAction *flipSectAct = new QAction(tr("&Show/Hide Section"), this);
    flipSectAct->setShortcut(tr("Ctrl+J"));
    connect(flipSectAct, SIGNAL(triggered()), this, SLOT(flipSectWin()));
    viewMenu->addAction(flipSectAct);

    QAction *flipSpecAct = new QAction(tr("&Show/Hide Spectrum"), this);
    flipSpecAct->setShortcut(tr("Ctrl+K"));
    connect(flipSpecAct, SIGNAL(triggered()), this, SLOT(flipSpecWin()));
    viewMenu->addAction(flipSpecAct);

    QAction *flipMainOptAct = new QAction(tr("&Show/Hide Image Viewer Options"), this);
    flipMainOptAct->setShortcut(tr("Ctrl+L"));
    connect(flipMainOptAct, SIGNAL(triggered()), this, SLOT(flipMainOpt()));
    viewMenu->addAction(flipMainOptAct);

    QAction *flipSectOptAct = new QAction(tr("&Show/Hide Section Viewer Options"), this);
    flipSectOptAct->setShortcut(tr("Ctrl+L"));
    connect(flipSectOptAct, SIGNAL(triggered()), this, SLOT(flipSectOpt()));
    viewMenu->addAction(flipSectOptAct);

    QAction *popAct = new QAction(tr("&Delete Last Point"), this);
    popAct->setShortcut(tr("Ctrl+N"));
    connect(popAct, SIGNAL(triggered()), this, SLOT(popPoint()));
    selectMenu->addAction(popAct);

    QAction *clearAct = new QAction(tr("&Delete All Points"), this);
    clearAct->setShortcut(tr("Ctrl+M"));
    connect(clearAct, SIGNAL(triggered()), this, SLOT(clearPoints()));
    selectMenu->addAction(clearAct);

    QAction *aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
    helpMenu->addAction(aboutAct);

    connect(m_maincan, SIGNAL(sample()), this, SLOT(sample()));
    connect(m_maincan, SIGNAL(moved(float, float)), this, SLOT(showMainPixel(float, float)));
    connect(m_sectwin, SIGNAL(moved(float, float)), this, SLOT(mapSect(float, float)));
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About"),
            tr("<p>This program allows the user to explor hyperspectral "
               "images through curve sections in spatial coordinates.  "
               "Points are added to the curve with each click, and points "
               "can be removed from the selection menu.  The GDAL library "
               "is used for reading input images, and the QWT library is "
               "used for section and spectrum rendering.</p>"));
}

void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                + ((factor - 1) * scrollBar->pageStep()/2)));
}

void MainWindow::popPoint()
{
    if (m_points.size() > 0) {
        m_points.pop_back();
        m_maincan->update();
        sample();
    }
}

void MainWindow::clearPoints()
{
    if (m_points.size() > 0) {
        m_points.clear();
        m_maincan->update();
        sample();
    }
}

void MainWindow::render(ColorMap cm, FloatImage *raster, QImage *image) {
    float dr = cm.redBright == cm.redDark ? 1.0 : cm.redBright - cm.redDark;
    float dg = cm.greenBright == cm.greenDark ? 1.0 : cm.greenBright - cm.greenDark;
    float db = cm.blueBright == cm.blueDark ? 1.0 : cm.blueBright - cm.blueDark;
    int w = image->width();
    int h = image->height();
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            float v = raster->get(i, j, 0);

            float ru = (v - cm.redDark)/dr;
            float gu = (v - cm.greenDark)/dg;
            float bu = (v - cm.blueDark)/db;

            ru = (ru > 1.0) ? 1.0 : (ru < 0.0) ? 0.0 : ru;
            gu = (gu > 1.0) ? 1.0 : (gu < 0.0) ? 0.0 : gu;
            bu = (bu > 1.0) ? 1.0 : (bu < 0.0) ? 0.0 : bu;

            int red = 255.0 * ru;
            int green = 255.0 * gu;
            int blue = 255.0 * bu;

            image->setPixel(i, j, qRgb(red, green ,blue));
        }
    }
}

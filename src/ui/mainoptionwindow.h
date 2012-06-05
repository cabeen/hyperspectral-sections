#ifndef MAINOPTIONWINDOW_H
#define MAINOPTIONWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QSpinBox>

class MainOptionWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainOptionWindow(QWidget *parent = 0);

signals:
    void changed();

private:

};

#endif // MAINOPTIONWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QDebug>
#include <QFileDialog>
#include <cv.hpp>
#include <vector>
#include "plot/qcustomplot.h"

using namespace cv;
using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    static void callBackFunction(int event, int x, int y, int flags, void* userdata);
    static MainWindow *singleToneWindow;


private slots:
    void on_selectFileButton_clicked();

    void on_thresholdSlider_valueChanged(int value);

    void on_maxSizeBox_valueChanged(int arg1);

    void on_windowBox_valueChanged(double arg1);

    void on_minimumSizeBox_valueChanged(double arg1);

    void on_textSizeBox_valueChanged(double arg1);

    void on_pushButton_clicked();

private:

};

#endif // MAINWINDOW_H

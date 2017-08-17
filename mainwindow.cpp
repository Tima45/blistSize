#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initPlot();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_selectFileButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,"Выбрать файл",ui->pathLabel->text());
    loadFile(path);
    process();
    calculateGist();
    drawSizes();
    showPics();
}

void MainWindow::loadFile(QString path)
{
    sourcePic = imread(path.toStdString());
    imagePath = path;
}

void MainWindow::process()
{
    if(sourcePic.data){
        double koef = ui->mkmBox->value()/(double)ui->pixelBox->value(); //
        grayPic;
        cvtColor(sourcePic,grayPic,CV_BGR2GRAY);
        threshold(grayPic,grayPic,ui->thresholdSlider->value(),255,CV_THRESH_BINARY);
        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        findContours(grayPic,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE);

        blistRects.clear();
        for(int i = 0; i < contours.size(); i++){
            if(hierarchy[i][3] != -1){
                Rect r = boundingRect(contours[i]);
                if(r.width*koef*sqrt(2) > ui->minimumSizeBox->value() && r.height*koef > ui->minimumSizeBox->value()){
                    blistRects.append(r);
                }
            }
        }

    }
}

void MainWindow::calculateGist()
{
    if(!blistRects.isEmpty()){

        double koef = ui->mkmBox->value()/(double)ui->pixelBox->value();

        int maxSize = ui->maxSizeBox->value();
        int countLevels = maxSize/ui->windowBox->value();

        widthS.clear();
        heightS.clear();

        wSize.clear();
        hSize.clear();

        for(int i = 0; i < countLevels; i++){
            widthS.append(0);
            heightS.append(0);
            wSize.append(i*ui->windowBox->value());
            hSize.append(i*ui->windowBox->value());
        }

        for(int i = 0; i < blistRects.count(); i++){
            int position = blistRects.at(i).width*koef*sqrt(2) / ui->windowBox->value();
            if(position < countLevels){
                widthS[position]+=1;
            }
            position = blistRects.at(i).height*koef / ui->windowBox->value();
            if(position < countLevels){
                heightS[position]+=1;
            }
        }
    }
}

void MainWindow::initPlot()
{
    ui->xPlot->setInteraction(QCP::iRangeDrag, true);
    ui->xPlot->setInteraction(QCP::iRangeZoom, true);
    ui->xPlot->axisRect()->setRangeDrag(Qt::Horizontal);
    ui->xPlot->axisRect()->setRangeZoom(Qt::Horizontal);
    ui->xPlot->xAxis->setLabel("Ширина(мкм)");
    ui->xPlot->yAxis->setLabel("Количество");

    ui->yPlot->setInteraction(QCP::iRangeDrag, true);
    ui->yPlot->setInteraction(QCP::iRangeZoom, true);
    ui->yPlot->axisRect()->setRangeDrag(Qt::Horizontal);
    ui->yPlot->axisRect()->setRangeZoom(Qt::Horizontal);
    ui->yPlot->xAxis->setLabel("Высота(мкм)");
    ui->yPlot->yAxis->setLabel("Количество");


    xPlotSizes = ui->xPlot->addGraph();
    yPlotSizes = ui->yPlot->addGraph();

    xBars = new QCPBars(ui->xPlot->xAxis,ui->xPlot->yAxis);
    yBars = new QCPBars(ui->yPlot->xAxis,ui->yPlot->yAxis);

}

void MainWindow::drawSizes()
{
    if(!widthS.isEmpty() && !heightS.isEmpty()){
        QVector<double> key;
        for(int i = 0; i < widthS.count();i++){
            key.append(i*ui->windowBox->value());
        }
        //xPlotSizes->setData(key,widthS);
        //yPlotSizes->setData(key,heightS);

        xBars->setWidth(ui->windowBox->value());
        yBars->setWidth(ui->windowBox->value());
        xBars->setData(key,widthS);
        yBars->setData(key,heightS);

        ui->xPlot->rescaleAxes();
        ui->xPlot->replot();
        ui->yPlot->rescaleAxes();
        ui->yPlot->replot();
    }
}

void MainWindow::showPics()
{
    showPic = sourcePic.clone();
    if(!blistRects.isEmpty()){
        double koef = ui->mkmBox->value()/(double)ui->pixelBox->value();
        for(int i = 0; i < blistRects.count(); i++){
            int b = rand()%255;
            int g = rand()%255;
            int r = rand()%255;
            Scalar rColor(b,g,r);

            rectangle(showPic,blistRects.at(i),rColor,2);

            double rate = ui->textSizeBox->value();
            if(rate > 0){
                putText(showPic,to_string((int)round(blistRects.at(i).width*sqrt(2)*koef)),Point(blistRects.at(i).x+10,blistRects.at(i).y+50),FONT_HERSHEY_SIMPLEX,rate,rColor);
                putText(showPic,to_string((int)round(blistRects.at(i).height*koef)),Point(blistRects.at(i).x+10,blistRects.at(i).y+70),FONT_HERSHEY_SIMPLEX,rate,rColor);
            }
        }
    }
    imshow("Source",showPic);
}

void MainWindow::on_thresholdSlider_valueChanged(int value)
{
    process();
    calculateGist();
    drawSizes();
    showPics();
}

void MainWindow::on_maxSizeBox_valueChanged(int arg1)
{
    process();
    calculateGist();
    drawSizes();
}

void MainWindow::on_windowBox_valueChanged(double arg1)
{
    drawSizes();
    calculateGist();
    drawSizes();
}

void MainWindow::on_minimumSizeBox_valueChanged(double arg1)
{
    process();
    calculateGist();
    drawSizes();
    showPics();
}

void MainWindow::on_textSizeBox_valueChanged(double arg1)
{
    showPics();
}

void MainWindow::on_pushButton_clicked()
{

    QStringList list = imagePath.split("/");
    QString fileName = list.at(list.count()-1);
    QStringList list2 = fileName.split(".");
    fileName = list2.at(0);

    list.removeLast();
    list.insert(list.count()-1,"size");

    QString folderName = list.join("/");
    folderName+="/";

    qDebug() << folderName;


    QDir().mkpath(folderName);

    ui->xPlot->grab().save(folderName+fileName+"_width.png","png");
    ui->yPlot->grab().save(folderName+fileName+"_height.png","png");



    Mat showPic = sourcePic.clone();

    Mat sizesPic = showPic.clone();
    if(!blistRects.isEmpty()){
        double koef = ui->mkmBox->value()/(double)ui->pixelBox->value();
        for(int i = 0; i < blistRects.count(); i++){
            int b = rand()%255;
            int g = rand()%255;
            int r = rand()%255;
            Scalar rColor(b,g,r);

            rectangle(showPic,blistRects.at(i),rColor,2);
            rectangle(sizesPic,blistRects.at(i),rColor,2);

            double rate = 1;
            if(rate > 0){
                putText(sizesPic,to_string((int)round(blistRects.at(i).width*sqrt(2)*koef)),Point(blistRects.at(i).x+10,blistRects.at(i).y+50),FONT_HERSHEY_SIMPLEX,rate,rColor);
                putText(sizesPic,to_string((int)round(blistRects.at(i).height*koef)),Point(blistRects.at(i).x+10,blistRects.at(i).y+70),FONT_HERSHEY_SIMPLEX,rate,rColor);
            }
        }
    }

    String s = QString(folderName+fileName+"_rects").toStdString();
    imwrite(s+".png",showPic);
    imwrite(s+"_sizes.png",sizesPic);

    QFile f(folderName+"gist.xls");
    f.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&f);
    out << "width(mkm)\tcount\theight(mkm)\tcount\n";
    for(int i = 0; i < widthS.count(); i++){
        out << QString::number(wSize.at(i))+"\t"+QString::number(widthS.at(i))+"\t"+QString::number(hSize.at(i))+"\t"+QString::number(heightS.at(i))+"\n";
    }
    f.close();
}

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "customscene.h"
#include "desenharcampo.h"
#include "fieldedit.h"
#include "fileselector.h"

#include <QTimer>
#include <QPoint>
#include <QDebug>
#include <QMessageBox>
#include <QGraphicsItem>
#include <QMouseEvent>
#include <qmath.h>
#include <QGraphicsView>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QIODevice>
#include <QDataStream>
#include <QDir>
#include <QGraphicsLineItem>

#include <QClipboard>

#include "QHostAddress"
#include <QUdpSocket>

QUdpSocket *udpSocketOut;
QUdpSocket *udpSocketIn;

QString MyAddress = "192.168.1.6";
QString DestineAddress = "192.168.1.6";

int MyPort = 1235;
int DestinePort = 1234;



QGraphicsLineItem *Line[72];
QLabel *lables[72];

bool pixeis[1472][992];//21*80;15*80
int oldX = 0;
int oldY = 0;
bool tracking = false;

QString ClipBoardAdd = "";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    pixeisMetro = 10.0;//4pixeis por 10cm
    RenderWidth = 18.4*pixeisMetro;  //(18+1.5+1.5) = 21*45 = 945
    RenderHeigth = 12.4*pixeisMetro; //(12+1.5+1.5) = 15*45 = 675


    ui->setupUi(this);

    this->installEventFilter(this);

    lb_Debug = ui->lbColor;
    scene = new CustomScene(this);

    QGraphicsView *view = ui->graphicsView;
    //view->setRenderHint(QPainter::Antialiasing);
    view->setFixedSize(RenderWidth,RenderHeigth);

    scene->setSceneRect(-RenderWidth/2,-RenderHeigth/2,RenderWidth,RenderHeigth-2.5);


    QBrush brush(QColor(0,255,0));
    QPen pen(QColor (255,255,255));
    pen.setWidth(1);
    scene->addRect(-RenderWidth/2,-RenderHeigth/2,RenderWidth,RenderHeigth,pen,brush);

    view->setScene(scene);

    //metros
    float ComprimentoCampo = 17.875,LarguraCampo = 11.875,espessuraLinhas = 0.125, RaioCentro = 2;
    float LarguraPequenaArea = 3.375,ComprimentoPequenaArea = 0.625, LarguraGrandeArea = 6.355, ComprimentoGrandeArea = 2.125;
    float CantoRaio = 0.75,bolaCentralRaio = 0.075,bolaPenaltiRaio = 0.05,distanciaPenalti = 3,larguraBaliza = 2,comprimentoBaliza = 0.5;
    margem = 0.2;

    DesenharCampo *campo = new DesenharCampo();
    campo->desenharLinhas(scene,ComprimentoCampo*pixeisMetro,LarguraCampo*pixeisMetro,espessuraLinhas*pixeisMetro,RaioCentro*pixeisMetro,LarguraPequenaArea*pixeisMetro,ComprimentoPequenaArea*pixeisMetro,LarguraGrandeArea*pixeisMetro,ComprimentoGrandeArea*pixeisMetro,CantoRaio*pixeisMetro,bolaCentralRaio*pixeisMetro,bolaPenaltiRaio*pixeisMetro,distanciaPenalti*pixeisMetro,RenderWidth,RenderHeigth,margem*pixeisMetro,larguraBaliza*pixeisMetro,comprimentoBaliza*pixeisMetro);

    reloadImage();

    getAllPixelColor();

    //QFont myfont("Times",6);

    for(int x =0;x<360;x+=5)
    {
        Line[x/5] = new QGraphicsLineItem(0,0,0,0);
        lables[x/5] = new QLabel(this);
        switch (x) {
        case 0:
            lables[x/5]->setStyleSheet("color : red;");
            break;
        case 90:
            lables[x/5]->setStyleSheet("color: rgb(128,0,128);");
            break;
        case 180:
            lables[x/5]->setStyleSheet("color: rgb(255,0,255);");
            break;
        case 270:
            lables[x/5]->setStyleSheet("color: rgb(0,0,0);");
            break;
        default:
            lables[x/5]->setStyleSheet("color: rgb(0,0,255);");
            break;
        }

        QFont myfont("Arial",5);
        lables[x/5]->setFont(myfont);
        lables[x/5]->setText(QString::number(x)+"º : ");
        if(x<180) ui->V2Layout->addWidget(lables[x/5]);
        else ui->V1Layout->addWidget(lables[x/5]);
    }

    initSocket();

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(mousetrack()));
    timer->start(34);//30fps

    QTimer *timeUDP = new QTimer(this);
    connect(timeUDP,SIGNAL(timeout()),this,SLOT(UDPSend()));
    timeUDP->start(50);//4fps
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::displayMessage(QString txt)
{
    ui->lbDebug->setText(txt);
}

QString MainWindow::getPixelColor(int Px, int Py)
{
    QColor color = QColor(image.pixel(Px,Py));
    return color.name();
}

void MainWindow::getAllPixelColor()
{
    for(int x = 0; x<RenderWidth;x++)
    {
        for(int y = 0; y<RenderHeigth;y++)
        {
            QString value = getPixelColor(x,y);
            if(value=="#00ff00")
            {
                pixeis[x][y] = false;
            }
            else
            {
                pixeis[x][y] = true;
            }
        }
    }
}


void MainWindow::reloadImage()
{
    image = QImage(RenderWidth,RenderHeigth,QImage::Format_RGB32);
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    scene->render( &painter, image.rect(),QRectF(-RenderWidth/2,-RenderHeigth/2,RenderWidth,RenderHeigth), Qt::KeepAspectRatio );
    painter.end();
    image.save("Render.png");
}

void MainWindow::generateFile(QString FileName)
{
    QFile file;
    file.setFileName(FileName);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)){
            QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream out(&file);

    for(int x = 0; x<RenderWidth;x++)
    {
        for(int y = 0; y<RenderHeigth;y++)
        {
            QString value = getPixelColor(x,y);
            if(value=="#00ff00")
            {
                out << 0;
            }
            else
            {
                out << 1;
            }
        }
        out << "\n";
    }
    file.close();
}

void MainWindow::mousetrack()
{
  QPoint p = ui->graphicsView->mapFromGlobal(QCursor::pos());
  if(tracking)
  {
    QPen myPen;
    int R = 0,G = 0,B = 255;
    int xPointCenter = p.x()- RenderWidth/2;
    int yPointCenter = p.y()- RenderHeigth/2;

    /*qDebug() << p.x();
    qDebug() << p.y();*/

    if((oldX!= xPointCenter || oldY!=yPointCenter) && (xPointCenter>(-RenderWidth/2) && yPointCenter>(-RenderHeigth/2) && xPointCenter<(RenderWidth/2) && yPointCenter<(RenderHeigth/2)))
    {
        QString ClipBoardAddTemp = "";

        float mascara = pixeisMetro*0.28;
        float Ponta1 = pixeisMetro*0.05;
        float Ponta2 = pixeisMetro*0.05;
        float Ponta3 = pixeisMetro*0.05;

        for(int angle =0;angle<360;angle+=5)
        {
            float tam = mascara;
            //QPoint P1(xPointCenter+mascara*cos((double)angle*M_PI/180.0),yPointCenter+mascara*sin((double)angle*M_PI/180.0));
            QPoint P1;
                switch (angle) {
                //////////////////////////////////////////////////////////////////////
                case 30:
                    P1.setX((xPointCenter+(mascara+Ponta1)*cos((double)angle*M_PI/180.0)));
                    P1.setY((yPointCenter+(mascara+Ponta1)*sin((double)angle*M_PI/180.0)));
                    tam = (mascara+Ponta1);
                    break;
                case 35:
                    P1.setX((xPointCenter+(mascara+Ponta2)*cos((double)angle*M_PI/180.0)));
                    P1.setY((yPointCenter+(mascara+Ponta2)*sin((double)angle*M_PI/180.0)));
                    tam = (mascara+Ponta2);
                    break;
                case 40:
                    P1.setX((xPointCenter+(mascara+Ponta3)*cos((double)angle*M_PI/180.0)));
                    P1.setY((yPointCenter+(mascara+Ponta3)*sin((double)angle*M_PI/180.0)));
                    tam = (mascara+Ponta3);
                    break;
                case 45:
                    P1.setX((xPointCenter+(mascara+Ponta3)*cos((double)angle*M_PI/180.0)));
                    P1.setY((yPointCenter+(mascara+Ponta3)*sin((double)angle*M_PI/180.0)));
                    tam = (mascara+Ponta3);
                    break;
                case 50:
                    P1.setX((xPointCenter+(mascara+Ponta2)*cos((double)angle*M_PI/180.0)));
                    P1.setY((yPointCenter+(mascara+Ponta2)*sin((double)angle*M_PI/180.0)));
                    tam = (mascara+Ponta2);
                    break;
                case 55:
                    P1.setX((xPointCenter+(mascara+Ponta1)*cos((double)angle*M_PI/180.0)));
                    P1.setY((yPointCenter+(mascara+Ponta1)*sin((double)angle*M_PI/180.0)));
                    tam = (mascara+Ponta1);
                    break;
                //////////////////////////////////////////////////////////////////////
                //////////////////////////////////////////////////////////////////////
                case 120:
                    P1.setX((xPointCenter+(mascara+Ponta1)*cos((double)angle*M_PI/180.0)));
                    P1.setY((yPointCenter+(mascara+Ponta1)*sin((double)angle*M_PI/180.0)));
                    tam = (mascara+Ponta1);
                    break;
                case 125:
                    P1.setX((xPointCenter+(mascara+Ponta2)*cos((double)angle*M_PI/180.0)));
                    P1.setY((yPointCenter+(mascara+Ponta2)*sin((double)angle*M_PI/180.0)));
                    tam = (mascara+Ponta2);
                    break;
                case 130:
                    P1.setX((xPointCenter+(mascara+Ponta3)*cos((double)angle*M_PI/180.0)));
                    P1.setY((yPointCenter+(mascara+Ponta3)*sin((double)angle*M_PI/180.0)));
                    tam = (mascara+Ponta3);
                    break;
                case 135:
                    P1.setX((xPointCenter+(mascara+Ponta3)*cos((double)angle*M_PI/180.0)));
                    P1.setY((yPointCenter+(mascara+Ponta3)*sin((double)angle*M_PI/180.0)));
                    tam = (mascara+Ponta3);
                    break;
                case 140:
                    P1.setX((xPointCenter+(mascara+Ponta2)*cos((double)angle*M_PI/180.0)));
                    P1.setY((yPointCenter+(mascara+Ponta2)*sin((double)angle*M_PI/180.0)));
                    tam = (mascara+Ponta2);
                    break;
                case 145:
                    P1.setX((xPointCenter+(mascara+Ponta1)*cos((double)angle*M_PI/180.0)));
                    P1.setY((yPointCenter+(mascara+Ponta1)*sin((double)angle*M_PI/180.0)));
                    tam = (mascara+Ponta1);
                    break;
                //////////////////////////////////////////////////////////////////////
                //////////////////////////////////////////////////////////////////////
                    case 210:
                        P1.setX((xPointCenter+(mascara+Ponta1)*cos((double)angle*M_PI/180.0)));
                        P1.setY((yPointCenter+(mascara+Ponta1)*sin((double)angle*M_PI/180.0)));
                        tam = (mascara+Ponta1);
                        break;
                    case 215:
                        P1.setX((xPointCenter+(mascara+Ponta2)*cos((double)angle*M_PI/180.0)));
                        P1.setY((yPointCenter+(mascara+Ponta2)*sin((double)angle*M_PI/180.0)));
                        tam = (mascara+Ponta2);
                        break;
                    case 220:
                        P1.setX((xPointCenter+(mascara+Ponta3)*cos((double)angle*M_PI/180.0)));
                        P1.setY((yPointCenter+(mascara+Ponta3)*sin((double)angle*M_PI/180.0)));
                        tam = (mascara+Ponta3);
                        break;
                    case 225:
                        P1.setX((xPointCenter+(mascara+Ponta3)*cos((double)angle*M_PI/180.0)));
                        P1.setY((yPointCenter+(mascara+Ponta3)*sin((double)angle*M_PI/180.0)));
                        tam = (mascara+Ponta3);
                        break;
                    case 230:
                        P1.setX((xPointCenter+(mascara+Ponta2)*cos((double)angle*M_PI/180.0)));
                        P1.setY((yPointCenter+(mascara+Ponta2)*sin((double)angle*M_PI/180.0)));
                        tam = (mascara+Ponta2);
                        break;
                    case 235:
                        P1.setX((xPointCenter+(mascara+Ponta1)*cos((double)angle*M_PI/180.0)));
                        P1.setY((yPointCenter+(mascara+Ponta1)*sin((double)angle*M_PI/180.0)));
                        tam = (mascara+Ponta1);
                        break;
                 //////////////////////////////////////////////////////////////////////
                 //////////////////////////////////////////////////////////////////////
                        case 300:
                            P1.setX((xPointCenter+(mascara+Ponta1)*cos((double)angle*M_PI/180.0)));
                            P1.setY((yPointCenter+(mascara+Ponta1)*sin((double)angle*M_PI/180.0)));
                            tam = (mascara+Ponta1);
                            break;
                        case 305:
                            P1.setX((xPointCenter+(mascara+Ponta2)*cos((double)angle*M_PI/180.0)));
                            P1.setY((yPointCenter+(mascara+Ponta2)*sin((double)angle*M_PI/180.0)));
                            tam = (mascara+Ponta2);
                            break;
                        case 310:
                            P1.setX((xPointCenter+(mascara+Ponta3)*cos((double)angle*M_PI/180.0)));
                            P1.setY((yPointCenter+(mascara+Ponta3)*sin((double)angle*M_PI/180.0)));
                            tam = (mascara+Ponta3);
                            break;
                        case 315:
                            P1.setX((xPointCenter+(mascara+Ponta3)*cos((double)angle*M_PI/180.0)));
                            P1.setY((yPointCenter+(mascara+Ponta3)*sin((double)angle*M_PI/180.0)));
                            tam = (mascara+Ponta3);
                            break;
                        case 320:
                            P1.setX((xPointCenter+(mascara+Ponta2)*cos((double)angle*M_PI/180.0)));
                            P1.setY((yPointCenter+(mascara+Ponta2)*sin((double)angle*M_PI/180.0)));
                            tam = (mascara+Ponta2);
                            break;
                        case 325:
                            P1.setX((xPointCenter+(mascara+Ponta1)*cos((double)angle*M_PI/180.0)));
                            P1.setY((yPointCenter+(mascara+Ponta1)*sin((double)angle*M_PI/180.0)));
                            tam = (mascara+Ponta1);
                            break;
                //////////////////////////////////////////////////////////////////////
                default:
                    P1.setX(xPointCenter+mascara*cos((double)angle*M_PI/180.0));
                    P1.setY(yPointCenter+mascara*sin((double)angle*M_PI/180.0));
                    break;
                }

            //Depois de Obter o P1 verificar se não está a começar fora dos limites


                                int xSearch = (int)(p.x()+tam*cos((double)angle*M_PI/180.0));
                                int ySearch = (int)(p.y()+tam*sin((double)angle*M_PI/180.0));
                                if(xSearch<0)
                                {
                                    xSearch = 0;
                                }
                                if(ySearch<0)
                                {
                                    ySearch = 0;
                                }
                                ///////////////////////////////NOVO/////////////////////////////////////////////
                                if(pixeis[xSearch][ySearch]==true)
                                {
                                    //qDebug() << "Em cima linha!";
                                    while(pixeis[xSearch][ySearch]==true && (tam<1200))
                                    {
                                        tam++;
                                        xSearch = (int)(p.x()+tam*cos((double)angle*M_PI/180.0));
                                        ySearch = (int)(p.y()+tam*sin((double)angle*M_PI/180.0));
                                        if(xSearch<0) xSearch = 0;
                                        if(ySearch<0) ySearch = 0;
                                    }

                                    while(pixeis[xSearch][ySearch]==false && (tam<1200))
                                    {
                                          tam++;
                                          xSearch = (int)(p.x()+tam*cos((double)angle*M_PI/180.0));
                                          ySearch = (int)(p.y()+tam*sin((double)angle*M_PI/180.0));
                                          if(xSearch<0) xSearch = 0;
                                          if(ySearch<0) ySearch = 0;
                                    }
                                }
                                else
                                {
                                    while(pixeis[xSearch][ySearch]==false && (tam<1200))
                                    {
                                          tam++;
                                          xSearch = (int)(p.x()+tam*cos((double)angle*M_PI/180.0));
                                          ySearch = (int)(p.y()+tam*sin((double)angle*M_PI/180.0));
                                          if(xSearch<0) xSearch = 0;
                                          if(ySearch<0) ySearch = 0;
                                    }
                                }
                                ///////////////////////////////NOVO/////////////////////////////////////////////
                               QPoint P2(xPointCenter+tam*cos((double)angle*M_PI/180.0),yPointCenter+tam*sin((double)angle*M_PI/180.0));

                                Line[angle/5]->setLine(P1.x(),P1.y(),P2.x(),P2.y());

                                scene->removeItem(Line[angle/5]);
                                switch (angle) {
                                case 0:
                                    myPen.setColor(QColor(255,0,0));
                                    break;
                                case 90:
                                    myPen.setColor(QColor(128,0,128));
                                    break;
                                case 180:
                                    myPen.setColor(QColor(255,0,255));
                                    break;
                                case 270:
                                    myPen.setColor(QColor(0,0,0));
                                    break;
                                default:
                                    myPen.setColor(QColor(R,G,B));
                                    break;
                                }
                                Line[angle/5]->setPen(myPen);

                                scene->addItem(Line[angle/5]);

                                float meters = (float)(tam/pixeisMetro);

                                QString str = QString::number(meters, 'f', 2);

                                if((P2.x()>(-RenderWidth/2) && P2.y()>(-RenderHeigth/2) && P2.x()<(RenderWidth/2) && P2.y()<(RenderHeigth/2)))
                                {
                                    lables[angle/5]->setText(QString::number(angle)+"º :"+str+" m "/*P2x:"+QString::number(P2.x())+" P2y:"+QString::number(P2.y())*/);
                                }
                                else
                                {
                                    lables[angle/5]->setText(QString::number(angle)+"º : ---");
                                }

                                if(angle+5<360)
                                {
                                   ClipBoardAddTemp+= str+",";
                                }
                                else
                                {
                                    ClipBoardAddTemp+= str;
                                }
                            }

                            ClipBoardAdd = ClipBoardAddTemp;
                            ClipBoardAddTemp = "";

                            ui->lbDebug->setText(QString::number(xPointCenter)+" | "+QString::number(yPointCenter));

                            oldX = xPointCenter;
                            oldY = yPointCenter;
    }

 }

}


void MainWindow::on_actionGenerate_Mesures_Wait_60s_triggered()
{
    QString FileName = "Metros.txt";
    QFile file;
    file.setFileName(FileName);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)){
            QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream out(&file);

    float step = pixeisMetro/10;

    qDebug() << "Step: " << step;

    float mascara = pixeisMetro*0.26;
    float Ponta1 = pixeisMetro*0.05,Ponta2 = pixeisMetro*0.06,Ponta3 = pixeisMetro*0.07;

    for(int x = 0; x<RenderWidth;x+=step)
    {
        for(int y = 0; y<RenderHeigth;y+=step)
        {
            //out << x << ";" << y << ":"; //remover
                for(int angle =0;angle<360;angle+=5)
                {
                    float tam = mascara;

                    switch (angle) {
                                    //////////////////////////////////////////////////////////////////////
                                    case 30:
                                        tam = (mascara+Ponta1);
                                        break;
                                    case 35:
                                        tam = (mascara+Ponta2);
                                        break;
                                    case 40:
                                        tam = (mascara+Ponta3);
                                        break;
                                    case 45:
                                        tam = (mascara+Ponta3);
                                        break;
                                    case 50:
                                        tam = (mascara+Ponta2);
                                        break;
                                    case 55:
                                        tam = (mascara+Ponta1);
                                        break;
                                    //////////////////////////////////////////////////////////////////////
                                    //////////////////////////////////////////////////////////////////////
                                    case 120:
                                        tam = (mascara+Ponta1);
                                        break;
                                    case 125:
                                        tam = (mascara+Ponta2);
                                        break;
                                    case 130:
                                        tam = (mascara+Ponta3);
                                        break;
                                    case 135:
                                        tam = (mascara+Ponta3);
                                        break;
                                    case 140:
                                        tam = (mascara+Ponta2);
                                        break;
                                    case 145:
                                        tam = (mascara+Ponta1);
                                        break;
                                    //////////////////////////////////////////////////////////////////////
                                    //////////////////////////////////////////////////////////////////////
                                        case 210:
                                            tam = (mascara+Ponta1);
                                            break;
                                        case 215:
                                            tam = (mascara+Ponta2);
                                            break;
                                        case 220:
                                            tam = (mascara+Ponta3);
                                            break;
                                        case 225:
                                            tam = (mascara+Ponta3);
                                            break;
                                        case 230:
                                            tam = (mascara+Ponta2);
                                            break;
                                        case 235:
                                            tam = (mascara+Ponta1);
                                            break;
                                     //////////////////////////////////////////////////////////////////////
                                     //////////////////////////////////////////////////////////////////////
                                            case 300:
                                                tam = (mascara+Ponta1);
                                                break;
                                            case 305:
                                                tam = (mascara+Ponta2);
                                                break;
                                            case 310:
                                                tam = (mascara+Ponta3);
                                                break;
                                            case 315:
                                                tam = (mascara+Ponta3);
                                                break;
                                            case 320:
                                                tam = (mascara+Ponta2);
                                                break;
                                            case 325:
                                                tam = (mascara+Ponta1);
                                                break;
                                    //////////////////////////////////////////////////////////////////////
                                    default:
                                             tam = mascara;
                                        break;
                                    }

                    int xSearch = (int)(x+tam*cos((double)angle*M_PI/180.0));
                    int ySearch = (int)(y+tam*sin((double)angle*M_PI/180.0));
                    if(xSearch<0)
                    {
                        xSearch = 0;
                    }
                    if(ySearch<0)
                    {
                        ySearch = 0;
                    }
                    ///////////////////////////////NOVO/////////////////////////////////////////////
                    if(pixeis[xSearch][ySearch]==true)
                    {
                        //qDebug() << "Em cima linha!";
                        while(pixeis[xSearch][ySearch]==true && (tam<1200))
                        {
                            tam++;
                            xSearch = (int)(x+tam*cos((double)angle*M_PI/180.0));
                            ySearch = (int)(y+tam*sin((double)angle*M_PI/180.0));
                            if(xSearch<0) xSearch = 0;
                            if(ySearch<0) ySearch = 0;
                        }

                        while(pixeis[xSearch][ySearch]==false && (tam<1200))
                        {
                              tam++;
                              xSearch = (int)(x+tam*cos((double)angle*M_PI/180.0));
                              ySearch = (int)(y+tam*sin((double)angle*M_PI/180.0));
                              if(xSearch<0)
                              {
                                 xSearch = 0;
                              }
                              if(ySearch<0)
                              {
                                 ySearch = 0;
                              }
                        }
                    }
                    else
                    {
                        while(pixeis[xSearch][ySearch]==false && (tam<1200))
                        {
                              tam++;
                              xSearch = (int)(x+tam*cos((double)angle*M_PI/180.0));
                              ySearch = (int)(y+tam*sin((double)angle*M_PI/180.0));
                              if(xSearch<0)
                              {
                                 xSearch = 0;
                              }
                              if(ySearch<0)
                              {
                                 ySearch = 0;
                              }
                        }
                    }
                    ///////////////////////////////NOVO/////////////////////////////////////////////
                    float meters = (float)(tam/pixeisMetro);
                    QString str = QString::number(meters, 'f', 2);
                    //out << x << "," << y << ": " << angle << "º ->" << meters << " m" <<"\n";
                    if(angle+5<360)
                    {
                        out  << str << ",";
                    }
                    else
                    {
                        out << str << "\n";
                    }
            }
        }
    }
    file.close();
    QMessageBox msgBox;msgBox.setText(("Gerado!"));msgBox.exec();
}

void MainWindow::on_bt_gerar_clicked()
{
    generateFile("valores.txt");
}
bool MainWindow::eventFilter(QObject *ob, QEvent *ev)
{
    Q_UNUSED(ob);
    Q_UNUSED(ev);

    if(ev->type()== QEvent::MouseButtonDblClick)
    {
        if(tracking)
        {
            for(int angle =0;angle<360;angle+=5)
            {
                scene->removeItem(Line[angle/5]);
            }
            tracking = false;
        }
        else
        {
            tracking = true;
        }
    }
    else if(ev->type()== QEvent::MouseButtonPress)
    {
        if(Qt::RightButton)
        {
            qDebug() << "Right";
            QClipboard *cb = QApplication::clipboard();
            cb->setText(ClipBoardAdd);
            //For PrintScreen
            /*if(timer->isActive())
            {
                timer->stop();
            }
            else
            {
                timer->start(34);
            }*/
        }
    }

    return false;
}

void MainWindow::on_cb_tracking_clicked(bool checked)
{
    if(tracking)
    {
        for(int angle =0;angle<360;angle+=5)
        {
            scene->removeItem(Line[angle/5]);
        }
    }
    tracking = checked;
}
bool MainWindow::loadFile(QString FileName)
{
    // Load xml file as raw data
    QFile f(FileName);

    if (!f.open(QIODevice::ReadOnly ))
    {
        // Error while loading file
        qDebug() << "Error while loading file";
        return false;
    }
    // Set data into the QDomDocument before processing
    xmlDOC.setContent(&f);
    f.close();
    extractXmlData();
    return true;
}

void MainWindow::extractXmlData()
{
    /*float ComprimentoCampo = 0,LarguraCampo = 0,espessuraLinhas = 0, RaioCentro = 0;
    float LarguraPequenaArea = 0,ComprimentoPequenaArea = 0, LarguraGrandeArea = 0, ComprimentoGrandeArea = 0;
    float CantoRaio = 0,bolaCentralRaio = 0,bolaPenaltiRaio = 0,distanciaPenalti = 0;
    float margemEsq = 12;

    // Extract the root markup
    QDomElement root = xmlDOC.documentElement();

    ///////////////////////////////////////////////////////////

    // Get the first child of the root (Markup COMPONENT is expected)
    QDomElement Component = root.firstChild().toElement();

    // Loop while there is a child
    while(!Component.isNull())
    {
        // Check if the child tag name is COMPONENT
        if (Component.tagName()=="MESURE")
        {
            // Read and display the component ID
            QString ID = Component.attribute("ID","No ID");

            // Get the first child of the component
            QDomElement Child = Component.firstChild().toElement();

            double Value;

            // Read each child of the component node
            while (!Child.isNull())
            {
                // Readvalue
                if (Child.tagName() == "VALUE") Value = Child.firstChild().toText().data().toFloat();

                // Next child
                Child = Child.nextSibling().toElement();
            }
            if( ID == "FACTOR")
                factor = Value;
            else if( ID == "WIDTH")
                ComprimentoCampo = Value;
            else if( ID == "HEIGHT")
                LarguraCampo = Value;
            else if( ID == "LINEWIDTH")
                espessuraLinhas = Value;
            else if( ID == "CENTERRADIUS")
                RaioCentro = Value;
            else if( ID == "HEIGHTSMALLAREA")
                LarguraPequenaArea = Value;
            else if( ID == "WIDTHSMALLAREA")
                ComprimentoPequenaArea = Value;
            else if( ID == "HEIGHTBIGAREA")
                LarguraGrandeArea = Value;
            else if( ID == "WIDTHBIGAREA")
                ComprimentoGrandeArea = Value;
            else if( ID == "CORNER")
                CantoRaio = Value;
            else if( ID == "CENTERCIRCLE")
                bolaCentralRaio = Value;
            else if( ID == "PENALTYCIRCLE")
                bolaPenaltiRaio = Value;
            else if( ID == "PENALTYDISTANCE")
                distanciaPenalti = Value;
        }

        // Next component
        Component = Component.nextSibling().toElement();
    }

    scene->clear();

    scene->setSceneRect(-RenderWidth/2,-RenderHeigth/2,RenderWidth,RenderHeigth);

    QBrush brush(QColor(0,255,0));
    QPen pen(QColor (255,255,255));
    pen.setWidth(1);
    scene->addRect(-RenderWidth/2,-RenderHeigth/2,RenderWidth,RenderHeigth,pen,brush);

    DesenharCampo *campo = new DesenharCampo();
    campo->desenharLinhas(scene,ComprimentoCampo*factor,LarguraCampo*factor,espessuraLinhas*factor,RaioCentro*factor,LarguraPequenaArea*factor,ComprimentoPequenaArea*factor,LarguraGrandeArea*factor,ComprimentoGrandeArea*factor,CantoRaio*factor,bolaCentralRaio*factor,bolaPenaltiRaio*factor,distanciaPenalti*factor,RenderWidth,margemEsq*factor);

*/
}

void MainWindow::on_actionEditField_triggered()
{
    FieldEdit *field = new FieldEdit(this);
    field->showMaximized();
}

void MainWindow::on_actionLoad_Field_Wait_15s_triggered()
{
    QLabel lbl;
    FileSelector *dialog = new FileSelector(&lbl);
    dialog->setLableText("Escreva o nome do ficheiro a ler: ");
    lbl.show();
    QString searchString;
    if (dialog->exec())//Confirm
    {
        searchString = dialog->searchString();
        lbl.setText(searchString+".xml");
        //QMessageBox msgBox;msgBox.setText(searchString);msgBox.exec();
        if(loadFile(searchString+".xml"))
        {
            reloadImage();
            getAllPixelColor();
        }
        else
        {
            QMessageBox msgBox;msgBox.setText(("Ficheiro não encontrado!"));msgBox.exec();
        }
    }
    else//Cancel
    {
        QMessageBox msgBox;msgBox.setText(("Canceled"));msgBox.exec();
    }
}


void MainWindow::initSocket()
{
    udpSocketOut = new QUdpSocket(this);
    udpSocketOut->bind(QHostAddress(DestineAddress), DestinePort);

    udpSocketIn = new QUdpSocket(this);
    udpSocketIn->bind(QHostAddress(MyAddress), MyPort);
    connect(udpSocketIn, SIGNAL(readyRead()),this, SLOT(readPendingDatagrams()));
}

void MainWindow::UDPSend()
{
    QByteArray datagram;
    datagram.append(ClipBoardAdd);
    //qDebug() <<"SendUDP: "<< ClipBoardAdd;
    udpSocketOut->writeDatagram(datagram,QHostAddress(DestineAddress), DestinePort);
}

void MainWindow::readPendingDatagrams()
{
    while (udpSocketIn->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocketIn->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        udpSocketIn->readDatagram(datagram.data(), datagram.size(),
                                &sender, &senderPort);


        //ui->plainTextEdit->appendPlainText(datagram);
        qDebug() << "Recieved UDP: " << datagram;
    }
}

void MainWindow::on_actionClose_triggered()
{
    this->close();
}

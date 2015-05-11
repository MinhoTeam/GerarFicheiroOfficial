#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QGraphicsScene>

// Library needed for processing XML documents
#include <QtXml>
// Library needed for processing files
#include <QFile>
//Debug
#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QLabel *lb_Debug;
    int RenderWidth,RenderHeigth;
    float pixeisMetro;
    float margem;


public slots:
    void displayMessage(QString txt);
    QString getPixelColor(int Px,int Py);

private slots:
    void getAllPixelColor();
    void reloadImage();
    void generateFile(QString FileName);

    void mousetrack();

    void on_bt_gerar_clicked();

    bool eventFilter(QObject *ob, QEvent *ev);

    void on_cb_tracking_clicked(bool checked);

    bool loadFile(QString FileName);

    void extractXmlData();

    void on_actionEditField_triggered();

    void on_actionLoad_Field_Wait_15s_triggered();

    void on_actionGenerate_Mesures_Wait_60s_triggered();

    void initSocket();

    void UDPSend();

    void readPendingDatagrams();

    void on_actionClose_triggered();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QImage image;
    QTimer *timer;

    //The QDomDocument class represents an XML document.
    QDomDocument xmlDOC;
    QXmlStreamWriter streamWrite;

    float factor = 1.6;
};

#endif // MAINWINDOW_H

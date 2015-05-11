#include "customscene.h"
#include <QGraphicsSceneEvent>
#include <QMouseEvent>
#include "mainwindow.h"

MainWindow *mainW;
CustomScene::CustomScene(MainWindow *mainWin)
{
    mainW = mainWin;

}

void CustomScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QString color = mainW->getPixelColor(event->scenePos().x()+(mainW->RenderWidth/2),event->scenePos().y()+(mainW->RenderHeigth/2));
    mainW->lb_Debug->setText(color+" :"+QString::number(event->scenePos().x()/*+(mainW->RenderWidth/2)*/)+" | "+QString::number(event->scenePos().y()/*+(mainW->RenderHeigth/2)*/));
}

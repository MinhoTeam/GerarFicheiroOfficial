#include "desenharcampo.h"
#include <QGraphicsEllipseItem>
#include "math.h"
#include <QDebug>

DesenharCampo::DesenharCampo()
{
}

void DesenharCampo::desenharLinhas(QGraphicsScene *scene, float ComprimentoCampo, float LarguraCampo, float espessuraLinhas, float RaioCentro, float LarguraPequenaArea, float ComprimentoPequenaArea, float LarguraGrandeArea, float ComprimentoGrandeArea, float CantoRaio, float bolaCentralRaio, float bolaPenaltiRaio, float distanciaPenalti,int WidthGraphs,int HeightGraphs,float margem,float larguraBaliza,float comprimentoBaliza)
{
    QPen penLimit(QColor (255,255,255));
    penLimit.setWidth(1);

    //CampoLimit
    scene->addRect(-WidthGraphs/2,-HeightGraphs/2,WidthGraphs,HeightGraphs,penLimit);


    QPen pen(QColor (255,255,255));
    pen.setWidth(espessuraLinhas);

    //qDebug() <<"EspessuraLinhas: "<< espessuraLinhas;

    int NewZeroX = (-WidthGraphs/2+margem);
    int NewZeroY = (-HeightGraphs/2+margem);
    //qDebug() <<"margem: "<< margem;
    //qDebug() <<"NewZeroX: "<< NewZeroX;
    //qDebug() <<"NewZeroY: "<< NewZeroY;

    //Campo
    scene->addRect(NewZeroX,NewZeroY,ComprimentoCampo,LarguraCampo,pen);
    //Linha MeioCampo
    scene->addLine(NewZeroX+ComprimentoCampo/2,NewZeroY,NewZeroX+ComprimentoCampo/2,NewZeroY+LarguraCampo,pen);
    //Circulo MeioCampo
    scene->addEllipse(NewZeroX+ComprimentoCampo/2-RaioCentro,NewZeroY+LarguraCampo/2-RaioCentro,RaioCentro*2,RaioCentro*2,pen);
    //Circulo centro campo
    scene->addEllipse(NewZeroX+ComprimentoCampo/2-bolaCentralRaio,NewZeroY+LarguraCampo/2-bolaCentralRaio,bolaCentralRaio*2,bolaCentralRaio*2,pen);
    //PequenaArea Direita
    scene->addRect(NewZeroX+(ComprimentoCampo-(ComprimentoPequenaArea)),NewZeroY+LarguraCampo/2-LarguraPequenaArea/2,ComprimentoPequenaArea,LarguraPequenaArea,pen);
    //PequenaArea Esquerda
    scene->addRect(NewZeroX,NewZeroY+LarguraCampo/2-LarguraPequenaArea/2,ComprimentoPequenaArea,LarguraPequenaArea,pen);
    //GrandeArea Direita
    scene->addRect(NewZeroX+(ComprimentoCampo-(ComprimentoGrandeArea)),NewZeroY+LarguraCampo/2-LarguraGrandeArea/2,ComprimentoGrandeArea,LarguraGrandeArea,pen);
    //GrandeArea Esquerda
    scene->addRect(NewZeroX,NewZeroY+LarguraCampo/2-LarguraGrandeArea/2,ComprimentoGrandeArea,LarguraGrandeArea,pen);

    QBrush brush(QColor(255,255,255));
    //bolaPenaltiRaioEsq
    scene->addEllipse(NewZeroX+distanciaPenalti,NewZeroY+LarguraCampo/2-bolaPenaltiRaio,bolaPenaltiRaio*2,bolaPenaltiRaio*2,pen,brush);
    //bolaPenaltiRaioDir
    scene->addEllipse(NewZeroX+ComprimentoCampo-distanciaPenalti,NewZeroY+LarguraCampo/2-bolaPenaltiRaio,bolaPenaltiRaio*2,bolaPenaltiRaio*2,pen,brush);

    //CantoRaio BaixoEsquerda
    QGraphicsEllipseItem *QEsqrB = new QGraphicsEllipseItem(NewZeroX-CantoRaio,NewZeroY+LarguraCampo-CantoRaio,CantoRaio*2,CantoRaio*2);
    int startAngle = 0;
    int endAngle = 90*16;
    QEsqrB->setStartAngle(startAngle);
    QEsqrB->setSpanAngle(endAngle - startAngle);
    QEsqrB->setPen(pen);
    scene->addItem(QEsqrB);

    //CantoRaio CimaEsquerda
    QGraphicsEllipseItem *QEsqrC = new QGraphicsEllipseItem(NewZeroX-CantoRaio,NewZeroY-CantoRaio,CantoRaio*2,CantoRaio*2);
    startAngle = 360*16;
    endAngle = 270*16;
    QEsqrC->setStartAngle(startAngle);
    QEsqrC->setSpanAngle(endAngle - startAngle);
    QEsqrC->setPen(pen);
    scene->addItem(QEsqrC);

    //CantoRaio BaixoDir
    QGraphicsEllipseItem *QDirB = new QGraphicsEllipseItem(NewZeroX+ComprimentoCampo-CantoRaio,NewZeroY+LarguraCampo-CantoRaio,CantoRaio*2,CantoRaio*2);
    startAngle = 90*16;
    endAngle = 180*16;
    QDirB->setStartAngle(startAngle);
    QDirB->setSpanAngle(endAngle - startAngle);
    QDirB->setPen(pen);
    scene->addItem(QDirB);

    //CantoRaio CimaDireita
    QGraphicsEllipseItem *QDirC = new QGraphicsEllipseItem(NewZeroX+ComprimentoCampo-CantoRaio,NewZeroY-CantoRaio,CantoRaio*2,CantoRaio*2);
    startAngle = 180*16;
    endAngle = 270*16;
    QDirC->setStartAngle(startAngle);
    QDirC->setSpanAngle(endAngle - startAngle);
    QDirC->setPen(pen);
    scene->addItem(QDirC);

    /*QBrush b1(QColor(0,255,0));
    //Baliza Direita
    scene->addRect(NewZeroX+(ComprimentoCampo),NewZeroY+LarguraCampo/2-larguraBaliza/2,comprimentoBaliza,larguraBaliza,pen,b1);
    QBrush b2(QColor(0,255,0));
    //Baliza Esquerda
    scene->addRect(NewZeroX-comprimentoBaliza,NewZeroY+LarguraCampo/2-larguraBaliza/2,comprimentoBaliza,larguraBaliza,pen,b2);
    //GrandeArea Direita*/

}



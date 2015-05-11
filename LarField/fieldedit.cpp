#include "fieldedit.h"
#include "ui_fieldedit.h"
#include "mainwindow.h"
#include "fileselector.h"
#include <QMessageBox>

FieldEdit::FieldEdit(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FieldEdit)
{
    ui->setupUi(this);
}

FieldEdit::~FieldEdit()
{
    delete ui;
}

void FieldEdit::on_actionClose_triggered()
{
    QCoreApplication::quit();
}

void FieldEdit::on_btSave_clicked()
{
    QLabel lbl;
    FileSelector *dialog = new FileSelector(&lbl);
    dialog->setLableText("Escreva o nome do ficheiro a gravar: ");
    lbl.show();
    QString searchString;
    if (dialog->exec())//Confirm
    {
        searchString = dialog->searchString();
        lbl.setText(searchString+".xml");
        //QMessageBox msgBox;msgBox.setText(searchString);msgBox.exec();
        if(!saveFile(searchString+".xml"))
        {
            QMessageBox msgBox;msgBox.setText(("Ficheiro não encontrado!"));msgBox.exec();
        }
        else
        {
            QMessageBox msgBox;msgBox.setText(("Saved"));msgBox.exec();
        }
    }
    else//Cancel
    {
        QMessageBox msgBox;msgBox.setText(("Canceled"));msgBox.exec();
    }

}

void FieldEdit::on_btRead_clicked()
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
        if(!loadFile(searchString+".xml"))
        {
            QMessageBox msgBox;msgBox.setText(("Ficheiro não encontrado!"));msgBox.exec();
        }

    }
    else//Cancel
    {
        QMessageBox msgBox;msgBox.setText(("Canceled"));msgBox.exec();
    }
}

bool FieldEdit::loadFile(QString FileName)
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

void FieldEdit::extractXmlData()
{

    // Extract the root markup
    QDomElement root = xmlDOC.documentElement();

    QString Field = root.attribute("FIELDNAME","No name");
    int Year = root.attribute("YEAR","1900").toInt();

    ui->tbFieldName->setText("Field = " + Field);
    ui->tbFieldYear->setText("Year  = " + QString::number(Year));

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
                if (Child.tagName() == "VALUE") Value = Child.firstChild().toText().data().toDouble();

                // Next child
                Child = Child.nextSibling().toElement();
            }
            if( ID == "FACTOR")
                ui->sFactor->setValue((float)Value);
            else if( ID == "WIDTH")
                ui->sWidth->setValue((float)Value*2/100);
            else if( ID == "HEIGHT")
                ui->sHeight->setValue((float)Value*2/100);
            else if( ID == "LINEWIDTH")
                ui->sLineWdth->setValue((float)Value*2/100);
            else if( ID == "CENTERRADIUS")
                ui->sCenterRadius->setValue((float)Value*2/100);
            else if( ID == "HEIGHTSMALLAREA")
                ui->sHeigthSmall->setValue((float)Value*2/100);
            else if( ID == "WIDTHSMALLAREA")
                ui->sWidthSmall->setValue((float)Value*2/100);
            else if( ID == "HEIGHTBIGAREA")
                ui->sHeigthBig->setValue((float)Value*2/100);
            else if( ID == "WIDTHBIGAREA")
                ui->sWidthBig->setValue((float)Value*2/100);
            else if( ID == "CORNER")
                ui->sCorner->setValue((float)Value*2/100);
            else if( ID == "CENTERCIRCLE")
                ui->sCenterCircleDiameter->setValue((float)Value*2/100);
            else if( ID == "PENALTYCIRCLE")
                ui->sPenaltyCircleDiameter->setValue((float)Value*2/100);
            else if( ID == "PENALTYDISTANCE")
                ui->sPenaltyDistance->setValue((float)Value*2/100);
        }

        // Next component
        Component = Component.nextSibling().toElement();
    }
}

bool FieldEdit::saveFile(QString FileName)
{
    // Load xml file as raw data
    QFile output(FileName);

    if (!output.open(QIODevice::ReadWrite | QFile::Text))
    {
        // Error while loading file
        qDebug() << "Error while loading file to Write";
        return false;
    }
    // Set data into the QDomDocument before processing

       streamWrite.setDevice(&output);
       streamWrite.setAutoFormatting(true);
       streamWrite.writeStartDocument();

       streamWrite.writeStartElement("FIELD");

       QString name = ui->tbFieldName->text();
       name = name.replace("Field = ","");
       streamWrite.writeAttribute("FIELDNAME", name);
       QString year = ui->tbFieldYear->text();
       year = year.replace("Year  = ","");
       streamWrite.writeAttribute("YEAR", year);

       streamWrite.writeStartElement("MESURE");

       streamWrite.writeAttribute("ID", "FACTOR");
       streamWrite.writeTextElement("VALUE", QString::number(ui->sFactor->value()));

       streamWrite.writeEndElement();
       streamWrite.writeStartElement("MESURE");

       streamWrite.writeAttribute("ID", "WIDTH");
       streamWrite.writeTextElement("VALUE", QString::number(ui->sWidth->value()*100/2));

       streamWrite.writeEndElement();
       streamWrite.writeStartElement("MESURE");

       streamWrite.writeAttribute("ID", "HEIGHT");
       streamWrite.writeTextElement("VALUE", QString::number(ui->sHeight->value()*100/2));

       streamWrite.writeEndElement();
       streamWrite.writeStartElement("MESURE");

       streamWrite.writeAttribute("ID", "LINEWIDTH");
       streamWrite.writeTextElement("VALUE", QString::number(ui->sLineWdth->value()*100/2));

       streamWrite.writeEndElement();
       streamWrite.writeStartElement("MESURE");

       streamWrite.writeAttribute("ID", "CENTERRADIUS");
       streamWrite.writeTextElement("VALUE", QString::number(ui->sCenterRadius->value()*100/2));

       streamWrite.writeEndElement();
       streamWrite.writeStartElement("MESURE");

       streamWrite.writeAttribute("ID", "HEIGHTSMALLAREA");
       streamWrite.writeTextElement("VALUE", QString::number(ui->sHeigthSmall->value()*100/2));

       streamWrite.writeEndElement();
       streamWrite.writeStartElement("MESURE");

       streamWrite.writeAttribute("ID", "WIDTHSMALLAREA");
       streamWrite.writeTextElement("VALUE", QString::number(ui->sWidthSmall->value()*100/2));

       streamWrite.writeEndElement();
       streamWrite.writeStartElement("MESURE");

       streamWrite.writeAttribute("ID", "HEIGHTBIGAREA");
       streamWrite.writeTextElement("VALUE", QString::number(ui->sHeigthBig->value()*100/2));

       streamWrite.writeEndElement();
       streamWrite.writeStartElement("MESURE");

       streamWrite.writeAttribute("ID", "WIDTHBIGAREA");
       streamWrite.writeTextElement("VALUE", QString::number(ui->sWidthBig->value()*100/2));

       streamWrite.writeEndElement();
       streamWrite.writeStartElement("MESURE");

       streamWrite.writeAttribute("ID", "CORNER");
       streamWrite.writeTextElement("VALUE", QString::number(ui->sCorner->value()*100/2));

       streamWrite.writeEndElement();
       streamWrite.writeStartElement("MESURE");

       streamWrite.writeAttribute("ID", "CENTERCIRCLE");
       streamWrite.writeTextElement("VALUE", QString::number(ui->sCenterCircleDiameter->value()*100/2));

       streamWrite.writeEndElement();
       streamWrite.writeStartElement("MESURE");

       streamWrite.writeAttribute("ID", "PENALTYCIRCLE");
       streamWrite.writeTextElement("VALUE", QString::number(ui->sPenaltyCircleDiameter->value()*100/2));

       streamWrite.writeEndElement();
       streamWrite.writeStartElement("MESURE");

       streamWrite.writeAttribute("ID", "PENALTYDISTANCE");
       streamWrite.writeTextElement("VALUE", QString::number(ui->sPenaltyDistance->value()*100/2));

       streamWrite.writeEndElement();

       streamWrite.writeEndElement();

       streamWrite.writeEndDocument();

       output.close();

       return true;
}

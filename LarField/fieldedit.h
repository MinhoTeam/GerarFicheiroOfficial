#ifndef FIELDEDIT_H
#define FIELDEDIT_H

#include <QMainWindow>

// Library needed for processing XML documents
#include <QtXml>
// Library needed for processing files
#include <QFile>
//Debug
#include <QDebug>

namespace Ui {
class FieldEdit;
}

class FieldEdit : public QMainWindow
{
    Q_OBJECT

public:
    explicit FieldEdit(QWidget *parent = 0);
    ~FieldEdit();

private slots:

    void on_actionClose_triggered();

    void on_btSave_clicked();

    void on_btRead_clicked();

    bool loadFile(QString FileName);

    void extractXmlData();

    bool saveFile(QString FileName);

private:
    Ui::FieldEdit *ui;

    //The QDomDocument class represents an XML document.
    QDomDocument xmlDOC;
    QXmlStreamWriter streamWrite;
};

#endif // FIELDEDIT_H

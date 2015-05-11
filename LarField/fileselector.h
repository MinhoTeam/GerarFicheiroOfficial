#ifndef FILESELECTOR_H
#define FILESELECTOR_H

#include <QDialog>
#include <QLineEdit>
#include <qlabel.h>

class FileSelector : public QDialog
{
    Q_OBJECT
public:
    explicit FileSelector(QWidget *parent = 0);
    QString searchString() const;
    void setLableText(QString txt);

private:
    QLineEdit *m_lineEdit;
    QLabel *info;
};

#endif // FILESELECTOR_H

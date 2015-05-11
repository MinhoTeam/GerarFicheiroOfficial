#include "fileselector.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>

FileSelector::FileSelector(QWidget *parent):
    QDialog(parent)
{
    m_lineEdit = new QLineEdit(this);
    info = new QLabel(this);

    QPushButton *searchButton = new QPushButton(tr("Abrir"));
    searchButton->setDefault(true);

    QPushButton *cancelButton = new QPushButton(tr("Cancelar"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal);
    buttonBox->addButton(searchButton, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(cancelButton, QDialogButtonBox::RejectRole);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *lt = new QVBoxLayout;

    lt->addWidget(info);
    lt->addWidget(m_lineEdit);
    lt->addWidget(buttonBox);


    setLayout(lt);
}

QString FileSelector::searchString() const
{
    return m_lineEdit->text();
}

void FileSelector::setLableText(QString txt)
{
    info->setText(txt);
}

#include "about.h"
#include "ui_about.h"

about::about(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::about)
{
    ui->setupUi(this);
    about::setWindowTitle("About");
    ui->label_3->setPixmap(QPixmap(":/icons/LSCP Editor-128px.png"));
}

about::~about()
{
    delete ui;
}

#include "about.h"
#include "ui_about.h"

about::about(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::about)
{
    ui->setupUi(this);
    about::setWindowTitle("About");
    ui->label_3->setPixmap(QPixmap(":/icons/icon.png"));
}

about::~about()
{
    delete ui;
}

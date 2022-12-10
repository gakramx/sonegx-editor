#include "lscpedit.h"
#include "./ui_lscpedit.h"

lscpedit::lscpedit(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::lscpedit)
{
    ui->setupUi(this);
}

lscpedit::~lscpedit()
{
    delete ui;
}


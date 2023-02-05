#include "mapdialog.h"
#include "ui_mapdialog.h"

MapDialog::MapDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MapDialog)
{
    ui->setupUi(this);
}

MapDialog::~MapDialog()
{
    delete ui;
}
QString MapDialog::getMapName() const
{
    return ui->mapName_lineEdit->text();
}
void MapDialog::setMapName(const QString &mapName)
{
    ui->mapName_lineEdit->setText(mapName);
}

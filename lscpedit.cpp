#include "lscpedit.h"
#include "./ui_lscpedit.h"
 #include <QFileDialog>
lscpedit::lscpedit(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::lscpedit)
{
    ui->setupUi(this);
     setAcceptDrops(true);
}

void lscpedit::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void lscpedit::dropEvent(QDropEvent *e)
{
    foreach (const QUrl &url, e->mimeData()->urls()) {
        QString fileName = url.toLocalFile();
        //adding path to some item
    }
}

lscpedit::~lscpedit()
{
    delete ui;
}


void lscpedit::on_openFilebtn_clicked()
{
          QString fileName = QFileDialog::getOpenFileName(this, ("Open File"), QDir::homePath(), ("lscp File(*.lscp)"));
}


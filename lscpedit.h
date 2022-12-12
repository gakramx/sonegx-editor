#ifndef LSCPEDIT_H
#define LSCPEDIT_H

#include <QMainWindow>
#include <QMimeData>
#include <QDragEnterEvent>
QT_BEGIN_NAMESPACE
namespace Ui { class lscpedit; }
QT_END_NAMESPACE

class lscpedit : public QMainWindow
{
    Q_OBJECT

public:
    lscpedit(QWidget *parent = nullptr);
    ~lscpedit();

void dragEnterEvent(QDragEnterEvent *e);
void dropEvent(QDropEvent *e);
private slots:
void on_openFilebtn_clicked();

private:
    Ui::lscpedit *ui;
};
#endif // LSCPEDIT_H

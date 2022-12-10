#ifndef LSCPEDIT_H
#define LSCPEDIT_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class lscpedit; }
QT_END_NAMESPACE

class lscpedit : public QMainWindow
{
    Q_OBJECT

public:
    lscpedit(QWidget *parent = nullptr);
    ~lscpedit();

private:
    Ui::lscpedit *ui;
};
#endif // LSCPEDIT_H

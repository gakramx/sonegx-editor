#ifndef LSCPEDIT_H
#define LSCPEDIT_H

#include <QMainWindow>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QStandardItemModel>
#include <QString>
#include <QStringList>
#include <QItemSelectionModel>
#include "mapdialog.h"
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
QStandardItemModel *model;
QString *filename;
QString *gigFileName;
QStringList splitLine(const QString &line);
QStringList extractWordsBetweenApostrophes(const QString& line);
QString removeWordsBetweenApostrophes(const QString& line);
void printFiletoTable(QString *file, int mapIndex);
void getGigFileName(QString *insfile);
int addDataFromInputsToTableview();
bool checkValueIfExist();
QItemSelectionModel *selectionModel;
QStringList createLinesFromTable();
int saveMapToFile(QString *file);
void deleteSelectedRows();
void renameMap(QString *file,const QString& oldName ,const QString& newName);
int removeMap(QString *file,const QString& mapName);
int orderMapIndex(QString *file, int currentIndex , int newIndex);
private slots:
void printMap(int mapIndex);
void on_actionOpen_triggered();
void calcBank();
void on_selectInstFile_pushButton_clicked();

void on_actionSave_triggered();

void on_newItem_pushButton_clicked();

void on_saveItem_pushButton_clicked();

void on_deleteItem_pushButton_clicked();

void on_newMap_pushButton_clicked();

void on_editMap_pushButton_clicked();

void on_deletMap_pushButton_clicked();

private:
    Ui::lscpedit *ui;
};
#endif // LSCPEDIT_H

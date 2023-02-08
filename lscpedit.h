#ifndef LSCPEDIT_H
#define LSCPEDIT_H

#include <QMainWindow>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QStandardItemModel>
#include <QString>
#include <QStringList>
#include <QItemSelectionModel>
#include <QLabel>
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
QString *originalFileName;
QString *gigFileName;
QStringList splitLine(const QString &line);
QStringList extractWordsBetweenApostrophes(const QString& line);
QString removeWordsBetweenApostrophes(const QString& line);
void printFiletoTable(QString *file, int mapIndex);
int addDataFromInputsToTableview();
bool checkValueIfExist();
void createTempFile(QString *originalFile, QString *tempFile);
bool saveChangesToFile(QString *originalFile, QString *tempFile);
QItemSelectionModel *selectionModel;
QStringList createLinesFromTable();
void deleteSelectedRows();
void renameMap(QString *file,const QString& oldName ,const QString& newName);
int removeMap(QString *file,const QString& mapName);
int orderMapIndex(QString *file, int currentIndex , int newIndex);
bool addMapToComboBox(const QString &item);
bool checkIfMapExist(const QString &item);
int addNewMaptoFile(QString *file,const QString& mapName);
bool isFileSaved(QString *originalFileName, QString *tempFileName);
void saveFile();
void removeFile(QString *file);
void saveAsFile(QString *file);
void createNewFile();
private slots:
int saveMapToFile(QString *file);
void printMap(int mapIndex);
void on_actionOpen_triggered();
void calcBank();
void on_selectInstFile_pushButton_clicked();

void on_actionSave_triggered();

void on_newItem_pushButton_clicked();



void on_deleteItem_pushButton_clicked();

void on_newMap_pushButton_clicked();

void on_editMap_pushButton_clicked();

void on_deletMap_pushButton_clicked();

void on_clearAll_pushButton_clicked();
void on_actionSave_As_triggered();



void on_actionQuit_triggered();

void on_actionNew_triggered();

void on_bank_spinBox_valueChanged(int arg1);

void on_instFilePath_lineEdit_textChanged(const QString &arg1);


void on_nameGig_lineEdit_textChanged(const QString &arg1);

void on_volume_doubleSpinBox_valueChanged(double arg1);

void on_prog_spinBox_valueChanged(int arg1);

void on_engine_comboBox_currentIndexChanged(int index);

void on_loadMode_comboBox_currentIndexChanged(int index);

void on_resetName_pushButton_clicked();

protected:
    void closeEvent(QCloseEvent *event) override;
private:
    Ui::lscpedit *ui;
    QLabel *label2;
};
#endif // LSCPEDIT_H

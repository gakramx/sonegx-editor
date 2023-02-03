#include "lscpedit.h"
#include "./ui_lscpedit.h"
#include <QFileDialog>
#include <QTextStream>
#include <QtConcurrent/QtConcurrent>
#include <iostream>
#include <string>
#include <gig.h>

#include <QComboBox>

lscpedit::lscpedit(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::lscpedit)
{


    ui->setupUi(this);
    setAcceptDrops(true);
    model = new QStandardItemModel(this);
    filename = new QString();
    model->setHorizontalHeaderItem(0, new QStandardItem("Bank"));
    model->setHorizontalHeaderItem(1, new QStandardItem("Prog"));
    model->setHorizontalHeaderItem(2, new QStandardItem("Engine"));
    model->setHorizontalHeaderItem(3, new QStandardItem("File"));
    model->setHorizontalHeaderItem(4, new QStandardItem("Volume"));
    model->setHorizontalHeaderItem(5, new QStandardItem("Load mode"));
    model->setHorizontalHeaderItem(6, new QStandardItem("Name"));
    ui->tableView->setModel(model);
    ui->volume_doubleSpinBox->setRange(0.0, 1.0);
    ui->volume_doubleSpinBox->setSingleStep(0.1);
    ui->bank_spinBox->setMaximum(16384);
    ui->lsb_spinBox->setMaximum(128);
    ui->msb_spinBox->setMaximum(128);
    ui->prog_spinBox->setMaximum(128);

    QStringList engine_items;
    engine_items << "GIG Engine" << "SF2 Engine" << "SFZ Engine";
    ui->engine_comboBox->addItems(engine_items);

    QStringList loadMode_items;
    loadMode_items << "Default" << "On Demand" << "On Demand and Hold" << "Persistent";
    ui->loadMode_comboBox->addItems(loadMode_items);


    QItemSelectionModel *selectionModel = ui->tableView->selectionModel();

    connect(selectionModel, &QItemSelectionModel::selectionChanged, [=](const QItemSelection &selected, const QItemSelection &deselected){
        QModelIndexList indexes = selected.indexes();
        for (int i = 0; i < indexes.count(); i++) {
            int row = indexes.at(i).row();

            QString bank_data = model->item(row, 0)->text();
            QString prog_data = model->item(row, 1)->text();
            QString engine_data = model->item(row, 2)->text();
            QString instFilePath_data = model->item(row, 3)->text();
            QString volume_data = model->item(row, 4)->text();
            QString loadmode_data = model->item(row, 5)->text();
            QString name_data = model->item(row, 6)->text();

            ui->bank_spinBox->setValue(bank_data.toInt());
            int value=bank_data.toInt();
            int MSB = value / 128;
            int LSB = value - MSB * 128;

            ui->msb_spinBox->setValue(MSB);
            ui->lsb_spinBox->setValue(LSB);

            ui->prog_spinBox->setValue(prog_data.toInt());
            ui->volume_doubleSpinBox->setValue(volume_data.toFloat());

            if(engine_data=="GIG")
                ui->engine_comboBox->setCurrentIndex(0);
            else if(engine_data=="SF2")
                ui->engine_comboBox->setCurrentIndex(1);
            else if(engine_data=="SFZ")
                ui->engine_comboBox->setCurrentIndex(2);

            ui->instFilePath_lineEdit->setText(instFilePath_data);

            if(loadmode_data=="ON_DEMAND")
                ui->loadMode_comboBox->setCurrentIndex(1);
            else if(loadmode_data=="ON_DEMAND_HOLD")
                ui->loadMode_comboBox->setCurrentIndex(2);
            else if(loadmode_data=="PERSISTENT")
                ui->loadMode_comboBox->setCurrentIndex(3);

            ui->nameGig_lineEdit->setText(name_data);



        }

    });
    connect(ui->msb_spinBox, SIGNAL(valueChanged(int)), this, SLOT(calcBank()));
    connect(ui->lsb_spinBox, SIGNAL(valueChanged(int)), this, SLOT(calcBank()));
    connect(ui->map_comboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(printMap(int)));
}
void lscpedit::calcBank(){
    if(ui->autoCalc_checkBox->isChecked()){

        int msb=ui->msb_spinBox->value();
        int lsb=ui->lsb_spinBox->value();
        int bank=msb*128+lsb;
        ui->bank_spinBox->setValue(bank);
    }
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
        ui->instFilePath_lineEdit->setText(fileName);
        //adding path to some item
    }
}

lscpedit::~lscpedit()
{
    delete ui;
}
QStringList lscpedit::splitLine(const QString &line)
{
    QStringList words;
    bool inApostrophes = false;
    int start = 0;
    for (int i = 0; i < line.length(); ++i)
    {
        if (line[i] == '\'')
        {
            if (!inApostrophes)
                start = i + 1;
            else
                words.append(line.mid(start, i - start));
            inApostrophes = !inApostrophes;
        }
        else if (line[i].isSpace() && !inApostrophes)
        {
            words.append(line.mid(start, i - start));
            start = i + 1;
        }
    }
    words.append(line.mid(start));
    return words;
}
QStringList lscpedit::extractWordsBetweenApostrophes(const QString& line) {
    QStringList words;
    int start = line.indexOf("'");
    while (start != -1) {
        int end = line.indexOf("'", start + 1);
        if (end != -1) {
            QString word = line.mid(start + 1, end - start - 1);
            words.append(word);
            start = line.indexOf("'", end + 1);
        } else {
            start = -1;
        }
    }
    return words;
}
QString lscpedit::removeWordsBetweenApostrophes(const QString& line) {
    QRegExp regex("'[^']+'");
    QString result = line;
    result.remove(regex);
    return result;
}
void lscpedit::printFiletoTable(QString *file,int mapIndex){

    model->removeRows(0, model->rowCount());

    QFile inputFile(*file);
    // QStandardItemModel *model = new QStandardItemModel(0,3);
    ui->map_comboBox->clear();
    QComboBox *comboBox = new QComboBox;
    comboBox->addItem("Default 1");
    comboBox->addItem("item 2");
    comboBox->addItem("item 3");



    if (inputFile.open(QIODevice::ReadOnly))
    {
        //int mapIndex=1;
        QTextStream stream(&inputFile);
        while (!stream.atEnd())
        {
            QString line = stream.readLine();
            /*    if (line.startsWith("#")) {
                     continue;
                 }*/

            if (line.startsWith("ADD MIDI_INSTRUMENT_MAP")) {
                QStringList maps = extractWordsBetweenApostrophes(line);
                ui->map_comboBox->addItem(maps.last());

                while (!stream.atEnd()) {
                    line = stream.readLine();

                    if (line.startsWith("MAP MIDI_INSTRUMENT")) {
                        QStringList getIndex = line.split(" ");
                        QString indexToInt = getIndex.at(3);

                        int finalInt= indexToInt.toInt();
                        if(finalInt==mapIndex)
                        {
                        //  QStringList words = splitLine(line);
                        QStringList words2 = extractWordsBetweenApostrophes(line);
                        QString modifiedLine = removeWordsBetweenApostrophes(line);

                        QStringList words = modifiedLine.split(" ");//QRegExp("\\b"), QString::SkipEmptyParts);

                        QString mapindexd_Word = words.at(3);
                        QString bank_Word = words.at(4);
                        QString prog_Word = words.at(5);
                        QString engine_Word = words.at(6);
                        QString filepath_Word = words2.at(0);
                        QString idont_Word = words.at(7);
                        QString volume_Word = words.at(9);
                        QString loadmode_Word = words.at(10);
                        QString name_Word = words2.at(1);
                        QList<QStandardItem*> items;
                        //items << new QStandardItem();
                        //items << new QStandardItem(mapindexd_Word);
                        items << new QStandardItem(bank_Word);
                        items << new QStandardItem(prog_Word);
                        items << new QStandardItem(engine_Word);
                        items << new QStandardItem(filepath_Word);
                        items << new QStandardItem(volume_Word);
                        items << new QStandardItem(loadmode_Word);
                        items << new QStandardItem(name_Word);
                        model->appendRow(items);
                    }
                    }else {
                        break;
                    }


                }
                // ui->tableView->setIndexWidget(model->index(1, 2), comboBox);
            }

            /*
                 else if (line.startsWith("ADD MIDI_INSTRUMENT_MAP")) {
                     QStringList words = line.split(" "); //splitting the line by space
                     words.removeFirst(); //removing the first element "ADD MIDI_INSTRUMENT_MAP"
                     // Append the remaining words to plainTextEdit
                     words.removeFirst();
                     words.replaceInStrings("'", "");
                     ui->comboBox->addItems(words);


             }*/
            /*else if (line.startsWith("MAP MIDI_INSTRUMENT")){

                    QStringList words2 = line.split(" ");
                 if (words2.length() > 4) {
                     QString fifthWord = words2[4];

                     // Do something with fifthWord
                 }

                 }*/

            inputFile.close();
        }
    }


}

void lscpedit::printMap(int mapIndex){
    qDebug()<<mapIndex;
    model->removeRows(0, model->rowCount());

    QFile inputFile(*filename);
    // QStandardItemModel *model = new QStandardItemModel(0,3);

    QComboBox *comboBox = new QComboBox;
    comboBox->addItem("Default 1");
    comboBox->addItem("item 2");
    comboBox->addItem("item 3");



    if (inputFile.open(QIODevice::ReadOnly))
    {
        //int mapIndex=1;
        QTextStream stream(&inputFile);
        while (!stream.atEnd())
        {
            QString line = stream.readLine();
            /*    if (line.startsWith("#")) {
                     continue;
                 }*/

            if (line.startsWith("ADD MIDI_INSTRUMENT_MAP")) {
                QStringList maps = extractWordsBetweenApostrophes(line);
              //  ui->map_comboBox->addItem(maps.last());

                while (!stream.atEnd()) {
                    line = stream.readLine();

                    if (line.startsWith("MAP MIDI_INSTRUMENT")) {
                        QStringList getIndex = line.split(" ");
                        QString indexToInt = getIndex.at(3);

                        int finalInt= indexToInt.toInt();
                        if(finalInt==mapIndex)
                        {
                            //  QStringList words = splitLine(line);
                            QStringList words2 = extractWordsBetweenApostrophes(line);
                            QString modifiedLine = removeWordsBetweenApostrophes(line);

                            QStringList words = modifiedLine.split(" ");//QRegExp("\\b"), QString::SkipEmptyParts);

                            QString mapindexd_Word = words.at(3);
                            QString bank_Word = words.at(4);
                            QString prog_Word = words.at(5);
                            QString engine_Word = words.at(6);
                            QString filepath_Word = words2.at(0);
                            QString idont_Word = words.at(7);
                            QString volume_Word = words.at(9);
                            QString loadmode_Word = words.at(10);
                            QString name_Word = words2.at(1);
                            QList<QStandardItem*> items;
                            //items << new QStandardItem();
                            //items << new QStandardItem(mapindexd_Word);
                            items << new QStandardItem(bank_Word);
                            items << new QStandardItem(prog_Word);
                            items << new QStandardItem(engine_Word);
                            items << new QStandardItem(filepath_Word);
                            items << new QStandardItem(volume_Word);
                            items << new QStandardItem(loadmode_Word);
                            items << new QStandardItem(name_Word);
                            model->appendRow(items);
                        }
                    }else {
                        break;
                    }


                }
                // ui->tableView->setIndexWidget(model->index(1, 2), comboBox);
            }

            /*
                 else if (line.startsWith("ADD MIDI_INSTRUMENT_MAP")) {
                     QStringList words = line.split(" "); //splitting the line by space
                     words.removeFirst(); //removing the first element "ADD MIDI_INSTRUMENT_MAP"
                     // Append the remaining words to plainTextEdit
                     words.removeFirst();
                     words.replaceInStrings("'", "");
                     ui->comboBox->addItems(words);


             }*/
            /*else if (line.startsWith("MAP MIDI_INSTRUMENT")){

                    QStringList words2 = line.split(" ");
                 if (words2.length() > 4) {
                     QString fifthWord = words2[4];

                     // Do something with fifthWord
                 }

                 }*/

            inputFile.close();
        }
    }


}
/*

void lscpedit::printFiletoTable(QString *file){

    model->removeRows(0, model->rowCount());

    QFile inputFile(*file);
    // QStandardItemModel *model = new QStandardItemModel(0,3);

    QComboBox *comboBox = new QComboBox;
    comboBox->addItem("Default 1");
    comboBox->addItem("item 2");
    comboBox->addItem("item 3");



    if (inputFile.open(QIODevice::ReadOnly))
    {

        QTextStream stream(&inputFile);
        while (!stream.atEnd())
        {
            QString line = stream.readLine();


            if (line.startsWith("ADD MIDI_INSTRUMENT_MAP")) {


                while (!stream.atEnd()) {
                    line = stream.readLine();
                    if (line.startsWith("MAP MIDI_INSTRUMENT")) {

                        //  QStringList words = splitLine(line);
                        QStringList words2 = extractWordsBetweenApostrophes(line);
                        QString modifiedLine = removeWordsBetweenApostrophes(line);

                        QStringList words = modifiedLine.split(" ");//QRegExp("\\b"), QString::SkipEmptyParts);

                        QString mapindexd_Word = words.at(3);
                        QString bank_Word = words.at(4);
                        QString prog_Word = words.at(5);
                        QString engine_Word = words.at(6);
                        QString filepath_Word = words2.at(0);
                        QString idont_Word = words.at(7);
                        QString volume_Word = words.at(9);
                        QString loadmode_Word = words.at(10);
                        QString name_Word = words2.at(1);
                        QList<QStandardItem*> items;
                        //items << new QStandardItem();
                        //items << new QStandardItem(mapindexd_Word);
                        items << new QStandardItem(bank_Word);
                        items << new QStandardItem(prog_Word);
                        items << new QStandardItem(engine_Word);
                        items << new QStandardItem(filepath_Word);
                        items << new QStandardItem(volume_Word);
                        items << new QStandardItem(loadmode_Word);
                        items << new QStandardItem(name_Word);
                        model->appendRow(items);
                    } else {
                        break;
                    }


                }
                // ui->tableView->setIndexWidget(model->index(1, 2), comboBox);
            }


            inputFile.close();
        }
    }


}
   */
void lscpedit::on_actionOpen_triggered()
{

    *filename = QFileDialog::getOpenFileName(this, ("Open File"), QDir::homePath(), ("lscp File(*.lscp)"));

    printFiletoTable(filename,0);
}

#include "lscpedit.h"
#include "./ui_lscpedit.h"
#include <QFileDialog>
#include <QTextStream>
#include <QtConcurrent/QtConcurrent>
#include <QMessageBox>

#include <iostream>
#include <string>


lscpedit::lscpedit(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::lscpedit)
{
    ui->setupUi(this);
    setAcceptDrops(true);
    lscpedit::setWindowTitle("SONEGX LSCP Editor");
    lscpedit::setWindowIcon(QIcon(":/icons/LSCP Editor-256px.png"));
    model = new QStandardItemModel(this);
    filename = new QString();
    originalFileName = new QString();
    gigFileName = new QString();
    model->setHorizontalHeaderItem(0, new QStandardItem("Bank"));
    model->setHorizontalHeaderItem(1, new QStandardItem("Prog"));
    model->setHorizontalHeaderItem(2, new QStandardItem("Engine"));
    model->setHorizontalHeaderItem(3, new QStandardItem("File"));
    model->setHorizontalHeaderItem(4, new QStandardItem("Volume"));
    model->setHorizontalHeaderItem(5, new QStandardItem("Load mode"));
    model->setHorizontalHeaderItem(6, new QStandardItem("Name"));
    ui->tableView->setModel(model);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->volume_doubleSpinBox->setRange(0.0, 1.0);
    ui->volume_doubleSpinBox->setSingleStep(0.1);
    ui->volume_doubleSpinBox->setDecimals(1);
    ui->volume_doubleSpinBox->setValue(1.1);
    ui->bank_spinBox->setMaximum(16384);
    ui->lsb_spinBox->setMaximum(128);
    ui->msb_spinBox->setMaximum(128);
    ui->prog_spinBox->setMaximum(128);
    ui->autoCalc_checkBox->setChecked(true);
    QStringList engine_items;
    engine_items << "GIG Engine" << "SF2 Engine" << "SFZ Engine";
    ui->engine_comboBox->addItems(engine_items);

    QStringList loadMode_items;
    loadMode_items << "Default" << "On Demand" << "On Demand and Hold" << "Persistent";
    ui->loadMode_comboBox->addItems(loadMode_items);


    //QItemSelectionModel *
    selectionModel = ui->tableView->selectionModel();

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
    connect(ui->map_comboBox, SIGNAL(currentIndexChanged(int)),this, SLOT(printMap(int)));

    // Add separator
    /*QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setFixedWidth(2);
    ui->statusbar->addWidget(separator);
*/

  //  label2->setFixedWidth(500);




    QLabel *label1 = new QLabel("    | v0.01 | ");
    ui->statusbar->addWidget(label1,0);

   label2 = new QLabel(" File : ");
    ui->statusbar->addWidget(label2, 1);
/*
    QLabel *label3 = new QLabel(" | Label 3 |    ");
    ui->statusbar->addPermanentWidget(label3, 0);
*/

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
       *gigFileName = url.toLocalFile();
        if(gigFileName!=nullptr)
        {
            ui->instFilePath_lineEdit->setText(*gigFileName);
            QFileInfo fileInfo(*gigFileName);
            QString name = fileInfo.baseName();
            ui->nameGig_lineEdit->setText(name);
        }
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
    QStringList mapsFinal;
    QFile inputFile(*file);
    ui->map_comboBox->clear();
    if (inputFile.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&inputFile);
        while (!stream.atEnd())
        {
            QString line = stream.readLine();
            if (line.startsWith("#"))
                continue;
            else if (line.isEmpty())
                continue;
            else if (line.startsWith("ADD MIDI_INSTRUMENT_MAP")) {
                qDebug()<< "Found1 ";
                qDebug()<<line;
                QStringList maps = extractWordsBetweenApostrophes(line);
                qDebug()<<maps.last();
                mapsFinal.append(maps.last());
            }
        }
        qDebug()<<mapsFinal;
        ui->map_comboBox->addItems(mapsFinal);
        ui->map_comboBox->setCurrentIndex(0);
        inputFile.close();
    }

}
void lscpedit::printMap(int mapIndex){

    model->removeRows(0, model->rowCount());
    QFile inputFile(*filename);
    if (inputFile.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&inputFile);
        while (!stream.atEnd())
        {
            QString line = stream.readLine();
            if (line.startsWith("#"))
                continue;
            else if (line.isEmpty())
                continue;
            else if (line.startsWith("MAP MIDI_INSTRUMENT")) {
                qDebug()<< "Found2";
                QStringList getIndex = line.split(" ");
                QString indexToInt = getIndex.at(3);
                int finalInt= indexToInt.toInt();
                qDebug()<< "Final int "<<finalInt;
                if(finalInt==mapIndex)
                {
                    qDebug()<< "Found3 ";
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
            }
        }
        for (int i = 0; i < ui->tableView->model()->columnCount(); i++) {
            ui->tableView->resizeColumnToContents(i);
        }
        inputFile.close();
    }
}

void lscpedit::on_actionOpen_triggered()
{

    *originalFileName = QFileDialog::getOpenFileName(this, ("Open File"), QDir::homePath(), ("lscp File(*.lscp)"));
    QFileInfo fileInfo(*originalFileName);
    QString directoryPath = fileInfo.absolutePath();
    QString name = fileInfo.fileName();
    *filename=directoryPath+"/.~"+name;
    qDebug()<<"File name :"<<*originalFileName;
    qDebug()<<"File temp :"<<*filename;
    createTempFile(originalFileName,filename);
    printFiletoTable(filename,0);
    label2->setText("File : " + *originalFileName);
}
void lscpedit::on_selectInstFile_pushButton_clicked()
{
    *gigFileName= QFileDialog::getOpenFileName(this, ("Open File"), QDir::homePath(), ("GIG File(*.gig)"));
    if(gigFileName!=nullptr)
    {
        ui->instFilePath_lineEdit->setText(*gigFileName);
        QFileInfo fileInfo(*gigFileName);
        QString name = fileInfo.baseName();
        ui->nameGig_lineEdit->setText(name);
    }

}
int lscpedit::addDataFromInputsToTableview(){
    int current_engine  = ui->engine_comboBox->currentIndex();
    if(checkValueIfExist())
        return 1;
    QString engine_Word;
    switch(current_engine)
    {
    case 0:
        engine_Word="GIG";
        break;
    case 1:
        engine_Word="SF2";
        break;
    case 2:
        engine_Word="SFZ";
        break;
    default:
        break;
    }

    int current_mode  = ui->loadMode_comboBox->currentIndex();
    QString mode_Word;
    switch(current_mode)
    {
    case 0:
        mode_Word="ON_DEMAND";
        break;
    case 1:
        mode_Word="ON_DEMAND";
        break;
    case 2:
        mode_Word="ON_DEMAND_HOLD";
        break;
    case 3:
        mode_Word="PERSISTENT";
        break;
    default:
        break;
    }
    QList<QStandardItem*> items;
    //items << new QStandardItem();
    //items << new QStandardItem(mapindexd_Word);
    items << new QStandardItem(ui->bank_spinBox->text());
    items << new QStandardItem(ui->prog_spinBox->text());
    items << new QStandardItem(engine_Word);
    items << new QStandardItem(ui->instFilePath_lineEdit->text());
    items << new QStandardItem(ui->volume_doubleSpinBox->text());
    items << new QStandardItem(mode_Word);
    items << new QStandardItem(ui->nameGig_lineEdit->text());
    model->appendRow(items);

    return 0;
}
void lscpedit::on_actionSave_triggered(){
    saveChangesToFile(originalFileName,filename);
    QFileInfo fileInfo(*originalFileName);
    QString directoryPath = fileInfo.absolutePath();
    QString name = fileInfo.fileName();
    *filename=directoryPath+"/.~"+name;
    createTempFile(originalFileName,filename);
    printFiletoTable(filename,0);
}
void lscpedit::on_newItem_pushButton_clicked()
{
    addDataFromInputsToTableview();
    saveMapToFile(filename);
}

bool lscpedit::checkValueIfExist(){

    // Get the value to check for from the line edit
    int bank = ui->bank_spinBox->value();
    int prog = ui->prog_spinBox->value();
    // The column to check (e.g. column 1)
    int columnToCheck = 0;
    int columnToCheck2 = 1;
    // Check if the value exist in all the rows in the specified column

    bool value1Exists = false;

    for (int row = 0; row < model->rowCount(); ++row) {
        QStandardItem *item = model->item(row, columnToCheck);
        QStandardItem *item2 = model->item(row, columnToCheck2);


        if (item->text().toInt() == bank && item2->text().toInt() == prog) {
            value1Exists = true;
                // qDebug() << "bank" << item->text().toInt()  << ".";
                // qDebug() << "Prog" << item2->text().toInt() << ".";
        }
    }
    if (value1Exists) {
        qDebug() << "Value exists in all rows in column" << columnToCheck << ".";
        QMessageBox::warning(this, "Error", "This Bank is exist");
        return true;
    } else {
        qDebug() << "Value does not exist in all rows in column" << columnToCheck << ".";
        return false;
    }

}

QStringList lscpedit::createLinesFromTable(){
    int currentMap=ui->map_comboBox->currentIndex();
    QStringList lines;
    QString bank;
    QString prog;
    QString engine;
    QString file;
    QString volume;
    QString loadMode;
    QString name;
    QString line;
    for (int row = 0; row < model->rowCount(); ++row) {
        QStandardItem *bank_item = model->item(row, 0);
        QStandardItem *prog_item = model->item(row, 1);
        QStandardItem *engine_item = model->item(row, 2);
        QStandardItem *file_item = model->item(row, 3);
        QStandardItem *volume_item = model->item(row, 4);
        QStandardItem *loadMode_item = model->item(row, 5);
        QStandardItem *name_item = model->item(row, 6);
        bank=bank_item->text();
        prog=prog_item->text();
        engine=engine_item->text();
        file=file_item->text();
        volume=volume_item->text();
        qDebug()<<"VVVVVVVVVV:"<<volume;
        loadMode=loadMode_item->text();
        name=name_item->text();
        line="MAP MIDI_INSTRUMENT NON_MODAL "+QString::number(currentMap)+" " +bank + " " +prog + " " +engine +" \'"+file+"\' "+"0 "+volume+ " " +loadMode+" \'"+name+"\'";
        lines.append(line);
        qDebug()<<line;
    }
    return lines;
}
int lscpedit::saveMapToFile(QString *file){
    QString map= ui->map_comboBox->currentText();
    QFile inputFile(*file);
    if (!inputFile.open(QIODevice::ReadWrite | QIODevice::Text))
        return -1;

    QStringList lines;
    QTextStream stream(&inputFile);
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        lines.append(line);
    }

    QStringList newLines = createLinesFromTable();
    for (int i = 0; i < lines.size(); i++) {
        if (lines[i].startsWith("ADD MIDI_INSTRUMENT_MAP")) {
            QStringList parts = lines[i].split(" ");
            if (parts.count() >= 3) {
                QString thirdWord = parts[2];
                thirdWord.remove("'");
                qDebug()<<thirdWord;
                if (thirdWord == map) {
                    int j = i + 1;
                    while (j < lines.size() && !lines[j].startsWith("ADD MIDI_INSTRUMENT_MAP")) {
                        lines.removeAt(j);
                    }
                    //lines.insert(i + 1, newLines);
                    int lineIndex = i + 1;
                    for (const QString &newLine : newLines) {
                        lines.insert(lineIndex, newLine);
                        ++lineIndex;
                    }
                }
            }
        }
    }
    inputFile.resize(0);
    stream << lines.join("\n");
    inputFile.close();
}
void lscpedit::deleteSelectedRows(){

    QAbstractItemModel *model = ui->tableView->model();
    QItemSelectionModel *selectionModel = ui->tableView->selectionModel();
    QModelIndexList selectedIndexes = selectionModel->selectedIndexes();

    if (selectedIndexes.empty())
        return;

    std::vector<int> rowsToRemove;
    for (const QModelIndex &index : selectedIndexes) {
        int row = index.row();
        auto it = std::find(rowsToRemove.begin(), rowsToRemove.end(), row);
        if (it == rowsToRemove.end())
            rowsToRemove.push_back(row);
    }

    std::sort(rowsToRemove.begin(), rowsToRemove.end(), std::greater<int>());
    for (int row : rowsToRemove) {
        model->removeRow(row);
    }
}

void lscpedit::on_deleteItem_pushButton_clicked()
{
    deleteSelectedRows();
    saveMapToFile(filename);
}
void lscpedit::on_newMap_pushButton_clicked()
{
    bool added=false;
    while(!added){
        MapDialog mapdialog;
        int result = mapdialog.exec();
        if (result == QDialog::Accepted) {
            QString text = mapdialog.getMapName();
            added=addMapToComboBox(text);
            if(added)
            {
                ui->map_comboBox->addItem(text);
                addNewMaptoFile(filename,text);
            }
        }
        else
            break;
    }
}

void lscpedit::on_editMap_pushButton_clicked()
{
    bool renamed=false;

    QString mapName=ui->map_comboBox->currentText();

    int index = ui->map_comboBox->currentIndex();

    while(!renamed){
        MapDialog mapdialog;
        mapdialog.setMapName(mapName);
         int result = mapdialog.exec();
        if (result == QDialog::Accepted) {
            QString text = mapdialog.getMapName();
            renamed=checkIfMapExist(text);
            if(renamed){
                if (index != -1)
                    renameMap(filename,mapName,text);
                ui->map_comboBox->setItemText(index, text);
            }
        }
        else
            break;
    }
}
void lscpedit::renameMap(QString *file,const QString& oldName ,const QString& newName){
    saveMapToFile(file);

    QString newLine = "ADD MIDI_INSTRUMENT_MAP \'"+newName+"\'";

    QFile inputFile(*file);
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QStringList lines;
    QTextStream in(&inputFile);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("ADD MIDI_INSTRUMENT_MAP")) {
            QStringList words = line.split(" ");
            QString thirdWord = words[2];
            thirdWord.remove("'");
            if (thirdWord == oldName)
                line = newLine;
        }
        lines << line;
    }

    inputFile.close();

    if (!inputFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&inputFile);
    for (const QString &line : lines)
        out << line << "\n";
    inputFile.close();

}
int lscpedit::removeMap(QString *file, const QString &mapName){
    QFile inputFile(*file);
    QStringList lines;
    if (inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&inputFile);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith("ADD MIDI_INSTRUMENT_MAP")) {
                QStringList words = line.split(" ");
                QString thirdWord = words[2];
                thirdWord.remove("'");
                if (thirdWord == mapName) {
                    while (!in.atEnd()) {
                        line = in.readLine();
                        if (line.startsWith("ADD MIDI_INSTRUMENT_MAP")) {
                            lines.append(line);
                            break;
                        }
                    }
                } else {
                    lines.append(line);
                }
            } else {
                lines.append(line);
            }
        }
        inputFile.close();
    }
    if (inputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&inputFile);
        for (const QString &line : lines) {
            out << line << endl;
        }
        inputFile.close();
    }
    return 0;
}

void lscpedit::on_deletMap_pushButton_clicked()
{


    QString mapName=ui->map_comboBox->currentText();
    int mapIndex=ui->map_comboBox->currentIndex();

    removeMap(filename,mapName);


    int count=ui->map_comboBox->count();
    for (int i = mapIndex; i < count; i++) {
        qDebug()<<i;

        orderMapIndex(filename,i+1,i);
    }
    ui->map_comboBox->removeItem(mapIndex);
}
int lscpedit::orderMapIndex(QString *file,int currentIndex , int newIndex){
    QFile inputFile(*file);
    if (!inputFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
        return 1;
    }

    QTextStream in(&inputFile);
    QStringList lines;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("MAP MIDI_INSTRUMENT")) {
            QStringList words = line.split(" ");

            QString indexToInt = words.at(3);
            int finalInt= indexToInt.toInt();
            if (finalInt == currentIndex) {
                words[3] = QString::number(newIndex);
                line = words.join(" ");
            }
        }
        lines.append(line);
    }

    inputFile.resize(0);
    QTextStream out(&inputFile);
    for (const QString &line : lines) {
        out << line << endl;
    }

    inputFile.close();
    return 0;
}
void lscpedit::createTempFile(QString *originalFile, QString *tempFile){
    // Open the original file
    QFile file(*originalFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // Handle error opening file
        return;
    }
    // Read the contents of the original file
    QTextStream in(&file);
    QString contents = in.readAll();
    file.close();
    // Make a copy of the original file as the temp file
    QFile temp(*tempFile);
    if (!temp.open(QIODevice::WriteOnly | QIODevice::Text)) {
        // Handle error opening temp file
        return;
    }
    // Write the contents of the original file to the temp file
    QTextStream out(&temp);
    out << contents;
    temp.close();
}
bool lscpedit::saveChangesToFile(QString *originalFile, QString *tempFile){
    // Open the original file
    QFile file(*originalFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // Handle error opening file
        return false;
    }

    // Read the contents of the original file
    QTextStream in(&file);
    QString originalContents = in.readAll();
    file.close();

    // Open the temp file
    QFile temp(*tempFile);
    if (!temp.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // Handle error opening temp file
        return false;
    }

    // Read the contents of the temp file
    QTextStream tempIn(&temp);
    QString tempContents = tempIn.readAll();
    temp.close();

    // Compare the contents of the original and temp files
    bool changesMade = originalContents != tempContents;

    if (changesMade) {
        // Changes have been made, save them to the original file
        if (!temp.open(QIODevice::WriteOnly | QIODevice::Text)) {
            // Handle error opening temp file
            return false;
        }
        QTextStream tempOut(&temp);
        tempOut << tempContents;
        temp.close();

        QFile::remove(*originalFile);
        temp.rename(*originalFile);
    }
    return changesMade;
}
void lscpedit::on_clearAll_pushButton_clicked()
{
    ui->tableView->clearSelection();
    *gigFileName= QFileDialog::getOpenFileName(this, ("Open File"), QDir::homePath(), ("GIG File(*.gig)"));
    if(gigFileName!=nullptr)
    {
    ui->instFilePath_lineEdit->setText(*gigFileName);
    QFileInfo fileInfo(*gigFileName);
    QString name = fileInfo.baseName();
    ui->nameGig_lineEdit->setText(name);
    }

}
bool lscpedit::addMapToComboBox(const QString &item){

    int index = ui->map_comboBox->findText(item);
    if (index == -1) {


        return true;
    } else {
        QMessageBox::warning(this, "Map already exists", "The map you are trying to add already exists in the list");
        return false;
    }

}
bool lscpedit::checkIfMapExist(const QString &item){

    int index = ui->map_comboBox->findText(item);
    if (index == -1) {
        return true;
    } else {
        QMessageBox::warning(this, "Map name already exists", "The map you are trying to rename already exists in the list");
        return false;
    }

}
int lscpedit::addNewMaptoFile(QString *file,const QString& mapName){

    QFile inputFile(*file);
    if (!inputFile.open(QIODevice::ReadWrite | QIODevice::Text))
        return 1;

    QTextStream in(&inputFile);
    QStringList lines;
    while (!in.atEnd()) {
        lines.append(in.readLine());
    }
    // Go to the end of the file
    inputFile.seek(inputFile.size());

    // Add a new line
    QTextStream out(&inputFile);
    out << "\n" << "ADD MIDI_INSTRUMENT_MAP \'"+mapName+"\'";

    // Close the file
    inputFile.close();
    return 0;
}
bool lscpedit::isFileSaved(QString *originalFileName, QString *tempFileName){
    QFile originalFile(*originalFileName);
    if (!originalFile.open(QFile::ReadOnly | QFile::Text)) {
        return true;
    }

    // Read the contents of the original file
    QTextStream originalIn(&originalFile);
    QString originalContents = originalIn.readAll();
    originalFile.close();

    // Read the contents of the temporary file
    QFile tempFile(*tempFileName);
    if (!tempFile.open(QFile::ReadOnly | QFile::Text)) {
        return true;
    }
    QTextStream tempIn(&tempFile);
    QString tempContents = tempIn.readAll();
    tempFile.close();

    // Compare the contents of the original file and the temporary file
    return (originalContents == tempContents);
}
void lscpedit::closeEvent(QCloseEvent *event)
{
    bool isSaved=isFileSaved(originalFileName,filename);
    if(!isSaved){
    int result = QMessageBox::warning(this, "Save Confirmation", "Do you want to save changes?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    if (result == QMessageBox::Yes) {
        // run the save function here
        saveFile();
        event->accept();
    } else if (result == QMessageBox::No) {
        event->accept();
        removeFile(filename);
    } else {
        event->ignore();
    }
    }
    else
        removeFile(filename);
}
void lscpedit::saveFile(){
    saveChangesToFile(originalFileName,filename);
}
void lscpedit::removeFile(QString *file){
    if (QFile::exists(*file)) {
        QFile::remove(*file);
    }
}
void lscpedit::saveAsFile(QString *file){
    QString outputFileName = QFileDialog::getSaveFileName(0, "Save As", QDir::homePath(), "LSCP files (*.lscp)");
    if (outputFileName.isEmpty())
        return;

    QFile inputFile(*file);
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(0, "Error", "Could not open input file");
        return;
    }

    QFile outputFile(outputFileName);
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(0, "Error", "Could not open output file for writing");
        return;
    }

    QTextStream in(&inputFile);
    QTextStream out(&outputFile);
    out << in.readAll();

    inputFile.close();
    outputFile.close();
    *originalFileName=outputFileName;
}

void lscpedit::on_actionSave_As_triggered()
{
    saveAsFile(filename);
    removeFile(filename);
    QFileInfo fileInfo(*originalFileName);
    QString directoryPath = fileInfo.absolutePath();
    QString name = fileInfo.fileName();
    *filename=directoryPath+"/.~"+name;
    createTempFile(originalFileName,filename);
    printFiletoTable(filename,0);
    label2->setText("File : " + *originalFileName);
}



void lscpedit::on_actionQuit_triggered()
{
    lscpedit::close();
}

void lscpedit::createNewFile(){
    QString newfileName = QFileDialog::getSaveFileName(nullptr, "Create New File",  QDir::homePath(), "LSCP files (*.lscp)");
    if (!newfileName.isEmpty()) {
        QFile file(newfileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file.close();
        }
    }
     *originalFileName=newfileName;

}

void lscpedit::on_actionNew_triggered()
{
    createNewFile();
    QFileInfo fileInfo(*originalFileName);
    QString directoryPath = fileInfo.absolutePath();
    QString name = fileInfo.fileName();
    *filename=directoryPath+"/.~"+name;
    createTempFile(originalFileName,filename);
    printFiletoTable(filename,0);
     label2->setText("File : " + *originalFileName);
}


void lscpedit::on_bank_spinBox_valueChanged(int arg1)
{
    QModelIndex index = ui->tableView->currentIndex();
    int row = index.row();

    QAbstractItemModel *model = ui->tableView->model();
    int column = 0;
    model->setData(model->index(row, column), arg1);

    // Emit the dataChanged signal to refresh the view
    QModelIndex startIndex = model->index(row, column);
    QModelIndex endIndex = model->index(row, column);
    emit model->dataChanged(startIndex, endIndex);
    saveMapToFile(filename);
}


void lscpedit::on_instFilePath_lineEdit_textChanged(const QString &arg1)
{
    QModelIndex index = ui->tableView->currentIndex();
    int row = index.row();

    QAbstractItemModel *model = ui->tableView->model();
    int column = 3;
    model->setData(model->index(row, column), arg1);

    // Emit the dataChanged signal to refresh the view
    QModelIndex startIndex = model->index(row, column);
    QModelIndex endIndex = model->index(row, column);
    emit model->dataChanged(startIndex, endIndex);
    saveMapToFile(filename);
}




void lscpedit::on_nameGig_lineEdit_textChanged(const QString &arg1)
{
    QModelIndex index = ui->tableView->currentIndex();
    int row = index.row();

    QAbstractItemModel *model = ui->tableView->model();
    int column = 6;
    model->setData(model->index(row, column), arg1);

    // Emit the dataChanged signal to refresh the view
    QModelIndex startIndex = model->index(row, column);
    QModelIndex endIndex = model->index(row, column);
    emit model->dataChanged(startIndex, endIndex);
    saveMapToFile(filename);
}


void lscpedit::on_volume_doubleSpinBox_valueChanged(double arg1)
{
    QModelIndex index = ui->tableView->currentIndex();
    int row = index.row();

    QAbstractItemModel *model = ui->tableView->model();
    int column = 4;
    QString str = QString::number(arg1, 'g', 1);
    model->setData(model->index(row, column), str);

    // Emit the dataChanged signal to refresh the view
    QModelIndex startIndex = model->index(row, column);
    QModelIndex endIndex = model->index(row, column);
    emit model->dataChanged(startIndex, endIndex);
    saveMapToFile(filename);
}


void lscpedit::on_prog_spinBox_valueChanged(int arg1)
{
    QModelIndex index = ui->tableView->currentIndex();
    int row = index.row();

    QAbstractItemModel *model = ui->tableView->model();
    int column = 1;
    model->setData(model->index(row, column), arg1);

    // Emit the dataChanged signal to refresh the view
    QModelIndex startIndex = model->index(row, column);
    QModelIndex endIndex = model->index(row, column);
    emit model->dataChanged(startIndex, endIndex);
    saveMapToFile(filename);
}


void lscpedit::on_engine_comboBox_currentIndexChanged(int index)
{

    QString engine_Word;
    switch(index)
    {
    case 0:
        engine_Word="GIG";
        break;
    case 1:
        engine_Word="SF2";
        break;
    case 2:
        engine_Word="SFZ";
        break;
    default:
        break;
    }
    QModelIndex index1 = ui->tableView->currentIndex();
    int row = index1.row();

    QAbstractItemModel *model = ui->tableView->model();
    int column = 2;
    model->setData(model->index(row, column), engine_Word);

    // Emit the dataChanged signal to refresh the view
    QModelIndex startIndex = model->index(row, column);
    QModelIndex endIndex = model->index(row, column);
    emit model->dataChanged(startIndex, endIndex);
    saveMapToFile(filename);
}


void lscpedit::on_loadMode_comboBox_currentIndexChanged(int index)
{

    QString mode_Word;
    switch(index)
    {
    case 0:
        mode_Word="ON_DEMAND";
        break;
    case 1:
        mode_Word="ON_DEMAND";
        break;
    case 2:
        mode_Word="ON_DEMAND_HOLD";
        break;
    case 3:
        mode_Word="PERSISTENT";
        break;
    default:
        break;
    }

    QModelIndex index1 = ui->tableView->currentIndex();
    int row = index1.row();

    QAbstractItemModel *model = ui->tableView->model();
    int column = 5;
    model->setData(model->index(row, column), mode_Word);

    // Emit the dataChanged signal to refresh the view
    QModelIndex startIndex = model->index(row, column);
    QModelIndex endIndex = model->index(row, column);
    emit model->dataChanged(startIndex, endIndex);
    saveMapToFile(filename);
}


void lscpedit::on_resetName_pushButton_clicked()
{
    if(gigFileName!=nullptr)
    {
        ui->instFilePath_lineEdit->setText(*gigFileName);
        QFileInfo fileInfo(*gigFileName);
        QString name = fileInfo.baseName();
        ui->nameGig_lineEdit->setText(name);
    }
}


void lscpedit::on_actionAbout_triggered()
{
    about abt;
    abt.exec();
}


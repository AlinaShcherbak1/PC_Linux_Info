#include "mainwindow.h"
#include "ui_mainwindow.h"


void setSets(QPushButton *button, QString image_name){
    button->setFixedWidth(100);
    button->setFixedHeight(100);
    button->setStyleSheet("QPushButton {"
                           "    border: 5px solid #8f8f91;"
                           "    border-radius: 15px;"
                           "    padding: 6px;"
                           "    border-image: url(:"+image_name+ ".png);"
                                          "    background-color: #1F9999;"
                                          "}"
                                          "QPushButton:hover {"
                                          "    background-color: #26B5B5;"
                                          "}"
                                          "QPushButton:pressed {"
                                          "    background-color: #1B8383;"
                                          "}");

}



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->first_column->setContentsMargins(0,10,0,0);
    ui->second_column->setContentsMargins(0,10,0,0);
    ui->first_column->setStyleSheet("font-family: JetBrains Mono NL; font-size: 18px; color: black;border: 5px solid #54D3AF;"
                                    "    border-radius: 15px;"
                                    "    padding: 6px;"
                                    "    background-color: #54D3AF;");
    ui->second_column->setStyleSheet("font-family: JetBrains Mono NL; font-size: 18px; color: black;border: 5px solid #54D3AF;"
                                     "    border-radius: 15px;"
                                     "    padding: 6px;"
                                     "    background-color: #54D3AF;");


    setSets(ui->CPU,"CPU");
    setSets(ui->RAM,"RAM");
    setSets(ui->GPU,"GPU");
    setSets(ui->MONITOR,"MONITOR");
    setSets(ui->BASEBOARD,"BASEBOARD");

   }


MainWindow::~MainWindow()
{
    delete ui;
}

void Clear_set_text(QLabel *f_label,QLabel *s_label, QString &first_column,  QString &second_column,QStringList &arguments){
    f_label->setText(first_column);
    s_label->setText(second_column);
    arguments.clear();
    first_column.clear();
    second_column.clear();
}

void set_text(QLabel *section_name,const QString &name ){
    section_name->setText(name);
}


void MainWindow::split(QString &result){

    QTextStream stream(&result);
    while(!stream.atEnd()){
        currentLine=stream.readLine();
        found = currentLine.indexOf(":");
        if(found!=-1){
            first_column.append(currentLine.left(found) + "\n");
            second_column.append(currentLine.mid(found+1) + "\n");
        }
        else{
            first_column.append(currentLine + "\n");
            second_column.append(currentLine + "\n");
        }

    }
}


void MainWindow::on_CPU_clicked()
{
    arguments<<"-E" << "model name|stepping|microcode|cpu MHz|cache size" << "/proc/cpuinfo";
    process.start("grep",arguments);
    process.waitForFinished();
    QString result = QString::fromUtf8(process.readAllStandardOutput());

    result.replace("\t", "");

    found = result.indexOf("KB");
    while(found!=-1){
        result.insert(found+2,"\n");
        found= result.indexOf("KB", found + 1);
    }

    split(result);
    Clear_set_text(ui->first_column,ui->second_column,first_column,second_column,arguments);
    set_text(ui->section_name,"CENTRAL PROCESSING UNIT");
}



void MainWindow::on_RAM_clicked()
{

    arguments << "--type" << "17";

    process.start("dmidecode", arguments);
    process.waitForFinished();

    QByteArray result = process.readAll();
    QStringList lines = QString(result).split('\n');

    QString filteredOutput;
    foreach (const QString &line, lines) {
        if (line.contains("Size") || line.contains("Configured Voltage")
           ||line.contains("Form Factor") || line.contains("Locator") || line.contains("Type Details")
           ||line.contains("Speed") || line.contains("Manufacturer") || line.contains("Type Details")
           ||line.contains("Form Factor") || line.contains("Locator"))
{
            filteredOutput.append(line + "\n");
        }
    }
     filteredOutput.replace("\t", "");
     found = filteredOutput.indexOf("Voltage: 1.2 V");
    while (found != std::string::npos) {
        filteredOutput.insert(found+14, "\n");
        found = filteredOutput.indexOf("Voltage: 1.2 V", found + 1);
    }

    split(filteredOutput);
    Clear_set_text(ui->first_column,ui->second_column,first_column,second_column,arguments);
    set_text(ui->section_name,"RANDOM ACCESS MEMORY");
}



void MainWindow::on_GPU_clicked()
{

    if(gpu_first_column.isEmpty() && gpu_second_column.isEmpty())
    {

        process.setProcessChannelMode(QProcess::MergedChannels);
        arguments << "-c" << "video";

        process.start("lshw", arguments);
        process.waitForFinished();

        QByteArray result = process.readAll();
        QStringList lines = QString(result).split('\n');

        QString filteredOutput;
        foreach (const QString &line, lines) {
            if (line.contains("description") || line.contains("product") || line.contains("vendor")
                ||line.contains("width") || line.contains("clock"))
            {
                filteredOutput.append(line + "\n");
            }
        }

        QTextStream stream(&filteredOutput);

        while(!stream.atEnd()){
            currentLine=stream.readLine();
            found =currentLine.indexOf(":");
            if(found!=-1){
                gpu_first_column.append( currentLine.left(found)+"\n");
                gpu_second_column.append( currentLine.mid(found+1)+"\n");
            }
            else{
                gpu_first_column.append( currentLine + "\n");
                gpu_second_column.append( currentLine + "\n");
            }
        }
    }

    gpu_first_column.replace(" ","");

    ui->first_column->setText(gpu_first_column);
    ui->second_column->setText(gpu_second_column);
    arguments.clear();
    set_text(ui->section_name,"GRAPHICS PROCESSING UNIT");
 }


void MainWindow::on_MONITOR_clicked()
{

    process.setProcessChannelMode(QProcess::MergedChannels);
    arguments << "-c" << "xrandr | awk '{print $1,$2}' | sed -n '3p' | sed 's/\\*+//g' | sed 's/x/ /g'";

    process.start("bash", arguments);
    process.waitForFinished();
    QString result = QString::fromUtf8(process.readAll());
    QString monitor;


    for(int i =0; i!=result.length(); i++ ){
        if(result[i]==' ')
            monitor+="\n";
        else monitor+= result[i];
    }
    first_column.append("height \nwidth  \nfrequency");


   Clear_set_text(ui->first_column,ui->second_column,first_column,monitor,arguments);
   set_text(ui->section_name,"DISPLAY");

}


void MainWindow::on_BASEBOARD_clicked()
{
    process.setProcessChannelMode(QProcess::MergedChannels);

    arguments << "-t" << "baseboard";

    process.start("dmidecode", arguments);
    process.waitForFinished();

    QByteArray result = process.readAll();
    QStringList lines = QString(result).split('\n');

    QString filteredOutput;
    foreach (const QString &line, lines) {
        if (line.contains("Manufacturer") || line.contains("Product Name") || line.contains("Version")
            ||line.contains("Serial Number"))
        {
            filteredOutput.append(line + "\n");
        }
    }


    split(filteredOutput);
    first_column.replace("\t","");

    Clear_set_text(ui->first_column,ui->second_column,first_column,second_column,arguments);
    set_text(ui->section_name,"BASEBOARD");

}


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <net/if.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    const char* CURRENT_PATH = get_current_dir_name();
    std::string s;
    s.append(CURRENT_PATH);
    s.append( "/Data/");
    const char* DATA_PATH =s.c_str();
    const char* PATH = "/sys/class/net/";
    const QString blank = "";
    /*
     * Einbinden aller Netzwerk Devices
    */

    ui->cb_technology->addItem(blank);
    DIR *dir = opendir(DATA_PATH);
    struct dirent *entry = readdir(dir);
    while(entry != NULL){
       if(entry->d_name[0] != '.'){
            QString qstr = QString::fromStdString(entry->d_name);
            ui->cb_technology->addItem(qstr);
       }
        entry = readdir(dir);
    }
    closedir(dir);


    /*
     * Einbinden aller Netzwerk Devices
    */

    dir = opendir(PATH);
    entry = readdir(dir);
    while(entry != NULL){
       if(entry->d_type != DT_DIR){
            QString qstr = QString::fromStdString(entry->d_name);
            ui->cb_interfaces->addItem(qstr);
       }
        entry = readdir(dir);
    }
    closedir(dir);




}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_cb_technology_currentTextChanged(const QString &arg1)
{
    const QString blank = "";
    ui->cb_region->clear();
    if (arg1 != blank){
        ui->cb_region->setEnabled(true);
        const char* CURRENT_PATH = get_current_dir_name();
        std::string s;
        s.append(CURRENT_PATH);
        s.append( "/Data/");
        s.append(arg1.toStdString().c_str());
        s.append("/");
        const char* DATA_PATH =s.c_str();
        const QString blank = "";
        /*
         * Einbinden aller Netzwerk Devices
        */

        ui->cb_region->addItem(blank);
        DIR *dir = opendir(DATA_PATH);
        struct dirent *entry = readdir(dir);
        while(entry != NULL){
           if(entry->d_name[0] != '.'){
                QString qstr = QString::fromStdString(entry->d_name);
                ui->cb_region->addItem(qstr);
           }
            entry = readdir(dir);
        }
        closedir(dir);

    }else{
         ui->cb_region->setEnabled(false);
    }
}


void MainWindow::on_cb_region_currentTextChanged(const QString &arg1){
    const QString blank = "";
    ui->cb_measurement->clear();
    if (arg1 != blank){
        ui->cb_measurement->setEnabled(true);
        const char* CURRENT_PATH = get_current_dir_name();
        std::string s;
        s.append(CURRENT_PATH);
        s.append( "/Data/");
        s.append(ui->cb_technology->currentText().toStdString().c_str());
        s.append("/");
        s.append(arg1.toStdString().c_str());
        s.append("/");
        const char* DATA_PATH =s.c_str();
        const QString blank = "";
        /*
         * Einbinden aller Netzwerk Devices
        */

        ui->cb_measurement->addItem(blank);
        DIR *dir = opendir(DATA_PATH);
        struct dirent *entry = readdir(dir);
        while(entry != NULL){
           if(entry->d_name[0] != '.'){
                QString qstr = QString::fromStdString(entry->d_name);
                ui->cb_measurement->addItem(qstr);
           }
            entry = readdir(dir);
        }
        closedir(dir);

    }else{
         ui->cb_measurement->setEnabled(false);
    }
}

void MainWindow::on_cb_measurement_currentTextChanged(const QString &arg1){
    const QString blank = "";
    if (arg1 != blank){
        const char* CURRENT_PATH = get_current_dir_name();
        std::string s;
        s.append(CURRENT_PATH);
        s.append( "/Data/");
        s.append(ui->cb_technology->currentText().toStdString().c_str());
        s.append("/");
        s.append(ui->cb_region->currentText().toStdString().c_str());
        s.append("/");
        s.append(arg1.toStdString().c_str());
        s.append("/");
        QString qs;
        qs = qs.fromStdString(s);
        qs.append("delay");
        ui->le_de_file->setText(qs);
        qs = qs.fromStdString(s);
        qs.append("loss");
        ui->le_pl_file->setText(qs);
    }else{
         ui->le_de_file->setText(blank);
         ui->le_pl_file->setText(blank);
    }
}

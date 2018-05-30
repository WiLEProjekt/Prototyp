/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QTabWidget *tabWidget;
    QWidget *tab;
    QWidget *tab_2;
    QPushButton *btnRun;
    QPushButton *btnGenerate;
    QPushButton *btnSave;
    QComboBox *cbModel;
    QLabel *lblModel;
    QMenuBar *menuBar;
    QMenu *menuTrace_Generator;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(461, 442);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setGeometry(QRect(10, 60, 341, 191));
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        tabWidget->addTab(tab_2, QString());
        btnRun = new QPushButton(centralWidget);
        btnRun->setObjectName(QStringLiteral("btnRun"));
        btnRun->setGeometry(QRect(360, 350, 89, 25));
        btnGenerate = new QPushButton(centralWidget);
        btnGenerate->setObjectName(QStringLiteral("btnGenerate"));
        btnGenerate->setGeometry(QRect(260, 350, 89, 25));
        btnSave = new QPushButton(centralWidget);
        btnSave->setObjectName(QStringLiteral("btnSave"));
        btnSave->setGeometry(QRect(60, 350, 89, 25));
        cbModel = new QComboBox(centralWidget);
        cbModel->setObjectName(QStringLiteral("cbModel"));
        cbModel->setGeometry(QRect(80, 10, 86, 25));
        lblModel = new QLabel(centralWidget);
        lblModel->setObjectName(QStringLiteral("lblModel"));
        lblModel->setGeometry(QRect(10, 10, 67, 17));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 461, 22));
        menuTrace_Generator = new QMenu(menuBar);
        menuTrace_Generator->setObjectName(QStringLiteral("menuTrace_Generator"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuTrace_Generator->menuAction());

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainWindow", "Tab 1", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainWindow", "Tab 2", 0));
        btnRun->setText(QApplication::translate("MainWindow", "Run", 0));
        btnGenerate->setText(QApplication::translate("MainWindow", "Generate", 0));
        btnSave->setText(QApplication::translate("MainWindow", "Speichern", 0));
        lblModel->setText(QApplication::translate("MainWindow", "Model", 0));
        menuTrace_Generator->setTitle(QApplication::translate("MainWindow", "Paketloss", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

/********************************************************************************
** Form generated from reading UI file 'FelixGUI.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FELIXGUI_H
#define UI_FELIXGUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FelixGUIClass
{
public:
    QAction *actionLaunch;
    QAction *actionExit;
    QAction *actionSelect_Directory;
    QAction *actionExisting_Process;
    QWidget *centralWidget;
    QLabel *label;
    QMenuBar *menuBar;
    QMenu *menuProcess;
    QMenu *menuFile_2;

    void setupUi(QMainWindow *FelixGUIClass)
    {
        if (FelixGUIClass->objectName().isEmpty())
            FelixGUIClass->setObjectName(QStringLiteral("FelixGUIClass"));
        FelixGUIClass->resize(1300, 800);
        FelixGUIClass->setMinimumSize(QSize(0, 0));
        FelixGUIClass->setMaximumSize(QSize(1300, 1000));
        actionLaunch = new QAction(FelixGUIClass);
        actionLaunch->setObjectName(QStringLiteral("actionLaunch"));
        actionExit = new QAction(FelixGUIClass);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionSelect_Directory = new QAction(FelixGUIClass);
        actionSelect_Directory->setObjectName(QStringLiteral("actionSelect_Directory"));
        actionExisting_Process = new QAction(FelixGUIClass);
        actionExisting_Process->setObjectName(QStringLiteral("actionExisting_Process"));
        centralWidget = new QWidget(FelixGUIClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(0, 877, 1200, 13));
        label->setTextFormat(Qt::AutoText);
        label->setScaledContents(false);
        label->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);
        FelixGUIClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(FelixGUIClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1300, 21));
        menuProcess = new QMenu(menuBar);
        menuProcess->setObjectName(QStringLiteral("menuProcess"));
        menuFile_2 = new QMenu(menuBar);
        menuFile_2->setObjectName(QStringLiteral("menuFile_2"));
        FelixGUIClass->setMenuBar(menuBar);

        menuBar->addAction(menuFile_2->menuAction());
        menuBar->addAction(menuProcess->menuAction());
        menuProcess->addAction(actionLaunch);
        menuProcess->addAction(actionExisting_Process);
        menuFile_2->addAction(actionExit);

        retranslateUi(FelixGUIClass);

        QMetaObject::connectSlotsByName(FelixGUIClass);
    } // setupUi

    void retranslateUi(QMainWindow *FelixGUIClass)
    {
        FelixGUIClass->setWindowTitle(QApplication::translate("FelixGUIClass", "Felix", Q_NULLPTR));
        actionLaunch->setText(QApplication::translate("FelixGUIClass", "Launch", Q_NULLPTR));
        actionExit->setText(QApplication::translate("FelixGUIClass", "Exit", Q_NULLPTR));
        actionSelect_Directory->setText(QApplication::translate("FelixGUIClass", "Select Directory", Q_NULLPTR));
        actionExisting_Process->setText(QApplication::translate("FelixGUIClass", "Existing Process", Q_NULLPTR));
        label->setText(QApplication::translate("FelixGUIClass", "Felix - File Events Logging, Interrupting and Examining", Q_NULLPTR));
        menuProcess->setTitle(QApplication::translate("FelixGUIClass", "Process", Q_NULLPTR));
        menuFile_2->setTitle(QApplication::translate("FelixGUIClass", "File", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class FelixGUIClass: public Ui_FelixGUIClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FELIXGUI_H

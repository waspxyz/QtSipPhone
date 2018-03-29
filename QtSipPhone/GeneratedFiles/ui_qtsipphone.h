/********************************************************************************
** Form generated from reading UI file 'qtsipphone.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTSIPPHONE_H
#define UI_QTSIPPHONE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtSipPhoneClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QtSipPhoneClass)
    {
        if (QtSipPhoneClass->objectName().isEmpty())
            QtSipPhoneClass->setObjectName(QStringLiteral("QtSipPhoneClass"));
        QtSipPhoneClass->resize(600, 400);
        menuBar = new QMenuBar(QtSipPhoneClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        QtSipPhoneClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QtSipPhoneClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        QtSipPhoneClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(QtSipPhoneClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        QtSipPhoneClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(QtSipPhoneClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        QtSipPhoneClass->setStatusBar(statusBar);

        retranslateUi(QtSipPhoneClass);

        QMetaObject::connectSlotsByName(QtSipPhoneClass);
    } // setupUi

    void retranslateUi(QMainWindow *QtSipPhoneClass)
    {
        QtSipPhoneClass->setWindowTitle(QApplication::translate("QtSipPhoneClass", "QtSipPhone", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QtSipPhoneClass: public Ui_QtSipPhoneClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTSIPPHONE_H

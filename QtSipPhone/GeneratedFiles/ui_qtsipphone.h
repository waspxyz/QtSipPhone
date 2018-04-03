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
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtSipPhoneClass
{
public:
    QAction *actionSet;
    QAction *actionExit;
    QAction *actionCall1;
    QAction *actionCall2;
    QAction *actionAbout;
    QAction *actionPlay;
    QAction *actionStop;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menu;
    QMenu *menu_2;
    QMenu *menu_3;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QtSipPhoneClass)
    {
        if (QtSipPhoneClass->objectName().isEmpty())
            QtSipPhoneClass->setObjectName(QStringLiteral("QtSipPhoneClass"));
        QtSipPhoneClass->resize(761, 515);
        actionSet = new QAction(QtSipPhoneClass);
        actionSet->setObjectName(QStringLiteral("actionSet"));
        actionExit = new QAction(QtSipPhoneClass);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionCall1 = new QAction(QtSipPhoneClass);
        actionCall1->setObjectName(QStringLiteral("actionCall1"));
        actionCall2 = new QAction(QtSipPhoneClass);
        actionCall2->setObjectName(QStringLiteral("actionCall2"));
        actionAbout = new QAction(QtSipPhoneClass);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        actionPlay = new QAction(QtSipPhoneClass);
        actionPlay->setObjectName(QStringLiteral("actionPlay"));
        actionStop = new QAction(QtSipPhoneClass);
        actionStop->setObjectName(QStringLiteral("actionStop"));
        centralWidget = new QWidget(QtSipPhoneClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        QtSipPhoneClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(QtSipPhoneClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 761, 26));
        menu = new QMenu(menuBar);
        menu->setObjectName(QStringLiteral("menu"));
        menu_2 = new QMenu(menuBar);
        menu_2->setObjectName(QStringLiteral("menu_2"));
        menu_3 = new QMenu(menuBar);
        menu_3->setObjectName(QStringLiteral("menu_3"));
        QtSipPhoneClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QtSipPhoneClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        QtSipPhoneClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(QtSipPhoneClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        QtSipPhoneClass->setStatusBar(statusBar);

        menuBar->addAction(menu->menuAction());
        menuBar->addAction(menu_2->menuAction());
        menuBar->addAction(menu_3->menuAction());
        menu->addAction(actionSet);
        menu->addSeparator();
        menu->addAction(actionExit);
        menu_2->addAction(actionCall1);
        menu_2->addAction(actionCall2);
        menu_2->addAction(actionPlay);
        menu_2->addAction(actionStop);
        menu_3->addAction(actionAbout);

        retranslateUi(QtSipPhoneClass);

        QMetaObject::connectSlotsByName(QtSipPhoneClass);
    } // setupUi

    void retranslateUi(QMainWindow *QtSipPhoneClass)
    {
        QtSipPhoneClass->setWindowTitle(QApplication::translate("QtSipPhoneClass", "QtSipPhone", nullptr));
        actionSet->setText(QApplication::translate("QtSipPhoneClass", "\350\256\276\347\275\256", nullptr));
        actionExit->setText(QApplication::translate("QtSipPhoneClass", "\351\200\200\345\207\272", nullptr));
        actionCall1->setText(QApplication::translate("QtSipPhoneClass", "\347\224\265\350\257\2351", nullptr));
        actionCall2->setText(QApplication::translate("QtSipPhoneClass", "\347\224\265\350\257\2352", nullptr));
        actionAbout->setText(QApplication::translate("QtSipPhoneClass", "\345\205\263\344\272\216", nullptr));
        actionPlay->setText(QApplication::translate("QtSipPhoneClass", "\345\233\236\346\224\276", nullptr));
        actionStop->setText(QApplication::translate("QtSipPhoneClass", "\345\201\234\346\255\242", nullptr));
        menu->setTitle(QApplication::translate("QtSipPhoneClass", "\346\226\207\344\273\266", nullptr));
        menu_2->setTitle(QApplication::translate("QtSipPhoneClass", "\345\267\245\345\205\267", nullptr));
        menu_3->setTitle(QApplication::translate("QtSipPhoneClass", "\345\270\256\345\212\251", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QtSipPhoneClass: public Ui_QtSipPhoneClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTSIPPHONE_H

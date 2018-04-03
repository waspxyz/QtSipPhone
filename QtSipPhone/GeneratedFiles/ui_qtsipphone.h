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
    QAction *action_regcall;
    QAction *action_call;
    QAction *actionAbout;
    QAction *actionPlay;
    QAction *actionStop;
    QAction *action_rec;
    QAction *action_stoprec;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menu;
    QMenu *menu_2;
    QMenu *menu_3;
    QMenu *menu_4;
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
        action_regcall = new QAction(QtSipPhoneClass);
        action_regcall->setObjectName(QStringLiteral("action_regcall"));
        action_call = new QAction(QtSipPhoneClass);
        action_call->setObjectName(QStringLiteral("action_call"));
        actionAbout = new QAction(QtSipPhoneClass);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        actionPlay = new QAction(QtSipPhoneClass);
        actionPlay->setObjectName(QStringLiteral("actionPlay"));
        actionStop = new QAction(QtSipPhoneClass);
        actionStop->setObjectName(QStringLiteral("actionStop"));
        action_rec = new QAction(QtSipPhoneClass);
        action_rec->setObjectName(QStringLiteral("action_rec"));
        action_stoprec = new QAction(QtSipPhoneClass);
        action_stoprec->setObjectName(QStringLiteral("action_stoprec"));
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
        menu_4 = new QMenu(menuBar);
        menu_4->setObjectName(QStringLiteral("menu_4"));
        QtSipPhoneClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QtSipPhoneClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        QtSipPhoneClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(QtSipPhoneClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        QtSipPhoneClass->setStatusBar(statusBar);

        menuBar->addAction(menu->menuAction());
        menuBar->addAction(menu_2->menuAction());
        menuBar->addAction(menu_4->menuAction());
        menuBar->addAction(menu_3->menuAction());
        menu->addAction(actionSet);
        menu->addSeparator();
        menu->addAction(actionExit);
        menu_2->addAction(action_regcall);
        menu_2->addAction(action_call);
        menu_3->addAction(actionAbout);
        menu_4->addAction(actionPlay);
        menu_4->addAction(actionStop);
        menu_4->addAction(action_rec);
        menu_4->addAction(action_stoprec);

        retranslateUi(QtSipPhoneClass);

        QMetaObject::connectSlotsByName(QtSipPhoneClass);
    } // setupUi

    void retranslateUi(QMainWindow *QtSipPhoneClass)
    {
        QtSipPhoneClass->setWindowTitle(QApplication::translate("QtSipPhoneClass", "QtSipPhone", nullptr));
        actionSet->setText(QApplication::translate("QtSipPhoneClass", "\350\256\276\347\275\256", nullptr));
        actionExit->setText(QApplication::translate("QtSipPhoneClass", "\351\200\200\345\207\272", nullptr));
        action_regcall->setText(QApplication::translate("QtSipPhoneClass", "\346\263\250\345\206\214\347\224\265\350\257\235", nullptr));
        action_call->setText(QApplication::translate("QtSipPhoneClass", "\346\213\250\346\211\223\347\224\265\350\257\235", nullptr));
        actionAbout->setText(QApplication::translate("QtSipPhoneClass", "\345\205\263\344\272\216", nullptr));
        actionPlay->setText(QApplication::translate("QtSipPhoneClass", "\350\257\255\351\237\263\346\222\255\346\224\276", nullptr));
        actionStop->setText(QApplication::translate("QtSipPhoneClass", "\345\201\234\346\255\242\346\222\255\346\224\276", nullptr));
        action_rec->setText(QApplication::translate("QtSipPhoneClass", "\345\221\274\345\205\245\345\275\225\351\237\263", nullptr));
        action_stoprec->setText(QApplication::translate("QtSipPhoneClass", "\345\201\234\346\255\242\345\275\225\351\237\263", nullptr));
        menu->setTitle(QApplication::translate("QtSipPhoneClass", "\346\226\207\344\273\266", nullptr));
        menu_2->setTitle(QApplication::translate("QtSipPhoneClass", "\347\224\265\350\257\235\345\267\245\345\205\267", nullptr));
        menu_3->setTitle(QApplication::translate("QtSipPhoneClass", "\345\270\256\345\212\251", nullptr));
        menu_4->setTitle(QApplication::translate("QtSipPhoneClass", "\350\257\255\351\237\263\345\267\245\345\205\267", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QtSipPhoneClass: public Ui_QtSipPhoneClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTSIPPHONE_H

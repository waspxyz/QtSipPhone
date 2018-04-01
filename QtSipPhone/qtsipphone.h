#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_qtsipphone.h"
#include <csipaccount.h>

class QtSipPhone : public QMainWindow
{
	Q_OBJECT

public:
	QtSipPhone(QWidget *parent = Q_NULLPTR);

private:
	void exitApp();

	void callPhone();

	void callPhone2();
	
private:
	Ui::QtSipPhoneClass ui;
	cSipAccount m_sipphone;
};

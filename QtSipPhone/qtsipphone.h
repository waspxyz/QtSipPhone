#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_qtsipphone.h"

class QtSipPhone : public QMainWindow
{
	Q_OBJECT

public:
	QtSipPhone(QWidget *parent = Q_NULLPTR);

private:
	Ui::QtSipPhoneClass ui;
};

#include "qtsipphone.h"

QtSipPhone::QtSipPhone(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	setWindowTitle(tr("Auto SIP Phone @JianJie v0.0.1"));

	connect(ui.actionExit, &QAction::triggered, this, &QtSipPhone::exitApp);

	connect(ui.actionCall1, &QAction::triggered, this, &QtSipPhone::callPhone);

	connect(ui.actionCall2, &QAction::triggered, this, &QtSipPhone::callPhone2);

	connect(ui.actionPlay, &QAction::triggered, this, &QtSipPhone::playBack);

	connect(ui.actionStop, &QAction::triggered, this, &QtSipPhone::stopPlay);

	m_sipphone.init_sphone();
}

void QtSipPhone::exitApp()
{
	exit(0);
}

void QtSipPhone::callPhone()
{
	m_sipphone.reg_user(u8"102", u8"192.168.3.11", u8"102");
}

void QtSipPhone::callPhone2()
{
	m_sipphone.reg_user(u8"100", u8"192.168.3.11", u8"100");
}

void QtSipPhone::playBack()
{
	m_sipphone.playback();
}

void QtSipPhone::stopPlay()
{
	m_sipphone.stopplayback();
}
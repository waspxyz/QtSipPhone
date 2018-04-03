#include "qtsipphone.h"

QtSipPhone::QtSipPhone(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	setWindowTitle(tr("Auto SIP Phone @JianJie v0.0.1"));

	connect(ui.actionExit, &QAction::triggered, this, &QtSipPhone::exitApp);

	connect(ui.action_regcall, &QAction::triggered, this, &QtSipPhone::registerPhone);

	connect(ui.action_call, &QAction::triggered, this, &QtSipPhone::callPhone);

	connect(ui.actionPlay, &QAction::triggered, this, &QtSipPhone::playBack);

	connect(ui.actionStop, &QAction::triggered, this, &QtSipPhone::stopPlay);

	connect(ui.action_rec, &QAction::triggered, this, &QtSipPhone::startrecord);

	connect(ui.action_stoprec, &QAction::triggered, this, &QtSipPhone::stoprecord);

	m_sipphone.init_sphone();
}

void QtSipPhone::exitApp()
{
	exit(0);
}

void QtSipPhone::registerPhone()
{
	m_sipphone.reg_user(u8"102", u8"192.168.3.11", u8"102");
}

void QtSipPhone::callPhone()
{
	m_sipphone.makecall(u8"sip:101@192.168.3.11");
}

void QtSipPhone::playBack()
{
	m_sipphone.playback();
}

void QtSipPhone::stopPlay()
{
	m_sipphone.stopplayback();
}

void QtSipPhone::startrecord()
{
	m_sipphone.startrecord();
}

void QtSipPhone::stoprecord()
{
	m_sipphone.stoprecord();
}
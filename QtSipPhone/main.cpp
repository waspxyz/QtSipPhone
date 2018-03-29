#include "qtsipphone.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QtSipPhone w;
	w.show();
	return a.exec();
}

#include "FelixGUI.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	FelixGUI w;
	w.show();
	return a.exec();
}

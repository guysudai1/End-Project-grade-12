#include "FelixGUI.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QIcon appIcon;
	appIcon.addFile(":/FelixGUI/rabbit");
	FelixGUI window;
	window.setWindowIcon(appIcon);
	window.show();
	return app.exec();
}

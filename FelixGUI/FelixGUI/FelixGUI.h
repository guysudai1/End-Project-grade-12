#pragma once

#include <Windows.h>
#include <QtWidgets/QMainWindow>
#include <QTableWidget>
#include "ui_FelixGUI.h"


class MyActions : public QObject
{
	Q_OBJECT

public:
	DWORD pid;
	QString procName;
	MyActions();
public slots:
	void on_action_launch();
	void on_action_select_directory();
	void on_action_exit();


};

class FelixGUI : public QMainWindow
{
	Q_OBJECT

public:
	FelixGUI(QWidget *parent = Q_NULLPTR);
	void handleContextMenu(const QPoint& pos);
	~FelixGUI();
	MyActions actions;
public slots:
	void close_tab(int);
	void on_action_inject();
	void on_action_view();
private:
	QWidget* generate_newtab();
	Ui::FelixGUIClass ui;
	QTabWidget* tabs;
	QTableWidget* tbl;

};

void draw_processes(QTableWidget*);



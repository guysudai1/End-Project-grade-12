#pragma once

#include <Windows.h>
#include <QtWidgets/QMainWindow>
#include <QTableWidget>
#include "ui_FelixGUI.h"
#include <vector>

class MyActions : public QObject
{
	Q_OBJECT

public:
	DWORD pid;
	QString procName;
	MyActions();
	std::vector< std::pair<DWORD, QTableWidget*>> processes;
public slots:
	void on_action_select_directory();
	void on_action_exit();
};

class FelixGUI : public QMainWindow
{
	Q_OBJECT

public:
	static void WINAPI startThread(void* This);
	FelixGUI(QWidget *parent = Q_NULLPTR);
	void handleContextMenu(const QPoint& pos);
	~FelixGUI();
	MyActions actions;
signals:
	void addToTable(unsigned int index, const wchar_t* path, const char* mode, wchar_t* time, unsigned int isFile);
public slots:
	void on_action_launch();
	void addToProcTable(unsigned int index, const wchar_t* path, const char* mode, wchar_t* time, unsigned int isFile);
	void close_tab(int);
	void on_action_inject();
	void on_action_view();
private:
	void recv_proc_info();
	QWidget* generate_newtab();
	Ui::FelixGUIClass ui;
	QTabWidget* tabs;
	QTableWidget* tbl;

};

void draw_processes(QTableWidget*);



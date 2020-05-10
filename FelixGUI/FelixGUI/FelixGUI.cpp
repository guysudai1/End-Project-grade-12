#include "FelixGUI.h"
#include "graphics.h"
#include "inject.h"
#include "icon_functions.h"
#include "proc.h"

#include <QScrollArea>
#include <QHBoxLayout>
#include <psapi.h>

#define UPDATE_RATE 3	   // seconds
#define TRANSLATION_SIZE 8 // bytes

#define ROW_HEIGHT 18
#define TABS_WIDTH 1300
#define TABS_HEIGHT 800

#define PROC_INFO_WIDTH 800
#define PROC_INFO_HEIGHT 300

#define TAB_WIDTH 1294
#define TAB_HEIGHT 774

#define NAME_COL 0
#define PID_COL 1

#define HOME_ICON_PATH "Resources/Icons/home.png"
#define PROCESS_ICON_PATH "Resources/Icons/process.png"
#define PID_ICON_PATH "Resources/Icons/pid.png"
#define COMMENT_ICON_PATH "Resources/Icons/comment.png"
#define EXECPATH_ICON_PATH "Resources/Icons/interface.png"
#define DLL_ICON_PATH "Resources/Icons/dll.png"
#define EXE_ICON_PATH "Resources/Icons/exe.ico"
#define INJECT_ICON_PATH "Resources/Icons/inject.png"
#define VIEW_ICON_PATH "Resources/Icons/view.png"

#define COLUMN_AMOUNT 4

#define GCL_HICONSM -34


FelixGUI::FelixGUI(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	QObject::connect(ui.actionLaunch, SIGNAL(triggered()),
					 &this->actions, SLOT(on_action_launch()));
	QObject::connect(ui.actionExit, SIGNAL(triggered()),
		&this->actions, SLOT(on_action_exit()));

	QTabWidget* tabs = new QTabWidget(ui.centralWidget);
	tabs->move(0, 0);
	tabs->setFixedWidth(TABS_WIDTH);
	tabs->setFixedHeight(TABS_HEIGHT);
	tabs->setTabsClosable(true);

	this->tabs = tabs;

	QWidget* homePage = new QWidget(tabs);
	homePage->move(0, 0);
	homePage->setFixedWidth(TAB_WIDTH);
	homePage->setFixedHeight(TAB_HEIGHT);

	QIcon* homeIcon = new QIcon(HOME_ICON_PATH);

	tabs->addTab(homePage, *homeIcon, "Home");

	tabs->tabBar()->tabButton(0, QTabBar::RightSide)->deleteLater();
	tabs->tabBar()->setTabButton(0, QTabBar::RightSide, 0);

	connect(tabs, SIGNAL(tabCloseRequested(int)),
		this, SLOT(close_tab(int)));

	QTableWidget* procs = new QTableWidget(0, COLUMN_AMOUNT, homePage);
	procs->setContextMenuPolicy(Qt::CustomContextMenu);
	procs->resize(TAB_WIDTH, TAB_HEIGHT - 20);
	procs->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	procs->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

	this->tbl = procs;
	QObject::connect(procs, &QTableWidget::customContextMenuRequested, this, &FelixGUI::handleContextMenu);

	// Icons made by <a href="https://www.flaticon.com/authors/pixel-perfect" title="Pixel perfect">Pixel perfect</a> from <a href="https://www.flaticon.com/" title="Flaticon"> www.flaticon.com</a>
	QIcon* processIcon = new QIcon(PROCESS_ICON_PATH);
	// Icons made by <a href="https://www.flaticon.com/authors/freepik" title="Freepik">Freepik</a> from <a href="https://www.flaticon.com/" title="Flaticon"> www.flaticon.com</a>
	QIcon* pidIcon = new QIcon(PID_ICON_PATH);
	// Icons made by <a href="https://www.flaticon.com/authors/freepik" title="Freepik">Freepik</a> from <a href="https://www.flaticon.com/" title="Flaticon">www.flaticon.com</a>
	QIcon* commentIcon = new QIcon(COMMENT_ICON_PATH);
	// Icons made by <a href="https://www.flaticon.com/authors/pixel-perfect" title="Pixel perfect">Pixel perfect</a> from <a href="https://www.flaticon.com/" title="Flaticon"> www.flaticon.com</a>
	QIcon* execIcon = new QIcon(EXECPATH_ICON_PATH);
	// Icons made by <a href="https://www.flaticon.com/authors/freepik" title="Freepik">Freepik</a> from <a href="https://www.flaticon.com/" title="Flaticon"> www.flaticon.com</a>
	QIcon* dllIcon = new QIcon(DLL_ICON_PATH);

	procs->setHorizontalHeaderItem(0, new QTableWidgetItem(*processIcon, "Process Name"));
	procs->setHorizontalHeaderItem(1, new QTableWidgetItem(*pidIcon, "PID"));
	procs->setHorizontalHeaderItem(2, new QTableWidgetItem(*commentIcon, "Description"));
	procs->setHorizontalHeaderItem(3, new QTableWidgetItem(*execIcon, "Execution path"));
	// procs->setHorizontalHeaderItem(4, new QTableWidgetItem(*dllIcon, "Dependencies"));

	procs->resizeColumnsToContents();

	QHeaderView* hView = procs->horizontalHeader();
	// hView->sortIndicatorOrder();
	hView->setMinimumSectionSize(110);

	QHeaderView* vView = procs->verticalHeader();
	vView->setVisible(false);
	vView->setSectionResizeMode(QHeaderView::Fixed);
	vView->setDefaultSectionSize(ROW_HEIGHT);

	HANDLE hThread = CreateThread(NULL, 0, 
		(LPTHREAD_START_ROUTINE)draw_processes, (LPVOID)procs, 
		0, NULL);

	if (hThread == NULL) {
		MessageBoxA(NULL, std::to_string(GetLastError()).c_str(), "Error: start_thread", MB_ICONERROR);
	}
	
}

FelixGUI::~FelixGUI() {
}

MyActions::MyActions() {
	/*
	Initiate actions
	*/

}

void MyActions::on_action_launch()
{
	std::wstring path;
	SelectFile(&path);
	PIDStruct pid;
	if (path.empty())
		return;
	// Named pipe for getting the process PID 
	if (launch_process(path, &pid) == FALSE) {
		MessageBoxW(NULL, L"Could not launch process due to an error in the launch_process function in inject.cpp", L"Launch process error", MB_ICONERROR);
	}

	wait_for_injection_and_resume(&pid);
}

void MyActions::on_action_select_directory()
{
}

void MyActions::on_action_exit()
{
	TerminateProcess(GetCurrentProcess(), 0);
}

void FelixGUI::on_action_inject()
{
	inject_to_process(this->actions.pid, DLL_NAME);
}

void FelixGUI::on_action_view()
{
	this->tabs->addTab(generate_newtab(), this->actions.procName);
}

void FelixGUI::close_tab(int tab_index) {
	this->tabs->removeTab(tab_index);
}

QWidget* FelixGUI::generate_newtab()
{
	QScrollArea* scrollWidget = new QScrollArea();

	QWidget* procTab = new QWidget();
	scrollWidget->setWidget(procTab);
	procTab->setFixedSize(TAB_WIDTH, TAB_HEIGHT);
	// procTab->setStyleSheet("background-color: red");
	QVBoxLayout* mainLayout = new QVBoxLayout(procTab);

	QHBoxLayout* topLayout = new QHBoxLayout();
	topLayout->setContentsMargins(0, 0, 0, 0);
	topLayout->setSizeConstraint(QLayout::SetMinimumSize);
	procTab->setLayout(topLayout);

	QLabel* title = new QLabel(this->actions.procName);
	title->setContentsMargins(0, 0, 0, 0);
	// title->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
	title->setScaledContents(true);
	title->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	// title->setMaximumHeight(50);
	title->setStyleSheet("QLabel { margin-top: 30px; font-size: 40px;border: solid #000000; border-spacing: 2px; border-width: 2px 2px 2px 2px; background-color : white; color: #696969; padding-bottom: 5%; padding-left: 2%; padding-right:2%;}");
	topLayout->addWidget(title, 0, Qt::AlignHCenter | Qt::AlignTop);
	// topLayout->addSpacing(100);

	QHBoxLayout* bottomLayout = new QHBoxLayout();
	bottomLayout->setContentsMargins(0, 10, 0, 0);
	bottomLayout->setSizeConstraint(QLayout::SetMinimumSize);

	QWidget* procInfo = new QWidget();
	procInfo->setFixedSize(PROC_INFO_WIDTH, PROC_INFO_HEIGHT);
	procInfo->setStyleSheet("background-color: #000000");
	bottomLayout->addWidget(procInfo, Qt::AlignCenter);

	QHBoxLayout* tableLayout = new QHBoxLayout();
	tableLayout->setContentsMargins(0, 10, 0, 0);

	QTableWidget* actions = new QTableWidget();
	actions->setFixedSize(700, 400);
	tableLayout->addWidget(actions);

	QHBoxLayout* spaceLayout = new QHBoxLayout();
	spaceLayout->setSpacing(100);

	mainLayout->addLayout(topLayout);
	mainLayout->addLayout(bottomLayout);
	mainLayout->addLayout(tableLayout);
	mainLayout->addLayout(spaceLayout);
	// layout->addStretch();

	return scrollWidget;
}

void FelixGUI::handleContextMenu(const QPoint& pos) {
	QTableWidgetItem* item = this->tbl->itemAt(pos);
	if (item) {
		QMenu* myMenu = new QMenu(this);

		// Icons made by <a href="https://www.flaticon.com/authors/good-ware" title="Good Ware">Good Ware</a> from <a href="https://www.flaticon.com/" title="Flaticon"> www.flaticon.com</a>
		QIcon* injectIcon = new QIcon(INJECT_ICON_PATH);
		int itemRow = item->row();
		DWORD pid = this->tbl->item(itemRow, PID_COL)->text().toInt();
		QAction* injectAction = new QAction(*injectIcon, "Inject");

		this->actions.procName = this->tbl->item(itemRow, NAME_COL)->text();
		this->actions.pid = pid;
		QObject::connect(injectAction, SIGNAL(triggered()),
			this, SLOT(on_action_inject()));

		myMenu->addAction(injectAction);

		// Icons made by <a href="https://www.flaticon.com/authors/freepik" title="Freepik">Freepik</a> from <a href="https://www.flaticon.com/" title="Flaticon"> www.flaticon.com</a>
		
		QIcon* viewIcon = new QIcon(VIEW_ICON_PATH);
		QAction* viewAction = new QAction(*viewIcon, "View");
		QObject::connect(viewAction, SIGNAL(triggered()),
			this, SLOT(on_action_view()));
		myMenu->addAction(viewAction);
		
		myMenu->exec(QCursor::pos());

	}
}

void draw_processes(QTableWidget* tbl)
{
	while (true) {
		DWORD size, pid;
		DWORD* procs = get_procs(&size);
		HANDLE hProc;
		HICON pIcon;
		
		int counter = 0;

		for (int i = 0; i < size; i++) {
			std::vector<HWND> windows;
			wchar_t* procName = new wchar_t[MAX_PATH];
			QIcon* qtpIcon = nullptr;

			pid = procs[i];
			hProc = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION,
				FALSE,
				pid);

			if (hProc == NULL) {
				// Attempt to open process with LIMITED information.
				DWORD error = GetLastError();
				hProc = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_LIMITED_INFORMATION,
					FALSE,
					pid);
				if (hProc == NULL) {
					DWORD error = GetLastError();
					continue;
				}
			}

			/* Get full path to process executable */
			if (GetModuleFileNameExW(hProc, NULL, procName, MAX_PATH) != 0) {
				PIDWindows windStruct;
				windStruct.hwnd = &windows;
				windStruct.pid = pid;

				/* Enumerate over all windows to acquire windows with PID of `pid` */
				EnumWindows(EnumWins, (LPARAM)&windStruct);

				/* Attempt to get icon of windows*/
				for (int wind = 0; wind < windows.size(); wind++) {
					pIcon = (HICON)GetClassLongA(windows[wind], GCL_HICONSM);
					int error = GetLastError();
					error = error;
					if (pIcon != 0) {
						QPixmap map = fromHICON(pIcon);
						qtpIcon = new QIcon(map);
						break;
					}

				}
				/* Attempt to get icon of executable (from path extracted) */
				if (qtpIcon == nullptr) {
					if (ExtractIconExW(procName, 0, NULL, &pIcon, 1) == 1) {
						qtpIcon = new QIcon(fromHICON(pIcon));
					}
				}
				/* Set icon to default executable icon if both earlier failed*/
				if (qtpIcon == nullptr) {
					qtpIcon = new QIcon(EXE_ICON_PATH);
				}

				/* Extract description */
				std::wstring description;
				DWORD _;
				DWORD version_info_size = GetFileVersionInfoSizeW(procName, &_);

				/* Get version info buffer size */
				if (version_info_size != 0) {
					// version info buffer
					char* version_info = new char[version_info_size];
					if (GetFileVersionInfoW(procName, NULL, version_info_size, (LPVOID)version_info)) {

						struct LANGANDCODEPAGE
						{
							WORD wLanguage;
							WORD wCodePage;
						} *lpTranslate = NULL;

						wchar_t* cDesc = NULL;
						wchar_t* translation = new wchar_t[TRANSLATION_SIZE + 1];
						wchar_t* descriptionPath = new wchar_t[TRANSLATION_SIZE + 32 + 1];
						wchar_t* versionPath = new wchar_t[TRANSLATION_SIZE + 36 + 1];

						UINT pLenFileInfo;

						/* Get file language */
						if (VerQueryValue(version_info, L"\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate, &pLenFileInfo)) {
							std::swprintf(translation, TRANSLATION_SIZE + 1, L"%04x%04x", lpTranslate[0].wLanguage, lpTranslate[0].wCodePage);
						}
						else {
							// Pretty common language
							translation = L"041904b0";
						}

						/* Generate path to VerQueryValueW */
						std::swprintf(descriptionPath, TRANSLATION_SIZE + 32 + 1, L"\\StringFileInfo\\%8ls\\FileDescription", translation);
						std::swprintf(versionPath, TRANSLATION_SIZE + 36 + 1, L"\\StringFileInfo\\%8ls\\FileVersion", translation);

						/* Get description of EXE */
						if (VerQueryValueW(version_info, descriptionPath, (LPVOID*)&cDesc, &pLenFileInfo)) {
							description = std::wstring(cDesc);
						}

						/* Get version of EXE */
						if (VerQueryValueW(version_info, versionPath, (LPVOID*)&cDesc, &pLenFileInfo)) {
							if (description.empty())
								description = std::wstring(cDesc);
							else {
								description.append(L" - ");
								description.append(std::wstring(cDesc));
							}
						}

						// Deallocate heap memory
						delete[] versionPath;
						delete[] descriptionPath;
						delete[] translation;
					}
					delete[] version_info;
				}

				if (description.empty())
					description.append(L"No description.");
				if (counter >= tbl->rowCount()) {
					tbl->insertRow(counter);
				}
				/* Process name */
				std::wstring shortProcName = std::wstring(procName);
				size_t last_index = shortProcName.find_last_of(L"\\");
				shortProcName = shortProcName.substr(last_index + 1, shortProcName.size() - last_index - 1);

				QTableWidgetItem* wiProcName = new QTableWidgetItem(*qtpIcon, QString::fromStdWString(shortProcName));
				wiProcName->setFlags(wiProcName->flags() & ~Qt::ItemIsEditable);
				QSize current_size = wiProcName->sizeHint();
				current_size.setHeight(100);
				wiProcName->setSizeHint(current_size);
				tbl->setItem(counter, 0, wiProcName);
				
				/* PID */
				QTableWidgetItem* wiPID = new QTableWidgetItem(QString::fromStdWString(std::to_wstring(pid)));
				wiPID->setFlags(wiPID->flags() & ~Qt::ItemIsEditable);
				tbl->setItem(counter, 1, wiPID);

				/* Description */
				QTableWidgetItem* wiDesc = new QTableWidgetItem(QString::fromStdWString(description));
				wiDesc->setFlags(wiDesc->flags() & ~Qt::ItemIsEditable);
				tbl->setItem(counter, 2, wiDesc);

				/* Execution Path */
				QTableWidgetItem* wiPath = new QTableWidgetItem(QString::fromStdWString(procName));
				wiPath->setFlags(wiPath->flags() & ~Qt::ItemIsEditable);
				tbl->setItem(counter, 3, wiPath);
				/*
				 *Dependencies*
				QTableWidgetItem* wiDep = new QTableWidgetItem(QString::fromStdWString(procDependencies));
				wiDep->setFlags(wiDep->flags() & ~Qt::ItemIsEditable);
				tbl->setItem(counter, 4, wiDep);
				*/
				counter += 1;

			}
			
			CloseHandle(hProc);
			
		}
		delete[] procs;
		tbl->setRowCount(counter);
		Sleep(UPDATE_RATE * 1000);
	}
}



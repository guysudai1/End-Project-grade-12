#pragma once

#ifndef INCLUDED_IOSTREAM
#define INCLUDED_IOSTREAM 1
#include <iostream>
#endif

#ifndef INCLUDED_WINDOWS
#define INCLUDED_WINDOWS 1
#include <Windows.h>
#endif

#ifndef INCLUDED_FILEAPI
#define INCLUDED_FILEAPI 1
#include <Fileapi.h>
#endif

#ifndef INCLUDED_WINBASE
#define INCLUDED_WINBASE 1
#include <WinBase.h>
#endif

#ifndef INCLUDED_STRING
#define INCLUDED_STRING 1
#include <string>
#endif

#ifndef INCLUDED_ERRORS
#define INCLUDED_ERRORS 1
#include <errhandlingapi.h>
#endif

#ifndef HOOKING_H
#define HOOKING_H
HANDLE open_monitored_file(std::wstring);
#endif

#define LET_ALL_PROCS_OPEN (FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE)
#define FILE_NOTIFY_ALL_CUSTOM  FILE_NOTIFY_CHANGE_FILE_NAME   |\
							    FILE_NOTIFY_CHANGE_DIR_NAME    |\
								FILE_NOTIFY_CHANGE_ATTRIBUTES  |\
								FILE_NOTIFY_CHANGE_SIZE		   |\
								FILE_NOTIFY_CHANGE_LAST_WRITE  |\
								FILE_NOTIFY_CHANGE_LAST_ACCESS |\
								FILE_NOTIFY_CHANGE_CREATION	   |\
								FILE_NOTIFY_CHANGE_SECURITY
#define MONITER_FOLDER(buf, file, watch_subtree, overlapped) ReadDirectoryChangesW(file, &buf[0],sizeof(buf),(BOOL)watch_subtree,FILE_NOTIFY_ALL_CUSTOM,NULL,&overlapped,NULL)

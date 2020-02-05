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
HANDLE open_monitored_file(std::string);
#endif

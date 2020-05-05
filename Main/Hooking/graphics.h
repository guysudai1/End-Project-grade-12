#pragma once

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "hashmap.h"

// void create_menu(sf::RenderWindow*);
HWND generate_window(HINSTANCE, std::wstring, int, int, int, int);

void SelectDir(std::wstring*);
void SelectFile(std::wstring*);

LRESULT CALLBACK OnEvent(HWND wnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HWND generate_window(HINSTANCE hInstance,
	std::wstring name,
	int x, int y,
	int width, int height);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow);

#endif
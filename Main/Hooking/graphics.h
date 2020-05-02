#pragma once

#ifndef GRAPHICS_H
#define GRAPHICS_H 1

#include "hashmap.h"

// void create_menu(sf::RenderWindow*);
HWND generate_window(HINSTANCE, std::wstring, int, int, int, int);
void run_forever(HINSTANCE, HINSTANCE, LPSTR, int, hash_map*);

#endif
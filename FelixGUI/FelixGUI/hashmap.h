#pragma once
// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

#ifndef HASHMAP_H
#define HASHMAP_H
#define MAX_ENUMERATE_FOLDERS 1000

#include <Windows.h>
#include <iostream>
#include <list> 
#include <string.h>
#include <string>
#include <tuple>
#include <typeinfo>


unsigned int hash(const wchar_t*, unsigned int);

class hash_map {
public:
	hash_map(int);
	~hash_map();
	std::wstring get_value(wchar_t* key);
	//R get_value(T);
	bool put(wchar_t* key, std::wstring value);
	//bool put(T, R);
private:
	unsigned int map_size;
	std::tuple<wchar_t*, std::wstring>* internal_map;
	// int calculate_hash(T);
	int calculate_hash(wchar_t* key);
};

int capture_folder(std::wstring, hash_map*, bool);
void hook_folder(std::wstring);
// TODO: add headers that you want to pre-compile here

#endif //HASHMAP_H

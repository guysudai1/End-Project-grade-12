
#include "hashmap.h"
#include "hooking.h"

unsigned int hash(wchar_t* str, unsigned int max_elements) {
	// djb2 hash
	unsigned int hash = 5381;
	int c;
	while ((c = *(str++)))
		hash = ((hash << 5) + hash) + c; // hash * 33 + c 
	return hash % max_elements;
}

// Max 1000 files
hash_map::hash_map(int map_size) {
	this->map_size = map_size;
	this->internal_map = new std::tuple<wchar_t*, std::wstring>[map_size];
	for (int i = 0; i < map_size; i++)
		this->internal_map[i] = std::make_tuple((wchar_t*)L"", std::wstring(L""));
}

hash_map::~hash_map() {
	delete this->internal_map;
}

// Extract value from hashmap
std::wstring hash_map::get_value(wchar_t* key) {
	unsigned int index = this->calculate_hash(key);
	wchar_t* key_in_arr = (wchar_t*)L"";
	do {
		key_in_arr = std::get<0>(this->internal_map[index++]);
	} while (index < this->map_size && key_in_arr != key);
	if (index == this->map_size)
		return std::wstring(L"");
	return std::get<1>(this->internal_map[--index]);
}

/*
template <class T, class R>
R hash_map<T, R>::get_value(T key) {
	return NULL;
}
*/

bool check_null(std::tuple<wchar_t*, std::wstring> tuple) {
	return std::get<0>(tuple) == (wchar_t*)L"" && std::get<1>(tuple).empty();
}

bool hash_map::put(wchar_t* key, std::wstring value) {
	// Find space to put in
	unsigned int index = this->calculate_hash(key);
	while (!check_null(this->internal_map[index]) && index < this->map_size) {
		index++;
	}
	if (index == this->map_size)
		return false;
	// Set element
	this->internal_map[index] = std::make_tuple(key, value);
	return true;
}

/*
template <class T, class R>
bool hash_map<T, R>::put(T key, R value) {
	return false;
}*/

int hash_map::calculate_hash(wchar_t* key) {
	return hash(key, this->map_size);
}


void read_file(std::wstring path, char buf[]) {

}

void hook_folder(std::wstring path) {

	/*wchar_t buf[1024];
	GetCurrentDirectoryW(1024, buf);

	std::wstring file_path = std::wstring(&buf[0], &buf[wcslen(buf)]);
	
	wchar_t buf[1024];
	if (path.c_str[path.length() - 1] != L"\\") {
		path.append(L"\\");
	}
	OVERLAPPED overlapped = { 0 };
	overlapped.hEvent = CreateEvent(0, 0, 0, 0);
	HANDLE file = open_monitored_file(path);
	memset(buf, 0, wcslen(buf));

	MONITER_FOLDER(buf, file, true, overlapped);
	// FILE_NOTIFY_INFORMATION* infoBuf = (FILE_NOTIFY_INFORMATION*)(&buf[0]);

	//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
	std::wcout << L"Filename: " << std::endl;
	//std::wcout << infoBuf->FileName << std::endl;	
	FILE_NOTIFY_INFORMATION *info = reinterpret_cast<FILE_NOTIFY_INFORMATION *>(buf);
	do {
		std::wstring str(info->FileName, info->FileNameLength / sizeof(wchar_t));
		std::wcout << str << std::endl;
		MessageBoxW(NULL, info->FileName, info->FileName, 1);
		info = reinterpret_cast<FILE_NOTIFY_INFORMATION *>((BYTE*)info + info->NextEntryOffset);
	} while (info->NextEntryOffset > 0);

	CloseHandle(file);
	*/
}

int capture_folder(std::wstring path,
	hash_map* map,
	bool recursive) {

	
	WIN32_FIND_DATAW FindFileData;
	std::list<std::wstring> paths;
	paths.push_back(path);
	while (paths.size()) {
		std::wstring current_path = paths.front();
		if (current_path[current_path.length() - 1] != L'\\') {
			current_path += '\\';
		}
		HANDLE hFind = FindFirstFileW((current_path + L"*").c_str(), &FindFileData);
		if (hFind == INVALID_HANDLE_VALUE) {
			printf("FindFirstFile failed (%d)\n", GetLastError());
			return -1;
		}
		int found = 0;

		do {

			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && recursive) {
				// MessageBoxW(NULL, (current_path + FindFileData.cFileName).c_str(), L"Directory", 1);
				// std::wcout << L"Found dir: " << FindFileData.cFileName << std::endl;
				if ((wcsncmp(FindFileData.cFileName, L".", 1)) && wcsncmp(FindFileData.cFileName, L"..", 2))
					paths.push_back(current_path + FindFileData.cFileName + L"\\");
			}
			else {
				wchar_t* cpath = new wchar_t[sizeof(current_path) * current_path.length() + 1];
				wcscpy(cpath, (current_path + FindFileData.cFileName).c_str());
				// std::wcout << L"Found file: " << FindFileData.cFileName << std::endl;
				// MessageBoxW(NULL, (current_path + FindFileData.cFileName).c_str(), L"File", 1);
				map->put(cpath, L"hello");
				found += 1;
			}
		} while (FindNextFile(hFind, &FindFileData) != 0);
		paths.pop_front();
		FindClose(hFind);
	}
	return 1;
}
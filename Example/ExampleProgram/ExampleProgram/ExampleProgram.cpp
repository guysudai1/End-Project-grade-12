#include <iostream>
#include <Windows.h>

int main()
{
    std::cout << "Hello World!\n";

    // Sleep(30000);
    
    HANDLE file = CreateFileA("C:\\Users\\sudai\\Desktop\\CreatedFile.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    
    std::cout << "Used CreateFileA";
}


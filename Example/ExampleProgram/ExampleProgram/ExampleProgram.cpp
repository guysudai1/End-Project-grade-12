#include <iostream>
#include <Windows.h>

int main()
{
    std::cout << "Hello World!\n";
    system("pause");
    HANDLE file = CreateFileA("ExampleFileTest.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    
    std::cout << "Used CreateFileA" << std::endl;
    system("pause");
}


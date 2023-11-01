#include <iostream>
#include <windows.h>
#include <string>
#include <locale>
#include <codecvt>

using namespace std;

const int group_number = 514;
const int my_number = 17;

const int X = group_number + my_number;
const int A = X % 4;						// A = 3
const int B = 7 + X % 7;					// B = 13

int main()
{
    LPWSTR cmdLine = GetCommandLine();
    int argc;
    LPWSTR* argv = CommandLineToArgvW(cmdLine, &argc);

    if (argv != nullptr) 
    {
        HANDLE Start_flag = OpenEvent(SYNCHRONIZE, FALSE, L"Global\\Start_processing");
        DWORD result = WaitForSingleObject(Start_flag, INFINITE);

        if (result == WAIT_OBJECT_0)
        {
            int array_size = _wtoi(argv[1]);
            int delta = _wtoi(argv[2]);

            HANDLE h_Map_File = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, L"Array");
            if (h_Map_File)
            {
                int* shared_array = (int*)MapViewOfFile(h_Map_File, FILE_MAP_ALL_ACCESS, 0, 0, array_size * sizeof(int));
                if (shared_array) 
                {
                    for (int i = array_size - delta - 1; i >= 0; i -= B)
                    {
                        shared_array[i] = (i * shared_array[i]) & 255;
                    }
                }
                UnmapViewOfFile(shared_array);
                CloseHandle(h_Map_File);
            }

            CloseHandle(Start_flag);
        }
        else
        {
            CloseHandle(Start_flag);
        }

        LocalFree(argv);
    }
}
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
        /*wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
        string str = converter.to_bytes(argv[1]);

        int* array = reinterpret_cast<int*>(stoull(str, nullptr, 16));*/

        int array_size = _wtoi(argv[2]);
        int delta = _wtoi(argv[3]);

        for (int i = array_size - delta - 1; i >= 0; i -= B)
        {
            //array[i] = (i * array[i]) & 255;
        }

        LocalFree(argv);
    }
}
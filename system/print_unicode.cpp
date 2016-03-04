#include <iostream>
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

extern "C" void print_unicode_string(int handle, wchar_t* ws)
{
#ifdef _WIN32
    if (handle == 1)
    {
        std::wcout.flush();
    }
    else if (handle == 2)
    {
        std::wcerr.flush();
    }
    int prevMode = _setmode(handle, _O_U16TEXT);
#endif    
   if (handle == 1)
   {
       std::wcout << ws;
   }
   else if (handle == 2)
   {
       std::wcerr << ws;
   }
#ifdef _WIN32
    if (handle == 1)
    {
        std::wcout.flush();
    }
    else if (handle == 2)
    {
        std::wcerr.flush();
    }
    _setmode(handle, prevMode);
#endif    
}

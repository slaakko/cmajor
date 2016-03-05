#include <iostream>
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

#ifdef _WIN32
extern "C" wchar_t* read_unicode_line()
{
    int prevMode = _setmode(0, _O_U16TEXT);
    std::wstring line;
    if (std::getline(std::wcin, line))
    {
        _setmode(0, prevMode);
        wchar_t* ws = new wchar_t[line.length() + 1];
        wchar_t* p = ws;
        for (wchar_t w : line)
        {
            *p++ = w;
        }
        *p = L'\0';
        return ws;
    }
    else
    {
        _setmode(0, prevMode);
        return nullptr;
    }
}
#else
extern "C" wchar_t* read_unicode_line()
{
    std::wstring line;
    if (std::getline(std::wcin, line))
    {
        wchar_t* ws = new wchar_t[line.length() + 1];
        wchar_t* p = ws;
        for (wchar_t w : line)
        {
            *p++ = w;
        }
        *p = L'\0';
        return ws;
    }
    else
    {
        return nullptr;
    }
}
#endif

extern "C" void delete_unicode_line(wchar_t* line)
{
    delete[](line);
}

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

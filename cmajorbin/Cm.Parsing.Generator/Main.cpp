/*========================================================================
    Copyright (c) 2011-2013 Seppo Laakko
    http://sourceforge.net/projects/soulparsing/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Parsing.Syntax/Compiler.hpp>
#include <Cm.Parsing.Syntax/CodeGenerator.hpp>
#include <Cm.Parsing/InitDone.hpp>
#include <Cm.Util/TextUtils.hpp>
#include <iostream>
#include <string>
#include <stdlib.h>

#if defined(_MSC_VER) && !defined(NDEBUG)
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
#endif

class InitDone
{
public:
    InitDone()
    {
        Cm::Parsing::Init();
    }
    ~InitDone()
    {
        Cm::Parsing::Done();
    }
};

using Cm::Util::Split;

void AddToLibraryDirectories(const std::string& libraryPath, std::vector<std::string>& libraryDirectories)
{
    std::vector<std::string> dirs = Split(libraryPath, ';');
    int n = int(dirs.size());
    for (int i = 0; i < n; ++i)
    {
        std::string dir = dirs[i];
        libraryDirectories.push_back(dir);
    }
}

const char* version = "1.0";

int main(int argc, const char** argv)
{
#if defined(_MSC_VER) && !defined(NDEBUG)
    int dbgFlags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    dbgFlags |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(dbgFlags);
    //_CrtSetBreakAlloc(40183);
#endif // defined(_MSC_VER) && !defined(NDEBUG)
    try
    {
        std::cout << "Cmajor Parser Generator version " << version << std::endl;
        if (argc < 2)
        {
            std::cout << "Usage: cmpg [options] {<file>.slp}\n" << "compile Cmajor parser project file(s) <file>.pp..." << std::endl;
            std::cout << "options:\n";
            std::cout << "-L <dir1>;<dir2>;...: add <dir1>, <dir2>, ... to library reference directories" << std::endl;
            std::cout << "-F                  : force code generation" << std::endl;
            return 1;
        }
        std::vector<std::string> projectFilePaths;
        std::vector<std::string> libraryDirectories;
        std::string libraryPath;
        const char* libraryPathEnv = getenv("CM_PARSING_LIBRARY_PATH");
        if (libraryPathEnv)
        {
            libraryPath = libraryPathEnv;
        }
        if (libraryPath.empty())
        {
            std::cerr << "please set 'CM_PARSING_LIBRARY_PATH' environment variable to contain at least /path/to/cmajorbin/Cm.Parsing directory." << std::endl;
            return 2;
        }
        AddToLibraryDirectories(libraryPath, libraryDirectories);
        bool prevWasL = false;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (arg == "-L")
            {
                prevWasL = true;
            }
            else if (arg == "-F")
            {
                Cm::Parsing::Syntax::SetForceCodeGen(true);
            }
            else if (prevWasL)
            {
                prevWasL = false;
                AddToLibraryDirectories(arg, libraryDirectories);
            }
            else
            {
                projectFilePaths.push_back(arg);
            }
        }
        InitDone initDone;
        int n = int(projectFilePaths.size());
        for (int i = 0; i < n; ++i)
        {
            const std::string& projectFilePath = projectFilePaths[i];
            Cm::Parsing::Syntax::Compile(projectFilePath, libraryDirectories);
        }
    }
    catch (std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 3;
    }
    catch (...)
    {
        std::cerr << "unknown exception" << std::endl;
        return 4;
    }
    return 0;
}

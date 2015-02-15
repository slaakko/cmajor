/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.Build/Build.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Sym/InitDone.hpp>
#include <Cm.Ast/InitDone.hpp>
#include <Cm.Parsing/InitDone.hpp>
#include <iostream>

#if defined(_MSC_VER) && !defined(NDEBUG)
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
#endif

const char* version = "1.0.0";

struct InitDone
{
    InitDone()
    {
        Cm::Parsing::Init();
        Cm::Ast::Init();
        Cm::Sym::Init();
    }
    ~InitDone()
    {
        Cm::Sym::Done();
        Cm::Ast::Done();
        Cm::Parsing::Done();
    }
};

int main(int argc, const char** argv)
{
#if defined(_MSC_VER) && !defined(NDEBUG)
    int dbgFlags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    dbgFlags |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(dbgFlags);
    //_CrtSetBreakAlloc(673562);
#endif //  defined(_MSC_VER) && !defined(NDEBUG)
    try
    {
        InitDone initDone;
        std::vector<std::string> projectFilePaths;
        std::cout << "Cmajor Binary Compiler version " << version << std::endl;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            projectFilePaths.push_back(arg);
        }
        for (const std::string& projectFilePath : projectFilePaths)
        {
            Cm::Build::Build(projectFilePath);
        }
    }
    catch (const Cm::Parsing::CombinedParsingError& ex)
    {
        for (const Cm::Parsing::ExpectationFailure& exp : ex.Errors())
        {
            std::cerr << exp.what() << std::endl;
        }
        return 4;
    }
    catch (const Cm::Core::ToolErrorExcecption& ex)
    {
        const Cm::Util::ToolError& error = ex.Error();
        std::cerr << error.ToolName() << ": " << error.Message() << " in file " << error.FilePath() << " line " << error.Line() << " column " << error.Column() << std::endl;
        return 3;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 2;
    }
    catch (...)
    {
        std::cerr << "unknown exception occurred" << std::endl;
        return 1;
    }
    return 0;
}

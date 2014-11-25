/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.Parser/Constant.hpp>
#include <Cm.Ast/Writer.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Factory.hpp>
#include <Cm.Ast/Identifier.hpp>

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
    }
    ~InitDone()
    {
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
    //_CrtSetBreakAlloc(5457);
#endif //  defined(_MSC_VER) && !defined(NDEBUG)
    try
    {
        std::cout << "Cmajor Binary Compiler version " << version << std::endl;
        InitDone initDone;
        Cm::Parser::ConstantGrammar* grammar = Cm::Parser::ConstantGrammar::Create();
        Cm::Parser::ParsingContext ctx;
        std::string s("public const int foo = 1;");
        std::unique_ptr<Cm::Ast::Node> node(grammar->Parse(s.c_str(), s.c_str() + s.length(), 0, "", &ctx));
        {
            Cm::Ast::Writer writer("C:\\temp\\constant.mc");
            writer.Write(node.get());
        }
        Cm::Ast::Reader reader("C:\\temp\\constant.mc");
        std::unique_ptr<Cm::Ast::Node> n(reader.ReadNode());
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

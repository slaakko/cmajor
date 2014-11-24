/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.Parser/Literal.hpp>
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
        Cm::Parser::LiteralGrammar* grammar = Cm::Parser::LiteralGrammar::Create();
        std::string s("123u");
        std::unique_ptr<Cm::Ast::Node> node(grammar->Parse(s.c_str(), s.c_str() + s.length(), 0, ""));
        {
            Cm::Ast::Writer writer("C:\\temp\\intliteral.mc");
            writer.Write(node.get());
        }
        Cm::Ast::Reader reader("C:\\temp\\intliteral.mc");
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

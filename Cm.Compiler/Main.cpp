/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.Sym/SymbolTable.hpp>
#include <Cm.Parser/Class.hpp>
#include <Cm.Ast/Writer.hpp>
#include <Cm.Ast/Reader.hpp>
#include <Cm.Ast/Factory.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Parsing/Exception.hpp>

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
    //_CrtSetBreakAlloc(55187);
#endif //  defined(_MSC_VER) && !defined(NDEBUG)
    try
    {
        InitDone initDone;
        Cm::Sym::SymbolTable symbolTable;
        symbolTable.BeginNamespaceScope("alpha.beta.gamma");
        symbolTable.EndNamespaceScope();
        symbolTable.BeginNamespaceScope("alpha.beta.gamma");
        symbolTable.EndNamespaceScope();
/*
        std::cout << "Cmajor Binary Compiler version " << version << std::endl;
        Cm::Parser::ClassGrammar* grammar = Cm::Parser::ClassGrammar::Create();
        grammar->SetRecover();
        Cm::Parser::ParsingContext ctx;
        std::string s("public class Foo { typedef int* iterator; }");
        std::unique_ptr<Cm::Ast::Node> node(grammar->Parse(s.c_str(), s.c_str() + s.length(), 0, "", &ctx));
        {
            Cm::Ast::Writer writer("C:\\temp\\class.mc");
            writer.Write(node.get());
        }
        Cm::Ast::Reader reader("C:\\temp\\class.mc");
        std::unique_ptr<Cm::Ast::Node> n(reader.ReadNode());
*/
    }
    catch (const Cm::Parsing::CombinedParsingError& ex)
    {
        for (const Cm::Parsing::ExpectationFailure& exp : ex.Errors())
        {
            std::cerr << exp.what() << std::endl;
        }
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

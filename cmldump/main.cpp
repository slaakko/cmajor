#include <Cm.Sym/InitDone.hpp>
#include <Cm.Sym/Module.hpp>
#include <Cm.Ast/InitDone.hpp>
#include <Cm.Parsing/InitDone.hpp>
#include <Cm.IrIntf/BackEnd.hpp>
#include <iostream>

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
    try
    {
        InitDone initDone;
        Cm::IrIntf::SetBackEnd(Cm::IrIntf::BackEnd::llvm);
        for (int i = 1; i < argc; ++i)
        {
            std::string fileName = argv[i];
            Cm::Sym::Module projectModule(fileName);
            projectModule.Dump();
        }
        return 0;
    }
    catch (std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
}

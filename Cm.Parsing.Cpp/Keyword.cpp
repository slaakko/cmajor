#include "Keyword.hpp"
#include <Cm.Parsing/Action.hpp>
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>
#include <Cm.Parsing/Primitive.hpp>
#include <Cm.Parsing/Composite.hpp>
#include <Cm.Parsing/Nonterminal.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Parsing/StdLib.hpp>
#include <Cm.Parsing/XmlLog.hpp>

namespace Cm { namespace Parsing { namespace Cpp {

using namespace Cm::Parsing;

KeywordGrammar* KeywordGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

KeywordGrammar* KeywordGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    KeywordGrammar* grammar(new KeywordGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

KeywordGrammar::KeywordGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("KeywordGrammar", parsingDomain_->GetNamespaceScope("Cm.Parsing.Cpp"), parsingDomain_)
{
    SetOwner(0);
    keywords0.push_back("alignas");
    keywords0.push_back("alignof");
    keywords0.push_back("asm");
    keywords0.push_back("auto");
    keywords0.push_back("bool");
    keywords0.push_back("break");
    keywords0.push_back("case");
    keywords0.push_back("catch");
    keywords0.push_back("char");
    keywords0.push_back("char16_t");
    keywords0.push_back("char32_t");
    keywords0.push_back("class");
    keywords0.push_back("const");
    keywords0.push_back("const_cast");
    keywords0.push_back("constexpr");
    keywords0.push_back("continue");
    keywords0.push_back("decltype");
    keywords0.push_back("default");
    keywords0.push_back("delete");
    keywords0.push_back("do");
    keywords0.push_back("double");
    keywords0.push_back("dynamic_cast");
    keywords0.push_back("else");
    keywords0.push_back("enum");
    keywords0.push_back("explicit");
    keywords0.push_back("export");
    keywords0.push_back("extern");
    keywords0.push_back("false");
    keywords0.push_back("float");
    keywords0.push_back("for");
    keywords0.push_back("friend");
    keywords0.push_back("goto");
    keywords0.push_back("if");
    keywords0.push_back("inline");
    keywords0.push_back("int");
    keywords0.push_back("long");
    keywords0.push_back("mutable");
    keywords0.push_back("namespace");
    keywords0.push_back("new");
    keywords0.push_back("noexcept");
    keywords0.push_back("nullptr");
    keywords0.push_back("operator");
    keywords0.push_back("private");
    keywords0.push_back("protected");
    keywords0.push_back("public");
    keywords0.push_back("register");
    keywords0.push_back("reinterpret_cast");
    keywords0.push_back("return");
    keywords0.push_back("short");
    keywords0.push_back("signed");
    keywords0.push_back("sizeof");
    keywords0.push_back("static");
    keywords0.push_back("static_assert");
    keywords0.push_back("static_cast");
    keywords0.push_back("struct");
    keywords0.push_back("switch");
    keywords0.push_back("template");
    keywords0.push_back("this");
    keywords0.push_back("thread_local");
    keywords0.push_back("throw");
    keywords0.push_back("true");
    keywords0.push_back("try");
    keywords0.push_back("typedef");
    keywords0.push_back("typeid");
    keywords0.push_back("typename");
    keywords0.push_back("union");
    keywords0.push_back("unsigned");
    keywords0.push_back("using");
    keywords0.push_back("virtual");
    keywords0.push_back("void");
    keywords0.push_back("volatile");
    keywords0.push_back("wchar_t");
    keywords0.push_back("while");
}

void KeywordGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* parsingDomain = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = parsingDomain->GetGrammar("Cm.Parsing.stdlib");
    if (!grammar0)
    {
        grammar0 = Cm::Parsing::stdlib::Create(parsingDomain);
    }
    AddGrammarReference(grammar0);
}

void KeywordGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("identifier", this, "Cm.Parsing.stdlib.identifier"));
    AddRule(new Cm::Parsing::Rule("Keyword", GetScope(),
        new Cm::Parsing::KeywordListParser("identifier", keywords0)));
}

} } } // namespace Cm.Parsing.Cpp

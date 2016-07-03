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

namespace Cm { namespace Parser {

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

KeywordGrammar::KeywordGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("KeywordGrammar", parsingDomain_->GetNamespaceScope("Cm.Parser"), parsingDomain_)
{
    SetOwner(0);
    keywords0.push_back("abstract");
    keywords0.push_back("and");
    keywords0.push_back("as");
    keywords0.push_back("axiom");
    keywords0.push_back("base");
    keywords0.push_back("bool");
    keywords0.push_back("break");
    keywords0.push_back("byte");
    keywords0.push_back("case");
    keywords0.push_back("cast");
    keywords0.push_back("catch");
    keywords0.push_back("cdecl");
    keywords0.push_back("char");
    keywords0.push_back("class");
    keywords0.push_back("concept");
    keywords0.push_back("const");
    keywords0.push_back("constexpr");
    keywords0.push_back("construct");
    keywords0.push_back("continue");
    keywords0.push_back("default");
    keywords0.push_back("delegate");
    keywords0.push_back("delete");
    keywords0.push_back("destroy");
    keywords0.push_back("do");
    keywords0.push_back("double");
    keywords0.push_back("else");
    keywords0.push_back("enum");
    keywords0.push_back("explicit");
    keywords0.push_back("extern");
    keywords0.push_back("false");
    keywords0.push_back("float");
    keywords0.push_back("for");
    keywords0.push_back("goto");
    keywords0.push_back("if");
    keywords0.push_back("inline");
    keywords0.push_back("int");
    keywords0.push_back("interface");
    keywords0.push_back("internal");
    keywords0.push_back("is");
    keywords0.push_back("long");
    keywords0.push_back("namespace");
    keywords0.push_back("new");
    keywords0.push_back("not");
    keywords0.push_back("nothrow");
    keywords0.push_back("null");
    keywords0.push_back("operator");
    keywords0.push_back("or");
    keywords0.push_back("override");
    keywords0.push_back("private");
    keywords0.push_back("protected");
    keywords0.push_back("public");
    keywords0.push_back("return");
    keywords0.push_back("sbyte");
    keywords0.push_back("short");
    keywords0.push_back("sizeof");
    keywords0.push_back("static");
    keywords0.push_back("suppress");
    keywords0.push_back("switch");
    keywords0.push_back("this");
    keywords0.push_back("throw");
    keywords0.push_back("true");
    keywords0.push_back("try");
    keywords0.push_back("typedef");
    keywords0.push_back("typename");
    keywords0.push_back("uchar");
    keywords0.push_back("uint");
    keywords0.push_back("ulong");
    keywords0.push_back("ushort");
    keywords0.push_back("using");
    keywords0.push_back("virtual");
    keywords0.push_back("void");
    keywords0.push_back("wchar");
    keywords0.push_back("where");
    keywords0.push_back("while");
}

void KeywordGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parsing.stdlib");
    if (!grammar0)
    {
        grammar0 = Cm::Parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
}

void KeywordGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("identifier", this, "Cm.Parsing.stdlib.identifier"));
    AddRule(new Cm::Parsing::Rule("Keyword", GetScope(),
        new Cm::Parsing::KeywordListParser("identifier", keywords0)));
}

} } // namespace Cm.Parser

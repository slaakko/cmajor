#include "ParserFile.hpp"
#include <Cm.Parsing/Action.hpp>
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>
#include <Cm.Parsing/Primitive.hpp>
#include <Cm.Parsing/Composite.hpp>
#include <Cm.Parsing/Nonterminal.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Parsing/StdLib.hpp>
#include <Cm.Parsing/XmlLog.hpp>
#include <Cm.Parsing.Cpp/Declaration.hpp>
#include <Cm.Parsing.Syntax/Element.hpp>
#include <Cm.Parsing.Syntax/Grammar.hpp>
#include <Cm.Parsing/Namespace.hpp>
#include <Cm.Parsing/Grammar.hpp>

namespace Cm { namespace Parsing { namespace Syntax {

using namespace Cm::Parsing;

ParserFileGrammar* ParserFileGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

ParserFileGrammar* ParserFileGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    ParserFileGrammar* grammar(new ParserFileGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

ParserFileGrammar::ParserFileGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("ParserFileGrammar", parsingDomain_->GetNamespaceScope("Cm.Parsing.Syntax"), parsingDomain_)
{
    SetOwner(0);
}

ParserFileContent* ParserFileGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, int id_, Cm::Parsing::ParsingDomain* parsingDomain_)
{
    Cm::Parsing::Scanner scanner(start, end, fileName, fileIndex, SkipRule());
    std::unique_ptr<Cm::Parsing::XmlLog> xmlLog;
    if (Log())
    {
        xmlLog.reset(new Cm::Parsing::XmlLog(*Log(), MaxLogLineLength()));
        scanner.SetLog(xmlLog.get());
        xmlLog->WriteBeginRule("parse");
    }
    Cm::Parsing::ObjectStack stack;
    stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<int>(id_)));
    stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<Cm::Parsing::ParsingDomain*>(parsingDomain_)));
    Cm::Parsing::Match match = Cm::Parsing::Grammar::Parse(scanner, stack);
    Cm::Parsing::Span stop = scanner.GetSpan();
    if (Log())
    {
        xmlLog->WriteEndRule("parse");
    }
    if (!match.Hit() || stop.Start() != int(end - start))
    {
        if (StartRule())
        {
            throw Cm::Parsing::ExpectationFailure(StartRule()->Info(), fileName, stop, start, end);
        }
        else
        {
            throw Cm::Parsing::ParsingException("grammar '" + Name() + "' has no start rule", fileName, scanner.GetSpan(), start, end);
        }
    }
    std::unique_ptr<Cm::Parsing::Object> value = std::move(stack.top());
    ParserFileContent* result = *static_cast<Cm::Parsing::ValueObject<ParserFileContent*>*>(value.get());
    stack.pop();
    return result;
}

class ParserFileGrammar::ParserFileRule : public Cm::Parsing::Rule
{
public:
    ParserFileRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("int", "id_"));
        AddInheritedAttribute(AttrOrVariable("Cm::Parsing::ParsingDomain*", "parsingDomain_"));
        SetValueTypeName("ParserFileContent*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> parsingDomain__value = std::move(stack.top());
        context.parsingDomain_ = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::ParsingDomain*>*>(parsingDomain__value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> id__value = std::move(stack.top());
        context.id_ = *static_cast<Cm::Parsing::ValueObject<int>*>(id__value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParserFileContent*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ParserFileRule>(this, &ParserFileRule::A0Action));
        Cm::Parsing::NonterminalParser* includeDirectivesNonterminalParser = GetNonterminal("IncludeDirectives");
        includeDirectivesNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ParserFileRule>(this, &ParserFileRule::PreIncludeDirectives));
        Cm::Parsing::NonterminalParser* namespaceContentNonterminalParser = GetNonterminal("NamespaceContent");
        namespaceContentNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ParserFileRule>(this, &ParserFileRule::PreNamespaceContent));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new ParserFileContent(context.id_, context.parsingDomain_);
        context.value->SetFilePath(fileName);
    }
    void PreIncludeDirectives(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParserFileContent*>(context.value)));
    }
    void PreNamespaceContent(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParserFileContent*>(context.value)));
    }
private:
    struct Context
    {
        Context(): id_(), parsingDomain_(), value() {}
        int id_;
        Cm::Parsing::ParsingDomain* parsingDomain_;
        ParserFileContent* value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ParserFileGrammar::IncludeDirectivesRule : public Cm::Parsing::Rule
{
public:
    IncludeDirectivesRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParserFileContent*", "parserFileContent"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> parserFileContent_value = std::move(stack.top());
        context.parserFileContent = *static_cast<Cm::Parsing::ValueObject<ParserFileContent*>*>(parserFileContent_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::NonterminalParser* includeDirectiveNonterminalParser = GetNonterminal("IncludeDirective");
        includeDirectiveNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<IncludeDirectivesRule>(this, &IncludeDirectivesRule::PreIncludeDirective));
    }
    void PreIncludeDirective(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParserFileContent*>(context.parserFileContent)));
    }
private:
    struct Context
    {
        Context(): parserFileContent() {}
        ParserFileContent* parserFileContent;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ParserFileGrammar::IncludeDirectiveRule : public Cm::Parsing::Rule
{
public:
    IncludeDirectiveRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParserFileContent*", "parserFileContent"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> parserFileContent_value = std::move(stack.top());
        context.parserFileContent = *static_cast<Cm::Parsing::ValueObject<ParserFileContent*>*>(parserFileContent_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<IncludeDirectiveRule>(this, &IncludeDirectiveRule::A0Action));
        Cm::Parsing::NonterminalParser* fileAttributeNonterminalParser = GetNonterminal("FileAttribute");
        fileAttributeNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<IncludeDirectiveRule>(this, &IncludeDirectiveRule::PostFileAttribute));
        Cm::Parsing::NonterminalParser* includeFileNameNonterminalParser = GetNonterminal("IncludeFileName");
        includeFileNameNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<IncludeDirectiveRule>(this, &IncludeDirectiveRule::PostIncludeFileName));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.parserFileContent->AddIncludeDirective(context.fromIncludeFileName, context.fromFileAttribute);
    }
    void PostFileAttribute(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromFileAttribute_value = std::move(stack.top());
            context.fromFileAttribute = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromFileAttribute_value.get());
            stack.pop();
        }
    }
    void PostIncludeFileName(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromIncludeFileName_value = std::move(stack.top());
            context.fromIncludeFileName = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromIncludeFileName_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): parserFileContent(), fromFileAttribute(), fromIncludeFileName() {}
        ParserFileContent* parserFileContent;
        std::string fromFileAttribute;
        std::string fromIncludeFileName;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ParserFileGrammar::FileAttributeRule : public Cm::Parsing::Rule
{
public:
    FileAttributeRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("std::string");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<std::string>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<FileAttributeRule>(this, &FileAttributeRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = std::string(matchBegin, matchEnd);
    }
private:
    struct Context
    {
        Context(): value() {}
        std::string value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ParserFileGrammar::IncludeFileNameRule : public Cm::Parsing::Rule
{
public:
    IncludeFileNameRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("std::string");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<std::string>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<IncludeFileNameRule>(this, &IncludeFileNameRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = std::string(matchBegin, matchEnd);
    }
private:
    struct Context
    {
        Context(): value() {}
        std::string value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ParserFileGrammar::NamespaceContentRule : public Cm::Parsing::Rule
{
public:
    NamespaceContentRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParserFileContent*", "parserFileContent"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> parserFileContent_value = std::move(stack.top());
        context.parserFileContent = *static_cast<Cm::Parsing::ValueObject<ParserFileContent*>*>(parserFileContent_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<NamespaceContentRule>(this, &NamespaceContentRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<NamespaceContentRule>(this, &NamespaceContentRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<NamespaceContentRule>(this, &NamespaceContentRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<NamespaceContentRule>(this, &NamespaceContentRule::A3Action));
        Cm::Parsing::NonterminalParser* fileAttributeNonterminalParser = GetNonterminal("FileAttribute");
        fileAttributeNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<NamespaceContentRule>(this, &NamespaceContentRule::PostFileAttribute));
        Cm::Parsing::NonterminalParser* usingDeclarationNonterminalParser = GetNonterminal("UsingDeclaration");
        usingDeclarationNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<NamespaceContentRule>(this, &NamespaceContentRule::PostUsingDeclaration));
        Cm::Parsing::NonterminalParser* usingDirectiveNonterminalParser = GetNonterminal("UsingDirective");
        usingDirectiveNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<NamespaceContentRule>(this, &NamespaceContentRule::PostUsingDirective));
        Cm::Parsing::NonterminalParser* namespaceAliasDefinitionNonterminalParser = GetNonterminal("NamespaceAliasDefinition");
        namespaceAliasDefinitionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<NamespaceContentRule>(this, &NamespaceContentRule::PostNamespaceAliasDefinition));
        Cm::Parsing::NonterminalParser* grammarNonterminalParser = GetNonterminal("Grammar");
        grammarNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<NamespaceContentRule>(this, &NamespaceContentRule::PreGrammar));
        grammarNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<NamespaceContentRule>(this, &NamespaceContentRule::PostGrammar));
        Cm::Parsing::NonterminalParser* namespaceNonterminalParser = GetNonterminal("Namespace");
        namespaceNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<NamespaceContentRule>(this, &NamespaceContentRule::PreNamespace));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.fromUsingDeclaration->SetFileAttr(context.fromFileAttribute);
        context.parserFileContent->AddUsingObject(context.fromUsingDeclaration);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.fromUsingDirective->SetFileAttr(context.fromFileAttribute);
        context.parserFileContent->AddUsingObject(context.fromUsingDirective);
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.fromNamespaceAliasDefinition->SetFileAttr(context.fromFileAttribute);
        context.parserFileContent->AddUsingObject(context.fromNamespaceAliasDefinition);
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.parserFileContent->AddGrammar(context.fromGrammar);
    }
    void PostFileAttribute(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromFileAttribute_value = std::move(stack.top());
            context.fromFileAttribute = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromFileAttribute_value.get());
            stack.pop();
        }
    }
    void PostUsingDeclaration(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromUsingDeclaration_value = std::move(stack.top());
            context.fromUsingDeclaration = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::UsingObject*>*>(fromUsingDeclaration_value.get());
            stack.pop();
        }
    }
    void PostUsingDirective(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromUsingDirective_value = std::move(stack.top());
            context.fromUsingDirective = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::UsingObject*>*>(fromUsingDirective_value.get());
            stack.pop();
        }
    }
    void PostNamespaceAliasDefinition(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromNamespaceAliasDefinition_value = std::move(stack.top());
            context.fromNamespaceAliasDefinition = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::UsingObject*>*>(fromNamespaceAliasDefinition_value.get());
            stack.pop();
        }
    }
    void PreGrammar(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::Scope*>(context.parserFileContent->CurrentScope())));
    }
    void PostGrammar(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromGrammar_value = std::move(stack.top());
            context.fromGrammar = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::Grammar*>*>(fromGrammar_value.get());
            stack.pop();
        }
    }
    void PreNamespace(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParserFileContent*>(context.parserFileContent)));
    }
private:
    struct Context
    {
        Context(): parserFileContent(), fromFileAttribute(), fromUsingDeclaration(), fromUsingDirective(), fromNamespaceAliasDefinition(), fromGrammar() {}
        ParserFileContent* parserFileContent;
        std::string fromFileAttribute;
        Cm::Parsing::CppObjectModel::UsingObject* fromUsingDeclaration;
        Cm::Parsing::CppObjectModel::UsingObject* fromUsingDirective;
        Cm::Parsing::CppObjectModel::UsingObject* fromNamespaceAliasDefinition;
        Cm::Parsing::Grammar* fromGrammar;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ParserFileGrammar::NamespaceRule : public Cm::Parsing::Rule
{
public:
    NamespaceRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParserFileContent*", "parserFileContent"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> parserFileContent_value = std::move(stack.top());
        context.parserFileContent = *static_cast<Cm::Parsing::ValueObject<ParserFileContent*>*>(parserFileContent_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<NamespaceRule>(this, &NamespaceRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<NamespaceRule>(this, &NamespaceRule::A1Action));
        Cm::Parsing::NonterminalParser* nsNonterminalParser = GetNonterminal("ns");
        nsNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<NamespaceRule>(this, &NamespaceRule::Postns));
        Cm::Parsing::NonterminalParser* namespaceContentNonterminalParser = GetNonterminal("NamespaceContent");
        namespaceContentNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<NamespaceRule>(this, &NamespaceRule::PreNamespaceContent));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.parserFileContent->BeginNamespace(context.fromns);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.parserFileContent->EndNamespace();
    }
    void Postns(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromns_value = std::move(stack.top());
            context.fromns = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromns_value.get());
            stack.pop();
        }
    }
    void PreNamespaceContent(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParserFileContent*>(context.parserFileContent)));
    }
private:
    struct Context
    {
        Context(): parserFileContent(), fromns() {}
        ParserFileContent* parserFileContent;
        std::string fromns;
    };
    std::stack<Context> contextStack;
    Context context;
};

void ParserFileGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parsing.stdlib");
    if (!grammar0)
    {
        grammar0 = Cm::Parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
    Cm::Parsing::Grammar* grammar1 = pd->GetGrammar("Cm.Parsing.Cpp.DeclarationGrammar");
    if (!grammar1)
    {
        grammar1 = Cm::Parsing::Cpp::DeclarationGrammar::Create(pd);
    }
    AddGrammarReference(grammar1);
    Cm::Parsing::Grammar* grammar2 = pd->GetGrammar("Cm.Parsing.Syntax.GrammarGrammar");
    if (!grammar2)
    {
        grammar2 = Cm::Parsing::Syntax::GrammarGrammar::Create(pd);
    }
    AddGrammarReference(grammar2);
}

void ParserFileGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("spaces_and_comments", this, "Cm.Parsing.stdlib.spaces_and_comments"));
    AddRuleLink(new Cm::Parsing::RuleLink("UsingDeclaration", this, "Cm.Parsing.Cpp.DeclarationGrammar.UsingDeclaration"));
    AddRuleLink(new Cm::Parsing::RuleLink("qualified_id", this, "Cm.Parsing.stdlib.qualified_id"));
    AddRuleLink(new Cm::Parsing::RuleLink("newline", this, "Cm.Parsing.stdlib.newline"));
    AddRuleLink(new Cm::Parsing::RuleLink("Grammar", this, "GrammarGrammar.Grammar"));
    AddRuleLink(new Cm::Parsing::RuleLink("UsingDirective", this, "Cm.Parsing.Cpp.DeclarationGrammar.UsingDirective"));
    AddRuleLink(new Cm::Parsing::RuleLink("NamespaceAliasDefinition", this, "Cm.Parsing.Cpp.DeclarationGrammar.NamespaceAliasDefinition"));
    AddRule(new ParserFileRule("ParserFile", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::EmptyParser()),
                new Cm::Parsing::OptionalParser(
                    new Cm::Parsing::NonterminalParser("IncludeDirectives", "IncludeDirectives", 1))),
            new Cm::Parsing::NonterminalParser("NamespaceContent", "NamespaceContent", 1))));
    AddRule(new IncludeDirectivesRule("IncludeDirectives", GetScope(),
        new Cm::Parsing::PositiveParser(
            new Cm::Parsing::NonterminalParser("IncludeDirective", "IncludeDirective", 1))));
    AddRule(new IncludeDirectiveRule("IncludeDirective", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::NonterminalParser("FileAttribute", "FileAttribute", 0)),
                    new Cm::Parsing::CharParser('#')),
                new Cm::Parsing::KeywordParser("include")),
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::TokenParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::NonterminalParser("IncludeFileName", "IncludeFileName", 0),
                            new Cm::Parsing::KleeneStarParser(
                                new Cm::Parsing::CharSetParser("\r\n", true))),
                        new Cm::Parsing::NonterminalParser("newline", "newline", 0)))))));
    AddRule(new FileAttributeRule("FileAttribute", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::CharParser('['),
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::StringParser("cpp"),
                        new Cm::Parsing::StringParser("hpp")))),
            new Cm::Parsing::CharParser(']'))));
    AddRule(new IncludeFileNameRule("IncludeFileName", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::TokenParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::CharParser('<'),
                        new Cm::Parsing::PositiveParser(
                            new Cm::Parsing::CharSetParser(">\r\n", true))),
                    new Cm::Parsing::CharParser('>'))))));
    AddRule(new NamespaceContentRule("NamespaceContent", GetScope(),
        new Cm::Parsing::KleeneStarParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::OptionalParser(
                            new Cm::Parsing::NonterminalParser("FileAttribute", "FileAttribute", 0)),
                        new Cm::Parsing::AlternativeParser(
                            new Cm::Parsing::AlternativeParser(
                                new Cm::Parsing::ActionParser("A0",
                                    new Cm::Parsing::NonterminalParser("UsingDeclaration", "UsingDeclaration", 0)),
                                new Cm::Parsing::ActionParser("A1",
                                    new Cm::Parsing::NonterminalParser("UsingDirective", "UsingDirective", 0))),
                            new Cm::Parsing::ActionParser("A2",
                                new Cm::Parsing::NonterminalParser("NamespaceAliasDefinition", "NamespaceAliasDefinition", 0)))),
                    new Cm::Parsing::ActionParser("A3",
                        new Cm::Parsing::NonterminalParser("Grammar", "Grammar", 1))),
                new Cm::Parsing::NonterminalParser("Namespace", "Namespace", 1)))));
    AddRule(new NamespaceRule("Namespace", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::KeywordParser("namespace"),
                        new Cm::Parsing::ActionParser("A0",
                            new Cm::Parsing::ExpectationParser(
                                new Cm::Parsing::NonterminalParser("ns", "qualified_id", 0)))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::CharParser('{'))),
                new Cm::Parsing::NonterminalParser("NamespaceContent", "NamespaceContent", 1)),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser('}'))))));
    SetSkipRuleName("spaces_and_comments");
}

} } } // namespace Cm.Parsing.Syntax

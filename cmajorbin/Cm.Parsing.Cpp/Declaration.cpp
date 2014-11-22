#include "Declaration.hpp"
#include <Cm.Parsing/Action.hpp>
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>
#include <Cm.Parsing/Primitive.hpp>
#include <Cm.Parsing/Composite.hpp>
#include <Cm.Parsing/Nonterminal.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Parsing/StdLib.hpp>
#include <Cm.Parsing/XmlLog.hpp>
#include <Cm.Parsing.Cpp/Declarator.hpp>
#include <Cm.Parsing.Cpp/Identifier.hpp>
#include <Cm.Parsing.Cpp/Expression.hpp>
#include <Cm.Parsing.CppObjectModel/Type.hpp>

namespace Cm { namespace Parsing { namespace Cpp {

using namespace Cm::Parsing::CppObjectModel;
using namespace Cm::Parsing;

DeclarationGrammar* DeclarationGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

DeclarationGrammar* DeclarationGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    DeclarationGrammar* grammar(new DeclarationGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

DeclarationGrammar::DeclarationGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("DeclarationGrammar", parsingDomain_->GetNamespaceScope("Cm.Parsing.Cpp"), parsingDomain_)
{
    SetOwner(0);
    keywords0.push_back("auto");
    keywords0.push_back("extern");
    keywords0.push_back("mutable");
    keywords0.push_back("register");
    keywords0.push_back("static");
    keywords1.push_back("bool");
    keywords1.push_back("char");
    keywords1.push_back("double");
    keywords1.push_back("float");
    keywords1.push_back("int");
    keywords1.push_back("long");
    keywords1.push_back("short");
    keywords1.push_back("signed");
    keywords1.push_back("unsigned");
    keywords1.push_back("void");
    keywords1.push_back("wchar_t");
}

Cm::Parsing::CppObjectModel::CppObject* DeclarationGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName)
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
    Cm::Parsing::CppObjectModel::CppObject* result = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(value.get());
    stack.pop();
    return result;
}

class DeclarationGrammar::BlockDeclarationRule : public Cm::Parsing::Rule
{
public:
    BlockDeclarationRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::CppObject*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<BlockDeclarationRule>(this, &BlockDeclarationRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<BlockDeclarationRule>(this, &BlockDeclarationRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<BlockDeclarationRule>(this, &BlockDeclarationRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<BlockDeclarationRule>(this, &BlockDeclarationRule::A3Action));
        Cm::Parsing::NonterminalParser* namespaceAliasDefinitionNonterminalParser = GetNonterminal("NamespaceAliasDefinition");
        namespaceAliasDefinitionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<BlockDeclarationRule>(this, &BlockDeclarationRule::PostNamespaceAliasDefinition));
        Cm::Parsing::NonterminalParser* usingDirectiveNonterminalParser = GetNonterminal("UsingDirective");
        usingDirectiveNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<BlockDeclarationRule>(this, &BlockDeclarationRule::PostUsingDirective));
        Cm::Parsing::NonterminalParser* usingDeclarationNonterminalParser = GetNonterminal("UsingDeclaration");
        usingDeclarationNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<BlockDeclarationRule>(this, &BlockDeclarationRule::PostUsingDeclaration));
        Cm::Parsing::NonterminalParser* simpleDeclarationNonterminalParser = GetNonterminal("SimpleDeclaration");
        simpleDeclarationNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<BlockDeclarationRule>(this, &BlockDeclarationRule::PostSimpleDeclaration));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromNamespaceAliasDefinition;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromUsingDirective;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromUsingDeclaration;
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromSimpleDeclaration;
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
    void PostUsingDirective(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromUsingDirective_value = std::move(stack.top());
            context.fromUsingDirective = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::UsingObject*>*>(fromUsingDirective_value.get());
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
    void PostSimpleDeclaration(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromSimpleDeclaration_value = std::move(stack.top());
            context.fromSimpleDeclaration = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::SimpleDeclaration*>*>(fromSimpleDeclaration_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromNamespaceAliasDefinition(), fromUsingDirective(), fromUsingDeclaration(), fromSimpleDeclaration() {}
        Cm::Parsing::CppObjectModel::CppObject* value;
        Cm::Parsing::CppObjectModel::UsingObject* fromNamespaceAliasDefinition;
        Cm::Parsing::CppObjectModel::UsingObject* fromUsingDirective;
        Cm::Parsing::CppObjectModel::UsingObject* fromUsingDeclaration;
        Cm::Parsing::CppObjectModel::SimpleDeclaration* fromSimpleDeclaration;
    };
    std::stack<Context> contextStack;
    Context context;
};

class DeclarationGrammar::SimpleDeclarationRule : public Cm::Parsing::Rule
{
public:
    SimpleDeclarationRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::SimpleDeclaration*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::SimpleDeclaration*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<SimpleDeclarationRule>(this, &SimpleDeclarationRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<SimpleDeclarationRule>(this, &SimpleDeclarationRule::A1Action));
        Cm::Parsing::NonterminalParser* declSpecifierSeqNonterminalParser = GetNonterminal("DeclSpecifierSeq");
        declSpecifierSeqNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<SimpleDeclarationRule>(this, &SimpleDeclarationRule::PreDeclSpecifierSeq));
        Cm::Parsing::NonterminalParser* initDeclaratorListNonterminalParser = GetNonterminal("InitDeclaratorList");
        initDeclaratorListNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<SimpleDeclarationRule>(this, &SimpleDeclarationRule::PostInitDeclaratorList));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new SimpleDeclaration;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->SetInitDeclaratorList(context.fromInitDeclaratorList);
    }
    void PreDeclSpecifierSeq(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::SimpleDeclaration*>(context.value)));
    }
    void PostInitDeclaratorList(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromInitDeclaratorList_value = std::move(stack.top());
            context.fromInitDeclaratorList = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::InitDeclaratorList*>*>(fromInitDeclaratorList_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromInitDeclaratorList() {}
        Cm::Parsing::CppObjectModel::SimpleDeclaration* value;
        Cm::Parsing::CppObjectModel::InitDeclaratorList* fromInitDeclaratorList;
    };
    std::stack<Context> contextStack;
    Context context;
};

class DeclarationGrammar::DeclSpecifierSeqRule : public Cm::Parsing::Rule
{
public:
    DeclSpecifierSeqRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Parsing::CppObjectModel::SimpleDeclaration*", "declaration"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> declaration_value = std::move(stack.top());
        context.declaration = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::SimpleDeclaration*>*>(declaration_value.get());
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DeclSpecifierSeqRule>(this, &DeclSpecifierSeqRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DeclSpecifierSeqRule>(this, &DeclSpecifierSeqRule::A1Action));
        Cm::Parsing::NonterminalParser* declSpecifierNonterminalParser = GetNonterminal("DeclSpecifier");
        declSpecifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DeclSpecifierSeqRule>(this, &DeclSpecifierSeqRule::PostDeclSpecifier));
        Cm::Parsing::NonterminalParser* typeNameNonterminalParser = GetNonterminal("TypeName");
        typeNameNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DeclSpecifierSeqRule>(this, &DeclSpecifierSeqRule::PostTypeName));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.declaration->Add(context.fromDeclSpecifier);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.declaration->Add(context.fromTypeName);
    }
    void PostDeclSpecifier(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromDeclSpecifier_value = std::move(stack.top());
            context.fromDeclSpecifier = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::DeclSpecifier*>*>(fromDeclSpecifier_value.get());
            stack.pop();
        }
    }
    void PostTypeName(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTypeName_value = std::move(stack.top());
            context.fromTypeName = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeName*>*>(fromTypeName_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): declaration(), fromDeclSpecifier(), fromTypeName() {}
        Cm::Parsing::CppObjectModel::SimpleDeclaration* declaration;
        Cm::Parsing::CppObjectModel::DeclSpecifier* fromDeclSpecifier;
        Cm::Parsing::CppObjectModel::TypeName* fromTypeName;
    };
    std::stack<Context> contextStack;
    Context context;
};

class DeclarationGrammar::DeclSpecifierRule : public Cm::Parsing::Rule
{
public:
    DeclSpecifierRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::DeclSpecifier*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::DeclSpecifier*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DeclSpecifierRule>(this, &DeclSpecifierRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DeclSpecifierRule>(this, &DeclSpecifierRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DeclSpecifierRule>(this, &DeclSpecifierRule::A2Action));
        Cm::Parsing::NonterminalParser* storageClassSpecifierNonterminalParser = GetNonterminal("StorageClassSpecifier");
        storageClassSpecifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DeclSpecifierRule>(this, &DeclSpecifierRule::PostStorageClassSpecifier));
        Cm::Parsing::NonterminalParser* typeSpecifierNonterminalParser = GetNonterminal("TypeSpecifier");
        typeSpecifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DeclSpecifierRule>(this, &DeclSpecifierRule::PostTypeSpecifier));
        Cm::Parsing::NonterminalParser* typedefNonterminalParser = GetNonterminal("Typedef");
        typedefNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DeclSpecifierRule>(this, &DeclSpecifierRule::PostTypedef));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromStorageClassSpecifier;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromTypeSpecifier;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromTypedef;
    }
    void PostStorageClassSpecifier(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromStorageClassSpecifier_value = std::move(stack.top());
            context.fromStorageClassSpecifier = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::StorageClassSpecifier*>*>(fromStorageClassSpecifier_value.get());
            stack.pop();
        }
    }
    void PostTypeSpecifier(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTypeSpecifier_value = std::move(stack.top());
            context.fromTypeSpecifier = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeSpecifier*>*>(fromTypeSpecifier_value.get());
            stack.pop();
        }
    }
    void PostTypedef(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTypedef_value = std::move(stack.top());
            context.fromTypedef = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::DeclSpecifier*>*>(fromTypedef_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromStorageClassSpecifier(), fromTypeSpecifier(), fromTypedef() {}
        Cm::Parsing::CppObjectModel::DeclSpecifier* value;
        Cm::Parsing::CppObjectModel::StorageClassSpecifier* fromStorageClassSpecifier;
        Cm::Parsing::CppObjectModel::TypeSpecifier* fromTypeSpecifier;
        Cm::Parsing::CppObjectModel::DeclSpecifier* fromTypedef;
    };
    std::stack<Context> contextStack;
    Context context;
};

class DeclarationGrammar::StorageClassSpecifierRule : public Cm::Parsing::Rule
{
public:
    StorageClassSpecifierRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::StorageClassSpecifier*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::StorageClassSpecifier*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StorageClassSpecifierRule>(this, &StorageClassSpecifierRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Cm::Parsing::CppObjectModel::StorageClassSpecifier(std::string(matchBegin, matchEnd));
    }
private:
    struct Context
    {
        Context(): value() {}
        Cm::Parsing::CppObjectModel::StorageClassSpecifier* value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class DeclarationGrammar::TypeSpecifierRule : public Cm::Parsing::Rule
{
public:
    TypeSpecifierRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::TypeSpecifier*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeSpecifier*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TypeSpecifierRule>(this, &TypeSpecifierRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TypeSpecifierRule>(this, &TypeSpecifierRule::A1Action));
        Cm::Parsing::NonterminalParser* simpleTypeSpecifierNonterminalParser = GetNonterminal("SimpleTypeSpecifier");
        simpleTypeSpecifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TypeSpecifierRule>(this, &TypeSpecifierRule::PostSimpleTypeSpecifier));
        Cm::Parsing::NonterminalParser* cVQualifierNonterminalParser = GetNonterminal("CVQualifier");
        cVQualifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TypeSpecifierRule>(this, &TypeSpecifierRule::PostCVQualifier));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromSimpleTypeSpecifier;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromCVQualifier;
    }
    void PostSimpleTypeSpecifier(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromSimpleTypeSpecifier_value = std::move(stack.top());
            context.fromSimpleTypeSpecifier = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeSpecifier*>*>(fromSimpleTypeSpecifier_value.get());
            stack.pop();
        }
    }
    void PostCVQualifier(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromCVQualifier_value = std::move(stack.top());
            context.fromCVQualifier = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeSpecifier*>*>(fromCVQualifier_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromSimpleTypeSpecifier(), fromCVQualifier() {}
        Cm::Parsing::CppObjectModel::TypeSpecifier* value;
        Cm::Parsing::CppObjectModel::TypeSpecifier* fromSimpleTypeSpecifier;
        Cm::Parsing::CppObjectModel::TypeSpecifier* fromCVQualifier;
    };
    std::stack<Context> contextStack;
    Context context;
};

class DeclarationGrammar::SimpleTypeSpecifierRule : public Cm::Parsing::Rule
{
public:
    SimpleTypeSpecifierRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::TypeSpecifier*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeSpecifier*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<SimpleTypeSpecifierRule>(this, &SimpleTypeSpecifierRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Cm::Parsing::CppObjectModel::TypeSpecifier(std::string(matchBegin, matchEnd));
    }
private:
    struct Context
    {
        Context(): value() {}
        Cm::Parsing::CppObjectModel::TypeSpecifier* value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class DeclarationGrammar::TypeNameRule : public Cm::Parsing::Rule
{
public:
    TypeNameRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::TypeName*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeName*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TypeNameRule>(this, &TypeNameRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TypeNameRule>(this, &TypeNameRule::A1Action));
        Cm::Parsing::NonterminalParser* qualifiedIdNonterminalParser = GetNonterminal("QualifiedId");
        qualifiedIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TypeNameRule>(this, &TypeNameRule::PostQualifiedId));
        Cm::Parsing::NonterminalParser* templateArgumentListNonterminalParser = GetNonterminal("TemplateArgumentList");
        templateArgumentListNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<TypeNameRule>(this, &TypeNameRule::PreTemplateArgumentList));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Cm::Parsing::CppObjectModel::TypeName(std::string(matchBegin, matchEnd));
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->IsTemplate() = true;
    }
    void PostQualifiedId(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromQualifiedId_value = std::move(stack.top());
            context.fromQualifiedId = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromQualifiedId_value.get());
            stack.pop();
        }
    }
    void PreTemplateArgumentList(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeName*>(context.value)));
    }
private:
    struct Context
    {
        Context(): value(), fromQualifiedId() {}
        Cm::Parsing::CppObjectModel::TypeName* value;
        std::string fromQualifiedId;
    };
    std::stack<Context> contextStack;
    Context context;
};

class DeclarationGrammar::TemplateArgumentListRule : public Cm::Parsing::Rule
{
public:
    TemplateArgumentListRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Parsing::CppObjectModel::TypeName*", "typeName"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> typeName_value = std::move(stack.top());
        context.typeName = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeName*>*>(typeName_value.get());
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TemplateArgumentListRule>(this, &TemplateArgumentListRule::A0Action));
        Cm::Parsing::NonterminalParser* templateArgumentNonterminalParser = GetNonterminal("TemplateArgument");
        templateArgumentNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TemplateArgumentListRule>(this, &TemplateArgumentListRule::PostTemplateArgument));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.typeName->AddTemplateArgument(context.fromTemplateArgument);
    }
    void PostTemplateArgument(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTemplateArgument_value = std::move(stack.top());
            context.fromTemplateArgument = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromTemplateArgument_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): typeName(), fromTemplateArgument() {}
        Cm::Parsing::CppObjectModel::TypeName* typeName;
        Cm::Parsing::CppObjectModel::CppObject* fromTemplateArgument;
    };
    std::stack<Context> contextStack;
    Context context;
};

class DeclarationGrammar::TemplateArgumentRule : public Cm::Parsing::Rule
{
public:
    TemplateArgumentRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::CppObject*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TemplateArgumentRule>(this, &TemplateArgumentRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TemplateArgumentRule>(this, &TemplateArgumentRule::A1Action));
        Cm::Parsing::NonterminalParser* typeIdNonterminalParser = GetNonterminal("TypeId");
        typeIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TemplateArgumentRule>(this, &TemplateArgumentRule::PostTypeId));
        Cm::Parsing::NonterminalParser* assignmentExpressionNonterminalParser = GetNonterminal("AssignmentExpression");
        assignmentExpressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TemplateArgumentRule>(this, &TemplateArgumentRule::PostAssignmentExpression));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromTypeId;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromAssignmentExpression;
    }
    void PostTypeId(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTypeId_value = std::move(stack.top());
            context.fromTypeId = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeId*>*>(fromTypeId_value.get());
            stack.pop();
        }
    }
    void PostAssignmentExpression(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromAssignmentExpression_value = std::move(stack.top());
            context.fromAssignmentExpression = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromAssignmentExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromTypeId(), fromAssignmentExpression() {}
        Cm::Parsing::CppObjectModel::CppObject* value;
        Cm::Parsing::CppObjectModel::TypeId* fromTypeId;
        Cm::Parsing::CppObjectModel::CppObject* fromAssignmentExpression;
    };
    std::stack<Context> contextStack;
    Context context;
};

class DeclarationGrammar::TypedefRule : public Cm::Parsing::Rule
{
public:
    TypedefRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::DeclSpecifier*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::DeclSpecifier*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TypedefRule>(this, &TypedefRule::A0Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Typedef;
    }
private:
    struct Context
    {
        Context(): value() {}
        Cm::Parsing::CppObjectModel::DeclSpecifier* value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class DeclarationGrammar::CVQualifierRule : public Cm::Parsing::Rule
{
public:
    CVQualifierRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::TypeSpecifier*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeSpecifier*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CVQualifierRule>(this, &CVQualifierRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CVQualifierRule>(this, &CVQualifierRule::A1Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Const;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Volatile;
    }
private:
    struct Context
    {
        Context(): value() {}
        Cm::Parsing::CppObjectModel::TypeSpecifier* value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class DeclarationGrammar::NamespaceAliasDefinitionRule : public Cm::Parsing::Rule
{
public:
    NamespaceAliasDefinitionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::UsingObject*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::UsingObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<NamespaceAliasDefinitionRule>(this, &NamespaceAliasDefinitionRule::A0Action));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("Identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<NamespaceAliasDefinitionRule>(this, &NamespaceAliasDefinitionRule::PostIdentifier));
        Cm::Parsing::NonterminalParser* qualifiedIdNonterminalParser = GetNonterminal("QualifiedId");
        qualifiedIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<NamespaceAliasDefinitionRule>(this, &NamespaceAliasDefinitionRule::PostQualifiedId));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Cm::Parsing::CppObjectModel::NamespaceAlias(context.fromIdentifier, context.fromQualifiedId);
    }
    void PostIdentifier(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromIdentifier_value = std::move(stack.top());
            context.fromIdentifier = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromIdentifier_value.get());
            stack.pop();
        }
    }
    void PostQualifiedId(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromQualifiedId_value = std::move(stack.top());
            context.fromQualifiedId = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromQualifiedId_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromIdentifier(), fromQualifiedId() {}
        Cm::Parsing::CppObjectModel::UsingObject* value;
        std::string fromIdentifier;
        std::string fromQualifiedId;
    };
    std::stack<Context> contextStack;
    Context context;
};

class DeclarationGrammar::UsingDeclarationRule : public Cm::Parsing::Rule
{
public:
    UsingDeclarationRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::UsingObject*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::UsingObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<UsingDeclarationRule>(this, &UsingDeclarationRule::A0Action));
        Cm::Parsing::NonterminalParser* qualifiedIdNonterminalParser = GetNonterminal("QualifiedId");
        qualifiedIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<UsingDeclarationRule>(this, &UsingDeclarationRule::PostQualifiedId));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Cm::Parsing::CppObjectModel::UsingDeclaration(context.fromQualifiedId);
    }
    void PostQualifiedId(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromQualifiedId_value = std::move(stack.top());
            context.fromQualifiedId = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromQualifiedId_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromQualifiedId() {}
        Cm::Parsing::CppObjectModel::UsingObject* value;
        std::string fromQualifiedId;
    };
    std::stack<Context> contextStack;
    Context context;
};

class DeclarationGrammar::UsingDirectiveRule : public Cm::Parsing::Rule
{
public:
    UsingDirectiveRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::UsingObject*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::UsingObject*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<UsingDirectiveRule>(this, &UsingDirectiveRule::A0Action));
        Cm::Parsing::NonterminalParser* qualifiedIdNonterminalParser = GetNonterminal("QualifiedId");
        qualifiedIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<UsingDirectiveRule>(this, &UsingDirectiveRule::PostQualifiedId));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Cm::Parsing::CppObjectModel::UsingDirective(context.fromQualifiedId);
    }
    void PostQualifiedId(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromQualifiedId_value = std::move(stack.top());
            context.fromQualifiedId = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromQualifiedId_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromQualifiedId() {}
        Cm::Parsing::CppObjectModel::UsingObject* value;
        std::string fromQualifiedId;
    };
    std::stack<Context> contextStack;
    Context context;
};

void DeclarationGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* parsingDomain = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = parsingDomain->GetGrammar("Cm.Parsing.Cpp.ExpressionGrammar");
    if (!grammar0)
    {
        grammar0 = Cm::Parsing::Cpp::ExpressionGrammar::Create(parsingDomain);
    }
    AddGrammarReference(grammar0);
    Cm::Parsing::Grammar* grammar1 = parsingDomain->GetGrammar("Soul.Parsing.stdlib");
    if (!grammar1)
    {
        grammar1 = Soul::Parsing::stdlib::Create(parsingDomain);
    }
    AddGrammarReference(grammar1);
    Cm::Parsing::Grammar* grammar2 = parsingDomain->GetGrammar("Cm.Parsing.Cpp.DeclaratorGrammar");
    if (!grammar2)
    {
        grammar2 = Cm::Parsing::Cpp::DeclaratorGrammar::Create(parsingDomain);
    }
    AddGrammarReference(grammar2);
    Cm::Parsing::Grammar* grammar3 = parsingDomain->GetGrammar("Cm.Parsing.Cpp.IdentifierGrammar");
    if (!grammar3)
    {
        grammar3 = Cm::Parsing::Cpp::IdentifierGrammar::Create(parsingDomain);
    }
    AddGrammarReference(grammar3);
}

void DeclarationGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("QualifiedId", this, "IdentifierGrammar.QualifiedId"));
    AddRuleLink(new Cm::Parsing::RuleLink("identifier", this, "Soul.Parsing.stdlib.identifier"));
    AddRuleLink(new Cm::Parsing::RuleLink("Identifier", this, "IdentifierGrammar.Identifier"));
    AddRuleLink(new Cm::Parsing::RuleLink("AssignmentExpression", this, "ExpressionGrammar.AssignmentExpression"));
    AddRuleLink(new Cm::Parsing::RuleLink("InitDeclaratorList", this, "DeclaratorGrammar.InitDeclaratorList"));
    AddRuleLink(new Cm::Parsing::RuleLink("TypeId", this, "DeclaratorGrammar.TypeId"));
    AddRule(new BlockDeclarationRule("BlockDeclaration", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::ActionParser("A0",
                        new Cm::Parsing::NonterminalParser("NamespaceAliasDefinition", "NamespaceAliasDefinition", 0)),
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::NonterminalParser("UsingDirective", "UsingDirective", 0))),
                new Cm::Parsing::ActionParser("A2",
                    new Cm::Parsing::NonterminalParser("UsingDeclaration", "UsingDeclaration", 0))),
            new Cm::Parsing::ActionParser("A3",
                new Cm::Parsing::NonterminalParser("SimpleDeclaration", "SimpleDeclaration", 0)))));
    AddRule(new SimpleDeclarationRule("SimpleDeclaration", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::ActionParser("A0",
                        new Cm::Parsing::EmptyParser()),
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::NonterminalParser("DeclSpecifierSeq", "DeclSpecifierSeq", 1))),
                new Cm::Parsing::OptionalParser(
                    new Cm::Parsing::ActionParser("A1",
                        new Cm::Parsing::NonterminalParser("InitDeclaratorList", "InitDeclaratorList", 0)))),
            new Cm::Parsing::CharParser(';'))));
    AddRule(new DeclSpecifierSeqRule("DeclSpecifierSeq", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::PositiveParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::NonterminalParser("DeclSpecifier", "DeclSpecifier", 0))),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::NonterminalParser("TypeName", "TypeName", 0)))));
    AddRule(new DeclSpecifierRule("DeclSpecifier", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::NonterminalParser("StorageClassSpecifier", "StorageClassSpecifier", 0)),
                new Cm::Parsing::ActionParser("A1",
                    new Cm::Parsing::NonterminalParser("TypeSpecifier", "TypeSpecifier", 0))),
            new Cm::Parsing::ActionParser("A2",
                new Cm::Parsing::NonterminalParser("Typedef", "Typedef", 0)))));
    AddRule(new StorageClassSpecifierRule("StorageClassSpecifier", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::KeywordListParser("identifier", keywords0))));
    AddRule(new TypeSpecifierRule("TypeSpecifier", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("SimpleTypeSpecifier", "SimpleTypeSpecifier", 0)),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::NonterminalParser("CVQualifier", "CVQualifier", 0)))));
    AddRule(new SimpleTypeSpecifierRule("SimpleTypeSpecifier", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::KeywordListParser("identifier", keywords1))));
    AddRule(new TypeNameRule("TypeName", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("QualifiedId", "QualifiedId", 0)),
            new Cm::Parsing::OptionalParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::ActionParser("A1",
                            new Cm::Parsing::CharParser('<')),
                        new Cm::Parsing::NonterminalParser("TemplateArgumentList", "TemplateArgumentList", 1)),
                    new Cm::Parsing::CharParser('>'))))));
    AddRule(new TemplateArgumentListRule("TemplateArgumentList", GetScope(),
        new Cm::Parsing::ListParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("TemplateArgument", "TemplateArgument", 0)),
            new Cm::Parsing::CharParser(','))));
    AddRule(new TemplateArgumentRule("TemplateArgument", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("TypeId", "TypeId", 0)),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::NonterminalParser("AssignmentExpression", "AssignmentExpression", 0)))));
    AddRule(new TypedefRule("Typedef", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::KeywordParser("typedef"))));
    AddRule(new CVQualifierRule("CVQualifier", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::KeywordParser("const")),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::KeywordParser("volatile")))));
    AddRule(new NamespaceAliasDefinitionRule("NamespaceAliasDefinition", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::KeywordParser("namespace"),
                        new Cm::Parsing::NonterminalParser("Identifier", "Identifier", 0)),
                    new Cm::Parsing::CharParser('=')),
                new Cm::Parsing::NonterminalParser("QualifiedId", "QualifiedId", 0)),
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::CharParser(';')))));
    AddRule(new UsingDeclarationRule("UsingDeclaration", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::KeywordParser("using"),
                new Cm::Parsing::NonterminalParser("QualifiedId", "QualifiedId", 0)),
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::CharParser(';')))));
    AddRule(new UsingDirectiveRule("UsingDirective", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::KeywordParser("using"),
                    new Cm::Parsing::KeywordParser("namespace")),
                new Cm::Parsing::NonterminalParser("QualifiedId", "QualifiedId", 0)),
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::CharParser(';')))));
}

} } } // namespace Cm.Parsing.Cpp

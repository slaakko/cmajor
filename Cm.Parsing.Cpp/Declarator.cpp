#include "Declarator.hpp"
#include <Cm.Parsing/Action.hpp>
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>
#include <Cm.Parsing/Primitive.hpp>
#include <Cm.Parsing/Composite.hpp>
#include <Cm.Parsing/Nonterminal.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Parsing/StdLib.hpp>
#include <Cm.Parsing/XmlLog.hpp>
#include <Cm.Parsing.Cpp/Expression.hpp>
#include <Cm.Parsing.Cpp/Declaration.hpp>
#include <Cm.Util/TextUtils.hpp>
#include <Cm.Parsing.CppObjectModel/Type.hpp>

namespace Cm { namespace Parsing { namespace Cpp {

using Cm::Util::Trim;
using namespace Cm::Parsing::CppObjectModel;
using namespace Cm::Parsing;

DeclaratorGrammar* DeclaratorGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

DeclaratorGrammar* DeclaratorGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    DeclaratorGrammar* grammar(new DeclaratorGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

DeclaratorGrammar::DeclaratorGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("DeclaratorGrammar", parsingDomain_->GetNamespaceScope("Cm.Parsing.Cpp"), parsingDomain_)
{
    SetOwner(0);
}

Cm::Parsing::CppObjectModel::InitDeclaratorList* DeclaratorGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName)
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
    Cm::Parsing::CppObjectModel::InitDeclaratorList* result = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::InitDeclaratorList*>*>(value.get());
    stack.pop();
    return result;
}

class DeclaratorGrammar::InitDeclaratorListRule : public Cm::Parsing::Rule
{
public:
    InitDeclaratorListRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::InitDeclaratorList*");
        AddLocalVariable(AttrOrVariable("std::unique_ptr<InitDeclaratorList>", "idl"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::InitDeclaratorList*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<InitDeclaratorListRule>(this, &InitDeclaratorListRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<InitDeclaratorListRule>(this, &InitDeclaratorListRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<InitDeclaratorListRule>(this, &InitDeclaratorListRule::A2Action));
        Cm::Parsing::NonterminalParser* initDeclaratorNonterminalParser = GetNonterminal("InitDeclarator");
        initDeclaratorNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<InitDeclaratorListRule>(this, &InitDeclaratorListRule::PostInitDeclarator));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.idl.reset(new InitDeclaratorList);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.idl.release();
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.idl->Add(context.fromInitDeclarator);
    }
    void PostInitDeclarator(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromInitDeclarator_value = std::move(stack.top());
            context.fromInitDeclarator = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::InitDeclarator*>*>(fromInitDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), idl(), fromInitDeclarator() {}
        Cm::Parsing::CppObjectModel::InitDeclaratorList* value;
        std::unique_ptr<InitDeclaratorList> idl;
        Cm::Parsing::CppObjectModel::InitDeclarator* fromInitDeclarator;
    };
    std::stack<Context> contextStack;
    Context context;
};

class DeclaratorGrammar::InitDeclaratorRule : public Cm::Parsing::Rule
{
public:
    InitDeclaratorRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::InitDeclarator*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::InitDeclarator*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<InitDeclaratorRule>(this, &InitDeclaratorRule::A0Action));
        Cm::Parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal("Declarator");
        declaratorNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<InitDeclaratorRule>(this, &InitDeclaratorRule::PostDeclarator));
        Cm::Parsing::NonterminalParser* initializerNonterminalParser = GetNonterminal("Initializer");
        initializerNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<InitDeclaratorRule>(this, &InitDeclaratorRule::PostInitializer));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new InitDeclarator(context.fromDeclarator, context.fromInitializer);
    }
    void PostDeclarator(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromDeclarator_value = std::move(stack.top());
            context.fromDeclarator = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromDeclarator_value.get());
            stack.pop();
        }
    }
    void PostInitializer(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromInitializer_value = std::move(stack.top());
            context.fromInitializer = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Initializer*>*>(fromInitializer_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromDeclarator(), fromInitializer() {}
        Cm::Parsing::CppObjectModel::InitDeclarator* value;
        std::string fromDeclarator;
        Cm::Parsing::CppObjectModel::Initializer* fromInitializer;
    };
    std::stack<Context> contextStack;
    Context context;
};

class DeclaratorGrammar::DeclaratorRule : public Cm::Parsing::Rule
{
public:
    DeclaratorRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DeclaratorRule>(this, &DeclaratorRule::A0Action));
        Cm::Parsing::NonterminalParser* directDeclaratorNonterminalParser = GetNonterminal("DirectDeclarator");
        Cm::Parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal("Declarator");
        declaratorNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DeclaratorRule>(this, &DeclaratorRule::PostDeclarator));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = Trim(std::string(matchBegin, matchEnd));
    }
    void PostDeclarator(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromDeclarator_value = std::move(stack.top());
            context.fromDeclarator = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromDeclarator() {}
        std::string value;
        std::string fromDeclarator;
    };
    std::stack<Context> contextStack;
    Context context;
};

class DeclaratorGrammar::DirectDeclaratorRule : public Cm::Parsing::Rule
{
public:
    DirectDeclaratorRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddLocalVariable(AttrOrVariable("std::unique_ptr<CppObject>", "o"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DirectDeclaratorRule>(this, &DirectDeclaratorRule::A0Action));
        Cm::Parsing::NonterminalParser* declaratorIdNonterminalParser = GetNonterminal("DeclaratorId");
        Cm::Parsing::NonterminalParser* constantExpressionNonterminalParser = GetNonterminal("ConstantExpression");
        constantExpressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DirectDeclaratorRule>(this, &DirectDeclaratorRule::PostConstantExpression));
        Cm::Parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal("Declarator");
        declaratorNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DirectDeclaratorRule>(this, &DirectDeclaratorRule::PostDeclarator));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.o.reset(context.fromConstantExpression);
    }
    void PostConstantExpression(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromConstantExpression_value = std::move(stack.top());
            context.fromConstantExpression = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromConstantExpression_value.get());
            stack.pop();
        }
    }
    void PostDeclarator(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromDeclarator_value = std::move(stack.top());
            context.fromDeclarator = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): o(), fromConstantExpression(), fromDeclarator() {}
        std::unique_ptr<CppObject> o;
        Cm::Parsing::CppObjectModel::CppObject* fromConstantExpression;
        std::string fromDeclarator;
    };
    std::stack<Context> contextStack;
    Context context;
};

class DeclaratorGrammar::DeclaratorIdRule : public Cm::Parsing::Rule
{
public:
    DeclaratorIdRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddLocalVariable(AttrOrVariable("std::unique_ptr<CppObject>", "o"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DeclaratorIdRule>(this, &DeclaratorIdRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DeclaratorIdRule>(this, &DeclaratorIdRule::A1Action));
        Cm::Parsing::NonterminalParser* idExpressionNonterminalParser = GetNonterminal("IdExpression");
        idExpressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DeclaratorIdRule>(this, &DeclaratorIdRule::PostIdExpression));
        Cm::Parsing::NonterminalParser* typeNameNonterminalParser = GetNonterminal("TypeName");
        typeNameNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DeclaratorIdRule>(this, &DeclaratorIdRule::PostTypeName));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.o.reset(context.fromIdExpression);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.o.reset(context.fromTypeName);
    }
    void PostIdExpression(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromIdExpression_value = std::move(stack.top());
            context.fromIdExpression = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::IdExpr*>*>(fromIdExpression_value.get());
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
        Context(): o(), fromIdExpression(), fromTypeName() {}
        std::unique_ptr<CppObject> o;
        Cm::Parsing::CppObjectModel::IdExpr* fromIdExpression;
        Cm::Parsing::CppObjectModel::TypeName* fromTypeName;
    };
    std::stack<Context> contextStack;
    Context context;
};

class DeclaratorGrammar::TypeIdRule : public Cm::Parsing::Rule
{
public:
    TypeIdRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::TypeId*");
        AddLocalVariable(AttrOrVariable("std::unique_ptr<TypeId>", "ti"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeId*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TypeIdRule>(this, &TypeIdRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TypeIdRule>(this, &TypeIdRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TypeIdRule>(this, &TypeIdRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TypeIdRule>(this, &TypeIdRule::A3Action));
        Cm::Parsing::NonterminalParser* typeSpecifierSeqNonterminalParser = GetNonterminal("TypeSpecifierSeq");
        typeSpecifierSeqNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<TypeIdRule>(this, &TypeIdRule::PreTypeSpecifierSeq));
        Cm::Parsing::NonterminalParser* typeNameNonterminalParser = GetNonterminal("TypeName");
        typeNameNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TypeIdRule>(this, &TypeIdRule::PostTypeName));
        Cm::Parsing::NonterminalParser* abstractDeclaratorNonterminalParser = GetNonterminal("AbstractDeclarator");
        abstractDeclaratorNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TypeIdRule>(this, &TypeIdRule::PostAbstractDeclarator));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ti.reset(new TypeId);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.ti.release();
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ti->Add(context.fromTypeName);
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ti->Declarator() = context.fromAbstractDeclarator;
    }
    void PreTypeSpecifierSeq(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeId*>(context.ti.get())));
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
    void PostAbstractDeclarator(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromAbstractDeclarator_value = std::move(stack.top());
            context.fromAbstractDeclarator = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromAbstractDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), ti(), fromTypeName(), fromAbstractDeclarator() {}
        Cm::Parsing::CppObjectModel::TypeId* value;
        std::unique_ptr<TypeId> ti;
        Cm::Parsing::CppObjectModel::TypeName* fromTypeName;
        std::string fromAbstractDeclarator;
    };
    std::stack<Context> contextStack;
    Context context;
};

class DeclaratorGrammar::TypeRule : public Cm::Parsing::Rule
{
public:
    TypeRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::Type*");
        AddLocalVariable(AttrOrVariable("std::unique_ptr<Type>", "t"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Type*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TypeRule>(this, &TypeRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TypeRule>(this, &TypeRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TypeRule>(this, &TypeRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TypeRule>(this, &TypeRule::A3Action));
        Cm::Parsing::NonterminalParser* typeSpecifierNonterminalParser = GetNonterminal("TypeSpecifier");
        typeSpecifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TypeRule>(this, &TypeRule::PostTypeSpecifier));
        Cm::Parsing::NonterminalParser* typeNameNonterminalParser = GetNonterminal("TypeName");
        typeNameNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TypeRule>(this, &TypeRule::PostTypeName));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.t.reset(new Type);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.t.release();
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.t->Add(context.fromTypeSpecifier);
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.t->Add(context.fromTypeName);
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
        Context(): value(), t(), fromTypeSpecifier(), fromTypeName() {}
        Cm::Parsing::CppObjectModel::Type* value;
        std::unique_ptr<Type> t;
        Cm::Parsing::CppObjectModel::TypeSpecifier* fromTypeSpecifier;
        Cm::Parsing::CppObjectModel::TypeName* fromTypeName;
    };
    std::stack<Context> contextStack;
    Context context;
};

class DeclaratorGrammar::TypeSpecifierSeqRule : public Cm::Parsing::Rule
{
public:
    TypeSpecifierSeqRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Parsing::CppObjectModel::TypeId*", "typeId"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> typeId_value = std::move(stack.top());
        context.typeId = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::TypeId*>*>(typeId_value.get());
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TypeSpecifierSeqRule>(this, &TypeSpecifierSeqRule::A0Action));
        Cm::Parsing::NonterminalParser* typeSpecifierNonterminalParser = GetNonterminal("TypeSpecifier");
        typeSpecifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TypeSpecifierSeqRule>(this, &TypeSpecifierSeqRule::PostTypeSpecifier));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.typeId->Add(context.fromTypeSpecifier);
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
private:
    struct Context
    {
        Context(): typeId(), fromTypeSpecifier() {}
        Cm::Parsing::CppObjectModel::TypeId* typeId;
        Cm::Parsing::CppObjectModel::TypeSpecifier* fromTypeSpecifier;
    };
    std::stack<Context> contextStack;
    Context context;
};

class DeclaratorGrammar::AbstractDeclaratorRule : public Cm::Parsing::Rule
{
public:
    AbstractDeclaratorRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AbstractDeclaratorRule>(this, &AbstractDeclaratorRule::A0Action));
        Cm::Parsing::NonterminalParser* abstractDeclaratorNonterminalParser = GetNonterminal("AbstractDeclarator");
        abstractDeclaratorNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<AbstractDeclaratorRule>(this, &AbstractDeclaratorRule::PostAbstractDeclarator));
        Cm::Parsing::NonterminalParser* directAbstractDeclaratorNonterminalParser = GetNonterminal("DirectAbstractDeclarator");
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = Trim(std::string(matchBegin, matchEnd));
    }
    void PostAbstractDeclarator(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromAbstractDeclarator_value = std::move(stack.top());
            context.fromAbstractDeclarator = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromAbstractDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromAbstractDeclarator() {}
        std::string value;
        std::string fromAbstractDeclarator;
    };
    std::stack<Context> contextStack;
    Context context;
};

class DeclaratorGrammar::DirectAbstractDeclaratorRule : public Cm::Parsing::Rule
{
public:
    DirectAbstractDeclaratorRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddLocalVariable(AttrOrVariable("std::unique_ptr<CppObject>", "o"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DirectAbstractDeclaratorRule>(this, &DirectAbstractDeclaratorRule::A0Action));
        Cm::Parsing::NonterminalParser* constantExpressionNonterminalParser = GetNonterminal("ConstantExpression");
        constantExpressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DirectAbstractDeclaratorRule>(this, &DirectAbstractDeclaratorRule::PostConstantExpression));
        Cm::Parsing::NonterminalParser* abstractDeclaratorNonterminalParser = GetNonterminal("AbstractDeclarator");
        abstractDeclaratorNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DirectAbstractDeclaratorRule>(this, &DirectAbstractDeclaratorRule::PostAbstractDeclarator));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.o.reset(context.fromConstantExpression);
    }
    void PostConstantExpression(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromConstantExpression_value = std::move(stack.top());
            context.fromConstantExpression = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::CppObject*>*>(fromConstantExpression_value.get());
            stack.pop();
        }
    }
    void PostAbstractDeclarator(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromAbstractDeclarator_value = std::move(stack.top());
            context.fromAbstractDeclarator = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromAbstractDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): o(), fromConstantExpression(), fromAbstractDeclarator() {}
        std::unique_ptr<CppObject> o;
        Cm::Parsing::CppObjectModel::CppObject* fromConstantExpression;
        std::string fromAbstractDeclarator;
    };
    std::stack<Context> contextStack;
    Context context;
};

class DeclaratorGrammar::CVQualifierSeqRule : public Cm::Parsing::Rule
{
public:
    CVQualifierSeqRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddLocalVariable(AttrOrVariable("std::unique_ptr<CppObject>", "o"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CVQualifierSeqRule>(this, &CVQualifierSeqRule::A0Action));
        Cm::Parsing::NonterminalParser* cVQualifierNonterminalParser = GetNonterminal("CVQualifier");
        cVQualifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<CVQualifierSeqRule>(this, &CVQualifierSeqRule::PostCVQualifier));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.o.reset(context.fromCVQualifier);
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
        Context(): o(), fromCVQualifier() {}
        std::unique_ptr<CppObject> o;
        Cm::Parsing::CppObjectModel::TypeSpecifier* fromCVQualifier;
    };
    std::stack<Context> contextStack;
    Context context;
};

class DeclaratorGrammar::InitializerRule : public Cm::Parsing::Rule
{
public:
    InitializerRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::Initializer*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::Initializer*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<InitializerRule>(this, &InitializerRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<InitializerRule>(this, &InitializerRule::A1Action));
        Cm::Parsing::NonterminalParser* initializerClauseNonterminalParser = GetNonterminal("InitializerClause");
        initializerClauseNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<InitializerRule>(this, &InitializerRule::PostInitializerClause));
        Cm::Parsing::NonterminalParser* expressionListNonterminalParser = GetNonterminal("ExpressionList");
        expressionListNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<InitializerRule>(this, &InitializerRule::PostExpressionList));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Initializer(context.fromInitializerClause, GetEmptyVector());
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Initializer(nullptr, context.fromExpressionList);
    }
    void PostInitializerClause(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromInitializerClause_value = std::move(stack.top());
            context.fromInitializerClause = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::AssignInit*>*>(fromInitializerClause_value.get());
            stack.pop();
        }
    }
    void PostExpressionList(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromExpressionList_value = std::move(stack.top());
            context.fromExpressionList = *static_cast<Cm::Parsing::ValueObject<std::vector<Cm::Parsing::CppObjectModel::CppObject*>>*>(fromExpressionList_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromInitializerClause(), fromExpressionList() {}
        Cm::Parsing::CppObjectModel::Initializer* value;
        Cm::Parsing::CppObjectModel::AssignInit* fromInitializerClause;
        std::vector<Cm::Parsing::CppObjectModel::CppObject*> fromExpressionList;
    };
    std::stack<Context> contextStack;
    Context context;
};

class DeclaratorGrammar::InitializerClauseRule : public Cm::Parsing::Rule
{
public:
    InitializerClauseRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Parsing::CppObjectModel::AssignInit*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::AssignInit*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<InitializerClauseRule>(this, &InitializerClauseRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<InitializerClauseRule>(this, &InitializerClauseRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<InitializerClauseRule>(this, &InitializerClauseRule::A2Action));
        Cm::Parsing::NonterminalParser* assignmentExpressionNonterminalParser = GetNonterminal("AssignmentExpression");
        assignmentExpressionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<InitializerClauseRule>(this, &InitializerClauseRule::PostAssignmentExpression));
        Cm::Parsing::NonterminalParser* initializerListNonterminalParser = GetNonterminal("InitializerList");
        initializerListNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<InitializerClauseRule>(this, &InitializerClauseRule::PreInitializerList));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new AssignInit(context.fromAssignmentExpression);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new AssignInit(nullptr);
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new AssignInit(nullptr);
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
    void PreInitializerList(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::AssignInit*>(context.value)));
    }
private:
    struct Context
    {
        Context(): value(), fromAssignmentExpression() {}
        Cm::Parsing::CppObjectModel::AssignInit* value;
        Cm::Parsing::CppObjectModel::CppObject* fromAssignmentExpression;
    };
    std::stack<Context> contextStack;
    Context context;
};

class DeclaratorGrammar::InitializerListRule : public Cm::Parsing::Rule
{
public:
    InitializerListRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Parsing::CppObjectModel::AssignInit*", "init"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> init_value = std::move(stack.top());
        context.init = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::AssignInit*>*>(init_value.get());
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<InitializerListRule>(this, &InitializerListRule::A0Action));
        Cm::Parsing::NonterminalParser* initializerClauseNonterminalParser = GetNonterminal("InitializerClause");
        initializerClauseNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<InitializerListRule>(this, &InitializerListRule::PostInitializerClause));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.init->Add(context.fromInitializerClause);
    }
    void PostInitializerClause(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromInitializerClause_value = std::move(stack.top());
            context.fromInitializerClause = *static_cast<Cm::Parsing::ValueObject<Cm::Parsing::CppObjectModel::AssignInit*>*>(fromInitializerClause_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): init(), fromInitializerClause() {}
        Cm::Parsing::CppObjectModel::AssignInit* init;
        Cm::Parsing::CppObjectModel::AssignInit* fromInitializerClause;
    };
    std::stack<Context> contextStack;
    Context context;
};

void DeclaratorGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* parsingDomain = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = parsingDomain->GetGrammar("Cm.Parsing.Cpp.DeclarationGrammar");
    if (!grammar0)
    {
        grammar0 = Cm::Parsing::Cpp::DeclarationGrammar::Create(parsingDomain);
    }
    AddGrammarReference(grammar0);
    Cm::Parsing::Grammar* grammar1 = parsingDomain->GetGrammar("Cm.Parsing.Cpp.ExpressionGrammar");
    if (!grammar1)
    {
        grammar1 = Cm::Parsing::Cpp::ExpressionGrammar::Create(parsingDomain);
    }
    AddGrammarReference(grammar1);
}

void DeclaratorGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("AssignmentExpression", this, "ExpressionGrammar.AssignmentExpression"));
    AddRuleLink(new Cm::Parsing::RuleLink("IdExpression", this, "ExpressionGrammar.IdExpression"));
    AddRuleLink(new Cm::Parsing::RuleLink("ConstantExpression", this, "ExpressionGrammar.ConstantExpression"));
    AddRuleLink(new Cm::Parsing::RuleLink("ExpressionList", this, "ExpressionGrammar.ExpressionList"));
    AddRuleLink(new Cm::Parsing::RuleLink("TypeSpecifier", this, "DeclarationGrammar.TypeSpecifier"));
    AddRuleLink(new Cm::Parsing::RuleLink("TypeName", this, "DeclarationGrammar.TypeName"));
    AddRuleLink(new Cm::Parsing::RuleLink("CVQualifier", this, "DeclarationGrammar.CVQualifier"));
    AddRule(new InitDeclaratorListRule("InitDeclaratorList", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::EmptyParser()),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::ListParser(
                    new Cm::Parsing::ActionParser("A2",
                        new Cm::Parsing::NonterminalParser("InitDeclarator", "InitDeclarator", 0)),
                    new Cm::Parsing::CharParser(','))))));
    AddRule(new InitDeclaratorRule("InitDeclarator", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::NonterminalParser("Declarator", "Declarator", 0),
                new Cm::Parsing::OptionalParser(
                    new Cm::Parsing::NonterminalParser("Initializer", "Initializer", 0))))));
    AddRule(new DeclaratorRule("Declarator", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::NonterminalParser("DirectDeclarator", "DirectDeclarator", 0),
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::NonterminalParser("PtrOperator", "PtrOperator", 0),
                    new Cm::Parsing::NonterminalParser("Declarator", "Declarator", 0))))));
    AddRule(new DirectDeclaratorRule("DirectDeclarator", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::NonterminalParser("DeclaratorId", "DeclaratorId", 0),
            new Cm::Parsing::KleeneStarParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::SequenceParser(
                                    new Cm::Parsing::CharParser('['),
                                    new Cm::Parsing::OptionalParser(
                                        new Cm::Parsing::ActionParser("A0",
                                            new Cm::Parsing::NonterminalParser("ConstantExpression", "ConstantExpression", 0)))),
                                new Cm::Parsing::CharParser(']')),
                            new Cm::Parsing::CharParser('(')),
                        new Cm::Parsing::NonterminalParser("Declarator", "Declarator", 0)),
                    new Cm::Parsing::CharParser(')'))))));
    AddRule(new DeclaratorIdRule("DeclaratorId", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("IdExpression", "IdExpression", 0)),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::NonterminalParser("TypeName", "TypeName", 0)))));
    AddRule(new TypeIdRule("TypeId", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::EmptyParser()),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::NonterminalParser("TypeSpecifierSeq", "TypeSpecifierSeq", 1),
                        new Cm::Parsing::ActionParser("A2",
                            new Cm::Parsing::NonterminalParser("TypeName", "TypeName", 0))),
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::ActionParser("A3",
                            new Cm::Parsing::NonterminalParser("AbstractDeclarator", "AbstractDeclarator", 0))))))));
    AddRule(new TypeRule("Type", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::EmptyParser()),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::PositiveParser(
                        new Cm::Parsing::ActionParser("A2",
                            new Cm::Parsing::NonterminalParser("TypeSpecifier", "TypeSpecifier", 0))),
                    new Cm::Parsing::ActionParser("A3",
                        new Cm::Parsing::NonterminalParser("TypeName", "TypeName", 0)))))));
    AddRule(new TypeSpecifierSeqRule("TypeSpecifierSeq", GetScope(),
        new Cm::Parsing::PositiveParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("TypeSpecifier", "TypeSpecifier", 0)))));
    AddRule(new AbstractDeclaratorRule("AbstractDeclarator", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::NonterminalParser("PtrOperator", "PtrOperator", 0),
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::NonterminalParser("AbstractDeclarator", "AbstractDeclarator", 0))),
                new Cm::Parsing::NonterminalParser("DirectAbstractDeclarator", "DirectAbstractDeclarator", 0)))));
    AddRule(new DirectAbstractDeclaratorRule("DirectAbstractDeclarator", GetScope(),
        new Cm::Parsing::PositiveParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::CharParser('['),
                        new Cm::Parsing::OptionalParser(
                            new Cm::Parsing::ActionParser("A0",
                                new Cm::Parsing::NonterminalParser("ConstantExpression", "ConstantExpression", 0)))),
                    new Cm::Parsing::CharParser(']')),
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::CharParser('('),
                        new Cm::Parsing::NonterminalParser("AbstractDeclarator", "AbstractDeclarator", 0)),
                    new Cm::Parsing::CharParser(')'))))));
    AddRule(new Cm::Parsing::Rule("PtrOperator", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::CharParser('*'),
                new Cm::Parsing::OptionalParser(
                    new Cm::Parsing::NonterminalParser("CVQualifierSeq", "CVQualifierSeq", 0))),
            new Cm::Parsing::CharParser('&'))));
    AddRule(new CVQualifierSeqRule("CVQualifierSeq", GetScope(),
        new Cm::Parsing::PositiveParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("CVQualifier", "CVQualifier", 0)))));
    AddRule(new InitializerRule("Initializer", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::CharParser('='),
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::NonterminalParser("InitializerClause", "InitializerClause", 0))),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::CharParser('('),
                        new Cm::Parsing::NonterminalParser("ExpressionList", "ExpressionList", 0)),
                    new Cm::Parsing::CharParser(')'))))));
    AddRule(new InitializerClauseRule("InitializerClause", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::NonterminalParser("AssignmentExpression", "AssignmentExpression", 0)),
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::ActionParser("A1",
                            new Cm::Parsing::CharParser('{')),
                        new Cm::Parsing::NonterminalParser("InitializerList", "InitializerList", 1)),
                    new Cm::Parsing::CharParser('}'))),
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::CharParser('{'),
                new Cm::Parsing::ActionParser("A2",
                    new Cm::Parsing::CharParser('}'))))));
    AddRule(new InitializerListRule("InitializerList", GetScope(),
        new Cm::Parsing::ListParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("InitializerClause", "InitializerClause", 0)),
            new Cm::Parsing::CharParser(','))));
}

} } } // namespace Cm.Parsing.Cpp

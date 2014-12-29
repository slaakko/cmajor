#include "CompileUnit.hpp"
#include <Cm.Parsing/Action.hpp>
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>
#include <Cm.Parsing/Primitive.hpp>
#include <Cm.Parsing/Composite.hpp>
#include <Cm.Parsing/Nonterminal.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Parsing/StdLib.hpp>
#include <Cm.Parsing/XmlLog.hpp>
#include <Cm.Parser/Identifier.hpp>
#include <Cm.Parser/Constant.hpp>
#include <Cm.Parser/Function.hpp>
#include <Cm.Parser/Enumeration.hpp>
#include <Cm.Parser/Typedef.hpp>
#include <Cm.Parser/Class.hpp>
#include <Cm.Parser/Delegate.hpp>
#include <Cm.Parser/Concept.hpp>
#include <Cm.Ast/Namespace.hpp>
#include <Cm.Ast/Identifier.hpp>
#include <Cm.Ast/Function.hpp>
#include <Cm.Ast/Constant.hpp>
#include <Cm.Ast/Enumeration.hpp>
#include <Cm.Ast/Typedef.hpp>
#include <Cm.Ast/Class.hpp>
#include <Cm.Ast/Delegate.hpp>
#include <Cm.Ast/Concept.hpp>

namespace Cm { namespace Parser {

using namespace Cm::Ast;
using namespace Cm::Parsing;

CompileUnitGrammar* CompileUnitGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

CompileUnitGrammar* CompileUnitGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    CompileUnitGrammar* grammar(new CompileUnitGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

CompileUnitGrammar::CompileUnitGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("CompileUnitGrammar", parsingDomain_->GetNamespaceScope("Cm.Parser"), parsingDomain_)
{
    SetOwner(0);
}

Cm::Ast::CompileUnitNode* CompileUnitGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, ParsingContext* ctx)
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
    stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<ParsingContext*>(ctx)));
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
    Cm::Ast::CompileUnitNode* result = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>*>(value.get());
    stack.pop();
    return result;
}

class CompileUnitGrammar::CompileUnitRule : public Cm::Parsing::Rule
{
public:
    CompileUnitRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::CompileUnitNode*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CompileUnitRule>(this, &CompileUnitRule::A0Action));
        Cm::Parsing::NonterminalParser* namespaceContentNonterminalParser = GetNonterminal("NamespaceContent");
        namespaceContentNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<CompileUnitRule>(this, &CompileUnitRule::PreNamespaceContent));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new CompileUnitNode(span, fileName);
    }
    void PreNamespaceContent(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>(context.value)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::NamespaceNode*>(context.value->GlobalNs())));
    }
private:
    struct Context
    {
        Context(): ctx(), value() {}
        ParsingContext* ctx;
        Cm::Ast::CompileUnitNode* value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class CompileUnitGrammar::NamespaceContentRule : public Cm::Parsing::Rule
{
public:
    NamespaceContentRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::CompileUnitNode*", "compileUnit"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::NamespaceNode*", "ns"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> ns_value = std::move(stack.top());
        context.ns = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::NamespaceNode*>*>(ns_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> compileUnit_value = std::move(stack.top());
        context.compileUnit = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>*>(compileUnit_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::NonterminalParser* usingDirectivesNonterminalParser = GetNonterminal("UsingDirectives");
        usingDirectivesNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<NamespaceContentRule>(this, &NamespaceContentRule::PreUsingDirectives));
        Cm::Parsing::NonterminalParser* definitionsNonterminalParser = GetNonterminal("Definitions");
        definitionsNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<NamespaceContentRule>(this, &NamespaceContentRule::PreDefinitions));
    }
    void PreUsingDirectives(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::NamespaceNode*>(context.ns)));
    }
    void PreDefinitions(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>(context.compileUnit)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::NamespaceNode*>(context.ns)));
    }
private:
    struct Context
    {
        Context(): ctx(), compileUnit(), ns() {}
        ParsingContext* ctx;
        Cm::Ast::CompileUnitNode* compileUnit;
        Cm::Ast::NamespaceNode* ns;
    };
    std::stack<Context> contextStack;
    Context context;
};

class CompileUnitGrammar::UsingDirectivesRule : public Cm::Parsing::Rule
{
public:
    UsingDirectivesRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::NamespaceNode*", "ns"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> ns_value = std::move(stack.top());
        context.ns = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::NamespaceNode*>*>(ns_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::NonterminalParser* usingDirectiveNonterminalParser = GetNonterminal("UsingDirective");
        usingDirectiveNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<UsingDirectivesRule>(this, &UsingDirectivesRule::PreUsingDirective));
    }
    void PreUsingDirective(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::NamespaceNode*>(context.ns)));
    }
private:
    struct Context
    {
        Context(): ctx(), ns() {}
        ParsingContext* ctx;
        Cm::Ast::NamespaceNode* ns;
    };
    std::stack<Context> contextStack;
    Context context;
};

class CompileUnitGrammar::UsingDirectiveRule : public Cm::Parsing::Rule
{
public:
    UsingDirectiveRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::NamespaceNode*", "ns"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> ns_value = std::move(stack.top());
        context.ns = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::NamespaceNode*>*>(ns_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<UsingDirectiveRule>(this, &UsingDirectiveRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<UsingDirectiveRule>(this, &UsingDirectiveRule::A1Action));
        Cm::Parsing::NonterminalParser* usingAliasDirectiveNonterminalParser = GetNonterminal("UsingAliasDirective");
        usingAliasDirectiveNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<UsingDirectiveRule>(this, &UsingDirectiveRule::PostUsingAliasDirective));
        Cm::Parsing::NonterminalParser* usingNamespaceDirectiveNonterminalParser = GetNonterminal("UsingNamespaceDirective");
        usingNamespaceDirectiveNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<UsingDirectiveRule>(this, &UsingDirectiveRule::PostUsingNamespaceDirective));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ns->AddMember(context.fromUsingAliasDirective);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ns->AddMember(context.fromUsingNamespaceDirective);
    }
    void PostUsingAliasDirective(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromUsingAliasDirective_value = std::move(stack.top());
            context.fromUsingAliasDirective = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::AliasNode*>*>(fromUsingAliasDirective_value.get());
            stack.pop();
        }
    }
    void PostUsingNamespaceDirective(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromUsingNamespaceDirective_value = std::move(stack.top());
            context.fromUsingNamespaceDirective = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::NamespaceImportNode*>*>(fromUsingNamespaceDirective_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), ns(), fromUsingAliasDirective(), fromUsingNamespaceDirective() {}
        ParsingContext* ctx;
        Cm::Ast::NamespaceNode* ns;
        Cm::Ast::AliasNode* fromUsingAliasDirective;
        Cm::Ast::NamespaceImportNode* fromUsingNamespaceDirective;
    };
    std::stack<Context> contextStack;
    Context context;
};

class CompileUnitGrammar::UsingAliasDirectiveRule : public Cm::Parsing::Rule
{
public:
    UsingAliasDirectiveRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Ast::AliasNode*");
        AddLocalVariable(AttrOrVariable("std::unique_ptr<IdentifierNode>", "id"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::AliasNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<UsingAliasDirectiveRule>(this, &UsingAliasDirectiveRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<UsingAliasDirectiveRule>(this, &UsingAliasDirectiveRule::A1Action));
        Cm::Parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal("Identifier");
        identifierNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<UsingAliasDirectiveRule>(this, &UsingAliasDirectiveRule::PostIdentifier));
        Cm::Parsing::NonterminalParser* qualifiedIdNonterminalParser = GetNonterminal("QualifiedId");
        qualifiedIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<UsingAliasDirectiveRule>(this, &UsingAliasDirectiveRule::PostQualifiedId));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new AliasNode(span, context.id.release(), context.fromQualifiedId);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.id.reset(context.fromIdentifier);
    }
    void PostIdentifier(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromIdentifier_value = std::move(stack.top());
            context.fromIdentifier = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::IdentifierNode*>*>(fromIdentifier_value.get());
            stack.pop();
        }
    }
    void PostQualifiedId(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromQualifiedId_value = std::move(stack.top());
            context.fromQualifiedId = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::IdentifierNode*>*>(fromQualifiedId_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), id(), fromIdentifier(), fromQualifiedId() {}
        Cm::Ast::AliasNode* value;
        std::unique_ptr<IdentifierNode> id;
        Cm::Ast::IdentifierNode* fromIdentifier;
        Cm::Ast::IdentifierNode* fromQualifiedId;
    };
    std::stack<Context> contextStack;
    Context context;
};

class CompileUnitGrammar::UsingNamespaceDirectiveRule : public Cm::Parsing::Rule
{
public:
    UsingNamespaceDirectiveRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Ast::NamespaceImportNode*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::NamespaceImportNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<UsingNamespaceDirectiveRule>(this, &UsingNamespaceDirectiveRule::A0Action));
        Cm::Parsing::NonterminalParser* qualifiedIdNonterminalParser = GetNonterminal("QualifiedId");
        qualifiedIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<UsingNamespaceDirectiveRule>(this, &UsingNamespaceDirectiveRule::PostQualifiedId));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new NamespaceImportNode(span, context.fromQualifiedId);
    }
    void PostQualifiedId(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromQualifiedId_value = std::move(stack.top());
            context.fromQualifiedId = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::IdentifierNode*>*>(fromQualifiedId_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromQualifiedId() {}
        Cm::Ast::NamespaceImportNode* value;
        Cm::Ast::IdentifierNode* fromQualifiedId;
    };
    std::stack<Context> contextStack;
    Context context;
};

class CompileUnitGrammar::DefinitionsRule : public Cm::Parsing::Rule
{
public:
    DefinitionsRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::CompileUnitNode*", "compileUnit"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::NamespaceNode*", "ns"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> ns_value = std::move(stack.top());
        context.ns = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::NamespaceNode*>*>(ns_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> compileUnit_value = std::move(stack.top());
        context.compileUnit = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>*>(compileUnit_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DefinitionsRule>(this, &DefinitionsRule::A0Action));
        Cm::Parsing::NonterminalParser* definitionNonterminalParser = GetNonterminal("Definition");
        definitionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DefinitionsRule>(this, &DefinitionsRule::PreDefinition));
        definitionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DefinitionsRule>(this, &DefinitionsRule::PostDefinition));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.ns->AddMember(context.fromDefinition);
    }
    void PreDefinition(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>(context.compileUnit)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::NamespaceNode*>(context.ns)));
    }
    void PostDefinition(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromDefinition_value = std::move(stack.top());
            context.fromDefinition = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromDefinition_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), compileUnit(), ns(), fromDefinition() {}
        ParsingContext* ctx;
        Cm::Ast::CompileUnitNode* compileUnit;
        Cm::Ast::NamespaceNode* ns;
        Cm::Ast::Node* fromDefinition;
    };
    std::stack<Context> contextStack;
    Context context;
};

class CompileUnitGrammar::DefinitionRule : public Cm::Parsing::Rule
{
public:
    DefinitionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::CompileUnitNode*", "compileUnit"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::NamespaceNode*", "ns"));
        SetValueTypeName("Cm::Ast::Node*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> ns_value = std::move(stack.top());
        context.ns = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::NamespaceNode*>*>(ns_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> compileUnit_value = std::move(stack.top());
        context.compileUnit = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>*>(compileUnit_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Node*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DefinitionRule>(this, &DefinitionRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DefinitionRule>(this, &DefinitionRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DefinitionRule>(this, &DefinitionRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DefinitionRule>(this, &DefinitionRule::A3Action));
        Cm::Parsing::ActionParser* a4ActionParser = GetAction("A4");
        a4ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DefinitionRule>(this, &DefinitionRule::A4Action));
        Cm::Parsing::ActionParser* a5ActionParser = GetAction("A5");
        a5ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DefinitionRule>(this, &DefinitionRule::A5Action));
        Cm::Parsing::ActionParser* a6ActionParser = GetAction("A6");
        a6ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DefinitionRule>(this, &DefinitionRule::A6Action));
        Cm::Parsing::ActionParser* a7ActionParser = GetAction("A7");
        a7ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DefinitionRule>(this, &DefinitionRule::A7Action));
        Cm::Parsing::ActionParser* a8ActionParser = GetAction("A8");
        a8ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DefinitionRule>(this, &DefinitionRule::A8Action));
        Cm::Parsing::NonterminalParser* namespaceDefinitionNonterminalParser = GetNonterminal("NamespaceDefinition");
        namespaceDefinitionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DefinitionRule>(this, &DefinitionRule::PreNamespaceDefinition));
        namespaceDefinitionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DefinitionRule>(this, &DefinitionRule::PostNamespaceDefinition));
        Cm::Parsing::NonterminalParser* functionDefinitionNonterminalParser = GetNonterminal("FunctionDefinition");
        functionDefinitionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DefinitionRule>(this, &DefinitionRule::PreFunctionDefinition));
        functionDefinitionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DefinitionRule>(this, &DefinitionRule::PostFunctionDefinition));
        Cm::Parsing::NonterminalParser* constantDefinitionNonterminalParser = GetNonterminal("ConstantDefinition");
        constantDefinitionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DefinitionRule>(this, &DefinitionRule::PreConstantDefinition));
        constantDefinitionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DefinitionRule>(this, &DefinitionRule::PostConstantDefinition));
        Cm::Parsing::NonterminalParser* enumerationDefinitionNonterminalParser = GetNonterminal("EnumerationDefinition");
        enumerationDefinitionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DefinitionRule>(this, &DefinitionRule::PreEnumerationDefinition));
        enumerationDefinitionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DefinitionRule>(this, &DefinitionRule::PostEnumerationDefinition));
        Cm::Parsing::NonterminalParser* typedefDefinitionNonterminalParser = GetNonterminal("TypedefDefinition");
        typedefDefinitionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DefinitionRule>(this, &DefinitionRule::PreTypedefDefinition));
        typedefDefinitionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DefinitionRule>(this, &DefinitionRule::PostTypedefDefinition));
        Cm::Parsing::NonterminalParser* classDefinitionNonterminalParser = GetNonterminal("ClassDefinition");
        classDefinitionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DefinitionRule>(this, &DefinitionRule::PreClassDefinition));
        classDefinitionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DefinitionRule>(this, &DefinitionRule::PostClassDefinition));
        Cm::Parsing::NonterminalParser* delegateDefinitionNonterminalParser = GetNonterminal("DelegateDefinition");
        delegateDefinitionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DefinitionRule>(this, &DefinitionRule::PreDelegateDefinition));
        delegateDefinitionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DefinitionRule>(this, &DefinitionRule::PostDelegateDefinition));
        Cm::Parsing::NonterminalParser* classDelegateDefinitionNonterminalParser = GetNonterminal("ClassDelegateDefinition");
        classDelegateDefinitionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DefinitionRule>(this, &DefinitionRule::PreClassDelegateDefinition));
        classDelegateDefinitionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DefinitionRule>(this, &DefinitionRule::PostClassDelegateDefinition));
        Cm::Parsing::NonterminalParser* conceptDefinitionNonterminalParser = GetNonterminal("ConceptDefinition");
        conceptDefinitionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DefinitionRule>(this, &DefinitionRule::PreConceptDefinition));
        conceptDefinitionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DefinitionRule>(this, &DefinitionRule::PostConceptDefinition));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromNamespaceDefinition;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromFunctionDefinition;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromConstantDefinition;
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromEnumerationDefinition;
    }
    void A4Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromTypedefDefinition;
    }
    void A5Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromClassDefinition;
    }
    void A6Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromDelegateDefinition;
    }
    void A7Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromClassDelegateDefinition;
    }
    void A8Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromConceptDefinition;
    }
    void PreNamespaceDefinition(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>(context.compileUnit)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::NamespaceNode*>(context.ns)));
    }
    void PostNamespaceDefinition(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromNamespaceDefinition_value = std::move(stack.top());
            context.fromNamespaceDefinition = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::NamespaceNode*>*>(fromNamespaceDefinition_value.get());
            stack.pop();
        }
    }
    void PreFunctionDefinition(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>(context.compileUnit)));
    }
    void PostFunctionDefinition(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromFunctionDefinition_value = std::move(stack.top());
            context.fromFunctionDefinition = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::FunctionNode*>*>(fromFunctionDefinition_value.get());
            stack.pop();
        }
    }
    void PreConstantDefinition(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostConstantDefinition(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromConstantDefinition_value = std::move(stack.top());
            context.fromConstantDefinition = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromConstantDefinition_value.get());
            stack.pop();
        }
    }
    void PreEnumerationDefinition(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostEnumerationDefinition(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromEnumerationDefinition_value = std::move(stack.top());
            context.fromEnumerationDefinition = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromEnumerationDefinition_value.get());
            stack.pop();
        }
    }
    void PreTypedefDefinition(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostTypedefDefinition(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTypedefDefinition_value = std::move(stack.top());
            context.fromTypedefDefinition = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromTypedefDefinition_value.get());
            stack.pop();
        }
    }
    void PreClassDefinition(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostClassDefinition(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromClassDefinition_value = std::move(stack.top());
            context.fromClassDefinition = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromClassDefinition_value.get());
            stack.pop();
        }
    }
    void PreDelegateDefinition(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostDelegateDefinition(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromDelegateDefinition_value = std::move(stack.top());
            context.fromDelegateDefinition = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromDelegateDefinition_value.get());
            stack.pop();
        }
    }
    void PreClassDelegateDefinition(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostClassDelegateDefinition(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromClassDelegateDefinition_value = std::move(stack.top());
            context.fromClassDelegateDefinition = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromClassDelegateDefinition_value.get());
            stack.pop();
        }
    }
    void PreConceptDefinition(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostConceptDefinition(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromConceptDefinition_value = std::move(stack.top());
            context.fromConceptDefinition = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromConceptDefinition_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), compileUnit(), ns(), value(), fromNamespaceDefinition(), fromFunctionDefinition(), fromConstantDefinition(), fromEnumerationDefinition(), fromTypedefDefinition(), fromClassDefinition(), fromDelegateDefinition(), fromClassDelegateDefinition(), fromConceptDefinition() {}
        ParsingContext* ctx;
        Cm::Ast::CompileUnitNode* compileUnit;
        Cm::Ast::NamespaceNode* ns;
        Cm::Ast::Node* value;
        Cm::Ast::NamespaceNode* fromNamespaceDefinition;
        Cm::Ast::FunctionNode* fromFunctionDefinition;
        Cm::Ast::Node* fromConstantDefinition;
        Cm::Ast::Node* fromEnumerationDefinition;
        Cm::Ast::Node* fromTypedefDefinition;
        Cm::Ast::Node* fromClassDefinition;
        Cm::Ast::Node* fromDelegateDefinition;
        Cm::Ast::Node* fromClassDelegateDefinition;
        Cm::Ast::Node* fromConceptDefinition;
    };
    std::stack<Context> contextStack;
    Context context;
};

class CompileUnitGrammar::NamespaceDefinitionRule : public Cm::Parsing::Rule
{
public:
    NamespaceDefinitionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::CompileUnitNode*", "compileUnit"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::NamespaceNode*", "ns"));
        SetValueTypeName("Cm::Ast::NamespaceNode*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> ns_value = std::move(stack.top());
        context.ns = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::NamespaceNode*>*>(ns_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> compileUnit_value = std::move(stack.top());
        context.compileUnit = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>*>(compileUnit_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::NamespaceNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<NamespaceDefinitionRule>(this, &NamespaceDefinitionRule::A0Action));
        Cm::Parsing::NonterminalParser* qualifiedIdNonterminalParser = GetNonterminal("QualifiedId");
        qualifiedIdNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<NamespaceDefinitionRule>(this, &NamespaceDefinitionRule::PostQualifiedId));
        Cm::Parsing::NonterminalParser* namespaceContentNonterminalParser = GetNonterminal("NamespaceContent");
        namespaceContentNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<NamespaceDefinitionRule>(this, &NamespaceDefinitionRule::PreNamespaceContent));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new NamespaceNode(span, context.fromQualifiedId);
    }
    void PostQualifiedId(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromQualifiedId_value = std::move(stack.top());
            context.fromQualifiedId = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::IdentifierNode*>*>(fromQualifiedId_value.get());
            stack.pop();
        }
    }
    void PreNamespaceContent(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>(context.compileUnit)));
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::NamespaceNode*>(context.value)));
    }
private:
    struct Context
    {
        Context(): ctx(), compileUnit(), ns(), value(), fromQualifiedId() {}
        ParsingContext* ctx;
        Cm::Ast::CompileUnitNode* compileUnit;
        Cm::Ast::NamespaceNode* ns;
        Cm::Ast::NamespaceNode* value;
        Cm::Ast::IdentifierNode* fromQualifiedId;
    };
    std::stack<Context> contextStack;
    Context context;
};

class CompileUnitGrammar::FunctionDefinitionRule : public Cm::Parsing::Rule
{
public:
    FunctionDefinitionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::CompileUnitNode*", "compileUnit"));
        SetValueTypeName("Cm::Ast::FunctionNode*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> compileUnit_value = std::move(stack.top());
        context.compileUnit = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::CompileUnitNode*>*>(compileUnit_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::FunctionNode*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<FunctionDefinitionRule>(this, &FunctionDefinitionRule::A0Action));
        Cm::Parsing::NonterminalParser* functionNonterminalParser = GetNonterminal("Function");
        functionNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<FunctionDefinitionRule>(this, &FunctionDefinitionRule::PreFunction));
        functionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<FunctionDefinitionRule>(this, &FunctionDefinitionRule::PostFunction));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromFunction;
        context.value->SetCompileUnit(context.compileUnit);
    }
    void PreFunction(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostFunction(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromFunction_value = std::move(stack.top());
            context.fromFunction = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::FunctionNode*>*>(fromFunction_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), compileUnit(), value(), fromFunction() {}
        ParsingContext* ctx;
        Cm::Ast::CompileUnitNode* compileUnit;
        Cm::Ast::FunctionNode* value;
        Cm::Ast::FunctionNode* fromFunction;
    };
    std::stack<Context> contextStack;
    Context context;
};

class CompileUnitGrammar::ConstantDefinitionRule : public Cm::Parsing::Rule
{
public:
    ConstantDefinitionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::Node*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Node*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConstantDefinitionRule>(this, &ConstantDefinitionRule::A0Action));
        Cm::Parsing::NonterminalParser* constantNonterminalParser = GetNonterminal("Constant");
        constantNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ConstantDefinitionRule>(this, &ConstantDefinitionRule::PreConstant));
        constantNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConstantDefinitionRule>(this, &ConstantDefinitionRule::PostConstant));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromConstant;
    }
    void PreConstant(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostConstant(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromConstant_value = std::move(stack.top());
            context.fromConstant = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromConstant_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromConstant() {}
        ParsingContext* ctx;
        Cm::Ast::Node* value;
        Cm::Ast::Node* fromConstant;
    };
    std::stack<Context> contextStack;
    Context context;
};

class CompileUnitGrammar::EnumerationDefinitionRule : public Cm::Parsing::Rule
{
public:
    EnumerationDefinitionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::Node*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Node*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<EnumerationDefinitionRule>(this, &EnumerationDefinitionRule::A0Action));
        Cm::Parsing::NonterminalParser* enumTypeNonterminalParser = GetNonterminal("EnumType");
        enumTypeNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<EnumerationDefinitionRule>(this, &EnumerationDefinitionRule::PreEnumType));
        enumTypeNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<EnumerationDefinitionRule>(this, &EnumerationDefinitionRule::PostEnumType));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromEnumType;
    }
    void PreEnumType(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostEnumType(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromEnumType_value = std::move(stack.top());
            context.fromEnumType = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::EnumTypeNode*>*>(fromEnumType_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromEnumType() {}
        ParsingContext* ctx;
        Cm::Ast::Node* value;
        Cm::Ast::EnumTypeNode* fromEnumType;
    };
    std::stack<Context> contextStack;
    Context context;
};

class CompileUnitGrammar::TypedefDefinitionRule : public Cm::Parsing::Rule
{
public:
    TypedefDefinitionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::Node*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Node*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TypedefDefinitionRule>(this, &TypedefDefinitionRule::A0Action));
        Cm::Parsing::NonterminalParser* typedefNonterminalParser = GetNonterminal("Typedef");
        typedefNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<TypedefDefinitionRule>(this, &TypedefDefinitionRule::PreTypedef));
        typedefNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TypedefDefinitionRule>(this, &TypedefDefinitionRule::PostTypedef));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromTypedef;
    }
    void PreTypedef(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostTypedef(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTypedef_value = std::move(stack.top());
            context.fromTypedef = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromTypedef_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromTypedef() {}
        ParsingContext* ctx;
        Cm::Ast::Node* value;
        Cm::Ast::Node* fromTypedef;
    };
    std::stack<Context> contextStack;
    Context context;
};

class CompileUnitGrammar::ClassDefinitionRule : public Cm::Parsing::Rule
{
public:
    ClassDefinitionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::Node*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Node*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ClassDefinitionRule>(this, &ClassDefinitionRule::A0Action));
        Cm::Parsing::NonterminalParser* classNonterminalParser = GetNonterminal("Class");
        classNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ClassDefinitionRule>(this, &ClassDefinitionRule::PreClass));
        classNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ClassDefinitionRule>(this, &ClassDefinitionRule::PostClass));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromClass;
    }
    void PreClass(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostClass(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromClass_value = std::move(stack.top());
            context.fromClass = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ClassNode*>*>(fromClass_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromClass() {}
        ParsingContext* ctx;
        Cm::Ast::Node* value;
        Cm::Ast::ClassNode* fromClass;
    };
    std::stack<Context> contextStack;
    Context context;
};

class CompileUnitGrammar::DelegateDefinitionRule : public Cm::Parsing::Rule
{
public:
    DelegateDefinitionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::Node*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Node*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DelegateDefinitionRule>(this, &DelegateDefinitionRule::A0Action));
        Cm::Parsing::NonterminalParser* delegateNonterminalParser = GetNonterminal("Delegate");
        delegateNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DelegateDefinitionRule>(this, &DelegateDefinitionRule::PreDelegate));
        delegateNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DelegateDefinitionRule>(this, &DelegateDefinitionRule::PostDelegate));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromDelegate;
    }
    void PreDelegate(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostDelegate(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromDelegate_value = std::move(stack.top());
            context.fromDelegate = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromDelegate_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromDelegate() {}
        ParsingContext* ctx;
        Cm::Ast::Node* value;
        Cm::Ast::Node* fromDelegate;
    };
    std::stack<Context> contextStack;
    Context context;
};

class CompileUnitGrammar::ClassDelegateDefinitionRule : public Cm::Parsing::Rule
{
public:
    ClassDelegateDefinitionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::Node*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Node*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ClassDelegateDefinitionRule>(this, &ClassDelegateDefinitionRule::A0Action));
        Cm::Parsing::NonterminalParser* classDelegateNonterminalParser = GetNonterminal("ClassDelegate");
        classDelegateNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ClassDelegateDefinitionRule>(this, &ClassDelegateDefinitionRule::PreClassDelegate));
        classDelegateNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ClassDelegateDefinitionRule>(this, &ClassDelegateDefinitionRule::PostClassDelegate));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromClassDelegate;
    }
    void PreClassDelegate(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostClassDelegate(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromClassDelegate_value = std::move(stack.top());
            context.fromClassDelegate = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Node*>*>(fromClassDelegate_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromClassDelegate() {}
        ParsingContext* ctx;
        Cm::Ast::Node* value;
        Cm::Ast::Node* fromClassDelegate;
    };
    std::stack<Context> contextStack;
    Context context;
};

class CompileUnitGrammar::ConceptDefinitionRule : public Cm::Parsing::Rule
{
public:
    ConceptDefinitionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("ParsingContext*", "ctx"));
        SetValueTypeName("Cm::Ast::Node*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> ctx_value = std::move(stack.top());
        context.ctx = *static_cast<Cm::Parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Node*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ConceptDefinitionRule>(this, &ConceptDefinitionRule::A0Action));
        Cm::Parsing::NonterminalParser* conceptNonterminalParser = GetNonterminal("Concept");
        conceptNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ConceptDefinitionRule>(this, &ConceptDefinitionRule::PreConcept));
        conceptNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ConceptDefinitionRule>(this, &ConceptDefinitionRule::PostConcept));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromConcept;
    }
    void PreConcept(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<ParsingContext*>(context.ctx)));
    }
    void PostConcept(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromConcept_value = std::move(stack.top());
            context.fromConcept = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ConceptNode*>*>(fromConcept_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): ctx(), value(), fromConcept() {}
        ParsingContext* ctx;
        Cm::Ast::Node* value;
        Cm::Ast::ConceptNode* fromConcept;
    };
    std::stack<Context> contextStack;
    Context context;
};

void CompileUnitGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parser.FunctionGrammar");
    if (!grammar0)
    {
        grammar0 = Cm::Parser::FunctionGrammar::Create(pd);
    }
    AddGrammarReference(grammar0);
    Cm::Parsing::Grammar* grammar1 = pd->GetGrammar("Cm.Parsing.stdlib");
    if (!grammar1)
    {
        grammar1 = Cm::Parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar1);
    Cm::Parsing::Grammar* grammar2 = pd->GetGrammar("Cm.Parser.IdentifierGrammar");
    if (!grammar2)
    {
        grammar2 = Cm::Parser::IdentifierGrammar::Create(pd);
    }
    AddGrammarReference(grammar2);
    Cm::Parsing::Grammar* grammar3 = pd->GetGrammar("Cm.Parser.ConceptGrammar");
    if (!grammar3)
    {
        grammar3 = Cm::Parser::ConceptGrammar::Create(pd);
    }
    AddGrammarReference(grammar3);
    Cm::Parsing::Grammar* grammar4 = pd->GetGrammar("Cm.Parser.DelegateGrammar");
    if (!grammar4)
    {
        grammar4 = Cm::Parser::DelegateGrammar::Create(pd);
    }
    AddGrammarReference(grammar4);
    Cm::Parsing::Grammar* grammar5 = pd->GetGrammar("Cm.Parser.ClassGrammar");
    if (!grammar5)
    {
        grammar5 = Cm::Parser::ClassGrammar::Create(pd);
    }
    AddGrammarReference(grammar5);
    Cm::Parsing::Grammar* grammar6 = pd->GetGrammar("Cm.Parser.EnumerationGrammar");
    if (!grammar6)
    {
        grammar6 = Cm::Parser::EnumerationGrammar::Create(pd);
    }
    AddGrammarReference(grammar6);
    Cm::Parsing::Grammar* grammar7 = pd->GetGrammar("Cm.Parser.ConstantGrammar");
    if (!grammar7)
    {
        grammar7 = Cm::Parser::ConstantGrammar::Create(pd);
    }
    AddGrammarReference(grammar7);
    Cm::Parsing::Grammar* grammar8 = pd->GetGrammar("Cm.Parser.TypedefGrammar");
    if (!grammar8)
    {
        grammar8 = Cm::Parser::TypedefGrammar::Create(pd);
    }
    AddGrammarReference(grammar8);
}

void CompileUnitGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("Function", this, "FunctionGrammar.Function"));
    AddRuleLink(new Cm::Parsing::RuleLink("Concept", this, "ConceptGrammar.Concept"));
    AddRuleLink(new Cm::Parsing::RuleLink("Identifier", this, "IdentifierGrammar.Identifier"));
    AddRuleLink(new Cm::Parsing::RuleLink("Class", this, "ClassGrammar.Class"));
    AddRuleLink(new Cm::Parsing::RuleLink("EnumType", this, "EnumerationGrammar.EnumType"));
    AddRuleLink(new Cm::Parsing::RuleLink("QualifiedId", this, "IdentifierGrammar.QualifiedId"));
    AddRuleLink(new Cm::Parsing::RuleLink("Constant", this, "ConstantGrammar.Constant"));
    AddRuleLink(new Cm::Parsing::RuleLink("Typedef", this, "TypedefGrammar.Typedef"));
    AddRuleLink(new Cm::Parsing::RuleLink("Delegate", this, "DelegateGrammar.Delegate"));
    AddRuleLink(new Cm::Parsing::RuleLink("ClassDelegate", this, "DelegateGrammar.ClassDelegate"));
    AddRuleLink(new Cm::Parsing::RuleLink("spaces_and_comments", this, "Cm.Parsing.stdlib.spaces_and_comments"));
    AddRule(new CompileUnitRule("CompileUnit", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::EmptyParser()),
            new Cm::Parsing::NonterminalParser("NamespaceContent", "NamespaceContent", 3))));
    AddRule(new NamespaceContentRule("NamespaceContent", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::NonterminalParser("UsingDirectives", "UsingDirectives", 2),
            new Cm::Parsing::NonterminalParser("Definitions", "Definitions", 3))));
    AddRule(new UsingDirectivesRule("UsingDirectives", GetScope(),
        new Cm::Parsing::KleeneStarParser(
            new Cm::Parsing::NonterminalParser("UsingDirective", "UsingDirective", 2))));
    AddRule(new UsingDirectiveRule("UsingDirective", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("UsingAliasDirective", "UsingAliasDirective", 0)),
            new Cm::Parsing::ActionParser("A1",
                new Cm::Parsing::NonterminalParser("UsingNamespaceDirective", "UsingNamespaceDirective", 0)))));
    AddRule(new UsingAliasDirectiveRule("UsingAliasDirective", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::KeywordParser("using"),
                            new Cm::Parsing::ActionParser("A1",
                                new Cm::Parsing::NonterminalParser("Identifier", "Identifier", 0))),
                        new Cm::Parsing::CharParser('=')),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::NonterminalParser("QualifiedId", "QualifiedId", 0))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new UsingNamespaceDirectiveRule("UsingNamespaceDirective", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::KeywordParser("using"),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::NonterminalParser("QualifiedId", "QualifiedId", 0))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new DefinitionsRule("Definitions", GetScope(),
        new Cm::Parsing::KleeneStarParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("Definition", "Definition", 3)))));
    AddRule(new DefinitionRule("Definition", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::AlternativeParser(
                            new Cm::Parsing::AlternativeParser(
                                new Cm::Parsing::AlternativeParser(
                                    new Cm::Parsing::AlternativeParser(
                                        new Cm::Parsing::ActionParser("A0",
                                            new Cm::Parsing::NonterminalParser("NamespaceDefinition", "NamespaceDefinition", 3)),
                                        new Cm::Parsing::ActionParser("A1",
                                            new Cm::Parsing::NonterminalParser("FunctionDefinition", "FunctionDefinition", 2))),
                                    new Cm::Parsing::ActionParser("A2",
                                        new Cm::Parsing::NonterminalParser("ConstantDefinition", "ConstantDefinition", 1))),
                                new Cm::Parsing::ActionParser("A3",
                                    new Cm::Parsing::NonterminalParser("EnumerationDefinition", "EnumerationDefinition", 1))),
                            new Cm::Parsing::ActionParser("A4",
                                new Cm::Parsing::NonterminalParser("TypedefDefinition", "TypedefDefinition", 1))),
                        new Cm::Parsing::ActionParser("A5",
                            new Cm::Parsing::NonterminalParser("ClassDefinition", "ClassDefinition", 1))),
                    new Cm::Parsing::ActionParser("A6",
                        new Cm::Parsing::NonterminalParser("DelegateDefinition", "DelegateDefinition", 1))),
                new Cm::Parsing::ActionParser("A7",
                    new Cm::Parsing::NonterminalParser("ClassDelegateDefinition", "ClassDelegateDefinition", 1))),
            new Cm::Parsing::ActionParser("A8",
                new Cm::Parsing::NonterminalParser("ConceptDefinition", "ConceptDefinition", 1)))));
    AddRule(new NamespaceDefinitionRule("NamespaceDefinition", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::ActionParser("A0",
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::KeywordParser("namespace"),
                            new Cm::Parsing::ExpectationParser(
                                new Cm::Parsing::NonterminalParser("QualifiedId", "QualifiedId", 0)))),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::CharParser('{'))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::NonterminalParser("NamespaceContent", "NamespaceContent", 3))),
            new Cm::Parsing::ExpectationParser(
                new Cm::Parsing::CharParser('}')))));
    AddRule(new FunctionDefinitionRule("FunctionDefinition", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("Function", "Function", 1))));
    AddRule(new ConstantDefinitionRule("ConstantDefinition", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("Constant", "Constant", 1))));
    AddRule(new EnumerationDefinitionRule("EnumerationDefinition", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("EnumType", "EnumType", 1))));
    AddRule(new TypedefDefinitionRule("TypedefDefinition", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("Typedef", "Typedef", 1))));
    AddRule(new ClassDefinitionRule("ClassDefinition", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("Class", "Class", 1))));
    AddRule(new DelegateDefinitionRule("DelegateDefinition", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("Delegate", "Delegate", 1))));
    AddRule(new ClassDelegateDefinitionRule("ClassDelegateDefinition", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("ClassDelegate", "ClassDelegate", 1))));
    AddRule(new ConceptDefinitionRule("ConceptDefinition", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("Concept", "Concept", 1))));
    SetSkipRuleName("spaces_and_comments");
    SetRecover();
}

} } // namespace Cm.Parser

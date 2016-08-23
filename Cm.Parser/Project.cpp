#include "Project.hpp"
#include <Cm.Parsing/Action.hpp>
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>
#include <Cm.Parsing/Primitive.hpp>
#include <Cm.Parsing/Composite.hpp>
#include <Cm.Parsing/Nonterminal.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Parsing/StdLib.hpp>
#include <Cm.Parsing/XmlLog.hpp>
#include <Cm.Parser/LlvmVersion.hpp>

namespace Cm { namespace Parser {

using namespace Cm::Ast;
using namespace Cm::Parsing;

ProjectGrammar* ProjectGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

ProjectGrammar* ProjectGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    ProjectGrammar* grammar(new ProjectGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

ProjectGrammar::ProjectGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("ProjectGrammar", parsingDomain_->GetNamespaceScope("Cm.Parser"), parsingDomain_)
{
    SetOwner(0);
}

Cm::Ast::Project* ProjectGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName, std::string config, std::string backend, std::string os, int bits, Cm::Ast::ProgramVersion llvmVersion)
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
    stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<std::string>(config)));
    stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<std::string>(backend)));
    stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<std::string>(os)));
    stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<int>(bits)));
    stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<Cm::Ast::ProgramVersion>(llvmVersion)));
    Cm::Parsing::Match match = Cm::Parsing::Grammar::Parse(scanner, stack);
    Cm::Parsing::Span stop = scanner.GetSpan();
    if (Log())
    {
        xmlLog->WriteEndRule("parse");
    }
    if (!match.Hit() || !CC() && stop.Start() != int(end - start))
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
    Cm::Ast::Project* result = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Project*>*>(value.get());
    stack.pop();
    return result;
}

class ProjectGrammar::ProjectRule : public Cm::Parsing::Rule
{
public:
    ProjectRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("std::string", "config"));
        AddInheritedAttribute(AttrOrVariable("std::string", "backend"));
        AddInheritedAttribute(AttrOrVariable("std::string", "os"));
        AddInheritedAttribute(AttrOrVariable("int", "bits"));
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::ProgramVersion", "llvmVersion"));
        SetValueTypeName("Cm::Ast::Project*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> llvmVersion_value = std::move(stack.top());
        context.llvmVersion = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ProgramVersion>*>(llvmVersion_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> bits_value = std::move(stack.top());
        context.bits = *static_cast<Cm::Parsing::ValueObject<int>*>(bits_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> os_value = std::move(stack.top());
        context.os = *static_cast<Cm::Parsing::ValueObject<std::string>*>(os_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> backend_value = std::move(stack.top());
        context.backend = *static_cast<Cm::Parsing::ValueObject<std::string>*>(backend_value.get());
        stack.pop();
        std::unique_ptr<Cm::Parsing::Object> config_value = std::move(stack.top());
        context.config = *static_cast<Cm::Parsing::ValueObject<std::string>*>(config_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Project*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ProjectRule>(this, &ProjectRule::A0Action));
        Cm::Parsing::NonterminalParser* qualified_idNonterminalParser = GetNonterminal("qualified_id");
        qualified_idNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ProjectRule>(this, &ProjectRule::Postqualified_id));
        Cm::Parsing::NonterminalParser* declarationsNonterminalParser = GetNonterminal("Declarations");
        declarationsNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<ProjectRule>(this, &ProjectRule::PreDeclarations));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Project(context.fromqualified_id, fileName, context.config, context.backend, context.os, context.bits, context.llvmVersion);
    }
    void Postqualified_id(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromqualified_id_value = std::move(stack.top());
            context.fromqualified_id = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromqualified_id_value.get());
            stack.pop();
        }
    }
    void PreDeclarations(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Project*>(context.value)));
    }
private:
    struct Context
    {
        Context(): config(), backend(), os(), bits(), llvmVersion(), value(), fromqualified_id() {}
        std::string config;
        std::string backend;
        std::string os;
        int bits;
        Cm::Ast::ProgramVersion llvmVersion;
        Cm::Ast::Project* value;
        std::string fromqualified_id;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ProjectGrammar::DeclarationsRule : public Cm::Parsing::Rule
{
public:
    DeclarationsRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::Project*", "project"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> project_value = std::move(stack.top());
        context.project = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Project*>*>(project_value.get());
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DeclarationsRule>(this, &DeclarationsRule::A0Action));
        Cm::Parsing::NonterminalParser* declarationNonterminalParser = GetNonterminal("Declaration");
        declarationNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DeclarationsRule>(this, &DeclarationsRule::PreDeclaration));
        declarationNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DeclarationsRule>(this, &DeclarationsRule::PostDeclaration));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.project->AddDeclaration(context.fromDeclaration);
    }
    void PreDeclaration(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Project*>(context.project)));
    }
    void PostDeclaration(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromDeclaration_value = std::move(stack.top());
            context.fromDeclaration = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ProjectDeclaration*>*>(fromDeclaration_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): project(), fromDeclaration() {}
        Cm::Ast::Project* project;
        Cm::Ast::ProjectDeclaration* fromDeclaration;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ProjectGrammar::DeclarationRule : public Cm::Parsing::Rule
{
public:
    DeclarationRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::Project*", "project"));
        SetValueTypeName("Cm::Ast::ProjectDeclaration*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> project_value = std::move(stack.top());
        context.project = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Project*>*>(project_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ProjectDeclaration*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DeclarationRule>(this, &DeclarationRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DeclarationRule>(this, &DeclarationRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DeclarationRule>(this, &DeclarationRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DeclarationRule>(this, &DeclarationRule::A3Action));
        Cm::Parsing::ActionParser* a4ActionParser = GetAction("A4");
        a4ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DeclarationRule>(this, &DeclarationRule::A4Action));
        Cm::Parsing::ActionParser* a5ActionParser = GetAction("A5");
        a5ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DeclarationRule>(this, &DeclarationRule::A5Action));
        Cm::Parsing::ActionParser* a6ActionParser = GetAction("A6");
        a6ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DeclarationRule>(this, &DeclarationRule::A6Action));
        Cm::Parsing::ActionParser* a7ActionParser = GetAction("A7");
        a7ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DeclarationRule>(this, &DeclarationRule::A7Action));
        Cm::Parsing::ActionParser* a8ActionParser = GetAction("A8");
        a8ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DeclarationRule>(this, &DeclarationRule::A8Action));
        Cm::Parsing::ActionParser* a9ActionParser = GetAction("A9");
        a9ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DeclarationRule>(this, &DeclarationRule::A9Action));
        Cm::Parsing::ActionParser* a10ActionParser = GetAction("A10");
        a10ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DeclarationRule>(this, &DeclarationRule::A10Action));
        Cm::Parsing::ActionParser* a11ActionParser = GetAction("A11");
        a11ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<DeclarationRule>(this, &DeclarationRule::A11Action));
        Cm::Parsing::NonterminalParser* sourceFileDeclarationNonterminalParser = GetNonterminal("SourceFileDeclaration");
        sourceFileDeclarationNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DeclarationRule>(this, &DeclarationRule::PreSourceFileDeclaration));
        sourceFileDeclarationNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DeclarationRule>(this, &DeclarationRule::PostSourceFileDeclaration));
        Cm::Parsing::NonterminalParser* asmSourceFileDeclarationNonterminalParser = GetNonterminal("AsmSourceFileDeclaration");
        asmSourceFileDeclarationNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DeclarationRule>(this, &DeclarationRule::PreAsmSourceFileDeclaration));
        asmSourceFileDeclarationNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DeclarationRule>(this, &DeclarationRule::PostAsmSourceFileDeclaration));
        Cm::Parsing::NonterminalParser* cSourceFileDeclarationNonterminalParser = GetNonterminal("CSourceFileDeclaration");
        cSourceFileDeclarationNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DeclarationRule>(this, &DeclarationRule::PreCSourceFileDeclaration));
        cSourceFileDeclarationNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DeclarationRule>(this, &DeclarationRule::PostCSourceFileDeclaration));
        Cm::Parsing::NonterminalParser* cppSourceFileDeclarationNonterminalParser = GetNonterminal("CppSourceFileDeclaration");
        cppSourceFileDeclarationNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DeclarationRule>(this, &DeclarationRule::PreCppSourceFileDeclaration));
        cppSourceFileDeclarationNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DeclarationRule>(this, &DeclarationRule::PostCppSourceFileDeclaration));
        Cm::Parsing::NonterminalParser* textFileDeclarationNonterminalParser = GetNonterminal("TextFileDeclaration");
        textFileDeclarationNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DeclarationRule>(this, &DeclarationRule::PreTextFileDeclaration));
        textFileDeclarationNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DeclarationRule>(this, &DeclarationRule::PostTextFileDeclaration));
        Cm::Parsing::NonterminalParser* referenceFileDeclarationNonterminalParser = GetNonterminal("ReferenceFileDeclaration");
        referenceFileDeclarationNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DeclarationRule>(this, &DeclarationRule::PreReferenceFileDeclaration));
        referenceFileDeclarationNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DeclarationRule>(this, &DeclarationRule::PostReferenceFileDeclaration));
        Cm::Parsing::NonterminalParser* cLibraryDeclarationNonterminalParser = GetNonterminal("CLibraryDeclaration");
        cLibraryDeclarationNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DeclarationRule>(this, &DeclarationRule::PostCLibraryDeclaration));
        Cm::Parsing::NonterminalParser* targetDeclarationNonterminalParser = GetNonterminal("TargetDeclaration");
        targetDeclarationNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DeclarationRule>(this, &DeclarationRule::PostTargetDeclaration));
        Cm::Parsing::NonterminalParser* stackSizeDeclarationNonterminalParser = GetNonterminal("StackSizeDeclaration");
        stackSizeDeclarationNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DeclarationRule>(this, &DeclarationRule::PostStackSizeDeclaration));
        Cm::Parsing::NonterminalParser* assemblyFileDeclarationNonterminalParser = GetNonterminal("AssemblyFileDeclaration");
        assemblyFileDeclarationNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DeclarationRule>(this, &DeclarationRule::PreAssemblyFileDeclaration));
        assemblyFileDeclarationNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DeclarationRule>(this, &DeclarationRule::PostAssemblyFileDeclaration));
        Cm::Parsing::NonterminalParser* executableFileDeclarationNonterminalParser = GetNonterminal("ExecutableFileDeclaration");
        executableFileDeclarationNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DeclarationRule>(this, &DeclarationRule::PreExecutableFileDeclaration));
        executableFileDeclarationNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DeclarationRule>(this, &DeclarationRule::PostExecutableFileDeclaration));
        Cm::Parsing::NonterminalParser* addLibraryPathDeclarationNonterminalParser = GetNonterminal("AddLibraryPathDeclaration");
        addLibraryPathDeclarationNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DeclarationRule>(this, &DeclarationRule::PreAddLibraryPathDeclaration));
        addLibraryPathDeclarationNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DeclarationRule>(this, &DeclarationRule::PostAddLibraryPathDeclaration));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromSourceFileDeclaration;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromAsmSourceFileDeclaration;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromCSourceFileDeclaration;
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromCppSourceFileDeclaration;
    }
    void A4Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromTextFileDeclaration;
    }
    void A5Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromReferenceFileDeclaration;
    }
    void A6Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromCLibraryDeclaration;
    }
    void A7Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromTargetDeclaration;
    }
    void A8Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromStackSizeDeclaration;
    }
    void A9Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromAssemblyFileDeclaration;
    }
    void A10Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromExecutableFileDeclaration;
    }
    void A11Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromAddLibraryPathDeclaration;
    }
    void PreSourceFileDeclaration(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Project*>(context.project)));
    }
    void PostSourceFileDeclaration(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromSourceFileDeclaration_value = std::move(stack.top());
            context.fromSourceFileDeclaration = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ProjectDeclaration*>*>(fromSourceFileDeclaration_value.get());
            stack.pop();
        }
    }
    void PreAsmSourceFileDeclaration(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Project*>(context.project)));
    }
    void PostAsmSourceFileDeclaration(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromAsmSourceFileDeclaration_value = std::move(stack.top());
            context.fromAsmSourceFileDeclaration = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ProjectDeclaration*>*>(fromAsmSourceFileDeclaration_value.get());
            stack.pop();
        }
    }
    void PreCSourceFileDeclaration(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Project*>(context.project)));
    }
    void PostCSourceFileDeclaration(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromCSourceFileDeclaration_value = std::move(stack.top());
            context.fromCSourceFileDeclaration = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ProjectDeclaration*>*>(fromCSourceFileDeclaration_value.get());
            stack.pop();
        }
    }
    void PreCppSourceFileDeclaration(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Project*>(context.project)));
    }
    void PostCppSourceFileDeclaration(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromCppSourceFileDeclaration_value = std::move(stack.top());
            context.fromCppSourceFileDeclaration = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ProjectDeclaration*>*>(fromCppSourceFileDeclaration_value.get());
            stack.pop();
        }
    }
    void PreTextFileDeclaration(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Project*>(context.project)));
    }
    void PostTextFileDeclaration(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTextFileDeclaration_value = std::move(stack.top());
            context.fromTextFileDeclaration = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ProjectDeclaration*>*>(fromTextFileDeclaration_value.get());
            stack.pop();
        }
    }
    void PreReferenceFileDeclaration(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Project*>(context.project)));
    }
    void PostReferenceFileDeclaration(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromReferenceFileDeclaration_value = std::move(stack.top());
            context.fromReferenceFileDeclaration = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ProjectDeclaration*>*>(fromReferenceFileDeclaration_value.get());
            stack.pop();
        }
    }
    void PostCLibraryDeclaration(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromCLibraryDeclaration_value = std::move(stack.top());
            context.fromCLibraryDeclaration = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ProjectDeclaration*>*>(fromCLibraryDeclaration_value.get());
            stack.pop();
        }
    }
    void PostTargetDeclaration(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromTargetDeclaration_value = std::move(stack.top());
            context.fromTargetDeclaration = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ProjectDeclaration*>*>(fromTargetDeclaration_value.get());
            stack.pop();
        }
    }
    void PostStackSizeDeclaration(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromStackSizeDeclaration_value = std::move(stack.top());
            context.fromStackSizeDeclaration = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ProjectDeclaration*>*>(fromStackSizeDeclaration_value.get());
            stack.pop();
        }
    }
    void PreAssemblyFileDeclaration(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Project*>(context.project)));
    }
    void PostAssemblyFileDeclaration(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromAssemblyFileDeclaration_value = std::move(stack.top());
            context.fromAssemblyFileDeclaration = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ProjectDeclaration*>*>(fromAssemblyFileDeclaration_value.get());
            stack.pop();
        }
    }
    void PreExecutableFileDeclaration(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Project*>(context.project)));
    }
    void PostExecutableFileDeclaration(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromExecutableFileDeclaration_value = std::move(stack.top());
            context.fromExecutableFileDeclaration = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ProjectDeclaration*>*>(fromExecutableFileDeclaration_value.get());
            stack.pop();
        }
    }
    void PreAddLibraryPathDeclaration(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Project*>(context.project)));
    }
    void PostAddLibraryPathDeclaration(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromAddLibraryPathDeclaration_value = std::move(stack.top());
            context.fromAddLibraryPathDeclaration = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ProjectDeclaration*>*>(fromAddLibraryPathDeclaration_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): project(), value(), fromSourceFileDeclaration(), fromAsmSourceFileDeclaration(), fromCSourceFileDeclaration(), fromCppSourceFileDeclaration(), fromTextFileDeclaration(), fromReferenceFileDeclaration(), fromCLibraryDeclaration(), fromTargetDeclaration(), fromStackSizeDeclaration(), fromAssemblyFileDeclaration(), fromExecutableFileDeclaration(), fromAddLibraryPathDeclaration() {}
        Cm::Ast::Project* project;
        Cm::Ast::ProjectDeclaration* value;
        Cm::Ast::ProjectDeclaration* fromSourceFileDeclaration;
        Cm::Ast::ProjectDeclaration* fromAsmSourceFileDeclaration;
        Cm::Ast::ProjectDeclaration* fromCSourceFileDeclaration;
        Cm::Ast::ProjectDeclaration* fromCppSourceFileDeclaration;
        Cm::Ast::ProjectDeclaration* fromTextFileDeclaration;
        Cm::Ast::ProjectDeclaration* fromReferenceFileDeclaration;
        Cm::Ast::ProjectDeclaration* fromCLibraryDeclaration;
        Cm::Ast::ProjectDeclaration* fromTargetDeclaration;
        Cm::Ast::ProjectDeclaration* fromStackSizeDeclaration;
        Cm::Ast::ProjectDeclaration* fromAssemblyFileDeclaration;
        Cm::Ast::ProjectDeclaration* fromExecutableFileDeclaration;
        Cm::Ast::ProjectDeclaration* fromAddLibraryPathDeclaration;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ProjectGrammar::SourceFileDeclarationRule : public Cm::Parsing::Rule
{
public:
    SourceFileDeclarationRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::Project*", "project"));
        SetValueTypeName("Cm::Ast::ProjectDeclaration*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> project_value = std::move(stack.top());
        context.project = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Project*>*>(project_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ProjectDeclaration*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<SourceFileDeclarationRule>(this, &SourceFileDeclarationRule::A0Action));
        Cm::Parsing::NonterminalParser* filePathNonterminalParser = GetNonterminal("FilePath");
        filePathNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<SourceFileDeclarationRule>(this, &SourceFileDeclarationRule::PostFilePath));
        Cm::Parsing::NonterminalParser* propertiesNonterminalParser = GetNonterminal("Properties");
        propertiesNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<SourceFileDeclarationRule>(this, &SourceFileDeclarationRule::PostProperties));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new SourceFileDeclaration(span, SourceFileType::cm, context.fromFilePath, context.project->BasePath(), context.fromProperties);
    }
    void PostFilePath(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromFilePath_value = std::move(stack.top());
            context.fromFilePath = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromFilePath_value.get());
            stack.pop();
        }
    }
    void PostProperties(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromProperties_value = std::move(stack.top());
            context.fromProperties = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Properties>*>(fromProperties_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): project(), value(), fromFilePath(), fromProperties() {}
        Cm::Ast::Project* project;
        Cm::Ast::ProjectDeclaration* value;
        std::string fromFilePath;
        Cm::Ast::Properties fromProperties;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ProjectGrammar::AsmSourceFileDeclarationRule : public Cm::Parsing::Rule
{
public:
    AsmSourceFileDeclarationRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::Project*", "project"));
        SetValueTypeName("Cm::Ast::ProjectDeclaration*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> project_value = std::move(stack.top());
        context.project = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Project*>*>(project_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ProjectDeclaration*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AsmSourceFileDeclarationRule>(this, &AsmSourceFileDeclarationRule::A0Action));
        Cm::Parsing::NonterminalParser* filePathNonterminalParser = GetNonterminal("FilePath");
        filePathNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<AsmSourceFileDeclarationRule>(this, &AsmSourceFileDeclarationRule::PostFilePath));
        Cm::Parsing::NonterminalParser* propertiesNonterminalParser = GetNonterminal("Properties");
        propertiesNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<AsmSourceFileDeclarationRule>(this, &AsmSourceFileDeclarationRule::PostProperties));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new SourceFileDeclaration(span, SourceFileType::asm_, context.fromFilePath, context.project->BasePath(), context.fromProperties);
    }
    void PostFilePath(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromFilePath_value = std::move(stack.top());
            context.fromFilePath = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromFilePath_value.get());
            stack.pop();
        }
    }
    void PostProperties(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromProperties_value = std::move(stack.top());
            context.fromProperties = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Properties>*>(fromProperties_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): project(), value(), fromFilePath(), fromProperties() {}
        Cm::Ast::Project* project;
        Cm::Ast::ProjectDeclaration* value;
        std::string fromFilePath;
        Cm::Ast::Properties fromProperties;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ProjectGrammar::CSourceFileDeclarationRule : public Cm::Parsing::Rule
{
public:
    CSourceFileDeclarationRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::Project*", "project"));
        SetValueTypeName("Cm::Ast::ProjectDeclaration*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> project_value = std::move(stack.top());
        context.project = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Project*>*>(project_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ProjectDeclaration*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CSourceFileDeclarationRule>(this, &CSourceFileDeclarationRule::A0Action));
        Cm::Parsing::NonterminalParser* filePathNonterminalParser = GetNonterminal("FilePath");
        filePathNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<CSourceFileDeclarationRule>(this, &CSourceFileDeclarationRule::PostFilePath));
        Cm::Parsing::NonterminalParser* propertiesNonterminalParser = GetNonterminal("Properties");
        propertiesNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<CSourceFileDeclarationRule>(this, &CSourceFileDeclarationRule::PostProperties));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new SourceFileDeclaration(span, SourceFileType::c, context.fromFilePath, context.project->BasePath(), context.fromProperties);
    }
    void PostFilePath(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromFilePath_value = std::move(stack.top());
            context.fromFilePath = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromFilePath_value.get());
            stack.pop();
        }
    }
    void PostProperties(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromProperties_value = std::move(stack.top());
            context.fromProperties = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Properties>*>(fromProperties_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): project(), value(), fromFilePath(), fromProperties() {}
        Cm::Ast::Project* project;
        Cm::Ast::ProjectDeclaration* value;
        std::string fromFilePath;
        Cm::Ast::Properties fromProperties;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ProjectGrammar::CppSourceFileDeclarationRule : public Cm::Parsing::Rule
{
public:
    CppSourceFileDeclarationRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::Project*", "project"));
        SetValueTypeName("Cm::Ast::ProjectDeclaration*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> project_value = std::move(stack.top());
        context.project = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Project*>*>(project_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ProjectDeclaration*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CppSourceFileDeclarationRule>(this, &CppSourceFileDeclarationRule::A0Action));
        Cm::Parsing::NonterminalParser* filePathNonterminalParser = GetNonterminal("FilePath");
        filePathNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<CppSourceFileDeclarationRule>(this, &CppSourceFileDeclarationRule::PostFilePath));
        Cm::Parsing::NonterminalParser* propertiesNonterminalParser = GetNonterminal("Properties");
        propertiesNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<CppSourceFileDeclarationRule>(this, &CppSourceFileDeclarationRule::PostProperties));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new SourceFileDeclaration(span, SourceFileType::cpp, context.fromFilePath, context.project->BasePath(), context.fromProperties);
    }
    void PostFilePath(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromFilePath_value = std::move(stack.top());
            context.fromFilePath = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromFilePath_value.get());
            stack.pop();
        }
    }
    void PostProperties(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromProperties_value = std::move(stack.top());
            context.fromProperties = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Properties>*>(fromProperties_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): project(), value(), fromFilePath(), fromProperties() {}
        Cm::Ast::Project* project;
        Cm::Ast::ProjectDeclaration* value;
        std::string fromFilePath;
        Cm::Ast::Properties fromProperties;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ProjectGrammar::TextFileDeclarationRule : public Cm::Parsing::Rule
{
public:
    TextFileDeclarationRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::Project*", "project"));
        SetValueTypeName("Cm::Ast::ProjectDeclaration*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> project_value = std::move(stack.top());
        context.project = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Project*>*>(project_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ProjectDeclaration*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TextFileDeclarationRule>(this, &TextFileDeclarationRule::A0Action));
        Cm::Parsing::NonterminalParser* filePathNonterminalParser = GetNonterminal("FilePath");
        filePathNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TextFileDeclarationRule>(this, &TextFileDeclarationRule::PostFilePath));
        Cm::Parsing::NonterminalParser* propertiesNonterminalParser = GetNonterminal("Properties");
        propertiesNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<TextFileDeclarationRule>(this, &TextFileDeclarationRule::PostProperties));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new SourceFileDeclaration(span, SourceFileType::text, context.fromFilePath, context.project->BasePath(), context.fromProperties);
    }
    void PostFilePath(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromFilePath_value = std::move(stack.top());
            context.fromFilePath = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromFilePath_value.get());
            stack.pop();
        }
    }
    void PostProperties(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromProperties_value = std::move(stack.top());
            context.fromProperties = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Properties>*>(fromProperties_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): project(), value(), fromFilePath(), fromProperties() {}
        Cm::Ast::Project* project;
        Cm::Ast::ProjectDeclaration* value;
        std::string fromFilePath;
        Cm::Ast::Properties fromProperties;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ProjectGrammar::ReferenceFileDeclarationRule : public Cm::Parsing::Rule
{
public:
    ReferenceFileDeclarationRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::Project*", "project"));
        SetValueTypeName("Cm::Ast::ProjectDeclaration*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> project_value = std::move(stack.top());
        context.project = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Project*>*>(project_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ProjectDeclaration*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ReferenceFileDeclarationRule>(this, &ReferenceFileDeclarationRule::A0Action));
        Cm::Parsing::NonterminalParser* filePathNonterminalParser = GetNonterminal("FilePath");
        filePathNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ReferenceFileDeclarationRule>(this, &ReferenceFileDeclarationRule::PostFilePath));
        Cm::Parsing::NonterminalParser* propertiesNonterminalParser = GetNonterminal("Properties");
        propertiesNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ReferenceFileDeclarationRule>(this, &ReferenceFileDeclarationRule::PostProperties));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new ReferenceFileDeclaration(span, context.fromFilePath, context.project->BasePath(), context.fromProperties);
    }
    void PostFilePath(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromFilePath_value = std::move(stack.top());
            context.fromFilePath = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromFilePath_value.get());
            stack.pop();
        }
    }
    void PostProperties(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromProperties_value = std::move(stack.top());
            context.fromProperties = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Properties>*>(fromProperties_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): project(), value(), fromFilePath(), fromProperties() {}
        Cm::Ast::Project* project;
        Cm::Ast::ProjectDeclaration* value;
        std::string fromFilePath;
        Cm::Ast::Properties fromProperties;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ProjectGrammar::AssemblyFileDeclarationRule : public Cm::Parsing::Rule
{
public:
    AssemblyFileDeclarationRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::Project*", "project"));
        SetValueTypeName("Cm::Ast::ProjectDeclaration*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> project_value = std::move(stack.top());
        context.project = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Project*>*>(project_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ProjectDeclaration*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AssemblyFileDeclarationRule>(this, &AssemblyFileDeclarationRule::A0Action));
        Cm::Parsing::NonterminalParser* filePathNonterminalParser = GetNonterminal("FilePath");
        filePathNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<AssemblyFileDeclarationRule>(this, &AssemblyFileDeclarationRule::PostFilePath));
        Cm::Parsing::NonterminalParser* propertiesNonterminalParser = GetNonterminal("Properties");
        propertiesNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<AssemblyFileDeclarationRule>(this, &AssemblyFileDeclarationRule::PostProperties));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new AssemblyFileDeclaration(span, context.fromFilePath, context.project->OutputBasePath(), context.fromProperties);
    }
    void PostFilePath(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromFilePath_value = std::move(stack.top());
            context.fromFilePath = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromFilePath_value.get());
            stack.pop();
        }
    }
    void PostProperties(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromProperties_value = std::move(stack.top());
            context.fromProperties = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Properties>*>(fromProperties_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): project(), value(), fromFilePath(), fromProperties() {}
        Cm::Ast::Project* project;
        Cm::Ast::ProjectDeclaration* value;
        std::string fromFilePath;
        Cm::Ast::Properties fromProperties;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ProjectGrammar::ExecutableFileDeclarationRule : public Cm::Parsing::Rule
{
public:
    ExecutableFileDeclarationRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::Project*", "project"));
        SetValueTypeName("Cm::Ast::ProjectDeclaration*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> project_value = std::move(stack.top());
        context.project = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Project*>*>(project_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ProjectDeclaration*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ExecutableFileDeclarationRule>(this, &ExecutableFileDeclarationRule::A0Action));
        Cm::Parsing::NonterminalParser* filePathNonterminalParser = GetNonterminal("FilePath");
        filePathNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ExecutableFileDeclarationRule>(this, &ExecutableFileDeclarationRule::PostFilePath));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new ExecutableFileDeclaration(span, context.fromFilePath, context.project->OutputBasePath());
    }
    void PostFilePath(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromFilePath_value = std::move(stack.top());
            context.fromFilePath = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromFilePath_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): project(), value(), fromFilePath() {}
        Cm::Ast::Project* project;
        Cm::Ast::ProjectDeclaration* value;
        std::string fromFilePath;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ProjectGrammar::AddLibraryPathDeclarationRule : public Cm::Parsing::Rule
{
public:
    AddLibraryPathDeclarationRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::Project*", "project"));
        SetValueTypeName("Cm::Ast::ProjectDeclaration*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> project_value = std::move(stack.top());
        context.project = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Project*>*>(project_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ProjectDeclaration*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<AddLibraryPathDeclarationRule>(this, &AddLibraryPathDeclarationRule::A0Action));
        Cm::Parsing::NonterminalParser* filePathNonterminalParser = GetNonterminal("FilePath");
        filePathNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<AddLibraryPathDeclarationRule>(this, &AddLibraryPathDeclarationRule::PostFilePath));
        Cm::Parsing::NonterminalParser* propertiesNonterminalParser = GetNonterminal("Properties");
        propertiesNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<AddLibraryPathDeclarationRule>(this, &AddLibraryPathDeclarationRule::PostProperties));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new AddLibraryPathDeclaration(span, context.fromFilePath, context.project->BasePath(), context.fromProperties);
    }
    void PostFilePath(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromFilePath_value = std::move(stack.top());
            context.fromFilePath = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromFilePath_value.get());
            stack.pop();
        }
    }
    void PostProperties(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromProperties_value = std::move(stack.top());
            context.fromProperties = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Properties>*>(fromProperties_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): project(), value(), fromFilePath(), fromProperties() {}
        Cm::Ast::Project* project;
        Cm::Ast::ProjectDeclaration* value;
        std::string fromFilePath;
        Cm::Ast::Properties fromProperties;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ProjectGrammar::CLibraryDeclarationRule : public Cm::Parsing::Rule
{
public:
    CLibraryDeclarationRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Ast::ProjectDeclaration*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ProjectDeclaration*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<CLibraryDeclarationRule>(this, &CLibraryDeclarationRule::A0Action));
        Cm::Parsing::NonterminalParser* filePathNonterminalParser = GetNonterminal("FilePath");
        filePathNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<CLibraryDeclarationRule>(this, &CLibraryDeclarationRule::PostFilePath));
        Cm::Parsing::NonterminalParser* propertiesNonterminalParser = GetNonterminal("Properties");
        propertiesNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<CLibraryDeclarationRule>(this, &CLibraryDeclarationRule::PostProperties));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new CLibraryDeclaration(span, context.fromFilePath, context.fromProperties);
    }
    void PostFilePath(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromFilePath_value = std::move(stack.top());
            context.fromFilePath = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromFilePath_value.get());
            stack.pop();
        }
    }
    void PostProperties(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromProperties_value = std::move(stack.top());
            context.fromProperties = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Properties>*>(fromProperties_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromFilePath(), fromProperties() {}
        Cm::Ast::ProjectDeclaration* value;
        std::string fromFilePath;
        Cm::Ast::Properties fromProperties;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ProjectGrammar::TargetDeclarationRule : public Cm::Parsing::Rule
{
public:
    TargetDeclarationRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Ast::ProjectDeclaration*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ProjectDeclaration*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TargetDeclarationRule>(this, &TargetDeclarationRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<TargetDeclarationRule>(this, &TargetDeclarationRule::A1Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new TargetDeclaration(span, Target::program);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new TargetDeclaration(span, Target::library);
    }
private:
    struct Context
    {
        Context(): value() {}
        Cm::Ast::ProjectDeclaration* value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ProjectGrammar::StackSizeDeclarationRule : public Cm::Parsing::Rule
{
public:
    StackSizeDeclarationRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Ast::ProjectDeclaration*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ProjectDeclaration*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<StackSizeDeclarationRule>(this, &StackSizeDeclarationRule::A0Action));
        Cm::Parsing::NonterminalParser* reserveNonterminalParser = GetNonterminal("reserve");
        reserveNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StackSizeDeclarationRule>(this, &StackSizeDeclarationRule::Postreserve));
        Cm::Parsing::NonterminalParser* commitNonterminalParser = GetNonterminal("commit");
        commitNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<StackSizeDeclarationRule>(this, &StackSizeDeclarationRule::Postcommit));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new StackSizeDeclaration(span, context.fromreserve, context.fromcommit);
    }
    void Postreserve(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromreserve_value = std::move(stack.top());
            context.fromreserve = *static_cast<Cm::Parsing::ValueObject<uint64_t>*>(fromreserve_value.get());
            stack.pop();
        }
    }
    void Postcommit(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromcommit_value = std::move(stack.top());
            context.fromcommit = *static_cast<Cm::Parsing::ValueObject<uint64_t>*>(fromcommit_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromreserve(), fromcommit() {}
        Cm::Ast::ProjectDeclaration* value;
        uint64_t fromreserve;
        uint64_t fromcommit;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ProjectGrammar::PropertiesRule : public Cm::Parsing::Rule
{
public:
    PropertiesRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Ast::Properties");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Properties>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PropertiesRule>(this, &PropertiesRule::A0Action));
        Cm::Parsing::NonterminalParser* nameNonterminalParser = GetNonterminal("name");
        nameNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PropertiesRule>(this, &PropertiesRule::Postname));
        Cm::Parsing::NonterminalParser* relNonterminalParser = GetNonterminal("rel");
        relNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PropertiesRule>(this, &PropertiesRule::Postrel));
        Cm::Parsing::NonterminalParser* valNonterminalParser = GetNonterminal("val");
        valNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PropertiesRule>(this, &PropertiesRule::Postval));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value.AddProperty(context.fromname, context.fromrel, context.fromval);
    }
    void Postname(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromname_value = std::move(stack.top());
            context.fromname = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromname_value.get());
            stack.pop();
        }
    }
    void Postrel(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromrel_value = std::move(stack.top());
            context.fromrel = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::RelOp>*>(fromrel_value.get());
            stack.pop();
        }
    }
    void Postval(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromval_value = std::move(stack.top());
            context.fromval = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromval_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromname(), fromrel(), fromval() {}
        Cm::Ast::Properties value;
        std::string fromname;
        Cm::Ast::RelOp fromrel;
        std::string fromval;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ProjectGrammar::RelOpRule : public Cm::Parsing::Rule
{
public:
    RelOpRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Ast::RelOp");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::RelOp>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<RelOpRule>(this, &RelOpRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<RelOpRule>(this, &RelOpRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<RelOpRule>(this, &RelOpRule::A2Action));
        Cm::Parsing::ActionParser* a3ActionParser = GetAction("A3");
        a3ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<RelOpRule>(this, &RelOpRule::A3Action));
        Cm::Parsing::ActionParser* a4ActionParser = GetAction("A4");
        a4ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<RelOpRule>(this, &RelOpRule::A4Action));
        Cm::Parsing::ActionParser* a5ActionParser = GetAction("A5");
        a5ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<RelOpRule>(this, &RelOpRule::A5Action));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = Cm::Ast::RelOp::equal;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = Cm::Ast::RelOp::notEq;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = Cm::Ast::RelOp::lessEq;
    }
    void A3Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = Cm::Ast::RelOp::greaterEq;
    }
    void A4Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = Cm::Ast::RelOp::less;
    }
    void A5Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = Cm::Ast::RelOp::greater;
    }
private:
    struct Context
    {
        Context(): value() {}
        Cm::Ast::RelOp value;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ProjectGrammar::PropertyValueRule : public Cm::Parsing::Rule
{
public:
    PropertyValueRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PropertyValueRule>(this, &PropertyValueRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PropertyValueRule>(this, &PropertyValueRule::A1Action));
        Cm::Parsing::ActionParser* a2ActionParser = GetAction("A2");
        a2ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<PropertyValueRule>(this, &PropertyValueRule::A2Action));
        Cm::Parsing::NonterminalParser* versionNumberNonterminalParser = GetNonterminal("VersionNumber");
        versionNumberNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PropertyValueRule>(this, &PropertyValueRule::PostVersionNumber));
        Cm::Parsing::NonterminalParser* idNonterminalParser = GetNonterminal("id");
        idNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PropertyValueRule>(this, &PropertyValueRule::Postid));
        Cm::Parsing::NonterminalParser* numNonterminalParser = GetNonterminal("num");
        numNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<PropertyValueRule>(this, &PropertyValueRule::Postnum));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = std::string(matchBegin, matchEnd);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromid;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = std::to_string(context.fromnum);
    }
    void PostVersionNumber(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromVersionNumber_value = std::move(stack.top());
            context.fromVersionNumber = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ProgramVersion>*>(fromVersionNumber_value.get());
            stack.pop();
        }
    }
    void Postid(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromid_value = std::move(stack.top());
            context.fromid = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromid_value.get());
            stack.pop();
        }
    }
    void Postnum(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromnum_value = std::move(stack.top());
            context.fromnum = *static_cast<Cm::Parsing::ValueObject<int64_t>*>(fromnum_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromVersionNumber(), fromid(), fromnum() {}
        std::string value;
        Cm::Ast::ProgramVersion fromVersionNumber;
        std::string fromid;
        int64_t fromnum;
    };
    std::stack<Context> contextStack;
    Context context;
};

class ProjectGrammar::FilePathRule : public Cm::Parsing::Rule
{
public:
    FilePathRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
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
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<FilePathRule>(this, &FilePathRule::A0Action));
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

void ProjectGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parsing.stdlib");
    if (!grammar0)
    {
        grammar0 = Cm::Parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
    Cm::Parsing::Grammar* grammar1 = pd->GetGrammar("Cm.Parser.VersionNumberParser");
    if (!grammar1)
    {
        grammar1 = Cm::Parser::VersionNumberParser::Create(pd);
    }
    AddGrammarReference(grammar1);
}

void ProjectGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("qualified_id", this, "Cm.Parsing.stdlib.qualified_id"));
    AddRuleLink(new Cm::Parsing::RuleLink("identifier", this, "Cm.Parsing.stdlib.identifier"));
    AddRuleLink(new Cm::Parsing::RuleLink("spaces_and_comments", this, "Cm.Parsing.stdlib.spaces_and_comments"));
    AddRuleLink(new Cm::Parsing::RuleLink("ulong", this, "Cm.Parsing.stdlib.ulong"));
    AddRuleLink(new Cm::Parsing::RuleLink("long", this, "Cm.Parsing.stdlib.long"));
    AddRuleLink(new Cm::Parsing::RuleLink("VersionNumber", this, "VersionNumberParser.VersionNumber"));
    AddRule(new ProjectRule("Project", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::KeywordParser("project"),
                        new Cm::Parsing::NonterminalParser("qualified_id", "qualified_id", 0)),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::CharParser(';')))),
            new Cm::Parsing::NonterminalParser("Declarations", "Declarations", 1))));
    AddRule(new DeclarationsRule("Declarations", GetScope(),
        new Cm::Parsing::KleeneStarParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::NonterminalParser("Declaration", "Declaration", 1)))));
    AddRule(new DeclarationRule("Declaration", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::AlternativeParser(
                            new Cm::Parsing::AlternativeParser(
                                new Cm::Parsing::AlternativeParser(
                                    new Cm::Parsing::AlternativeParser(
                                        new Cm::Parsing::AlternativeParser(
                                            new Cm::Parsing::AlternativeParser(
                                                new Cm::Parsing::AlternativeParser(
                                                    new Cm::Parsing::ActionParser("A0",
                                                        new Cm::Parsing::NonterminalParser("SourceFileDeclaration", "SourceFileDeclaration", 1)),
                                                    new Cm::Parsing::ActionParser("A1",
                                                        new Cm::Parsing::NonterminalParser("AsmSourceFileDeclaration", "AsmSourceFileDeclaration", 1))),
                                                new Cm::Parsing::ActionParser("A2",
                                                    new Cm::Parsing::NonterminalParser("CSourceFileDeclaration", "CSourceFileDeclaration", 1))),
                                            new Cm::Parsing::ActionParser("A3",
                                                new Cm::Parsing::NonterminalParser("CppSourceFileDeclaration", "CppSourceFileDeclaration", 1))),
                                        new Cm::Parsing::ActionParser("A4",
                                            new Cm::Parsing::NonterminalParser("TextFileDeclaration", "TextFileDeclaration", 1))),
                                    new Cm::Parsing::ActionParser("A5",
                                        new Cm::Parsing::NonterminalParser("ReferenceFileDeclaration", "ReferenceFileDeclaration", 1))),
                                new Cm::Parsing::ActionParser("A6",
                                    new Cm::Parsing::NonterminalParser("CLibraryDeclaration", "CLibraryDeclaration", 0))),
                            new Cm::Parsing::ActionParser("A7",
                                new Cm::Parsing::NonterminalParser("TargetDeclaration", "TargetDeclaration", 0))),
                        new Cm::Parsing::ActionParser("A8",
                            new Cm::Parsing::NonterminalParser("StackSizeDeclaration", "StackSizeDeclaration", 0))),
                    new Cm::Parsing::ActionParser("A9",
                        new Cm::Parsing::NonterminalParser("AssemblyFileDeclaration", "AssemblyFileDeclaration", 1))),
                new Cm::Parsing::ActionParser("A10",
                    new Cm::Parsing::NonterminalParser("ExecutableFileDeclaration", "ExecutableFileDeclaration", 1))),
            new Cm::Parsing::ActionParser("A11",
                new Cm::Parsing::NonterminalParser("AddLibraryPathDeclaration", "AddLibraryPathDeclaration", 1)))));
    AddRule(new SourceFileDeclarationRule("SourceFileDeclaration", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::KeywordParser("source"),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("FilePath", "FilePath", 0))),
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::NonterminalParser("Properties", "Properties", 0))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new AsmSourceFileDeclarationRule("AsmSourceFileDeclaration", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::KeywordParser("asmsource"),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("FilePath", "FilePath", 0))),
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::NonterminalParser("Properties", "Properties", 0))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new CSourceFileDeclarationRule("CSourceFileDeclaration", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::KeywordParser("csource"),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("FilePath", "FilePath", 0))),
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::NonterminalParser("Properties", "Properties", 0))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new CppSourceFileDeclarationRule("CppSourceFileDeclaration", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::KeywordParser("cppsource"),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("FilePath", "FilePath", 0))),
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::NonterminalParser("Properties", "Properties", 0))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new TextFileDeclarationRule("TextFileDeclaration", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::KeywordParser("text"),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("FilePath", "FilePath", 0))),
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::NonterminalParser("Properties", "Properties", 0))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new ReferenceFileDeclarationRule("ReferenceFileDeclaration", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::KeywordParser("reference"),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("FilePath", "FilePath", 0))),
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::NonterminalParser("Properties", "Properties", 0))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new AssemblyFileDeclarationRule("AssemblyFileDeclaration", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::KeywordParser("assembly"),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("FilePath", "FilePath", 0))),
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::NonterminalParser("Properties", "Properties", 0))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new ExecutableFileDeclarationRule("ExecutableFileDeclaration", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::KeywordParser("executable"),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::NonterminalParser("FilePath", "FilePath", 0))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new AddLibraryPathDeclarationRule("AddLibraryPathDeclaration", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::KeywordParser("addlibrarypath"),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("FilePath", "FilePath", 0))),
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::NonterminalParser("Properties", "Properties", 0))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new CLibraryDeclarationRule("CLibraryDeclaration", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::KeywordParser("clib"),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("FilePath", "FilePath", 0))),
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::NonterminalParser("Properties", "Properties", 0))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new TargetDeclarationRule("TargetDeclaration", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::KeywordParser("target"),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::CharParser('='))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::ActionParser("A0",
                            new Cm::Parsing::KeywordParser("program")),
                        new Cm::Parsing::ActionParser("A1",
                            new Cm::Parsing::KeywordParser("library"))))),
            new Cm::Parsing::ExpectationParser(
                new Cm::Parsing::CharParser(';')))));
    AddRule(new StackSizeDeclarationRule("StackSizeDeclaration", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::KeywordParser("stack"),
                            new Cm::Parsing::ExpectationParser(
                                new Cm::Parsing::CharParser('='))),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("reserve", "ulong", 0))),
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::CharParser(','),
                            new Cm::Parsing::NonterminalParser("commit", "ulong", 0)))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new PropertiesRule("Properties", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::CharParser('['),
                new Cm::Parsing::ListParser(
                    new Cm::Parsing::ActionParser("A0",
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::NonterminalParser("name", "identifier", 0),
                                new Cm::Parsing::NonterminalParser("rel", "RelOp", 0)),
                            new Cm::Parsing::NonterminalParser("val", "PropertyValue", 0))),
                    new Cm::Parsing::CharParser(','))),
            new Cm::Parsing::ExpectationParser(
                new Cm::Parsing::CharParser(']')))));
    AddRule(new RelOpRule("RelOp", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::AlternativeParser(
                    new Cm::Parsing::AlternativeParser(
                        new Cm::Parsing::AlternativeParser(
                            new Cm::Parsing::ActionParser("A0",
                                new Cm::Parsing::CharParser('=')),
                            new Cm::Parsing::ActionParser("A1",
                                new Cm::Parsing::StringParser("!="))),
                        new Cm::Parsing::ActionParser("A2",
                            new Cm::Parsing::StringParser("<="))),
                    new Cm::Parsing::ActionParser("A3",
                        new Cm::Parsing::StringParser(">="))),
                new Cm::Parsing::ActionParser("A4",
                    new Cm::Parsing::CharParser('<'))),
            new Cm::Parsing::ActionParser("A5",
                new Cm::Parsing::CharParser('>')))));
    AddRule(new PropertyValueRule("PropertyValue", GetScope(),
        new Cm::Parsing::AlternativeParser(
            new Cm::Parsing::AlternativeParser(
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::NonterminalParser("VersionNumber", "VersionNumber", 0)),
                new Cm::Parsing::ActionParser("A1",
                    new Cm::Parsing::NonterminalParser("id", "identifier", 0))),
            new Cm::Parsing::ActionParser("A2",
                new Cm::Parsing::NonterminalParser("num", "long", 0)))));
    AddRule(new FilePathRule("FilePath", GetScope(),
        new Cm::Parsing::TokenParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::CharParser('<'),
                    new Cm::Parsing::ActionParser("A0",
                        new Cm::Parsing::PositiveParser(
                            new Cm::Parsing::CharSetParser(">", true)))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser('>'))))));
    SetSkipRuleName("spaces_and_comments");
}

} } // namespace Cm.Parser

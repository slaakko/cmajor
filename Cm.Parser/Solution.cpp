#include "Solution.hpp"
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

using namespace Cm::Ast;
using namespace Cm::Parsing;

SolutionGrammar* SolutionGrammar::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

SolutionGrammar* SolutionGrammar::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    SolutionGrammar* grammar(new SolutionGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

SolutionGrammar::SolutionGrammar(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("SolutionGrammar", parsingDomain_->GetNamespaceScope("Cm.Parser"), parsingDomain_)
{
    SetOwner(0);
}

Cm::Ast::Solution* SolutionGrammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName)
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
    Cm::Ast::Solution* result = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Solution*>*>(value.get());
    stack.pop();
    return result;
}

class SolutionGrammar::SolutionRule : public Cm::Parsing::Rule
{
public:
    SolutionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Ast::Solution*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Solution*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<SolutionRule>(this, &SolutionRule::A0Action));
        Cm::Parsing::NonterminalParser* qualified_idNonterminalParser = GetNonterminal("qualified_id");
        qualified_idNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<SolutionRule>(this, &SolutionRule::Postqualified_id));
        Cm::Parsing::NonterminalParser* declarationsNonterminalParser = GetNonterminal("Declarations");
        declarationsNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<SolutionRule>(this, &SolutionRule::PreDeclarations));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new Solution(context.fromqualified_id, fileName);
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
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Solution*>(context.value)));
    }
private:
    struct Context
    {
        Context(): value(), fromqualified_id() {}
        Cm::Ast::Solution* value;
        std::string fromqualified_id;
    };
    std::stack<Context> contextStack;
    Context context;
};

class SolutionGrammar::DeclarationsRule : public Cm::Parsing::Rule
{
public:
    DeclarationsRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::Solution*", "solution"));
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> solution_value = std::move(stack.top());
        context.solution = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Solution*>*>(solution_value.get());
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
        context.solution->AddDeclaration(context.fromDeclaration);
    }
    void PreDeclaration(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Solution*>(context.solution)));
    }
    void PostDeclaration(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromDeclaration_value = std::move(stack.top());
            context.fromDeclaration = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::SolutionDeclaration*>*>(fromDeclaration_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): solution(), fromDeclaration() {}
        Cm::Ast::Solution* solution;
        Cm::Ast::SolutionDeclaration* fromDeclaration;
    };
    std::stack<Context> contextStack;
    Context context;
};

class SolutionGrammar::DeclarationRule : public Cm::Parsing::Rule
{
public:
    DeclarationRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::Solution*", "solution"));
        SetValueTypeName("Cm::Ast::SolutionDeclaration*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> solution_value = std::move(stack.top());
        context.solution = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Solution*>*>(solution_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::SolutionDeclaration*>(context.value)));
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
        Cm::Parsing::NonterminalParser* projectFileDeclarationNonterminalParser = GetNonterminal("ProjectFileDeclaration");
        projectFileDeclarationNonterminalParser->SetPreCall(new Cm::Parsing::MemberPreCall<DeclarationRule>(this, &DeclarationRule::PreProjectFileDeclaration));
        projectFileDeclarationNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DeclarationRule>(this, &DeclarationRule::PostProjectFileDeclaration));
        Cm::Parsing::NonterminalParser* activeProjectDeclarationNonterminalParser = GetNonterminal("ActiveProjectDeclaration");
        activeProjectDeclarationNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DeclarationRule>(this, &DeclarationRule::PostActiveProjectDeclaration));
        Cm::Parsing::NonterminalParser* projectDependencyDeclarationNonterminalParser = GetNonterminal("ProjectDependencyDeclaration");
        projectDependencyDeclarationNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<DeclarationRule>(this, &DeclarationRule::PostProjectDependencyDeclaration));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromProjectFileDeclaration;
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromActiveProjectDeclaration;
    }
    void A2Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromProjectDependencyDeclaration;
    }
    void PreProjectFileDeclaration(Cm::Parsing::ObjectStack& stack)
    {
        stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::Solution*>(context.solution)));
    }
    void PostProjectFileDeclaration(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromProjectFileDeclaration_value = std::move(stack.top());
            context.fromProjectFileDeclaration = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::SolutionDeclaration*>*>(fromProjectFileDeclaration_value.get());
            stack.pop();
        }
    }
    void PostActiveProjectDeclaration(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromActiveProjectDeclaration_value = std::move(stack.top());
            context.fromActiveProjectDeclaration = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::SolutionDeclaration*>*>(fromActiveProjectDeclaration_value.get());
            stack.pop();
        }
    }
    void PostProjectDependencyDeclaration(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromProjectDependencyDeclaration_value = std::move(stack.top());
            context.fromProjectDependencyDeclaration = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ProjectDependencyDeclaration*>*>(fromProjectDependencyDeclaration_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): solution(), value(), fromProjectFileDeclaration(), fromActiveProjectDeclaration(), fromProjectDependencyDeclaration() {}
        Cm::Ast::Solution* solution;
        Cm::Ast::SolutionDeclaration* value;
        Cm::Ast::SolutionDeclaration* fromProjectFileDeclaration;
        Cm::Ast::SolutionDeclaration* fromActiveProjectDeclaration;
        Cm::Ast::ProjectDependencyDeclaration* fromProjectDependencyDeclaration;
    };
    std::stack<Context> contextStack;
    Context context;
};

class SolutionGrammar::ProjectFileDeclarationRule : public Cm::Parsing::Rule
{
public:
    ProjectFileDeclarationRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        AddInheritedAttribute(AttrOrVariable("Cm::Ast::Solution*", "solution"));
        SetValueTypeName("Cm::Ast::SolutionDeclaration*");
    }
    virtual void Enter(Cm::Parsing::ObjectStack& stack)
    {
        contextStack.push(std::move(context));
        context = Context();
        std::unique_ptr<Cm::Parsing::Object> solution_value = std::move(stack.top());
        context.solution = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::Solution*>*>(solution_value.get());
        stack.pop();
    }
    virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::SolutionDeclaration*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ProjectFileDeclarationRule>(this, &ProjectFileDeclarationRule::A0Action));
        Cm::Parsing::NonterminalParser* filePathNonterminalParser = GetNonterminal("FilePath");
        filePathNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ProjectFileDeclarationRule>(this, &ProjectFileDeclarationRule::PostFilePath));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new ProjectFileDeclaration(span, context.fromFilePath, context.solution->BasePath());
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
        Context(): solution(), value(), fromFilePath() {}
        Cm::Ast::Solution* solution;
        Cm::Ast::SolutionDeclaration* value;
        std::string fromFilePath;
    };
    std::stack<Context> contextStack;
    Context context;
};

class SolutionGrammar::ActiveProjectDeclarationRule : public Cm::Parsing::Rule
{
public:
    ActiveProjectDeclarationRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Ast::SolutionDeclaration*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::SolutionDeclaration*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ActiveProjectDeclarationRule>(this, &ActiveProjectDeclarationRule::A0Action));
        Cm::Parsing::NonterminalParser* qualified_idNonterminalParser = GetNonterminal("qualified_id");
        qualified_idNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ActiveProjectDeclarationRule>(this, &ActiveProjectDeclarationRule::Postqualified_id));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new ActiveProjectDeclaration(context.fromqualified_id);
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
private:
    struct Context
    {
        Context(): value(), fromqualified_id() {}
        Cm::Ast::SolutionDeclaration* value;
        std::string fromqualified_id;
    };
    std::stack<Context> contextStack;
    Context context;
};

class SolutionGrammar::ProjectDependencyDeclarationRule : public Cm::Parsing::Rule
{
public:
    ProjectDependencyDeclarationRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Ast::ProjectDependencyDeclaration*");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ProjectDependencyDeclaration*>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ProjectDependencyDeclarationRule>(this, &ProjectDependencyDeclarationRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<ProjectDependencyDeclarationRule>(this, &ProjectDependencyDeclarationRule::A1Action));
        Cm::Parsing::NonterminalParser* projectNameNonterminalParser = GetNonterminal("projectName");
        projectNameNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ProjectDependencyDeclarationRule>(this, &ProjectDependencyDeclarationRule::PostprojectName));
        Cm::Parsing::NonterminalParser* dependsOnNonterminalParser = GetNonterminal("dependsOn");
        dependsOnNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<ProjectDependencyDeclarationRule>(this, &ProjectDependencyDeclarationRule::PostdependsOn));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = new ProjectDependencyDeclaration(context.fromprojectName);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value->AddDependency(context.fromdependsOn);
    }
    void PostprojectName(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromprojectName_value = std::move(stack.top());
            context.fromprojectName = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromprojectName_value.get());
            stack.pop();
        }
    }
    void PostdependsOn(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromdependsOn_value = std::move(stack.top());
            context.fromdependsOn = *static_cast<Cm::Parsing::ValueObject<std::string>*>(fromdependsOn_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromprojectName(), fromdependsOn() {}
        Cm::Ast::ProjectDependencyDeclaration* value;
        std::string fromprojectName;
        std::string fromdependsOn;
    };
    std::stack<Context> contextStack;
    Context context;
};

class SolutionGrammar::FilePathRule : public Cm::Parsing::Rule
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

void SolutionGrammar::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parsing.stdlib");
    if (!grammar0)
    {
        grammar0 = Cm::Parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
}

void SolutionGrammar::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("qualified_id", this, "Cm.Parsing.stdlib.qualified_id"));
    AddRuleLink(new Cm::Parsing::RuleLink("spaces_and_comments", this, "Cm.Parsing.stdlib.spaces_and_comments"));
    AddRule(new SolutionRule("Solution", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::ActionParser("A0",
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::KeywordParser("solution"),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::NonterminalParser("qualified_id", "qualified_id", 0))),
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
                new Cm::Parsing::ActionParser("A0",
                    new Cm::Parsing::NonterminalParser("ProjectFileDeclaration", "ProjectFileDeclaration", 1)),
                new Cm::Parsing::ActionParser("A1",
                    new Cm::Parsing::NonterminalParser("ActiveProjectDeclaration", "ActiveProjectDeclaration", 0))),
            new Cm::Parsing::ActionParser("A2",
                new Cm::Parsing::NonterminalParser("ProjectDependencyDeclaration", "ProjectDependencyDeclaration", 0)))));
    AddRule(new ProjectFileDeclarationRule("ProjectFileDeclaration", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::KeywordParser("project"),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::NonterminalParser("FilePath", "FilePath", 0))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new ActiveProjectDeclarationRule("ActiveProjectDeclaration", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::KeywordParser("activeProject"),
                    new Cm::Parsing::ExpectationParser(
                        new Cm::Parsing::NonterminalParser("qualified_id", "qualified_id", 0))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(';'))))));
    AddRule(new ProjectDependencyDeclarationRule("ProjectDependencyDeclaration", GetScope(),
        new Cm::Parsing::SequenceParser(
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::KeywordParser("dependency"),
                            new Cm::Parsing::ActionParser("A0",
                                new Cm::Parsing::ExpectationParser(
                                    new Cm::Parsing::NonterminalParser("projectName", "qualified_id", 0)))),
                        new Cm::Parsing::ExpectationParser(
                            new Cm::Parsing::CharParser('('))),
                    new Cm::Parsing::OptionalParser(
                        new Cm::Parsing::ListParser(
                            new Cm::Parsing::ActionParser("A1",
                                new Cm::Parsing::NonterminalParser("dependsOn", "qualified_id", 0)),
                            new Cm::Parsing::CharParser(',')))),
                new Cm::Parsing::ExpectationParser(
                    new Cm::Parsing::CharParser(')'))),
            new Cm::Parsing::ExpectationParser(
                new Cm::Parsing::CharParser(';')))));
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

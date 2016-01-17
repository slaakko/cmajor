#include "LlvmVersion.hpp"
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

LlvmVersionParser* LlvmVersionParser::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

LlvmVersionParser* LlvmVersionParser::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    LlvmVersionParser* grammar(new LlvmVersionParser(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

LlvmVersionParser::LlvmVersionParser(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("LlvmVersionParser", parsingDomain_->GetNamespaceScope("Cm.Parser"), parsingDomain_)
{
    SetOwner(0);
}

Cm::Ast::ProgramVersion LlvmVersionParser::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName)
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
    Cm::Ast::ProgramVersion result = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ProgramVersion>*>(value.get());
    stack.pop();
    return result;
}

class LlvmVersionParser::LlvmVersionRule : public Cm::Parsing::Rule
{
public:
    LlvmVersionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Ast::ProgramVersion");
        AddLocalVariable(AttrOrVariable("std::string", "versionText"));
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ProgramVersion>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<LlvmVersionRule>(this, &LlvmVersionRule::A0Action));
        Cm::Parsing::ActionParser* a1ActionParser = GetAction("A1");
        a1ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<LlvmVersionRule>(this, &LlvmVersionRule::A1Action));
        Cm::Parsing::NonterminalParser* majorNonterminalParser = GetNonterminal("Major");
        majorNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LlvmVersionRule>(this, &LlvmVersionRule::PostMajor));
        Cm::Parsing::NonterminalParser* minorNonterminalParser = GetNonterminal("Minor");
        minorNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LlvmVersionRule>(this, &LlvmVersionRule::PostMinor));
        Cm::Parsing::NonterminalParser* revisionNonterminalParser = GetNonterminal("Revision");
        revisionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LlvmVersionRule>(this, &LlvmVersionRule::PostRevision));
        Cm::Parsing::NonterminalParser* buildNonterminalParser = GetNonterminal("Build");
        buildNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<LlvmVersionRule>(this, &LlvmVersionRule::PostBuild));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = Cm::Ast::ProgramVersion(context.fromMajor, context.fromMinor, context.fromRevision, context.fromBuild, context.versionText);
    }
    void A1Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.versionText = std::string(matchBegin, matchEnd);
    }
    void PostMajor(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromMajor_value = std::move(stack.top());
            context.fromMajor = *static_cast<Cm::Parsing::ValueObject<int>*>(fromMajor_value.get());
            stack.pop();
        }
    }
    void PostMinor(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromMinor_value = std::move(stack.top());
            context.fromMinor = *static_cast<Cm::Parsing::ValueObject<int>*>(fromMinor_value.get());
            stack.pop();
        }
    }
    void PostRevision(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromRevision_value = std::move(stack.top());
            context.fromRevision = *static_cast<Cm::Parsing::ValueObject<int>*>(fromRevision_value.get());
            stack.pop();
        }
    }
    void PostBuild(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromBuild_value = std::move(stack.top());
            context.fromBuild = *static_cast<Cm::Parsing::ValueObject<int>*>(fromBuild_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), versionText(), fromMajor(), fromMinor(), fromRevision(), fromBuild() {}
        Cm::Ast::ProgramVersion value;
        std::string versionText;
        int fromMajor;
        int fromMinor;
        int fromRevision;
        int fromBuild;
    };
    std::stack<Context> contextStack;
    Context context;
};

void LlvmVersionParser::GetReferencedGrammars()
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

void LlvmVersionParser::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("newline", this, "Cm.Parsing.stdlib.newline"));
    AddRuleLink(new Cm::Parsing::RuleLink("Major", this, "VersionNumberParser.Major"));
    AddRuleLink(new Cm::Parsing::RuleLink("Minor", this, "VersionNumberParser.Minor"));
    AddRuleLink(new Cm::Parsing::RuleLink("Revision", this, "VersionNumberParser.Revision"));
    AddRuleLink(new Cm::Parsing::RuleLink("Build", this, "VersionNumberParser.Build"));
    AddRule(new LlvmVersionRule("LlvmVersion", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::SequenceParser(
                                    new Cm::Parsing::StringParser("LLVM"),
                                    new Cm::Parsing::KleeneStarParser(
                                        new Cm::Parsing::DifferenceParser(
                                            new Cm::Parsing::AnyCharParser(),
                                            new Cm::Parsing::NonterminalParser("newline", "newline", 0)))),
                                new Cm::Parsing::NonterminalParser("newline", "newline", 0)),
                            new Cm::Parsing::StringParser("  LLVM version ")),
                        new Cm::Parsing::ActionParser("A1",
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::SequenceParser(
                                    new Cm::Parsing::SequenceParser(
                                        new Cm::Parsing::SequenceParser(
                                            new Cm::Parsing::NonterminalParser("Major", "Major", 0),
                                            new Cm::Parsing::CharParser('.')),
                                        new Cm::Parsing::NonterminalParser("Minor", "Minor", 0)),
                                    new Cm::Parsing::OptionalParser(
                                        new Cm::Parsing::SequenceParser(
                                            new Cm::Parsing::SequenceParser(
                                                new Cm::Parsing::CharParser('.'),
                                                new Cm::Parsing::NonterminalParser("Revision", "Revision", 0)),
                                            new Cm::Parsing::OptionalParser(
                                                new Cm::Parsing::SequenceParser(
                                                    new Cm::Parsing::CharParser('.'),
                                                    new Cm::Parsing::NonterminalParser("Build", "Build", 0)))))),
                                new Cm::Parsing::KleeneStarParser(
                                    new Cm::Parsing::DifferenceParser(
                                        new Cm::Parsing::AnyCharParser(),
                                        new Cm::Parsing::NonterminalParser("newline", "newline", 0)))))),
                    new Cm::Parsing::NonterminalParser("newline", "newline", 0)),
                new Cm::Parsing::KleeneStarParser(
                    new Cm::Parsing::AnyCharParser())))));
}

VersionNumberParser* VersionNumberParser::Create()
{
    return Create(new Cm::Parsing::ParsingDomain());
}

VersionNumberParser* VersionNumberParser::Create(Cm::Parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    VersionNumberParser* grammar(new VersionNumberParser(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

VersionNumberParser::VersionNumberParser(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar("VersionNumberParser", parsingDomain_->GetNamespaceScope("Cm.Parser"), parsingDomain_)
{
    SetOwner(0);
}

Cm::Ast::ProgramVersion VersionNumberParser::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName)
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
    Cm::Ast::ProgramVersion result = *static_cast<Cm::Parsing::ValueObject<Cm::Ast::ProgramVersion>*>(value.get());
    stack.pop();
    return result;
}

class VersionNumberParser::VersionNumberRule : public Cm::Parsing::Rule
{
public:
    VersionNumberRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("Cm::Ast::ProgramVersion");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<Cm::Ast::ProgramVersion>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<VersionNumberRule>(this, &VersionNumberRule::A0Action));
        Cm::Parsing::NonterminalParser* majorNonterminalParser = GetNonterminal("Major");
        majorNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<VersionNumberRule>(this, &VersionNumberRule::PostMajor));
        Cm::Parsing::NonterminalParser* minorNonterminalParser = GetNonterminal("Minor");
        minorNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<VersionNumberRule>(this, &VersionNumberRule::PostMinor));
        Cm::Parsing::NonterminalParser* revisionNonterminalParser = GetNonterminal("Revision");
        revisionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<VersionNumberRule>(this, &VersionNumberRule::PostRevision));
        Cm::Parsing::NonterminalParser* buildNonterminalParser = GetNonterminal("Build");
        buildNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<VersionNumberRule>(this, &VersionNumberRule::PostBuild));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = Cm::Ast::ProgramVersion(context.fromMajor, context.fromMinor, context.fromRevision, context.fromBuild, "");
    }
    void PostMajor(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromMajor_value = std::move(stack.top());
            context.fromMajor = *static_cast<Cm::Parsing::ValueObject<int>*>(fromMajor_value.get());
            stack.pop();
        }
    }
    void PostMinor(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromMinor_value = std::move(stack.top());
            context.fromMinor = *static_cast<Cm::Parsing::ValueObject<int>*>(fromMinor_value.get());
            stack.pop();
        }
    }
    void PostRevision(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromRevision_value = std::move(stack.top());
            context.fromRevision = *static_cast<Cm::Parsing::ValueObject<int>*>(fromRevision_value.get());
            stack.pop();
        }
    }
    void PostBuild(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromBuild_value = std::move(stack.top());
            context.fromBuild = *static_cast<Cm::Parsing::ValueObject<int>*>(fromBuild_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromMajor(), fromMinor(), fromRevision(), fromBuild() {}
        Cm::Ast::ProgramVersion value;
        int fromMajor;
        int fromMinor;
        int fromRevision;
        int fromBuild;
    };
    std::stack<Context> contextStack;
    Context context;
};

class VersionNumberParser::MajorRule : public Cm::Parsing::Rule
{
public:
    MajorRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("int");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<int>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<MajorRule>(this, &MajorRule::A0Action));
        Cm::Parsing::NonterminalParser* majorNonterminalParser = GetNonterminal("major");
        majorNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<MajorRule>(this, &MajorRule::Postmajor));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.frommajor;
    }
    void Postmajor(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> frommajor_value = std::move(stack.top());
            context.frommajor = *static_cast<Cm::Parsing::ValueObject<int>*>(frommajor_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), frommajor() {}
        int value;
        int frommajor;
    };
    std::stack<Context> contextStack;
    Context context;
};

class VersionNumberParser::MinorRule : public Cm::Parsing::Rule
{
public:
    MinorRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("int");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<int>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<MinorRule>(this, &MinorRule::A0Action));
        Cm::Parsing::NonterminalParser* minorNonterminalParser = GetNonterminal("minor");
        minorNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<MinorRule>(this, &MinorRule::Postminor));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromminor;
    }
    void Postminor(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromminor_value = std::move(stack.top());
            context.fromminor = *static_cast<Cm::Parsing::ValueObject<int>*>(fromminor_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromminor() {}
        int value;
        int fromminor;
    };
    std::stack<Context> contextStack;
    Context context;
};

class VersionNumberParser::RevisionRule : public Cm::Parsing::Rule
{
public:
    RevisionRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("int");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<int>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<RevisionRule>(this, &RevisionRule::A0Action));
        Cm::Parsing::NonterminalParser* revisionNonterminalParser = GetNonterminal("revision");
        revisionNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<RevisionRule>(this, &RevisionRule::Postrevision));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.fromrevision;
    }
    void Postrevision(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> fromrevision_value = std::move(stack.top());
            context.fromrevision = *static_cast<Cm::Parsing::ValueObject<int>*>(fromrevision_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), fromrevision() {}
        int value;
        int fromrevision;
    };
    std::stack<Context> contextStack;
    Context context;
};

class VersionNumberParser::BuildRule : public Cm::Parsing::Rule
{
public:
    BuildRule(const std::string& name_, Scope* enclosingScope_, Parser* definition_):
        Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()
    {
        SetValueTypeName("int");
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
            stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<int>(context.value)));
        }
        context = std::move(contextStack.top());
        contextStack.pop();
    }
    virtual void Link()
    {
        Cm::Parsing::ActionParser* a0ActionParser = GetAction("A0");
        a0ActionParser->SetAction(new Cm::Parsing::MemberParsingAction<BuildRule>(this, &BuildRule::A0Action));
        Cm::Parsing::NonterminalParser* buildNonterminalParser = GetNonterminal("build");
        buildNonterminalParser->SetPostCall(new Cm::Parsing::MemberPostCall<BuildRule>(this, &BuildRule::Postbuild));
    }
    void A0Action(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)
    {
        context.value = context.frombuild;
    }
    void Postbuild(Cm::Parsing::ObjectStack& stack, bool matched)
    {
        if (matched)
        {
            std::unique_ptr<Cm::Parsing::Object> frombuild_value = std::move(stack.top());
            context.frombuild = *static_cast<Cm::Parsing::ValueObject<int>*>(frombuild_value.get());
            stack.pop();
        }
    }
private:
    struct Context
    {
        Context(): value(), frombuild() {}
        int value;
        int frombuild;
    };
    std::stack<Context> contextStack;
    Context context;
};

void VersionNumberParser::GetReferencedGrammars()
{
    Cm::Parsing::ParsingDomain* pd = GetParsingDomain();
    Cm::Parsing::Grammar* grammar0 = pd->GetGrammar("Cm.Parsing.stdlib");
    if (!grammar0)
    {
        grammar0 = Cm::Parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
}

void VersionNumberParser::CreateRules()
{
    AddRuleLink(new Cm::Parsing::RuleLink("int", this, "Cm.Parsing.stdlib.int"));
    AddRule(new VersionNumberRule("VersionNumber", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::SequenceParser(
                new Cm::Parsing::SequenceParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::NonterminalParser("Major", "Major", 0),
                        new Cm::Parsing::CharParser('.')),
                    new Cm::Parsing::NonterminalParser("Minor", "Minor", 0)),
                new Cm::Parsing::OptionalParser(
                    new Cm::Parsing::SequenceParser(
                        new Cm::Parsing::SequenceParser(
                            new Cm::Parsing::CharParser('.'),
                            new Cm::Parsing::NonterminalParser("Revision", "Revision", 0)),
                        new Cm::Parsing::OptionalParser(
                            new Cm::Parsing::SequenceParser(
                                new Cm::Parsing::CharParser('.'),
                                new Cm::Parsing::NonterminalParser("Build", "Build", 0)))))))));
    AddRule(new MajorRule("Major", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("major", "int", 0))));
    AddRule(new MinorRule("Minor", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("minor", "int", 0))));
    AddRule(new RevisionRule("Revision", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("revision", "int", 0))));
    AddRule(new BuildRule("Build", GetScope(),
        new Cm::Parsing::ActionParser("A0",
            new Cm::Parsing::NonterminalParser("build", "int", 0))));
}

} } // namespace Cm.Parser

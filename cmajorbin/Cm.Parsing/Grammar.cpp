/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Scope.hpp>
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing/Exception.hpp>
#include <Cm.Parsing/Visitor.hpp>
#include <Cm.Parsing/Composite.hpp>
#include <Cm.Parsing/XmlLog.hpp>
#include <Cm.Parsing/Linking.hpp>
#include <Cm.Parsing/ParsingDomain.hpp>

namespace Cm { namespace Parsing {

Grammar::Grammar(const std::string& name_, Scope* enclosingScope_): ParsingObject(name_, enclosingScope_), parsingDomain(new ParsingDomain()), ns(nullptr),
    linking(false), linked(false), contentParser(nullptr), startRule(nullptr), skipRule(nullptr), log(0), maxLogLineLength(256), recover(false)
{
    RegisterParsingDomain(parsingDomain);
    SetScope(new Scope(Name(), EnclosingScope()));
}

Grammar::Grammar(const std::string& name_, Scope* enclosingScope_, ParsingDomain* parsingDomain_): ParsingObject(name_, enclosingScope_), parsingDomain(parsingDomain_), ns(nullptr), 
    linking(false), linked(false), contentParser(nullptr), startRule(nullptr), skipRule(nullptr), log(0), maxLogLineLength(256), recover(false)
{
    SetScope(new Scope(Name(), EnclosingScope()));
}

void Grammar::AddRule(Rule* rule)
{
    Own(rule);
    rule->SetGrammar(this);
    rule->GetScope()->SetEnclosingScope(GetScope());
    rule->SetEnclosingScope(GetScope());
    rules.push_back(rule);
    try
    {
        GetScope()->Add(rule);
    }
    catch (std::exception& ex)
    {
        ThrowException(ex.what(), GetSpan());
    }
}

Rule* Grammar::GetRule(const std::string& ruleName) const
{
    ParsingObject* object = GetScope()->Get(ruleName);
    if (!object)
    {
        ThrowException("rule '" + ruleName + "' not found in grammar '" + FullName() + "'", GetSpan());
    }
    if (object->IsRule())
    {
        return static_cast<Rule*>(object);
    }
    else if (object->IsRuleLink())
    {
        RuleLink* ruleLink = static_cast<RuleLink*>(object);
        Rule* rule = ruleLink->GetRule();
        if (rule)
        {
            return rule;
        }
        else
        {
            ThrowException("rule link '" + ruleName + "' not bound in grammar '" + FullName() + "'", GetSpan());
        }
    }
    else
    {
        ThrowException("'" + ruleName + "' is not a rule or rule link", GetSpan());
    }
    return nullptr;
}

void Grammar::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    RuleLinkSetIt e = ruleLinks.cend();
    for (RuleLinkSetIt i = ruleLinks.cbegin(); i != e; ++i)
    {
        RuleLink* ruleLink = *i;
        ruleLink->Accept(visitor);
    }
    int n = int(rules.size());
    for (int i = 0; i < n; ++i)
    {
        Rule* rule = rules[i];
        rule->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

void Grammar::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName)
{
    Scanner scanner(start, end, fileName, fileIndex, skipRule);
    if (recover)
    {
        scanner.SetRecover();
    }
    std::unique_ptr<XmlLog> xmlLog;
    if (log)
    {
        xmlLog.reset(new XmlLog(*log, maxLogLineLength));
        scanner.SetLog(xmlLog.get());
        xmlLog->WriteBeginRule("parse");
    }
    ObjectStack stack;
    Match match = Parse(scanner, stack);
    Span stop = scanner.GetSpan();
    if (log)
    {
        xmlLog->WriteEndRule("parse");
    }
    if (!match.Hit() || stop.Start() != int(end - start))
    {
        if (start)
        {
            throw ExpectationFailure(startRule->Info(), fileName, stop, start, end);
        }
        else
        {
            throw ParsingException("grammar '" + FullName() + "' has no start rule", fileName, stop, start, end);
        }
    }
}

Match Grammar::Parse(Scanner& scanner, ObjectStack& stack)
{
    if (startRule)
    {
        if (!contentParser)
        {
            if (skipRule)
            {
                contentParser = new SequenceParser(new SequenceParser(new OptionalParser(skipRule), startRule), new OptionalParser(skipRule));
                Own(contentParser);
            }
            else
            {
                contentParser = startRule;
            }
        }
        if (recover)
        {
            scanner.SetRecover();
        }
        Match match = contentParser->Parse(scanner, stack);
        if (recover && scanner.HasErrors())
        {
            CombinedParsingError combinedError = scanner.GetCombinedError();
            throw combinedError;
        }
        return match;
    }
    return Match::Nothing();
}

void Grammar::ResolveStartRule()
{
    if (startRuleName.empty())
    {
        if (!rules.empty())
        {
            startRule = rules.front();
        }
        else
        {
            ThrowException("cannot resolve start rule because grammar '" + FullName() + "' is empty", GetSpan());
        }
    }
    else
    {
        startRule = GetRule(startRuleName);
    }
}

void Grammar::ResolveSkipRule()
{
    if (!skipRuleName.empty())
    {
        skipRule = GetRule(skipRuleName);
    }
}

void Grammar::Link()
{
    if (!linked && !linking)
    {
        linking = true;
        GetReferencedGrammars();
        GrammarSetIt e = grammarReferences.end();
        for (GrammarSetIt i = grammarReferences.begin(); i != e; ++i)
        {
            Grammar* grammarReference = *i;
            grammarReference->Link();
        }
        LinkerVisitor linkerVisitor;
        Accept(linkerVisitor);
        linking = false;
        linked = true;
    }
}

void Grammar::AddGrammarReference(Grammar* grammarReference)
{
    Own(grammarReference);
    grammarReferences.insert(grammarReference);
}

void Grammar::AddRuleLink(RuleLink* ruleLink)
{
    Own(ruleLink);
    ruleLinks.insert(ruleLink);
}

} } // namespace Cm::Parsing
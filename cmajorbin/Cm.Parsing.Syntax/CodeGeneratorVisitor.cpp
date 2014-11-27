/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.Parsing.Syntax/CodeGeneratorVisitor.hpp>
#include <Cm.Util/TextUtils.hpp>
#include <Cm.Parsing/Keyword.hpp>
#include <Cm.Parsing/Grammar.hpp>
#include <Cm.Parsing/Rule.hpp>
#include <Cm.Parsing/Action.hpp>
#include <Cm.Parsing/Nonterminal.hpp>
#include <Cm.Parsing/Namespace.hpp>

namespace Cm { namespace Parsing { namespace Syntax {

using namespace Cm::Parsing;
using Cm::Util::StringStr;
using Cm::Util::CharStr;
using Cm::Util::Replace;

class KeywordListCreator : public Cm::Parsing::Visitor
{
public:
    KeywordListCreator (CodeFormatter& cppFormatter_, CodeFormatter& hppFormatter_): 
        cppFormatter(cppFormatter_), hppFormatter(hppFormatter_), keywordListNumber(0), keywordRuleListNumber(0)
    {
    }
    virtual void Visit(KeywordListParser& parser)
    {
        std::string keywordVecName = "keywords" + std::to_string(keywordListNumber);
        ++keywordListNumber;
        hppFormatter.WriteLine("std::vector<std::string> " + keywordVecName + ";");
        KeywordListParser::KeywordSetIt e = parser.Keywords().end();
        for (KeywordListParser::KeywordSetIt i = parser.Keywords().begin(); i != e; ++i)
        {
            std::string keywordStr = "\"" + StringStr(*i) + "\"";
            cppFormatter.WriteLine(keywordVecName + ".push_back(" + keywordStr + ");");
        }
        parser.KeywordVecName() = keywordVecName;
    }
private:
    CodeFormatter& cppFormatter;
    CodeFormatter& hppFormatter;
    int keywordListNumber;
    int keywordRuleListNumber;
};

CodeGeneratorVisitor::CodeGeneratorVisitor(CodeFormatter& cppFormatter_, CodeFormatter& hppFormatter_): 
    cppFormatter(cppFormatter_), hppFormatter(hppFormatter_)
{
}

void CodeGeneratorVisitor::BeginVisit(Grammar& grammar)
{
    std::string grammarPtrType = grammar.Name() + "*";
    hppFormatter.WriteLine("class " + grammar.Name() + " : public Cm::Parsing::Grammar");
    hppFormatter.WriteLine("{");
    hppFormatter.WriteLine("public:");
    hppFormatter.IncIndent();
    hppFormatter.WriteLine("static " + grammarPtrType + " Create();");
    hppFormatter.WriteLine("static " + grammarPtrType + " Create(Cm::Parsing::ParsingDomain* parsingDomain);");

    std::string valueTypeName;
    std::string parameters;
    if (grammar.StartRule())
    {
        valueTypeName = !grammar.StartRule()->ValueTypeName().empty() ? grammar.StartRule()->ValueTypeName() : "void";
        parameters = "";
        if (grammar.StartRule()->TakesOrReturnsValuesOrHasLocals())
        {
            int n = int(grammar.StartRule()->InheritedAttributes().size());
            for (int i = 0; i < n; ++i)
            {
                const AttrOrVariable& attr = grammar.StartRule()->InheritedAttributes()[i];
                parameters.append(", " + attr.TypeName() + " " + attr.Name());
            }
            hppFormatter.WriteLine(valueTypeName + " Parse(const char* start, const char* end, int fileIndex, const std::string& fileName" + parameters + ");");
        }
    }
    hppFormatter.DecIndent();
    hppFormatter.WriteLine("private:");
    hppFormatter.IncIndent();

    cppFormatter.WriteLine(grammarPtrType + " " + grammar.Name() + "::Create()");
    cppFormatter.WriteLine("{");
    cppFormatter.IncIndent();
    cppFormatter.WriteLine("return Create(new Cm::Parsing::ParsingDomain());");
    cppFormatter.DecIndent();
    cppFormatter.WriteLine("}");
    cppFormatter.NewLine();

    cppFormatter.WriteLine(grammarPtrType + " " + grammar.Name() + "::Create(Cm::Parsing::ParsingDomain* parsingDomain)");
    cppFormatter.WriteLine("{");
    cppFormatter.IncIndent();
    cppFormatter.WriteLine("RegisterParsingDomain(parsingDomain);");
    cppFormatter.WriteLine(grammarPtrType + " grammar(new " + grammar.Name() + "(parsingDomain));");
    cppFormatter.WriteLine("parsingDomain->AddGrammar(grammar);");
    cppFormatter.WriteLine("grammar->CreateRules();");
    cppFormatter.WriteLine("grammar->Link();");
    cppFormatter.WriteLine("return grammar;");
    cppFormatter.DecIndent();
    cppFormatter.WriteLine("}");
    cppFormatter.NewLine();

    std::string grammarNamespaceName = grammar.Ns()->FullName();
    cppFormatter.WriteLine(grammar.Name() + "::" + grammar.Name() + "(Cm::Parsing::ParsingDomain* parsingDomain_): Cm::Parsing::Grammar(\"" + grammar.Name() + 
        "\", parsingDomain_->GetNamespaceScope(\"" + grammarNamespaceName + "\"), parsingDomain_)");
    cppFormatter.WriteLine("{");
    cppFormatter.IncIndent();
    cppFormatter.WriteLine("SetOwner(0);");
    KeywordListCreator creator(cppFormatter, hppFormatter);
    int n = int(grammar.Rules().size());
    for (int i = 0; i < n; ++i)
    {
        Rule* rule = grammar.Rules()[i];
        rule->Accept(creator);
    }
    cppFormatter.DecIndent();
    cppFormatter.WriteLine("}");
    cppFormatter.NewLine();

    if (grammar.StartRule())
    {
        if (grammar.StartRule()->TakesOrReturnsValuesOrHasLocals())
        {
            cppFormatter.WriteLine(valueTypeName + " " + grammar.Name() + 
                "::Parse(const char* start, const char* end, int fileIndex, const std::string& fileName" + parameters + ")");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            cppFormatter.WriteLine("Cm::Parsing::Scanner scanner(start, end, fileName, fileIndex, SkipRule());");
            cppFormatter.WriteLine("std::unique_ptr<Cm::Parsing::XmlLog> xmlLog;");
            cppFormatter.WriteLine("if (Log())");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            cppFormatter.WriteLine("xmlLog.reset(new Cm::Parsing::XmlLog(*Log(), MaxLogLineLength()));");
            cppFormatter.WriteLine("scanner.SetLog(xmlLog.get());");
            cppFormatter.WriteLine("xmlLog->WriteBeginRule(\"parse\");");
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");
            cppFormatter.WriteLine("Cm::Parsing::ObjectStack stack;");

            int n = int(grammar.StartRule()->InheritedAttributes().size());
            for (int i = 0; i < n; ++i)
            {
                const AttrOrVariable& attr = grammar.StartRule()->InheritedAttributes()[i];
                cppFormatter.WriteLine("stack.push(std::unique_ptr<Cm::Parsing::Object>(new ValueObject<" + attr.TypeName() + ">(" + attr.Name() + ")));");
            }

            cppFormatter.WriteLine("Cm::Parsing::Match match = Cm::Parsing::Grammar::Parse(scanner, stack);");
            cppFormatter.WriteLine("Cm::Parsing::Span stop = scanner.GetSpan();");
            cppFormatter.WriteLine("if (Log())");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            cppFormatter.WriteLine("xmlLog->WriteEndRule(\"parse\");");
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");
            cppFormatter.WriteLine("if (!match.Hit() || stop.Start() != int(end - start))");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            cppFormatter.WriteLine("if (StartRule())");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            cppFormatter.WriteLine("throw Cm::Parsing::ExpectationFailure(StartRule()->Info(), fileName, stop, start, end);");
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");
            cppFormatter.WriteLine("else");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            cppFormatter.WriteLine("throw Cm::Parsing::ParsingException(\"grammar '\" + Name() + \"' has no start rule\", fileName, scanner.GetSpan(), start, end);");
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");

            if (valueTypeName != "void")
            {
                cppFormatter.WriteLine("std::unique_ptr<Cm::Parsing::Object> value = std::move(stack.top());");
                cppFormatter.WriteLine(valueTypeName + " result = *static_cast<Cm::Parsing::ValueObject<" + valueTypeName + ">*>(value.get());");
                cppFormatter.WriteLine("stack.pop();");
                cppFormatter.WriteLine("return result;");
            }
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");
            cppFormatter.NewLine();
        }
    }
    n = int(grammar.Rules().size());
    for (int i = 0; i < n; ++i)
    {
        Rule* rule = grammar.Rules()[i];
        if (rule->Specialized())
        {
            cppFormatter.WriteLine("class " + grammar.Name() + "::" + rule->SpecializedTypeName() + " : public Cm::Parsing::Rule");
            cppFormatter.WriteLine("{");
            cppFormatter.WriteLine("public:");
            cppFormatter.IncIndent();
            // constructor:
            cppFormatter.WriteLine(rule->SpecializedTypeName() + "(const std::string& name_, Scope* enclosingScope_, Parser* definition_):");
            cppFormatter.IncIndent();
            cppFormatter.WriteLine("Cm::Parsing::Rule(name_, enclosingScope_, definition_), contextStack(), context()");
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            int m = int(rule->InheritedAttributes().size());
            for (int i = 0; i < m; ++i)
            {
                const AttrOrVariable& attr = rule->InheritedAttributes()[i];
                cppFormatter.WriteLine("AddInheritedAttribute(AttrOrVariable(\"" + attr.TypeName() + "\", \"" + attr.Name() + "\"));");
            }
            if (!rule->ValueTypeName().empty())
            {
                cppFormatter.WriteLine("SetValueTypeName(\"" + rule->ValueTypeName() + "\");");
            }
            m = int(rule->LocalVariables().size());
            for (int i = 0; i < m; ++i)
            {
                const AttrOrVariable& var = rule->LocalVariables()[i];
                cppFormatter.WriteLine("AddLocalVariable(AttrOrVariable(\"" + var.TypeName() + "\", \"" + var.Name() + "\"));");
            }
            if (rule->IsSynchronizingRule())
            {
                cppFormatter.WriteLine("Synchronize(\"" + StringStr(rule->GetSynchronizeCharacters()) + "\");");
            }
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");

            // enter:
            cppFormatter.WriteLine("virtual void Enter(Cm::Parsing::ObjectStack& stack)");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            cppFormatter.WriteLine("contextStack.push(std::move(context));");
            cppFormatter.WriteLine("context = Context();");
            m = int(rule->InheritedAttributes().size());
            for (int i = m - 1; i >= 0; --i)
            {
                const AttrOrVariable& attr = rule->InheritedAttributes()[i];
                cppFormatter.WriteLine("std::unique_ptr<Cm::Parsing::Object> " + attr.Name() + "_value = std::move(stack.top());");
                cppFormatter.WriteLine("context." + attr.Name() + " = *static_cast<Cm::Parsing::ValueObject<" + attr.TypeName() + ">*>(" + attr.Name() + "_value.get());");
                cppFormatter.WriteLine("stack.pop();");
            }
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");
            // leave:
            cppFormatter.WriteLine("virtual void Leave(Cm::Parsing::ObjectStack& stack, bool matched)");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            if (!rule->ValueTypeName().empty())
            {
                cppFormatter.WriteLine("if (matched)");
                cppFormatter.WriteLine("{");
                cppFormatter.IncIndent();
                cppFormatter.WriteLine("stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<" + rule->ValueTypeName() + ">(context.value)));");
                cppFormatter.DecIndent();
                cppFormatter.WriteLine("}");
            }
            cppFormatter.WriteLine("context = std::move(contextStack.top());");
            cppFormatter.WriteLine("contextStack.pop();");
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");

            // link:
            cppFormatter.WriteLine("virtual void Link()");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            m = int(rule->Actions().size());
            for (int i = 0; i < m; ++i)
            {
                ActionParser* action = rule->Actions()[i];
                cppFormatter.WriteLine("Cm::Parsing::ActionParser* " + action->VariableName() + " = GetAction(\"" + action->Name() + "\");");
                cppFormatter.WriteLine(action->VariableName() + "->SetAction(new Cm::Parsing::MemberParsingAction<" + rule->SpecializedTypeName() + ">(this, &" + rule->SpecializedTypeName() + "::" + action->MethodName() + "));");
                if (action->FailCode())
                {
                    cppFormatter.WriteLine(action->VariableName() + "->SetFailureAction(new Cm::Parsing::MemberFailureAction<" + rule->SpecializedTypeName() + ">(this, &" + rule->SpecializedTypeName() + "::" + action->MethodName() + "Fail));");
                }
            }
            m = int(rule->Nonterminals().size());
            for (int i = 0; i < m; ++i)
            {
                NonterminalParser* nonterminal = rule->Nonterminals()[i];
                if (nonterminal->Specialized())
                {
                    cppFormatter.WriteLine("Cm::Parsing::NonterminalParser* " + nonterminal->VariableName() + " = GetNonterminal(\"" + nonterminal->Name() + "\");");
                    if (!nonterminal->Arguments().empty())
                    {
                        cppFormatter.WriteLine(nonterminal->VariableName() + "->SetPreCall(new Cm::Parsing::MemberPreCall<" + rule->SpecializedTypeName() + ">(this, &" + rule->SpecializedTypeName() + "::" + nonterminal->PreCallMethodName() + "));");
                    }
                    if (!nonterminal->GetRule()->ValueTypeName().empty())
                    {
                        cppFormatter.WriteLine(nonterminal->VariableName() + "->SetPostCall(new Cm::Parsing::MemberPostCall<" + rule->SpecializedTypeName() + ">(this, &" + rule->SpecializedTypeName() + "::" + nonterminal->PostCallMethodName() + "));");
                    }
                }
            }
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");

            // action methods:
            m = int(rule->Actions().size());
            for (int i = 0; i < m; ++i)
            {
                ActionParser* action = rule->Actions()[i];
                cppFormatter.WriteLine("void " + action->MethodName() +
                    "(const char* matchBegin, const char* matchEnd, const Span& span, const std::string& fileName, bool& pass)");
                action->SuccessCode()->Print(cppFormatter);
                if (action->FailCode())
                {
                    cppFormatter.WriteLine("void " + action->MethodName() + "Fail()");
                    action->FailCode()->Print(cppFormatter);
                }
            }

            // pre-post calls:
            m = int(rule->Nonterminals().size());
            for (int i = 0; i < m; ++i)
            {
                NonterminalParser* nonterminal = rule->Nonterminals()[i];
                if (nonterminal->Specialized())
                {
                    if (!nonterminal->Arguments().empty())
                    {
                        cppFormatter.WriteLine("void " + nonterminal->PreCallMethodName() + "(Cm::Parsing::ObjectStack& stack)");
                        cppFormatter.WriteLine("{");
                        cppFormatter.IncIndent();
                        int p = int(nonterminal->Arguments().size());
                        for (int j = 0; j < p; ++j)
                        {
                            Cm::Parsing::CppObjectModel::CppObject* argument = nonterminal->Arguments()[j];
                            std::string argumentTypeName = nonterminal->GetRule()->InheritedAttributes()[j].TypeName();
                            cppFormatter.Write("stack.push(std::unique_ptr<Cm::Parsing::Object>(new Cm::Parsing::ValueObject<" + argumentTypeName + ">(");
                            argument->Print(cppFormatter);
                            cppFormatter.WriteLine(")));");
                        }
                        cppFormatter.DecIndent();
                        cppFormatter.WriteLine("}");
                    }
                    if (!nonterminal->GetRule()->ValueTypeName().empty())
                    {
                        cppFormatter.WriteLine("void " + nonterminal->PostCallMethodName() + "(Cm::Parsing::ObjectStack& stack, bool matched)");
                        cppFormatter.WriteLine("{");
                        cppFormatter.IncIndent();
                        cppFormatter.WriteLine("if (matched)");
                        cppFormatter.WriteLine("{");
                        cppFormatter.IncIndent();
                        cppFormatter.WriteLine("std::unique_ptr<Cm::Parsing::Object> " + nonterminal->ValueFieldName() + "_value = std::move(stack.top());");
                        cppFormatter.WriteLine("context." + nonterminal->ValueFieldName() + " = *static_cast<Cm::Parsing::ValueObject<" + 
                            nonterminal->GetRule()->ValueTypeName() + ">*>(" + nonterminal->ValueFieldName() + "_value.get());");
                        cppFormatter.WriteLine("stack.pop();");
                        cppFormatter.DecIndent();
                        cppFormatter.WriteLine("}");
                        cppFormatter.DecIndent();
                        cppFormatter.WriteLine("}");
                    }
                }
            }
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("private:");
            cppFormatter.IncIndent();

            // Context:
            cppFormatter.WriteLine("struct Context");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            cppFormatter.Write("Context(): ");
            bool first = true;
            m = int(rule->InheritedAttributes().size());
            for (int j = 0; j < m; ++j)
            {
                if (first) { first = false; } else { cppFormatter.Write(", "); }
                const AttrOrVariable& attr = rule->InheritedAttributes()[j];
                cppFormatter.Write(attr.Name() + "()");
            }
            if (!rule->ValueTypeName().empty())
            {
                if (first) { first = false; } else { cppFormatter.Write(", "); }
                cppFormatter.Write("value()");
            }
            m = int(rule->LocalVariables().size());
            for (int j = 0; j < m; ++j)
            {
                if (first) { first = false; } else { cppFormatter.Write(", "); }
                const AttrOrVariable& var = rule->LocalVariables()[j];
                cppFormatter.Write(var.Name() + "()");
            }
            m = int(rule->Nonterminals().size());
            for (int j = 0; j < m; ++j)
            {
                NonterminalParser* nonterminal = rule->Nonterminals()[j];
                if (!nonterminal->GetRule()->ValueTypeName().empty())
                {
                    if (first) { first = false; } else { cppFormatter.Write(", "); }
                    cppFormatter.Write(nonterminal->ValueFieldName() + "()");
                }
            }
            cppFormatter.WriteLine(" {}");
            m = int(rule->InheritedAttributes().size());
            for (int j = 0; j < m; ++j)
            {
                const AttrOrVariable& attr = rule->InheritedAttributes()[j];
                cppFormatter.WriteLine(attr.TypeName() + " " + attr.Name() + ";");
            }
            if (!rule->ValueTypeName().empty())
            {
                cppFormatter.WriteLine(rule->ValueTypeName() + " value;");
            }
            m = int(rule->LocalVariables().size());
            for (int j = 0; j < m; ++j)
            {
                const AttrOrVariable& var = rule->LocalVariables()[j];
                cppFormatter.WriteLine(var.TypeName() + " " + var.Name() + ";");
            }
            m = int(rule->Nonterminals().size());
            for (int j = 0; j < m; ++j)
            {
                NonterminalParser* nonterminal = rule->Nonterminals()[j];
                if (!nonterminal->GetRule()->ValueTypeName().empty())
                {
                    cppFormatter.WriteLine(nonterminal->GetRule()->ValueTypeName() + " " + nonterminal->ValueFieldName() + ";");
                }
            }
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("};");
            cppFormatter.WriteLine("std::stack<Context> contextStack;");
            cppFormatter.WriteLine("Context context;");
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("};");
            cppFormatter.NewLine();
        }
    }

    cppFormatter.WriteLine("void " + grammar.Name() + "::GetReferencedGrammars()");
    cppFormatter.WriteLine("{");
    cppFormatter.IncIndent();
    int grammarIndex = 0;
    if (!grammar.GrammarReferences().empty())
    {
        cppFormatter.WriteLine("Cm::Parsing::ParsingDomain* pd = GetParsingDomain();");
        GrammarSetIt e = grammar.GrammarReferences().cend();
        for (GrammarSetIt i = grammar.GrammarReferences().cbegin(); i != e; ++i)
        {
            Grammar* grammarReference = *i;
            std::string grammarReferenceName = grammarReference->FullName();
            std::string grammarVar = "grammar" + std::to_string(grammarIndex);
            ++grammarIndex;
            cppFormatter.WriteLine("Cm::Parsing::Grammar* " + grammarVar + " = pd->GetGrammar(\"" + grammarReferenceName + "\");");
            cppFormatter.WriteLine("if (!" + grammarVar + ")");
            cppFormatter.WriteLine("{");
            cppFormatter.IncIndent();
            cppFormatter.WriteLine(grammarVar + " = " + Replace(grammarReferenceName, ".", "::") + "::Create(pd);");
            cppFormatter.DecIndent();
            cppFormatter.WriteLine("}");
            cppFormatter.WriteLine("AddGrammarReference(" + grammarVar + ");");
        }
    }
    cppFormatter.DecIndent();
    cppFormatter.WriteLine("}");
    cppFormatter.NewLine();
    cppFormatter.WriteLine("void " + grammar.Name() + "::CreateRules()");
    cppFormatter.WriteLine("{");
    cppFormatter.IncIndent();
}

void CodeGeneratorVisitor::EndVisit(Grammar& grammar)
{
    if (!grammar.StartRuleName().empty())
    {
        cppFormatter.WriteLine("SetStartRuleName(\"" + grammar.StartRuleName() + "\");");
    }
    if (!grammar.SkipRuleName().empty())
    {
        cppFormatter.WriteLine("SetSkipRuleName(\"" + grammar.SkipRuleName() + "\");");
    }
    if (grammar.Recover())
    {
        cppFormatter.WriteLine("SetRecover();");
    }
    cppFormatter.DecIndent();
    cppFormatter.WriteLine("}");
    cppFormatter.WriteLine();

    hppFormatter.WriteLine(grammar.Name() + "(Cm::Parsing::ParsingDomain* parsingDomain_);");
    hppFormatter.WriteLine("virtual void CreateRules();");
    hppFormatter.WriteLine("virtual void GetReferencedGrammars();");

    int n = int(grammar.Rules().size());
    for (int i = 0; i < n; ++i)
    {
        Rule* rule = grammar.Rules()[i];
        if (rule->Specialized())
        {
            hppFormatter.WriteLine("class " + rule->SpecializedTypeName() + ";");
        }
    }

    hppFormatter.DecIndent();
    hppFormatter.WriteLine("};");
    hppFormatter.WriteLine();
}

void CodeGeneratorVisitor::Visit(CharParser& parser)
{
    cppFormatter.Write("new Cm::Parsing::CharParser('");
    cppFormatter.Write(CharStr(parser.GetChar()));
    cppFormatter.Write("')");
}

void CodeGeneratorVisitor::Visit(StringParser& parser)
{
    cppFormatter.Write("new Cm::Parsing::StringParser(\"" + StringStr(parser.GetString()) + "\")");
}

void CodeGeneratorVisitor::Visit(CharSetParser& parser)
{
    cppFormatter.Write("new Cm::Parsing::CharSetParser(\"" + StringStr(parser.Set()) + "\"");
    if (parser.Inverse())
    {
        cppFormatter.Write(", true)");
    }
    else
    {
        cppFormatter.Write(")");
    }
}

void CodeGeneratorVisitor::Visit(KeywordParser& parser)
{
    if (parser.ContinuationRuleName().empty())
    {
        cppFormatter.Write("new Cm::Parsing::KeywordParser(\"" + StringStr(parser.Keyword()) + "\")");
    }
    else
    {
        cppFormatter.Write("new Cm::Parsing::KeywordParser(\"" + StringStr(parser.Keyword()) + "\", \"" + parser.ContinuationRuleName() + "\")");
    }
}

void CodeGeneratorVisitor::Visit(KeywordListParser& parser)
{
    cppFormatter.Write("new Cm::Parsing::KeywordListParser(\"" + StringStr(parser.SelectorRuleName()) + "\", " + parser.KeywordVecName() + ")");
}

void CodeGeneratorVisitor::Visit(EmptyParser& parser)
{
    cppFormatter.Write("new Cm::Parsing::EmptyParser()");
}

void CodeGeneratorVisitor::Visit(SpaceParser& parser)
{
    cppFormatter.Write("new Cm::Parsing::SpaceParser()");
}

void CodeGeneratorVisitor::Visit(LetterParser& parser)
{
    cppFormatter.Write("new Cm::Parsing::LetterParser()");
}

void CodeGeneratorVisitor::Visit(DigitParser& parser)
{
    cppFormatter.Write("new Cm::Parsing::DigitParser()");
}

void CodeGeneratorVisitor::Visit(HexDigitParser& parser)
{
    cppFormatter.Write("new Cm::Parsing::HexDigitParser()");
}

void CodeGeneratorVisitor::Visit(PunctuationParser& parser)
{
    cppFormatter.Write("new Cm::Parsing::PunctuationParser()");
}

void CodeGeneratorVisitor::Visit(AnyCharParser& parser)
{
    cppFormatter.Write("new Cm::Parsing::AnyCharParser()");
}

void CodeGeneratorVisitor::Visit(NonterminalParser& parser)
{
    cppFormatter.Write("new Cm::Parsing::NonterminalParser(\"" + parser.Name() + "\", \"" + parser.RuleName() + "\", " + 
        std::to_string(parser.NumberOfArguments()) + ")");
}

void CodeGeneratorVisitor::Visit(RuleLink& link)
{
    cppFormatter.WriteLine("AddRuleLink(new Cm::Parsing::RuleLink(\"" + link.Name() + "\", this, \"" + link.LinkedRuleName() + "\"));");
}

void CodeGeneratorVisitor::BeginVisit(Rule& rule)
{
    if (rule.Specialized())
    {
        cppFormatter.Write("AddRule(new " + rule.SpecializedTypeName() + "(\"" + rule.Name() + "\", GetScope(),");
    }
    else
    {
        cppFormatter.Write("AddRule(new Cm::Parsing::Rule(\"" + rule.Name() + "\", GetScope(),");
    }
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::EndVisit(Rule& rule)
{
    cppFormatter.Write("));");
    cppFormatter.DecIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::BeginVisit(OptionalParser& parser)
{
    cppFormatter.Write("new Cm::Parsing::OptionalParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::EndVisit(OptionalParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(PositiveParser& parser)
{
    cppFormatter.Write("new Cm::Parsing::PositiveParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::EndVisit(PositiveParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(KleeneStarParser& parser)
{
    cppFormatter.Write("new Cm::Parsing::KleeneStarParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::EndVisit(KleeneStarParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(ActionParser& parser)
{
    cppFormatter.Write("new Cm::Parsing::ActionParser(\"" + parser.Name() + "\",");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::EndVisit(ActionParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(ExpectationParser& parser)
{
    cppFormatter.Write("new Cm::Parsing::ExpectationParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::EndVisit(ExpectationParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(TokenParser& parser)
{
    cppFormatter.Write("new Cm::Parsing::TokenParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::EndVisit(TokenParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(SequenceParser& parser)
{
    cppFormatter.Write("new Cm::Parsing::SequenceParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::Visit(SequenceParser& parser)
{
    cppFormatter.WriteLine(",");
}

void CodeGeneratorVisitor::EndVisit(SequenceParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(AlternativeParser& parser)
{
    cppFormatter.Write("new Cm::Parsing::AlternativeParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::Visit(AlternativeParser& parser)
{
    cppFormatter.WriteLine(",");
}

void CodeGeneratorVisitor::EndVisit(AlternativeParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(DifferenceParser& parser)
{
    cppFormatter.Write("new Cm::Parsing::DifferenceParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::Visit(DifferenceParser& parser)
{
    cppFormatter.WriteLine(",");
}

void CodeGeneratorVisitor::EndVisit(DifferenceParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(IntersectionParser& parser)
{
    cppFormatter.Write("new Cm::Parsing::IntersectionParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::Visit(IntersectionParser& parser)
{
    cppFormatter.WriteLine(",");
}

void CodeGeneratorVisitor::EndVisit(IntersectionParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(ExclusiveOrParser& parser)
{
    cppFormatter.Write("new Cm::Parsing::ExclusiveOrParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::Visit(ExclusiveOrParser& parser)
{
    cppFormatter.WriteLine(",");
}

void CodeGeneratorVisitor::EndVisit(ExclusiveOrParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

void CodeGeneratorVisitor::BeginVisit(ListParser& parser)
{
    cppFormatter.Write("new Cm::Parsing::ListParser(");
    cppFormatter.IncIndent();
    cppFormatter.NewLine();
}

void CodeGeneratorVisitor::Visit(ListParser& parser)
{
    cppFormatter.WriteLine(",");
}

void CodeGeneratorVisitor::EndVisit(ListParser& parser)
{
    cppFormatter.Write(")");
    cppFormatter.DecIndent();
}

} } } // namespace Cm::Parsing::Syntax

/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef CM_PARSING_RULE_INCLUDED
#define CM_PARSING_RULE_INCLUDED
#include <Cm.Parsing/Parser.hpp>

namespace Cm { namespace Parsing {

class AttrOrVariable
{
public:
    AttrOrVariable(const std::string& typeName_, const std::string& name_);
    const std::string& TypeName() const { return typeName; }
    const std::string& Name() const { return name; }
private:
    std::string typeName;
    std::string name;
};

class ActionParser;
class NonterminalParser;
class Grammar;

class Rule: public Parser
{
public:
    typedef std::vector<AttrOrVariable> AttrOrVariableVec;
    typedef std::vector<ActionParser*> ActionVec;
    typedef std::vector<NonterminalParser*> NonterminalVec;
    Rule(const std::string& name_, Scope* enclosingScope_, Parser* definition_);
    Rule(const std::string& name_, Scope* enclosingScope_);
    virtual bool IsRule() const { return true; }
    Grammar* GetGrammar() const { return grammar; }
    void SetGrammar(Grammar* grammar_) { grammar = grammar_; }
    Parser* Definition() const { return definition; }
    void SetDefinition(Parser* definition_) ;
    const AttrOrVariableVec& InheritedAttributes() const { return inheritedAttributes; }
    void AddInheritedAttribute(const AttrOrVariable& attr) { inheritedAttributes.push_back(attr); }
    const AttrOrVariableVec& LocalVariables() const { return localVariables; }
    void AddLocalVariable(const AttrOrVariable& var) { localVariables.push_back(var); }
    const std::string& ValueTypeName() const { return valueTypeName; }
    void SetValueTypeName(const std::string& valueTypeName_) { valueTypeName = valueTypeName_; }
    int NumberOfParameters() const { return int(inheritedAttributes.size()); }
    const ActionVec& Actions() const { return actions; }
    void AddAction(ActionParser* action);
    ActionParser* GetAction(const std::string& actionName) const;
    const NonterminalVec& Nonterminals() const { return nonterminals; }
    void AddNonterminal(NonterminalParser* nonterminal);
    NonterminalParser* GetNonterminal(const std::string& nonterminalName) const;
    bool TakesOrReturnsValuesOrHasLocals() const { return !inheritedAttributes.empty() || !localVariables.empty() || !valueTypeName.empty(); }
    bool Specialized() const { return TakesOrReturnsValuesOrHasLocals() || !actions.empty(); }
    std::string SpecializedTypeName() const { return Specialized() ? Name() + "Rule" : "Rule"; }
    virtual void Link() {}
    void ExpandCode();
    virtual Match Parse(Scanner& scanner, ObjectStack& stack);
    virtual void Enter(ObjectStack& stack) {}
    virtual void Leave(ObjectStack& stack, bool matched) {}
    virtual void Accept(Visitor& visitor);
    void Synchronize(const std::string& synchronizeCharacters_);
    bool IsSynchronizingRule() const { return !synchronizeCharacters.empty(); }
    const std::string& GetSynchronizeCharacters() const { return synchronizeCharacters; }
private:
    Grammar* grammar;
    Parser* definition;
    AttrOrVariableVec inheritedAttributes;
    AttrOrVariableVec localVariables;
    std::string valueTypeName;
    ActionVec actions;
    NonterminalVec nonterminals;
    std::string synchronizeCharacters;
};

class RuleLink: public ParsingObject
{
public:
    RuleLink(const std::string& name_, Grammar* grammar_, const std::string& linkedRuleName_);
    RuleLink(Grammar* grammar_, const std::string& linkedRuleName_);
    virtual bool IsRuleLink() const { return true; }
    virtual void Accept(Visitor& visitor);
    virtual void AddToScope();
    const std::string& LinkedRuleName() const { return linkedRuleName; }
    void SetRule(Rule* rule_) { rule = rule_; }
    Rule* GetRule() const { return rule; }
private:
    std::string linkedRuleName;
    Grammar* grammar;
    Rule* rule;
};

} } // namespace Cm::Parsing

#endif // CM_PARSING_RULE_INCLUDED


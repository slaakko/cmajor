/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef CM_PARSING_PARSINGDOMAIN_INCLUDED
#define CM_PARSING_PARSINGDOMAIN_INCLUDED
#include <Cm.Parsing/ParsingObject.hpp>
#include <string>
#include <stack>
#include <unordered_map>

namespace Cm { namespace Parsing {

class Scope;
class Grammar;
class Namespace;

class ParsingDomain: public ParsingObject
{
public:
    ParsingDomain();
    Scope* GetNamespaceScope(const std::string& fullNamespaceName);
    Grammar* GetGrammar(const std::string& grammarName);
    void AddGrammar(Grammar* grammar);
    void BeginNamespace(const std::string& ns);
    void EndNamespace();
    Namespace* GlobalNamespace() const { return globalNamespace; }
    Namespace* CurrentNamespace() const { return currentNamespace; }
    Scope* CurrentScope() const;
    virtual void Accept(Visitor& visitor);
private:
    typedef std::unordered_map<std::string, Grammar*> GrammarMap;
    typedef GrammarMap::const_iterator GrammarMapIt;
    GrammarMap grammarMap;
    Namespace* globalNamespace;
    Scope* globalScope;
    typedef std::unordered_map<std::string, Namespace*> NamespaceMap;
    typedef NamespaceMap::const_iterator NamespaceMapIt;
    NamespaceMap namespaceMap;
    std::stack<Namespace*> namespaceStack;
    Namespace* currentNamespace;
};

void RegisterParsingDomain(ParsingDomain* parsingDomain);
void ParsingDomainInit();
void ParsingDomainDone();

} } // namespace Cm::Parsing

#endif // CM_PARSING_PARSINGDOMAIN_INCLUDED

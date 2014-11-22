/*========================================================================
    Copyright (c) 2011-2013 Seppo Laakko
    http://sourceforge.net/projects/soulparsing/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Parsing.Syntax/ParserFileContent.hpp>
#include <Cm.Parsing/Grammar.hpp>

namespace Cm { namespace Parsing { namespace Syntax {

using namespace Cm::Parsing;

ParserFileContent::ParserFileContent(int id_, Cm::Parsing::ParsingDomain* parsingDomain_): id(id_), parsingDomain(parsingDomain_)
{
}

void ParserFileContent::AddIncludeDirective(const std::string& includeDirective_, const std::string& fileAttribute_)
{
    includeDirectives.push_back(std::make_pair(includeDirective_, fileAttribute_));
}

void ParserFileContent::AddUsingObject(Cm::Parsing::CppObjectModel::UsingObject* usingObjectSubject)
{
    Cm::Parsing::Scope* enclosingScope = parsingDomain->CurrentScope();
    UsingObject* usingObject(new UsingObject(usingObjectSubject, enclosingScope));
    usingObject->SetOwner(id);
    usingObjects.push_back(std::unique_ptr<UsingObject>(usingObject));
}

void ParserFileContent::AddGrammar(Cm::Parsing::Grammar* grammar)
{
    grammar->SetOwner(id);
    parsingDomain->AddGrammar(grammar);
}

void ParserFileContent::BeginNamespace(const std::string& ns)
{
    parsingDomain->BeginNamespace(ns);
}

void ParserFileContent::EndNamespace()
{
    parsingDomain->EndNamespace();
}

} } } // namespace Cm::Parsing::Syntax

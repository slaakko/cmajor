/*========================================================================
    Copyright (c) 2011-2013 Seppo Laakko
    http://sourceforge.net/projects/soulparsing/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_PARSING_SYNTAX_PARSERFILECONTENT_INCLUDED
#define CM_PARSING_SYNTAX_PARSERFILECONTENT_INCLUDED

#include <Cm.Parsing/ParsingDomain.hpp>
#include <Cm.Parsing/Namespace.hpp>

namespace Cm { namespace Parsing { namespace Syntax {

class ParserFileContent
{
public:
    ParserFileContent(int id_, Cm::Parsing::ParsingDomain* parsingDomain_);
    void SetFilePath(const std::string& filePath_) { filePath = filePath_; }
    void AddIncludeDirective(const std::string& includeDirective_, const std::string& fileAttribute_);
    void AddUsingObject(Cm::Parsing::CppObjectModel::UsingObject* usingObjectSubject);
    void AddGrammar(Cm::Parsing::Grammar* grammar);
    void BeginNamespace(const std::string& ns);
    void EndNamespace();
    int Id() const { return id; }
    const std::string& FilePath() const { return filePath; }
    Cm::Parsing::ParsingDomain* ParsingDomain() const { return parsingDomain; }
    const std::vector<std::pair<std::string, std::string>>& IncludeDirectives() const { return includeDirectives; }
    const std::vector<std::unique_ptr<Cm::Parsing::UsingObject>>& UsingObjects() const { return usingObjects; }
    Cm::Parsing::Scope* CurrentScope() const { return parsingDomain->CurrentScope(); }
private:
    int id;
    std::string filePath;
    Cm::Parsing::ParsingDomain* parsingDomain;
    std::vector<std::pair<std::string, std::string>> includeDirectives;
    std::vector<std::unique_ptr<Cm::Parsing::UsingObject>> usingObjects;
};

} } } // namespace Cm::Parsing::Syntax

#endif // CM_PARSING_SYNTAX_PARSERFILECONTENT_INCLUDED

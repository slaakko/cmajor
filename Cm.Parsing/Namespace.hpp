/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef CM_PARSING_NAMESPACE_INCLUDED
#define CM_PARSING_NAMESPACE_INCLUDED
#include <Cm.Parsing/ParsingObject.hpp>
#include <Cm.Parsing.CppObjectModel/Declaration.hpp>

namespace Cm { namespace Parsing {

class Namespace: public ParsingObject
{
public:
    Namespace(const std::string& name_, Scope* enclosingScope_);
    virtual void Accept(Visitor& visitor);
    virtual bool IsNamespace() const { return true; }
};

class Scope;

class UsingObject: public ParsingObject
{
public:
    UsingObject(Cm::Parsing::CppObjectModel::UsingObject* subject_, Scope* enclosingScope_);
    virtual void Accept(Visitor& visitor);
    Cm::Parsing::CppObjectModel::UsingObject* Subject() const { return subject; }
private:
    Cm::Parsing::CppObjectModel::UsingObject* subject;
    std::unique_ptr<Cm::Parsing::CppObjectModel::UsingObject> ownedSubject;
};

} } // namespace Cm::Parsing

#endif // CM_PARSING_NAMESPACE_INCLUDED

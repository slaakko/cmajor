/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Cm.Parsing/ParsingObject.hpp>
#include <Cm.Parsing/Scope.hpp>
#include <Cm.Parsing/Exception.hpp>

namespace Cm { namespace Parsing {

ParsingObject::ParsingObject(const std::string& name_): name(name_), isOwned(false), owner(external), enclosingScope(nullptr)
{
}

ParsingObject::ParsingObject(const std::string& name_, Scope* enclosingScope_): name(name_), isOwned(false), owner(external), enclosingScope(enclosingScope_)
{
}

ParsingObject::~ParsingObject()
{
}

void SetEnclosingScope(Scope* enclosingScope_)
{
}


std::string ParsingObject::FullName() const
{
    std::string scopeName = "";
    if (enclosingScope)
    {
        scopeName = enclosingScope->FullName();
    }
    if (scopeName.empty())
    {
        return name;
    }
    return scopeName + "." + name;
}

void ParsingObject::Own(ParsingObject* object)
{
    if (object)
    {
        if (!object->IsOwned())
        {
            object->SetOwned();
            ownedObjects.push_back(std::unique_ptr<ParsingObject>(object));
        }
    }
}

void ParsingObject::SetScope(Scope* scope_)
{
    scope = scope_;
    Own(scope);
}

void ParsingObject::AddToScope()
{
    if (enclosingScope)
    {
        try
        {
            enclosingScope->Add(this);
        }
        catch (std::exception& ex)
        {
            ThrowException(ex.what(), span);
        }
        if (scope)
        {
            scope->SetEnclosingScope(enclosingScope);
        }
    }

}

} } // namespace Cm::Parsing


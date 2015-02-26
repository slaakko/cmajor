/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Core/FunctionTemplateRepository.hpp>
#include <Cm.Sym/TypeSymbol.hpp>

namespace Cm { namespace Core {

FunctionTemplateKey::FunctionTemplateKey(Cm::Sym::FunctionSymbol* subject_, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments_): subject(subject_), templateArguments(templateArguments_),
    hashCodeValid(false)
{
    if (subject->Name().find("ConstructiveMove") != std::string::npos)
    {
        int x = 0;
    }
}

size_t FunctionTemplateKey::GetHashCode() const
{
    if (!hashCodeValid)
    {
        const_cast<FunctionTemplateKey*>(this)->ComputeHashCode();
    }
    return hashCode;
}

void FunctionTemplateKey::ComputeHashCode()
{
    hashCode = std::hash<Cm::Sym::FunctionSymbol*>()(subject);
    for (Cm::Sym::TypeSymbol* templateArgument : templateArguments)
    {
        size_t templateArgHashCode = templateArgument->Id().GetHashCode();
        hashCode ^= templateArgHashCode;
    }
    hashCodeValid = true;
}

bool operator==(const FunctionTemplateKey& left, const FunctionTemplateKey& right)
{
    return left.Subject() == right.Subject() && left.TemplateArguments() == right.TemplateArguments();
}

Cm::Sym::FunctionSymbol* FunctionTemplateRepository::GetFunctionTemplateInstance(const FunctionTemplateKey& key) const
{
    FunctionTemplateMapIt i = functionTemplateMap.find(key);
    if (i != functionTemplateMap.end())
    {
        return i->second;
    }
    return nullptr;
}

void FunctionTemplateRepository::AddFunctionTemplateInstance(const FunctionTemplateKey& key, Cm::Sym::FunctionSymbol* functionTemplateInstance)
{
    functionTemplateMap[key] = functionTemplateInstance;
}

} } // namespace Cm::Core
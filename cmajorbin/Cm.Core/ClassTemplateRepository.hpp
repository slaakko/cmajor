/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_CLASS_TEMPLATE_REPOSITORY_INCLUDED
#define CM_CORE_CLASS_TEMPLATE_REPOSITORY_INCLUDED
#include <Cm.Core/Argument.hpp>
#include <Cm.Core/Exception.hpp>

namespace Cm { namespace Core {

// implementation provided by Cm::Bind::ClassTemplateRepository

class ClassTemplateRepository
{
public:
    virtual ~ClassTemplateRepository();
    virtual void CollectViableFunctions(const std::string& groupName, int arity, const std::vector<Argument>& arguments, const Cm::Parsing::Span& span, Cm::Sym::ContainerScope* containerScope,
        std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) = 0;
    virtual void Instantiate(Cm::Sym::ContainerScope* containerScope, Cm::Sym::FunctionSymbol* memberFunctionSymbol) = 0;
    virtual void BindTemplateTypeSymbol(Cm::Sym::TemplateTypeSymbol* templateTypeSymbol, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes) = 0;
    virtual void InstantiateVirtualFunctionsFor(Cm::Sym::ContainerScope* containerScope, Cm::Sym::ClassTypeSymbol* templateTypeSymbol) = 0;
    virtual void ResolveDefaultTypeArguments(std::vector<Cm::Sym::TypeSymbol*>& typeArguments, Cm::Sym::ClassTypeSymbol* subjectClassTypeSymbol, 
        Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, 
        const Cm::Parsing::Span& span) = 0;
    virtual void Write(Cm::Sym::BcuWriter& writer) = 0;
    virtual void Read(Cm::Sym::BcuReader& reader) = 0;
};

} } // namespace Cm::Core

#endif // CM_CORE_CLASS_TEMPLATE_REPOSITORY_INCLUDED

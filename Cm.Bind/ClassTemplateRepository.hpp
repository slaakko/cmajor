/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_CLASS_TEMPLATE_REPOSITORY_INCLUDED
#define CM_BIND_CLASS_TEMPLATE_REPOSITORY_INCLUDED
#include <Cm.Core/ClassTemplateRepository.hpp>
#include <Cm.BoundTree/BoundCompileUnit.hpp>

namespace Cm { namespace Bind {

class ClassTemplateRepository : public Cm::Core::ClassTemplateRepository
{
public:
    ClassTemplateRepository(Cm::BoundTree::BoundCompileUnit& boundCompileUnit_);
    ~ClassTemplateRepository();
    void CollectViableFunctions(const std::string& groupName, int arity, const std::vector<Cm::Core::Argument>& arguments, const Cm::Parsing::Span& span, Cm::Sym::ContainerScope* containerScope,
        std::unordered_set<Cm::Sym::FunctionSymbol*>& viableFunctions) override;
    void Instantiate(Cm::Sym::ContainerScope* containerScope, Cm::Sym::FunctionSymbol* memberFunctionSymbol) override;
    void BindTemplateTypeSymbol(Cm::Sym::TemplateTypeSymbol* templateTypeSymbol, Cm::Sym::ContainerScope* containerScope, const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes) override;
    void InstantiateVirtualFunctionsFor(Cm::Sym::ContainerScope* containerScope, Cm::Sym::ClassTypeSymbol* templateTypeSymbol) override;
    void ResolveDefaultTypeArguments(std::vector<Cm::Sym::TypeSymbol*>& typeArguments, Cm::Sym::ClassTypeSymbol* subjectClassTypeSymbol, Cm::Sym::ContainerScope* containerScope,
        const std::vector<std::unique_ptr<Cm::Sym::FileScope>>& fileScopes, const Cm::Parsing::Span& span) override;
    void Write(Cm::Sym::BcuWriter& writer) override;
    void Read(Cm::Sym::BcuReader& reader) override;
private:
    Cm::BoundTree::BoundCompileUnit& boundCompileUnit;
    typedef std::unordered_set<Cm::Sym::ClassTypeSymbol*> ClassTemplateSet;
    typedef ClassTemplateSet::const_iterator ClassTemplateSetIt;
    ClassTemplateSet classTemplates;
    ClassTemplateSet virtualFunctionsInstantiated;
    std::vector<Cm::Sym::FunctionSymbol*> memberFunctionSymbols;
    std::vector<std::unique_ptr<Cm::Sym::FunctionSymbol>> ownedMemberFunctionSymbols;
    std::unordered_set<Cm::Sym::TemplateTypeSymbol*> templateTypeSymbols;
};

} } // namespace Cm::Bind

#endif // CM_BIND_CLASS_TEMPLATE_REPOSITORY_INCLUDED

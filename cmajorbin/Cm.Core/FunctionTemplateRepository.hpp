/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_FUNCTION_TEMPLATE_REPOSITORY_INCLUDED
#define CM_CORE_FUNCTION_TEMPLATE_REPOSITORY_INCLUDED
#include <Cm.Sym/FunctionSymbol.hpp>

namespace Cm { namespace Core {

class FunctionTemplateKey
{
public:
    FunctionTemplateKey(Cm::Sym::FunctionSymbol* subject_, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments_);
    size_t GetHashCode() const;
    Cm::Sym::FunctionSymbol* Subject() const { return subject; }
    const std::vector<Cm::Sym::TypeSymbol*>& TemplateArguments() const { return templateArguments; }
private:
    Cm::Sym::FunctionSymbol* subject;
    std::vector<Cm::Sym::TypeSymbol*> templateArguments;
    bool hashCodeValid;
    size_t hashCode;
    void ComputeHashCode();
};

bool operator==(const FunctionTemplateKey& left, const FunctionTemplateKey& right);

struct FunctionTempalateKeyHash
{
    size_t operator()(const FunctionTemplateKey& key) const { return key.GetHashCode(); }
};

class FunctionTemplateRepository
{
public:
    FunctionTemplateRepository(Cm::Sym::SymbolTable& symbolTable_);
    Cm::Sym::FunctionSymbol* GetFunctionTemplateInstance(const FunctionTemplateKey& key) const;
    void AddFunctionTemplateInstance(const FunctionTemplateKey& key, Cm::Sym::FunctionSymbol* functionTemplateInstance);
    void Read(Cm::Sym::BcuReader& reader);
    void Write(Cm::Sym::BcuWriter& writer);
private:
    Cm::Sym::SymbolTable& symbolTable;
    typedef std::unordered_map<FunctionTemplateKey, Cm::Sym::FunctionSymbol*, FunctionTempalateKeyHash> FunctionTemplateMap;
    typedef FunctionTemplateMap::const_iterator FunctionTemplateMapIt;
    FunctionTemplateMap functionTemplateMap;
    std::vector<std::unique_ptr<Cm::Sym::FunctionSymbol>> functionSymbols;
};

} } // namespace Cm::Core

#endif // CM_CORE_FUNCTION_TEMPLATE_REPOSITORY_INCLUDED

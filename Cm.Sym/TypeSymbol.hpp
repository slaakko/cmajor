/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_TYPE_SYMBOL_INCLUDED
#define CM_SYM_TYPE_SYMBOL_INCLUDED
#include <Cm.Sym/ContainerSymbol.hpp>
#include <Cm.Sym/TypeId.hpp>

namespace Cm { namespace Sym {

class TypeSymbol : public ContainerSymbol
{
public:
    TypeSymbol(const Span& span_, const std::string& name_);
    TypeSymbol(const Span& span_, const std::string& name_, const TypeId& id_);
    SymbolType GetSymbolType() const override { return SymbolType::typeSymbol; }
    bool IsExportSymbol() const override { return Source() == SymbolSource::project; }
    bool IsTypeSymbol() const override { return true; }
    void Write(Writer& writer) override;
    void Read(Reader& reader) override;
    const TypeId& Id() const { return id; }
    void SetId(const TypeId& id_) { id = id_;  }
    bool IsDerivedTypeSymbol() const { return id.Derivations().NumDerivations() > 0; }
    TypeSymbol* GetBaseType(const SymbolTable& symbolTable) const;
private:
    TypeId id;
};

inline bool TypesEqual(TypeSymbol* left, TypeSymbol* right)
{ 
    return left->Id() == right->Id();
}

} } // namespace Cm::Sym

#endif // CM_SYM_TYPE_SYMBOL_INCLUDED

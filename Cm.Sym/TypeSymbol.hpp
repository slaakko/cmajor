/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_TYPE_SYMBOL_INCLUDED
#define CM_SYM_TYPE_SYMBOL_INCLUDED
#include <Cm.Sym/ContainerSymbol.hpp>
#include <Cm.Util/Uuid.hpp>

namespace Cm { namespace Sym {

class TypeSymbol : public ContainerSymbol
{
public:
    TypeSymbol(const Span& span_, const std::string& name_);
    TypeSymbol(const Span& span_, const std::string& name_, const Cm::Util::Uuid& id_);
    const Cm::Util::Uuid& Id() const { return id; }
    void SetId(Cm::Util::Uuid& id_) { id = id_;  }
    virtual TypeSymbol* GetBaseType() const { return const_cast<TypeSymbol*>(this); }
    virtual bool Equals(TypeSymbol* that) const { return id == that->Id();  }
    virtual bool IsBasicTypeSymbol() const { return false; }
    virtual bool IsDerivedTypeSymbol() const { return false; }
    virtual bool IsBoolTypeSymbol() const { return false; }
    virtual bool IsIntegerTypeSymbol() const { return false; }
    virtual bool IsFloatingPointTypeSymbol() const { return false; }
    virtual bool IsCharTypeSymbol() const { return false; }
    virtual TypeSymbol* GetUnderlyingType() const { return const_cast<TypeSymbol*>(this); }
private:
    Cm::Util::Uuid id;
};

inline bool TypesEqual(TypeSymbol* left, TypeSymbol* right)
{ 
    return left->Equals(right);
}

} } // namespace Cm::Sym

#endif // CM_SYM_TYPE_SYMBOL_INCLUDED

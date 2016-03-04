/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_IR_INTERFACE_TYPE_REPOSITORY_INCLUDED
#define CM_CORE_IR_INTERFACE_TYPE_REPOSITORY_INCLUDED
#include <Cm.Sym/InterfaceTypeSymbol.hpp>

namespace Cm { namespace Core {

class IrInterfaceTypeRepository
{
public:
    void Add(Cm::Sym::InterfaceTypeSymbol* interfaceType);
    virtual ~IrInterfaceTypeRepository();
    virtual void Write(Cm::Util::CodeFormatter& formatter) = 0;
    const std::unordered_set<Cm::Sym::InterfaceTypeSymbol*>& InterfaceTypes() const { return interfaceTypes; }
private:
    std::unordered_set<Cm::Sym::InterfaceTypeSymbol*> interfaceTypes;
};

class LlvmIrInterfaceTypeRepository : public IrInterfaceTypeRepository
{
public:
    void Write(Cm::Util::CodeFormatter& formatter) override;
};

class CIrInterfaceTypeRepository : public IrInterfaceTypeRepository
{
public:
    void Write(Cm::Util::CodeFormatter& formatter) override;
};

} } // namespace Cm::Core

#endif // CM_CORE_IR_INTERFACE_TYPE_REPOSITORY_INCLUDED

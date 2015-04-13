/*========================================================================
    Copyright (c) 2012-2014 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef C_IR_FACTORY_INCLUDED
#define C_IR_FACTORY_INCLUDED
#include <Ir.Intf/Factory.hpp>
#include <memory>

namespace C {

class Factory : public Ir::Intf::Factory
{
public:
    Factory();
    Ir::Intf::Type* GetLabelType() override;
    Ir::Intf::Type* GetMetadataType() override;
    Ir::Intf::Type* GetI1() override;
    Ir::Intf::Type* GetI8() override;
    Ir::Intf::Type* GetUI8() override;
    Ir::Intf::Type* GetI16() override;
    Ir::Intf::Type* GetUI16() override;
    Ir::Intf::Type* GetI32() override;
    Ir::Intf::Type* GetUI32() override;
    Ir::Intf::Type* GetI64() override;
    Ir::Intf::Type* GetUI64() override;
    Ir::Intf::Type* GetFloat() override;
    Ir::Intf::Type* GetDouble() override;
    Ir::Intf::Type* GetVoid() override;
private:
    std::unique_ptr<Ir::Intf::Type> labelType;
    std::unique_ptr<Ir::Intf::Type> metadataType;
    std::unique_ptr<Ir::Intf::Type> i1;
    std::unique_ptr<Ir::Intf::Type> i8;
    std::unique_ptr<Ir::Intf::Type> ui8;
    std::unique_ptr<Ir::Intf::Type> i16;
    std::unique_ptr<Ir::Intf::Type> ui16;
    std::unique_ptr<Ir::Intf::Type> i32;
    std::unique_ptr<Ir::Intf::Type> ui32;
    std::unique_ptr<Ir::Intf::Type> i64;
    std::unique_ptr<Ir::Intf::Type> ui64;
    std::unique_ptr<Ir::Intf::Type> float_;
    std::unique_ptr<Ir::Intf::Type> double_;
    std::unique_ptr<Ir::Intf::Type> void_;
};

} // namespace C

#endif // C_IR_FACTORY_INCLUDED

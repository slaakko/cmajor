/*========================================================================
    Copyright (c) 2012-2014 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef LLVM_IR_FACTORY_INCLUDED
#define LLVM_IR_FACTORY_INCLUDED
#include <Ir.Intf/Factory.hpp>
#include <memory>

namespace Llvm { 

class Factory: public Ir::Intf::Factory
{
public:
    Factory();
    Ir::Intf::Type* GetLabelType() override;
    Ir::Intf::Type* GetMetadataType() override;
    Ir::Intf::Type* GetI1() override;
    Ir::Intf::Type* GetI8() override;
    Ir::Intf::Type* GetI16() override;
    Ir::Intf::Type* GetI32() override;
    Ir::Intf::Type* GetI64() override;
    Ir::Intf::Type* GetFloat() override;
    Ir::Intf::Type* GetDouble() override;
private:
    std::unique_ptr<Ir::Intf::Type> labelType;
    std::unique_ptr<Ir::Intf::Type> metadataType;
    std::unique_ptr<Ir::Intf::Type> i1;
    std::unique_ptr<Ir::Intf::Type> i8;
    std::unique_ptr<Ir::Intf::Type> i16;
    std::unique_ptr<Ir::Intf::Type> i32;
    std::unique_ptr<Ir::Intf::Type> i64;
    std::unique_ptr<Ir::Intf::Type> float_;
    std::unique_ptr<Ir::Intf::Type> double_;
};

} // namespace Llvm

#endif // LLVM_IR_FACTORY_INCLUDED

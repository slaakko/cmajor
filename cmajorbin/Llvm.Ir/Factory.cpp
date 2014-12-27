/*========================================================================
    Copyright (c) 2012-2014 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Llvm.Ir/Factory.hpp>
#include <Llvm.Ir/Type.hpp>

namespace Llvm { 

Factory::Factory(): labelType(Label()), metadataType(Metadata()), i1(I1()), i8(I8()), i16(I16()), i32(I32()), i64(I64()), float_(Float()), double_(Double()), void_(Void())
{
    labelType->SetOwned();
    metadataType->SetOwned();
    i1->SetOwned();
    i8->SetOwned();
    i16->SetOwned();
    i32->SetOwned();
    i64->SetOwned();
    float_->SetOwned();
    double_->SetOwned();
    void_->SetOwned();
}

Ir::Intf::Type* Factory::GetLabelType()
{
    return labelType.get();
}

Ir::Intf::Type* Factory::GetMetadataType()
{
    return metadataType.get();
}

Ir::Intf::Type* Factory::GetI1()
{
    return i1.get();
}

Ir::Intf::Type* Factory::GetI8()
{
    return i8.get();
}

Ir::Intf::Type* Factory::GetI16()
{
    return i16.get();
}

Ir::Intf::Type* Factory::GetI32()
{
    return i32.get();
}

Ir::Intf::Type* Factory::GetI64()
{
    return i64.get();
}

Ir::Intf::Type* Factory::GetFloat()
{
    return float_.get();
}

Ir::Intf::Type* Factory::GetDouble()
{
    return double_.get();
}

Ir::Intf::Type* Factory::GetVoid()
{
    return void_.get();
}

} // namespace Llvm

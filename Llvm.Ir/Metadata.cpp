/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#include <Llvm.Ir/Metadata.hpp>
#include <Llvm.Ir/Type.hpp>
#include <Llvm.Ir/Constant.hpp>
#include <Llvm.Ir/Factory.hpp>
#include <stdexcept>

namespace Llvm { 

MetadataNull::MetadataNull(): Ir::Intf::MetadataNull()
{
}

void MetadataNull::InitTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to)
{
    throw std::runtime_error("cannot init metadata null");
}

void MetadataNull::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant)
{
    throw std::runtime_error("cannot init metadata null");
}

void MetadataNull::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global)
{
    throw std::runtime_error("cannot init metadata null");
}

void MetadataNull::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    throw std::runtime_error("cannot init metadata null");
}

void MetadataNull::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar)
{
    throw std::runtime_error("cannot init metadata null");
}

void MetadataNull::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar)
{
    throw std::runtime_error("cannot init metadata null");
}

void MetadataNull::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar)
{
    throw std::runtime_error("cannot init metadata null");
}

void MetadataNull::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter)
{
    throw std::runtime_error("cannot init metadata null");
}

void MetadataNull::AssignTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to)
{
    throw std::runtime_error("cannot assign metadata null");
}

void MetadataNull::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant)
{
    throw std::runtime_error("cannot assign metadata null");
}

void MetadataNull::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    throw std::runtime_error("cannot assign metadata null");
}

void MetadataNull::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global)
{
    throw std::runtime_error("cannot assign metadata null");
}

void MetadataNull::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar)
{
    throw std::runtime_error("cannot assign metadata null");
}

void MetadataNull::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar)
{
    throw std::runtime_error("cannot assign metadata null");
}

void MetadataNull::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar)
{
    throw std::runtime_error("cannot assign metadata null");
}

void MetadataNull::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter)
{
    throw std::runtime_error("cannot assign metadata null");
}

std::string MakeMetadataStringName(const std::string& value)
{
    return "!" + MakeStringConstantName(value, true);
}

MetadataString::MetadataString(const std::string& value_): Ir::Intf::MetadataString(MakeMetadataStringName(value_), Ir::Intf::GetFactory()->GetMetadataType())
{
}

void MetadataString::InitTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to)
{
    throw std::runtime_error("cannot init metadata strings");
}

void MetadataString::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant)
{
    throw std::runtime_error("cannot init metadata strings");
}

void MetadataString::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global)
{
    throw std::runtime_error("cannot init metadata strings");
}

void MetadataString::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    throw std::runtime_error("cannot init metadata strings");
}

void MetadataString::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar)
{
    throw std::runtime_error("cannot init metadata strings");
}

void MetadataString::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar)
{
    throw std::runtime_error("cannot init metadata strings");
}

void MetadataString::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar)
{
    throw std::runtime_error("cannot init metadata strings");
}

void MetadataString::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter)
{
    throw std::runtime_error("cannot init metadata strings");
}

void MetadataString::AssignTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to)
{
    throw std::runtime_error("cannot assign metadata strings");
}

void MetadataString::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant)
{
    throw std::runtime_error("cannot assign metadata strings");
}

void MetadataString::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    throw std::runtime_error("cannot assign metadata strings");
}

void MetadataString::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global)
{
    throw std::runtime_error("cannot assign metadata strings");
}

void MetadataString::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar)
{
    throw std::runtime_error("cannot assign metadata strings");
}

void MetadataString::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar)
{
    throw std::runtime_error("cannot assign metadata strings");
}

void MetadataString::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar)
{
    throw std::runtime_error("cannot assign metadata strings");
}

void MetadataString::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter)
{
    throw std::runtime_error("cannot assign metadata strings");
}

MetadataNode::MetadataNode(int id_): Ir::Intf::MetadataNode(id_, Ir::Intf::GetFactory()->GetMetadataType())
{
}

void MetadataNode::InitTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to)
{
    throw std::runtime_error("cannot init metadata nodes");
}

void MetadataNode::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant)
{
    throw std::runtime_error("cannot init metadata nodes");
}

void MetadataNode::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global)
{
    throw std::runtime_error("cannot init metadata nodes");
}

void MetadataNode::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    throw std::runtime_error("cannot init metadata nodes");
}

void MetadataNode::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar)
{
    throw std::runtime_error("cannot init metadata nodes");
}

void MetadataNode::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar)
{
    throw std::runtime_error("cannot init metadata nodes");
}

void MetadataNode::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar)
{
    throw std::runtime_error("cannot init metadata nodes");
}

void MetadataNode::InitFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter)
{
    throw std::runtime_error("cannot init metadata nodes");
}

void MetadataNode::AssignTo(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Object* to)
{
    throw std::runtime_error("cannot assign metadata nodes");
}

void MetadataNode::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Constant& constant)
{
    throw std::runtime_error("cannot assign metadata nodes");
}

void MetadataNode::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RegVar& regVar)
{
    throw std::runtime_error("cannot assign metadata nodes");
}

void MetadataNode::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Global& global)
{
    throw std::runtime_error("cannot assign metadata nodes");
}

void MetadataNode::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::StackVar& stackVar)
{
    throw std::runtime_error("cannot assign metadata nodes");
}

void MetadataNode::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::MemberVar& memberVar)
{
    throw std::runtime_error("cannot assign metadata nodes");
}

void MetadataNode::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::RefVar& refVar)
{
    throw std::runtime_error("cannot assign metadata nodes");
}

void MetadataNode::AssignFrom(Ir::Intf::Emitter& emitter, Ir::Intf::Type* type, Ir::Intf::Parameter& parameter)
{
    throw std::runtime_error("cannot assign metadata nodes");
}

} // namespace Llvm

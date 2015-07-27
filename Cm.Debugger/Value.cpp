/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Debugger/Value.hpp>
#include <Cm.Debugger/Type.hpp>
#include <Cm.Util/TextUtils.hpp>
#include <sstream>

namespace Cm { namespace Debugger {

Value::~Value()
{
}

ErrorValue::ErrorValue(const std::string& errorMessage_) : errorMessage(errorMessage_)
{
}

std::string ErrorValue::ToString() const
{
    return "<error: " + errorMessage + ">";
}

FloatingValue::FloatingValue(double value_) : value(value_)
{
}

std::string FloatingValue::ToString() const
{
    return std::to_string(value);
}

IntegerValue::IntegerValue(uint64_t value_, bool neg_) : value(value_), neg(neg_)
{
}

std::string IntegerValue::ToString() const
{
    return (neg ? "-" : "") + std::to_string(value);
}

Value* MakeIntegerValue(int64_t value)
{
    if (value < 0)
    {
        return new IntegerValue(-value, true);
    }
    return new IntegerValue(value, false);
}

StringValue::StringValue(const std::string& value_) : value(value_)
{
}

std::string StringValue::ToString() const
{
    return "\"" + Cm::Util::StringStr(value) + "\"";
}

AddressValue::AddressValue(uint64_t address_) : address(address_)
{
}

std::string AddressValue::ToString() const
{
    if (address == 0)
    {
        return "null";
    }
    else
    {
        std::stringstream s;
        s << "0x" << std::hex << address;
        return s.str();
    }
}

bool AddressValue::HasSubItems() const 
{
    if (address == 0) return false;
    return true;
}

CharValue::CharValue(char value_) : value(value_)
{
}

std::string CharValue::ToString() const
{
    return "'" + Cm::Util::CharStr(value) + "'";
}

BoolValue::BoolValue(bool value_) : value(value_)
{
}

std::string BoolValue::ToString() const
{
    return value ? "true" : "false";
}

Field::Field() : name(), value()
{
}

Field::Field(const std::string& name_, Value* value_) : name(name_), value(value_)
{
}

std::string Field::ToString() const
{
    return name + "=" + value->ToString();
}

void StructureValue::AddField(Field&& field)
{
    fields.push_back(std::move(field));
}

std::string StructureValue::ToString() const
{
    std::string s = "{ ";
    bool first = true;
    for (const Field& field : fields)
    {
        if (!first)
        {
            s.append(", ");
        }
        if (field.Name() == "__base")
        {
            if (first)
            {
                first = false;
            }
            s.append(field.GetValue()->ToString());        
        }
        else if (field.Name() != "__dummy" && field.Name() != "__vptr")
        {
            if (first)
            {
                first = false;
            }
            s.append(field.ToString());
        }
    }
    s.append(" }");
    return s;
}

bool StructureValue::HasSubItems() const
{
    if (fields.empty()) return false;
    if (fields.size() == 1 && (fields[0].Name() == "__dummy" || fields[0].Name() == "__vptr")) return false;
    return true;
}

Value* StructureValue::GetFieldValue(const std::string& fieldName) const
{
    for (const Field& field : fields)
    {
        if (field.Name() == fieldName) return field.GetValue();
    }
    return nullptr;
}

std::string ArrayValue::ToString() const 
{
    std::string s = "{ ";
    bool first = true;
    for (const std::unique_ptr<Value>& item : items)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            s.append(", ");
        }
        s.append(item->ToString());
    }
    s.append(" }");
    return s;
}

bool ArrayValue::HasSubItems() const
{
    return !items.empty();
}

void ArrayValue::AddItem(Value* item)
{
    items.push_back(std::unique_ptr<Value>(item));
}

Result::Result(const std::string& name_, int handle_) : name(name_), handle(handle_)
{
}

void Result::SetValue(Value* value_)
{
    value.reset(value_);
}

void Result::SetType(const std::string& type_)
{
    type = type_;
}

void Result::SetDisplayType(const std::string& displayType_)
{
    displayType = displayType_;
}

} } // namespace Cm::Debugger

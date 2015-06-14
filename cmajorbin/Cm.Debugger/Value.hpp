/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_DEBUGGER_VALUE_INCLUDED
#define CM_DEBUGGER_VALUE_INCLUDED
#include <string>
#include <memory>
#include <vector>

namespace Cm { namespace Debugger {

class Value
{
public:
    virtual ~Value();
    virtual std::string ToString() const = 0;
    virtual bool HasSubItems() const { return false; }
    virtual bool IsStringValue() const { return false; }
    virtual bool IsIntegerValue() const { return false; }
    virtual bool IsStructureValue() const { return false; }
};

class ErrorValue : public Value
{
public:
    ErrorValue(const std::string& errorMessage_);
    std::string ToString() const override;
private:
    std::string errorMessage;
};

class StringValue : public Value
{
public:
    StringValue(const std::string& value_);
    std::string ToString() const override;
    bool IsStringValue() const override { return true; }
    const std::string& Value() const { return value; }
private:
    std::string value;
};

class AddressValue : public Value
{
public:
    AddressValue(uint64_t address_);
    std::string ToString() const override;
    bool HasSubItems() const override;
private:
    uint64_t address;
};

class CharValue : public Value
{
public:
    CharValue(char value_);
    std::string ToString() const override;
private:
    char value;
};

class BoolValue : public Value
{
public:
    BoolValue(bool value_);
    std::string ToString() const override;
private:
    bool value;
};

class FloatingValue : public Value
{
public:
    FloatingValue(double value_);
    std::string ToString() const override;
private:
    double value;
};

class IntegerValue : public Value
{
public:
    IntegerValue(uint64_t value_, bool neg_);
    std::string ToString() const override;
    bool IsIntegerValue() const override { return true; }
    bool IsNegative() const { return neg; }
    uint64_t AbsoluteValue() const { return value; }
private:
    uint64_t value;
    bool neg;
};

Value* MakeIntegerValue(int64_t value);

class Field
{
public:
    Field();
    Field(const std::string& name_, Value* value_);
    const std::string& Name() const { return name; }
    Value* GetValue() const { return value.get(); }
    std::string ToString() const;
private:
    std::string name;
    std::unique_ptr<Value> value;
};

class StructureValue : public Value
{
public:
    void AddField(Field&& field);
    std::string ToString() const override;
    bool HasSubItems() const override;
    bool IsStructureValue() const override { return true; }
    Value* GetFieldValue(const std::string& fieldName) const;
private:
    std::vector<Field> fields;
};

class Result
{
public:
    Result(const std::string& name_, int handle_);
    const std::string& Name() const { return name; }
    int Handle() const { return handle; }
    void SetValue(Value* value_);
    Value* GetValue() const { return value.get(); }
    void SetType(const std::string& type_);
    const std::string& Type() const { return type; }
    void SetDisplayType(const std::string& displayType_);
    const std::string& DisplayType() const { return displayType;  }
private:
    std::string name;
    int handle;
    std::unique_ptr<Value> value;
    std::string type;
    std::string displayType;
};

} } // namespace Cm::Debugger

#endif // CM_DEBUGGER_VALUE_INCLUDED

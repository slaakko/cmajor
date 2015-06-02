/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_CORE_JSON_INCLUDED
#define CM_CORE_JSON_INCLUDED
#include <string>
#include <memory>
#include <map>
#include <vector>

namespace Cm { namespace Core {

class JsonValue
{
public:
    virtual ~JsonValue();
    virtual std::string ToString() const = 0;
    virtual bool IsObject() const { return false; }
    virtual bool IsArray() const { return false; }
    virtual bool IsString() const { return false; }
    virtual bool IsNumber() const { return false; }
    virtual bool IsBool() const { return false; }
    virtual bool IsNull() const { return false; }
};

class JsonString : public JsonValue
{
public:
    bool IsString() const override { return true; }
    JsonString();
    JsonString(const std::string& value_);
    void Append(char c);
    void Append(const std::string& s);
    const std::string& Value() const { return value; }
    void SetValue(const std::string& value_) { value = value_; }
    std::string ToString() const override;
private:
    std::string value;
};

inline bool operator==(const JsonString& left, const JsonString& right)
{
    return left.Value() == right.Value();
}

inline bool operator<(const JsonString& left, const JsonString& right)
{
    return left.Value() < right.Value();
}

inline bool operator!=(const JsonString& left, const JsonString& right)
{
    return std::rel_ops::operator!=(left, right);
}

inline bool operator>(const JsonString& left, const JsonString& right)
{
    return std::rel_ops::operator>(left, right);
}

inline bool operator>=(const JsonString& left, const JsonString& right)
{
    return std::rel_ops::operator>=(left, right);
}

inline bool operator<=(const JsonString& left, const JsonString& right)
{
    return std::rel_ops::operator<=(left, right);
}

class JsonNumber : public JsonValue
{
public:
    JsonNumber();
    JsonNumber(double value_);
    bool IsNumber() const override { return true; }
    std::string ToString() const override;
    double Value() const { return value; }
    void SetValue(double value_) { value = value_; }
private:
    double value;
};

class JsonBool : public JsonValue
{
public:
    JsonBool();
    JsonBool(bool value_);
    bool IsBool() const override { return true; }
    std::string ToString() const override;
    bool Value() const { return value; }
    void SetValue(bool value_) { value = value_; }
private:
    bool value;
};

class JsonNull : public JsonValue
{
public:
    bool IsNull() const override { return true; }
    std::string ToString() const override;
};

class JsonObject : public JsonValue
{
public:
    JsonObject();
    bool IsObject() const override { return true; }
    void AddField(JsonString&& name, JsonValue* value);
    JsonValue* GetField(const JsonString& name) const;
    std::string ToString() const override;
private:
    std::map<JsonString, JsonValue*> fields;
    std::vector<std::unique_ptr<JsonValue>> values;
};

class JsonArray : public JsonValue
{
public:
    JsonArray();
    bool IsArray() const override { return true; }
    void AddItem(JsonValue* item);
    JsonValue* GetItem(int index) const { return items[index]; }
    JsonValue* operator[](int index) const { return items[index]; }
    int Count() const { return int(items.size()); }
    std::string ToString() const override;
private:
    std::vector<JsonValue*> items;
    std::vector<std::unique_ptr<JsonValue>> values;
};

} } // namespace Cm::Core

#endif // CM_CORE_JSON_INCLUDED

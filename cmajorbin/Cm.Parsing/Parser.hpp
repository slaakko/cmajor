/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef CM_PARSING_PARSER_INCLUDED
#define CM_PARSING_PARSER_INCLUDED
#include <Cm.Parsing/Match.hpp>
#include <Cm.Parsing/ParsingObject.hpp>
#include <stack>

namespace Cm { namespace Parsing {

class Object
{
public:
    virtual ~Object();
};

typedef std::stack<std::unique_ptr<Object>> ObjectStack;

template<typename ValueType>
class ValueObject: public Object
{
public:
    ValueObject(): value() {}
    ValueObject(const ValueType& value_): value(value_) {}
    ValueObject<ValueType>& operator=(const ValueType& v) { value = v; return *this; }
    operator ValueType() const { return value; }
private:
    ValueType value;
};

class Scanner;

class Parser: public ParsingObject
{
public:
    Parser(const std::string& name_, const std::string& info_);
    const std::string& Info() const { return info; }
    virtual Match Parse(Scanner& scanner, ObjectStack& stack) = 0;
private:
    std::string info;
};

} } // namespace Cm::Parsing

#endif // CM_PARSING_PARSER_INCLUDED


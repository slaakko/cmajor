/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/
 
    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

 ========================================================================*/

#ifndef BASIC_TYPE_INCLUDED
#define BASIC_TYPE_INCLUDED
#include <Cm.Ast/Node.hpp>

namespace Cm { namespace Ast {

class BoolNode: public Node
{
public:
    BoolNode();
    virtual std::ostream& Write(std::ostream& s);
};

class SByteNode: public Node
{
public:
    SByteNode();
    virtual std::ostream& Write(std::ostream& s);
};

class ByteNode: public Node
{
public:
    ByteNode();
    virtual std::ostream& Write(std::ostream& s);
};

class ShortNode: public Node
{
public:
    ShortNode();
    virtual std::ostream& Write(std::ostream& s);
};

class UShortNode: public Node
{
public:
    UShortNode();
    virtual std::ostream& Write(std::ostream& s);
};

class IntNode: public Node
{
public:
    IntNode();
    virtual std::ostream& Write(std::ostream& s);
};

class UIntNode: public Node
{
public:
    UIntNode();
    virtual std::ostream& Write(std::ostream& s);
};

class LongNode: public Node
{
public:
    LongNode();
    virtual std::ostream& Write(std::ostream& s);
};

class ULongNode: public Node
{
public:
    ULongNode();
    virtual std::ostream& Write(std::ostream& s);
};

class FloatNode: public Node
{
public:
    FloatNode();
    virtual std::ostream& Write(std::ostream& s);
};

class DoubleNode: public Node
{
public:
    DoubleNode();
    virtual std::ostream& Write(std::ostream& s);
};

class CharNode: public Node
{
public:
    CharNode();
    virtual std::ostream& Write(std::ostream& s);
};

class VoidNode: public Node
{
public:
    VoidNode();
    virtual std::ostream& Write(std::ostream& s);
};

} } // namespace Cm::Ast

#endif // BASIC_TYPE_INCLUDED

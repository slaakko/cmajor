/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_EXCEPTION_INCLUDED
#define CM_AST_EXCEPTION_INCLUDED
#include <Cm.Parsing/Scanner.hpp>
#include <string>
#include <stdexcept>

namespace Cm { namespace Ast {

class Exception : public std::runtime_error
{
public:
    Exception(const std::string& message_);
    virtual void AddReference(const Cm::Parsing::Span& span) = 0;
};

} } // namespace Cm.Ast

#endif // CM_AST_EXCEPTION_INCLUDED

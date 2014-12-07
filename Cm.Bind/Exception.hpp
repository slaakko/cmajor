/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_BIND_EXCEPTION_INCLUDED
#define CM_BIND_EXCEPTION_INCLUDED
#include <Cm.Ast/Node.hpp>
#include <stdexcept>

namespace Cm { namespace Bind {

class Exception : public std::runtime_error
{
public:
    Exception(const std::string& message_, Cm::Ast::Node* defNode_, Cm::Ast::Node* refNode_);
private:
    Cm::Ast::Node* defNode;
    Cm::Ast::Node* refNode;
};

} } // namespace Cm::Bind

#endif // CM_BIND_EXCEPTION_INCLUDED

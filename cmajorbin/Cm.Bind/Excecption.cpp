/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Exception.hpp>

namespace Cm { namespace Bind {

Exception::Exception(const std::string& message_, Cm::Ast::Node* defNode_, Cm::Ast::Node* refNode_) : std::runtime_error(message_), defNode(defNode_), refNode(refNode_)
{
}

} } // namespace Cm::Bind

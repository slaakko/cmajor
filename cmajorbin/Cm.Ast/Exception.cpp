/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Ast/Exception.hpp>

namespace Cm { namespace Ast {

Exception::Exception(const std::string& message_) : std::runtime_error(message_)
{
}

} } // namespace Cm.Ast

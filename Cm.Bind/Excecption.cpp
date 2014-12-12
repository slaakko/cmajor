/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Bind/Exception.hpp>
#include <Cm.Parser/Error.hpp>

namespace Cm { namespace Bind {

Exception::Exception(const std::string& message_) : std::runtime_error(message_), defined(), referenced()
{
}

Exception::Exception(const std::string& message_, const Span& defined_) : std::runtime_error(Cm::Parser::Expand(message_, defined_)), defined(defined_), referenced()
{
}

Exception::Exception(const std::string& message_, const Span& defined_, const Span& referenced_) : std::runtime_error(Cm::Parser::Expand(message_, defined_, referenced_)), defined(defined_), referenced(referenced_)
{
}

} } // namespace Cm::Bind

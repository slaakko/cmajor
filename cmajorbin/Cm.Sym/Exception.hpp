/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_EXCEPTION_INCLUDED
#define CM_SYM_EXCEPTION_INCLUDED
#include <Cm.Parsing/Scanner.hpp>
#include <stdexcept>

namespace Cm { namespace Sym {

using Cm::Parsing::Span;

class Exception : public std::runtime_error
{
public:
    Exception(const std::string& message_, const Span& defined_);
    Exception(const std::string& message_, const Span& defined_, const Span& referenced_);
private:
    Span defined;
    Span referenced;
};

} } // namespace Cm::Sym

#endif // CM_SYM_EXCEPTION_INCLUDED
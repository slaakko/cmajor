/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_EXCEPTION_INCLUDED
#define CM_SYM_EXCEPTION_INCLUDED
#include <Cm.Parsing/Scanner.hpp>
#include <Cm.Ast/Exception.hpp>

namespace Cm { namespace Sym {

using Cm::Parsing::Span;

class Exception : public Cm::Ast::Exception
{
public:
    Exception(const std::string& message_, const Span& defined_);
    Exception(const std::string& message_, const Span& defined_, const Span& referenced_);
    const Span& Defined() const { return defined; }
    const Span& Referenced() const { return referenced; }
    const std::string& Message() const { return message; }
    void AddReference(const Span& span) override;
private:
    std::string message;
    Span defined;
    Span referenced;
};

} } // namespace Cm::Sym

#endif // CM_SYM_EXCEPTION_INCLUDED

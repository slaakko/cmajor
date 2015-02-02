/*========================================================================
Copyright (c) 2012-2015 Seppo Laakko
http://sourceforge.net/projects/cmajor/

Distributed under the GNU General Public License, version 3 (GPLv3).
(See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/TemplateParameterSymbol.hpp>
#include <Cm.Ast/Identifier.hpp>

namespace Cm { namespace Sym {

TemplateParameterSymbol::TemplateParameterSymbol(const Span& span_, const std::string& name_) : TypeSymbol(span_, name_), index(-1)
{
}

void TemplateParameterSymbol::Write(Writer& writer)
{
    TypeSymbol::Write(writer);
}

void TemplateParameterSymbol::Read(Reader& reader) 
{
    TypeSymbol::Read(reader);
}


BoundTemplateParameterSymbol::BoundTemplateParameterSymbol(const Span& span_, const std::string& name_): Symbol(span_, name_), type(nullptr)
{
}

} } // namespace Cm::Sym
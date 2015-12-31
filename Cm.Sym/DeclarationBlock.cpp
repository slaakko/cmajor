/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/DeclarationBlock.hpp>

namespace Cm { namespace Sym {

DeclarationBlock::DeclarationBlock(const Span& span_, const std::string& name_) : ContainerSymbol(span_, name_)
{
}

bool DeclarationBlock::IsExportSymbol() const
{
    return false;
}

} } // namespace Cm::Sym

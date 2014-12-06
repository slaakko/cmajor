/*========================================================================
Copyright (c) 2012-2015 Seppo Laakko
http://sourceforge.net/projects/cmajor/

Distributed under the GNU General Public License, version 3 (GPLv3).
(See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_AST_RANK_INCLUDED
#define CM_AST_RANK_INCLUDED

namespace Cm { namespace Ast {

enum class Rank
{
    primary, postfix, prefix, multiplicative, additive, shift, relational, equality, bitAnd, bitXor, bitOr, conjunction, disjunction, implication, equivalence
};

} } // namespace Cm::Ast

#endif // CM_AST_RANK_INCLUDED

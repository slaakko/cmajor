/*========================================================================
Copyright (c) 2012-2015 Seppo Laakko
http://sourceforge.net/projects/cmajor/

Distributed under the GNU General Public License, version 3 (GPLv3).
(See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_PARSER_OPERATOR
#define CM_PARSER_OPERATOR
#include <stdint.h>

namespace Cm { namespace Parser {

enum class Operator : uint8_t
{
    or_, and_, eq, neq, less, greater, lessOrEq, greaterOrEq, plus, minus, mul, div, rem, not_, const_, typename_, addressOf, deref,
    bitOr, bitXor, bitAnd, shiftLeft, shiftRight, complement, preInc, preDec, postInc, postDec
};

} } // namespace Cm::Parser

#endif // CM_PARSER_OPERATOR

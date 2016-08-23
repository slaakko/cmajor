/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_META_INCLUDED
#define CM_SYM_META_INCLUDED

#include <Cm.Sym/Value.hpp>

namespace Cm { namespace Sym {

class IntrinsicFunction
{
public:
    virtual ~IntrinsicFunction();
    virtual bool IsTypePredicate() const { return false; }
    virtual int Arity() const = 0;
    virtual const char* GroupName() const = 0;
    virtual int NumberOfTypeParameters() const = 0;
    virtual void DoEvaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments) = 0;
    void Evaluate(Cm::Sym::EvaluationStack& stack, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments);
};

void MetaInit(SymbolTable& symbolTable);

} } // namespace Cm::Sym

#endif // CM_SYM_META_INCLUDED


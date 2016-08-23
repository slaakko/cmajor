/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_SYMBOL_TYPE_SET_INCLUDED
#define CM_SYM_SYMBOL_TYPE_SET_INCLUDED
#include <Cm.Sym/Symbol.hpp>
#include <stack>
#include <unordered_set>

namespace Cm { namespace Sym {

enum class SymbolTypeSetId : uint8_t
{
    lookupAllSymbols,
    lookupTypeSymbols,
    lookupTypeAndNamespaceSymbols,
    lookupTypeAndFunctionGroupSymbols,
    lookupDotSubjectSymbols,
    lookupContainerSymbols,
    lookupClassAndNamespaceSymbols,
    lookupClassSymbols,
    lookupInterfaceSymbols,
    lookupFunctionGroupAndMemberVariable,
    lookupLocalVariable,
    lookupMemberVariable,
    lookupParameter,
    lookupTypedef,
    lookupVariableAndParameter,
    lookupInvokeSubject,
    lookupSizeOfSubject,
    lookupCastSource,
    lookupArgumentSymbol,
    lookupTypenameSubject,
    lookupFunctionGroup,
    lookupNamespace,
    lookupTypeAndConceptSymbols,
    lookupConceptGroup,
    lookupConstant,
    lookupDelegate,
    lookupClassDelegate,
    lookupEnumType,
    lookupEnumConstant,
    lookupConstantAndEnumConstantSymbols,
    lookupConstantEnumConstantAndVariableValueSymbols,
    max 
};

class LookupIdStack
{
public:
    void Push(SymbolTypeSetId id) { stack.push(id); }
    SymbolTypeSetId Pop() { SymbolTypeSetId id = stack.top(); stack.pop(); return id; }
private:
    std::stack<SymbolTypeSetId> stack;
};

class SymbolTypeSetCollection
{
public:
    SymbolTypeSetCollection();
    SymbolTypeSet& GetSymbolTypeSet(SymbolTypeSetId id);
private:
    std::vector<SymbolTypeSet> symbolTypeSets;
};

SymbolTypeSetCollection* GetSymbolTypeSetCollection();
void SetSymbolTypeSetCollection(SymbolTypeSetCollection* collection);

} } // namespace Cm::Sym

#endif // CM_SYM_SYMBOL_TYPE_SET_INCLUDED

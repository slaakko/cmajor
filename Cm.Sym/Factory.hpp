/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_FACTORY_INCLUDED
#define CM_SYM_FACTORY_INCLUDED
#include <Cm.Sym/Value.hpp>

namespace Cm { namespace Sym {

class ValueCreator
{
public:
    virtual ~ValueCreator();
    virtual Value* CreateValue() = 0;
};

class ValueFactory
{
public:
    ValueFactory();
    static void Init();
    static void Done();
    static ValueFactory& Instance();
    void Register(ValueType valueType, ValueCreator* creator);
    Value* CreateValue(ValueType valueType);
private:
    static std::unique_ptr<ValueFactory> instance;
    std::vector<std::unique_ptr<ValueCreator>> creators;
};

class SymbolCreator
{
public:
    virtual ~SymbolCreator();
    virtual Symbol* CreateSymbol(const Span& span, const std::string& name) = 0;
    virtual Symbol* CreateBasicTypeSymbol() = 0;
};

class SymbolFactory
{
public:
    SymbolFactory();
    static void Init();
    static void Done();
    static SymbolFactory& Instance();
    void Register(SymbolType symbolType, SymbolCreator* creator);
    Symbol* CreateBasicTypeSymbol(SymbolType basicTypeSymbolType);
    Symbol* CreateSymbol(SymbolType symbolType, const Span& span, const std::string& name);
private:
    static std::unique_ptr<SymbolFactory> instance;
    std::vector<std::unique_ptr<SymbolCreator>> creators;
};

void InitFactory();
void DoneFactory();

} } // namespace Cm::Sym

#endif // CM_SYM_FACTORY_INCLUDED

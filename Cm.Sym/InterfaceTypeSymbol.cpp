/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/InterfaceTypeSymbol.hpp>
#include <Cm.Sym/NameMangling.hpp>
#include <Cm.Sym/FunctionSymbol.hpp>
#include <Cm.IrIntf/Rep.hpp>

namespace Cm { namespace Sym {

InterfaceCounter* interfaceCounter = nullptr;

InterfaceCounter::InterfaceCounter() : nextIid(0)
{
}

InterfaceCounter* GetInterfaceCounter()
{
    return interfaceCounter;
}

void SetInterfaceCounter(InterfaceCounter* interfaceCounter_)
{
    interfaceCounter = interfaceCounter_;
}

InterfaceTypeSymbol::InterfaceTypeSymbol(const Span& span_, const std::string& name_) : TypeSymbol(span_, name_), iid(noIid)
{
}

void InterfaceTypeSymbol::MakeIrType()
{
    if (IrTypeMade()) return;
    SetIrType(Cm::IrIntf::CreateClassTypeName(FullName()));
}

bool InterfaceTypeSymbol::IsInterfaceTypeSymbol() const
{
    return true;
}

void InterfaceTypeSymbol::Write(Writer& writer)
{
    TypeSymbol::Write(writer);
    writer.GetBinaryWriter().Write(iid);
}

void InterfaceTypeSymbol::Read(Reader& reader)
{
    TypeSymbol::Read(reader);
    iid = reader.GetBinaryReader().ReadULong();
    reader.EnqueueMakeIrTypeFor(this);
}

std::string InterfaceTypeSymbol::GetMangleId() const
{
    return MakeAssemblyName(FullName());
}

void InterfaceTypeSymbol::AddSymbol(Symbol* symbol)
{
    TypeSymbol::AddSymbol(symbol);
    if (symbol->IsFunctionSymbol())
    {
        FunctionSymbol* functionSymbol = static_cast<FunctionSymbol*>(symbol);
        functionSymbol->SetItblIndex(int(memFuns.size()));
        memFuns.push_back(functionSymbol);
    }
    else if (symbol->IsMemberVariableSymbol())
    {
        memberVars.push_back(static_cast<MemberVariableSymbol*>(symbol));
    }
}

} } // namespace Cm::Sym

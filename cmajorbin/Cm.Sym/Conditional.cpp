/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/Conditional.hpp>
#include <memory>

namespace Cm { namespace Sym {

class Defines
{
public:
    static void Init();
    static void Done();
    static Defines& Instance();
    void Set(const std::unordered_set<std::string>& defines_);
    bool Defined(const std::string& symbol) const;
private:
    static std::unique_ptr<Defines> instance;
    std::unordered_set<std::string> defines;
};

void Defines::Init()
{
    instance.reset(new Defines());
}

void Defines::Done()
{
    instance.reset();
}

Defines& Defines::Instance()
{
    return *instance;
}

std::unique_ptr<Defines> Defines::instance;

void Defines::Set(const std::unordered_set<std::string>& defines_)
{
    defines = defines_;
}

bool Defines::Defined(const std::string& symbol) const
{
    return defines.find(symbol) != defines.end();
}

void Define(const std::unordered_set<std::string>& symbols)
{
    Defines::Instance().Set(symbols);
}

bool IsSymbolDefined(const std::string& symbol)
{
    return Defines::Instance().Defined(symbol);
}

void InitConditional()
{
    Defines::Init();
}

void DoneConditional()
{
    Defines::Done();
}

} } // namespace Cm::Sym

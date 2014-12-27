/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/NameMangling.hpp>
#include <Cm.IrIntf/Rep.hpp>

namespace Cm { namespace Sym {

class FunctionGroupMangleMap
{
public:
    static void Init();
    static void Done();
    static FunctionGroupMangleMap& Instance();
    const std::string& MangleFunctionGroupName(const std::string& functionGroupName) const;
private:
    static std::unique_ptr<FunctionGroupMangleMap> instance;
    typedef std::unordered_map<std::string, std::string> MangleMap; 
    typedef MangleMap::const_iterator MangleMapIt;
    MangleMap mangledFunctionGroupNames;
    FunctionGroupMangleMap();
};

std::unique_ptr<FunctionGroupMangleMap> FunctionGroupMangleMap::instance;

void FunctionGroupMangleMap::Init()
{
    instance.reset(new FunctionGroupMangleMap());
}

void FunctionGroupMangleMap::Done()
{
    instance.reset();
}

FunctionGroupMangleMap& FunctionGroupMangleMap::Instance()
{
    return *instance;
}

FunctionGroupMangleMap::FunctionGroupMangleMap()
{
    mangledFunctionGroupNames["operator+"] = "pl";
    mangledFunctionGroupNames["operator-"] = "mi";
    mangledFunctionGroupNames["operator*"] = "tm";
    mangledFunctionGroupNames["operator/"] = "di";
    mangledFunctionGroupNames["operator%"] = "mo";
    mangledFunctionGroupNames["operator=="] = "eq";
    mangledFunctionGroupNames["operator<"] = "ls";
    mangledFunctionGroupNames["operator!"] = "nt";
    mangledFunctionGroupNames["operator~"] = "cp";
    mangledFunctionGroupNames["operator&"] = "an";
    mangledFunctionGroupNames["operator|"] = "or";
    mangledFunctionGroupNames["operator^"] = "xo";
    mangledFunctionGroupNames["operator<<"] = "sl";
    mangledFunctionGroupNames["operator>>"] = "sr";
    mangledFunctionGroupNames["operator->"] = "pm";
    mangledFunctionGroupNames["operator++"] = "pp";
    mangledFunctionGroupNames["operator--"] = "mm";
    mangledFunctionGroupNames["operator="] = "as";
    mangledFunctionGroupNames["operator()"] = "ap";
    mangledFunctionGroupNames["operator[]"] = "ix";
    mangledFunctionGroupNames["@operator_conv"] = "cv";
    mangledFunctionGroupNames["@constructor"] = "ct";
    mangledFunctionGroupNames["@destructor"] = "dt";
    mangledFunctionGroupNames["@static_constructor"] = "sc";
}

const std::string& FunctionGroupMangleMap::MangleFunctionGroupName(const std::string& functionGroupName) const
{
    MangleMapIt i = mangledFunctionGroupNames.find(functionGroupName);
    if (i != mangledFunctionGroupNames.end())
    {
        return i->second;
    }
    return functionGroupName;
}

std::string MakeAssemblyName(const std::string& name)
{
    return Cm::IrIntf::MakeAssemblyName(name);
}

std::string MangleName(const std::string& namespaceName, const std::string& functionGroupName, const std::vector<Cm::Sym::TypeSymbol*>& templateArguments, 
    const std::vector<Cm::Sym::ParameterSymbol*>& parameters)
{
    std::string mangledNs = namespaceName;
    int nm = int(mangledNs.size());
    if (nm > 0)
    {
        mangledNs.append(1, '.');
    }
    std::string mangledName(mangledNs + FunctionGroupMangleMap::Instance().MangleFunctionGroupName(functionGroupName));
    if (!templateArguments.empty())
    {
        mangledName.append("$$");
        int n = int(templateArguments.size());
        for (int i = 0; i < n; ++i)
        {
            if (i > 0)
            {
                mangledName.append(1, '.');
            }
            mangledName.append(MakeAssemblyName(templateArguments[i]->FullName()));
        }
        mangledName.append("$$");
    }
    int n = int(parameters.size());
    if (n > 0)
    {
        mangledName.append("$");
    }
    for (int i = 0; i < n; ++i)
    {
        Cm::Sym::ParameterSymbol* parameter = parameters[i];
        Cm::Sym::TypeSymbol* type = parameter->GetType();
        mangledName.append(type->GetMangleId());
    }
    return mangledName;
}

void InitNameMangling()
{
    FunctionGroupMangleMap::Init();
}

void DoneNameMangling()
{
    FunctionGroupMangleMap::Done();
}

} } // namespace Cm::Sym

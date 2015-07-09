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
    const std::string& GetFunctionGroupDocId(const std::string& functionGroupName) const;
private:
    static std::unique_ptr<FunctionGroupMangleMap> instance;
    typedef std::unordered_map<std::string, std::string> MangleMap; 
    typedef MangleMap::const_iterator MangleMapIt;
    MangleMap mangledFunctionGroupNames;
    MangleMap functionGroupDocIds;
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

    functionGroupDocIds["operator+"] = "operator.plus";
    functionGroupDocIds["operator-"] = "operator.minus";
    functionGroupDocIds["operator*"] = "operator.times";
    functionGroupDocIds["operator/"] = "operator.divides";
    functionGroupDocIds["operator%"] = "operator.remainder";
    functionGroupDocIds["operator=="] = "operator.equal";
    functionGroupDocIds["operator<"] = "operator.less";
    functionGroupDocIds["operator!"] = "operator.not";
    functionGroupDocIds["operator~"] = "operator.complement";
    functionGroupDocIds["operator&"] = "operator.and";
    functionGroupDocIds["operator|"] = "operator.or";
    functionGroupDocIds["operator^"] = "operator.xor";
    functionGroupDocIds["operator<<"] = "operator.shiftLeft";
    functionGroupDocIds["operator>>"] = "operator.shiftRight";
    functionGroupDocIds["operator->"] = "operator.pointer";
    functionGroupDocIds["operator++"] = "operator.increment";
    functionGroupDocIds["operator--"] = "operator.decrement";
    functionGroupDocIds["operator="] = "operator.assign";
    functionGroupDocIds["operator()"] = "operator.apply";
    functionGroupDocIds["operator[]"] = "operator.index";
    functionGroupDocIds["@operator_conv"] = "operator.convert";
    functionGroupDocIds["@constructor"] = "constructor";
    functionGroupDocIds["@destructor"] = "destructor";
    functionGroupDocIds["@static_constructor"] = "staticConstructor";
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

const std::string& FunctionGroupMangleMap::GetFunctionGroupDocId(const std::string& functionGroupName) const
{
    MangleMapIt i = functionGroupDocIds.find(functionGroupName);
    if (i != functionGroupDocIds.end())
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
    Cm::IrIntf::BackEnd backend = Cm::IrIntf::GetBackEnd();
    if (backend == Cm::IrIntf::BackEnd::llvm)
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
    else if (backend == Cm::IrIntf::BackEnd::c)
    {
        std::string mangledNs;
        int n = int(namespaceName.size());
        for (int i = 0; i < n; ++i)
        {
            if (namespaceName[i] == '.')
            {
                mangledNs.append("_N_");
            }
            else
            {
                mangledNs.append(1, namespaceName[i]);
            }
        }
        int nm = int(mangledNs.size());
        if (nm > 0)
        {
            mangledNs.append("_N_");
        }
        std::string mangledName(mangledNs + FunctionGroupMangleMap::Instance().MangleFunctionGroupName(functionGroupName));
        if (!templateArguments.empty())
        {
            mangledName.append("_B_");
            int n = int(templateArguments.size());
            for (int i = 0; i < n; ++i)
            {
                if (i > 0)
                {
                    mangledName.append(1, '_');
                }
                mangledName.append(MakeAssemblyName(templateArguments[i]->FullName()));
            }
            mangledName.append("_E_");
        }
        int np = int(parameters.size());
        if (np > 0)
        {
            mangledName.append("_T_");
        }
        for (int i = 0; i < np; ++i)
        {
            ParameterSymbol* parameter = parameters[i];
            TypeSymbol* type = parameter->GetType();
            mangledName.append(type->GetMangleId());
        }
        return mangledName;
    }
    else
    {
        return "";
    }
}

std::string MakeGroupDocId(const std::string& functionGroupName)
{
    return FunctionGroupMangleMap::Instance().GetFunctionGroupDocId(functionGroupName);
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

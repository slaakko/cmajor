/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Sym/FunctionTable.hpp>
#include <Cm.Util/TextUtils.hpp>
#include <fstream>
#include <iostream>
#include <boost/filesystem.hpp>

namespace Cm { namespace Sym {

FunctionInfo::FunctionInfo() : currentProject(false), fid(uint32_t(-1))
{
}

FunctionInfo::FunctionInfo(bool currentProject_, uint32_t fid_) : currentProject(currentProject_), fid(fid_)
{
}

FunctionTable* FunctionTable::instance;

FunctionTable::FunctionTable() : nextFid(0)
{
}

uint32_t FunctionTable::GetFid(const std::string& functionName)
{
    std::unordered_map<std::string, FunctionInfo>::const_iterator i = functionMap.find(functionName);
    if (i != functionMap.cend())
    {
        const FunctionInfo& functionInfo = i->second;
        return functionInfo.Fid();
    }
    uint32_t fid = nextFid++;
    FunctionInfo functionInfo(true, fid);
    functionMap[functionName] = functionInfo;
    return fid;
}

void FunctionTable::Import(const std::string& fileName)
{
    if (!boost::filesystem::exists(fileName)) return;
    std::ifstream file(fileName);
    std::string line;
    while (std::getline(file, line))
    {
        std::vector<std::string> components = Cm::Util::Split(line, ':');
        if (components.size() != 2)
        {
            throw std::runtime_error("two components expected");
        }
        std::string functionName = components[0];
        std::string fidStr = components[1];
        uint32_t fid = static_cast<uint32_t>(std::stoi(fidStr));
        FunctionInfo functionInfo(false, fid);
        functionMap[functionName] = functionInfo;
    }
}

struct NameLess
{
    bool operator()(const std::pair<std::string, uint32_t>& left, const std::pair<std::string, uint32_t>& right) const
    {
        return left.first < right.first;
    }
};

void FunctionTable::Export(const std::string& fileName)
{
    std::vector<std::pair<std::string, uint32_t>> data;
    std::unordered_map<std::string, FunctionInfo>::iterator e = functionMap.end();
    for (std::unordered_map<std::string, FunctionInfo>::iterator i = functionMap.begin(); i != e; ++i)
    {
        const std::string& functionName = i->first;
        FunctionInfo& functionInfo = i->second;
        if (functionInfo.CurrentProject())
        {
            functionInfo.ResetCurrentProject();
            data.push_back(std::make_pair(functionName, functionInfo.Fid()));
        }
    }
    std::sort(data.begin(), data.end(), NameLess());
    std::ofstream file(fileName);
    for (const std::pair<std::string, uint32_t>& fun : data)
    {
        file << fun.first << ":" << fun.second << std::endl;
    }
}

struct FidLess
{
    bool operator()(const std::pair<uint32_t, std::string>& left, const std::pair<uint32_t, std::string>& right) const
    {
        return left.first < right.first;
    }
};

void FunctionTable::Write(const std::string& fileName)
{
    std::vector<std::pair<uint32_t, std::string>> data;
    std::unordered_map<std::string, FunctionInfo>::iterator e = functionMap.end();
    for (std::unordered_map<std::string, FunctionInfo>::iterator i = functionMap.begin(); i != e; ++i)
    {
        const std::string& functionName = i->first;
        FunctionInfo& functionInfo = i->second;
        data.push_back(std::make_pair(functionInfo.Fid(), functionName));
    }
    std::sort(data.begin(), data.end(), FidLess());
    std::ofstream file(fileName);
    for (const std::pair<uint32_t, std::string>& idFunNamePair : data)
    {
        file << idFunNamePair.first << ":" << idFunNamePair.second << std::endl;
    }
}

} } // Cm::Sym

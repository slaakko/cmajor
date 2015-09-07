/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
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

uint32_t FunctionTable::GetFid(const std::string& functionName)
{
    std::unordered_map<std::string, FunctionInfo>::const_iterator i = functionMap.find(functionName);
    if (i != functionMap.cend())
    {
        const FunctionInfo& functionInfo = i->second;
        return functionInfo.Fid();
    }
    uint32_t fid = uint32_t(functionMap.size());
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
        int fid = std::stoi(fidStr);
        FunctionInfo functionInfo(false, uint32_t(fid));
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

void FunctionTable::ReadFunctionsById(const std::string& fileName)
{
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
        int fid = std::stoi(fidStr);
        functionsById[uint32_t(fid)] = functionName;
    }
}

void FunctionTable::WriteFunctionsById(const std::string& fileName)
{
    std::vector<std::string> functionNames;
    std::unordered_map<uint32_t, std::string>::const_iterator e = functionsById.cend();
    for (std::unordered_map<uint32_t, std::string>::const_iterator i = functionsById.cbegin(); i != e; ++i)
    {
        uint32_t fid = i->first;
        std::string functionName = i->second;
        while (functionNames.size() <= fid)
        {
            functionNames.push_back(std::string());
        }
        if (!functionNames[fid].empty())
        {
            std::cout << "warning: nonempty function name for fid " << fid << std::endl;
        }
        functionNames[fid] = functionName;
    }
    std::ofstream file(fileName);
    int n = int(functionNames.size());
    for (int i = 0; i < n; ++i)
    {
        const std::string& functionName = functionNames[i];
        file << i << ":" << functionName << std::endl;
    }
}

} } // Cm::Sym

/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_SYM_FUNCTION_TABLE_INCLUDED
#define CM_SYM_FUNCTION_TABLE_INCLUDED
#include <string>
#include <unordered_map>

namespace Cm { namespace Sym {

class FunctionInfo
{
public:
    FunctionInfo();
    FunctionInfo(bool currentProject_, uint32_t fid_);
    bool CurrentProject() const { return currentProject; }
    void ResetCurrentProject() { currentProject = false; }
    uint32_t Fid() const { return fid; }
private:
    bool currentProject;
    uint32_t fid;
};

class FunctionTable
{
public:
    static void SetInstance(FunctionTable* instance_) { instance = instance_; }
    static FunctionTable* Instance() { return instance; }
    uint32_t GetFid(const std::string& functionName);
    void Import(const std::string& fileName);
    void Export(const std::string& fileName);
    void ReadFunctionsById(const std::string& fileName);
    void WriteFunctionsById(const std::string& fileName);
private:
    static FunctionTable* instance;
    std::unordered_map<std::string, FunctionInfo> functionMap;
    std::unordered_map<uint32_t, std::string> functionsById;
};

} } // Cm::Sym

#endif // CM_SYM_FUNCTION_TABLE_INCLUDED

/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_DEBUGGER_DEBUG_INFO_INCLUDED
#define CM_DEBUGGER_DEBUG_INFO_INCLUDED
#include <Cm.Core/CDebugInfo.hpp>
#include <unordered_map>
#include <string>
#include <memory>

namespace Cm { namespace Debugger {

enum class State
{
    debugging, idle, exit
};

class DebugInfo
{
public:
    DebugInfo(const std::string& debugInfoFilePath);
    Cm::Core::CFunctionDebugInfo* GetFunctionDebugInfo(const std::string& mangledFunctionName) const;
    State GetState() const { return state; }
    void SetState(State state_);
private:
    State state;
    std::vector<std::unique_ptr<Cm::Core::CDebugInfoFile>> files;
    Cm::Core::CFunctionDebugInfo* mainFunctionDebugInfo;
    Cm::Core::CFunctionDebugInfo* currentFunctionDebugInfo;
    typedef std::unordered_map<std::string, Cm::Core::CFunctionDebugInfo*> UniqueFunctionDebugInfoMap;
    typedef UniqueFunctionDebugInfoMap::const_iterator UniqueFunctionDebugInfoMapIt;
    UniqueFunctionDebugInfoMap uniqueFunctionDebugInfoMap;
    void Process(Cm::Core::CDebugInfoFile* file);
};

} } // Cm::Debugger

#endif // CM_DEBUGGER_DEBUG_INFO_INCLUDED

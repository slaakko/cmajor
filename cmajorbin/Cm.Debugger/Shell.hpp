/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_DEBUGGER_SHELL_INCLUDED
#define CM_DEBUGGER_SHELL_INCLUDED
#include <Cm.Debugger/InputReader.hpp>
#include <Cm.Debugger/Util.hpp>

namespace Cm { namespace Debugger {

class DebugInfo;
class Gdb;
class Command;
typedef std::shared_ptr<Command> CommandPtr;

class Shell
{
public:
    Shell(DebugInfo& debugInfo_, Gdb& gdb_, const std::string& commandFileName_);
    void Execute();
    void SetLastCommand(CommandPtr lastCommand_) { lastCommand = lastCommand_; }
    CallStack ParseBackTraceReply(const std::string& backTraceReply) const;
private:
    DebugInfo& debugInfo;
    Gdb& gdb;
    InputReader inputReader;
    CommandPtr lastCommand;
    bool ExecuteNextCommand();
};

} } // Cm::Debugger

#endif // CM_DEBUGGER_SHELL_INCLUDED

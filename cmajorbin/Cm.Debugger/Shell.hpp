/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_DEBUGGER_SHELL_INCLUDED
#define CM_DEBUGGER_SHELL_INCLUDED
#include <Cm.Debugger/InputReader.hpp>

namespace Cm { namespace Debugger {

class DebugInfo;
class Gdb;
class Command;

class Shell
{
public:
    Shell(DebugInfo& debugInfo_, Gdb& gdb_);
    void Execute();
private:
    DebugInfo& debugInfo;
    Gdb& gdb;
    InputReader inputReader;
    std::shared_ptr<Command> lastCommand;
    bool ExecuteNextCommand();
};

} } // Cm::Debugger

#endif // CM_DEBUGGER_SHELL_INCLUDED

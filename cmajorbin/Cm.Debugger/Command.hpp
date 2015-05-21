/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_DEBUGGER_COMMAND_INCLUDED
#define CM_DEBUGGER_COMMAND_INCLUDED

namespace Cm { namespace Debugger {

class DebugInfo;
class Gdb;
class InputReader;
class Shell;

class Command
{
public:
    virtual ~Command();
    virtual void Execute(DebugInfo& debugInfo, Gdb& gdb, InputReader& inputReader, Shell& shell);
    virtual bool IsQuit() const { return false; }
};

class QuitCommand : public Command
{
public:
    void Execute(DebugInfo& debugInfo, Gdb& gdb, InputReader& inputReader, Shell& shell) override;
    bool IsQuit() const override { return true; }
};

} } // Cm::Debugger

#endif // CM_DEBUGGER_COMMAND_INCLUDED

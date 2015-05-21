/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Debugger/Command.hpp>
#include <Cm.Debugger/Gdb.hpp>
#include <Cm.Debugger/InputReader.hpp>
#include <Cm.Debugger/DebugInfo.hpp>

namespace Cm { namespace Debugger {

Command::~Command()
{
}

void Command::Execute(DebugInfo& debugInfo, Gdb& gdb, InputReader& inputReader, Shell& shell)
{
}

void QuitCommand::Execute(DebugInfo& debugInfo, Gdb& gdb, InputReader& inputReader, Shell& shell)
{
    gdb.Quit();
    inputReader.Exit();
    debugInfo.SetState(State::exit);
}


} } // Cm::Debugger

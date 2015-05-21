/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Debugger/Shell.hpp>
#include <Cm.Debugger/Gdb.hpp>
#include <Cm.Debugger/InputReader.hpp>

namespace Cm { namespace Debugger {

Shell::Shell(DebugInfo& debugInfo_, Gdb& gdb_) : debugInfo(debugInfo_), gdb(gdb_), inputReader(gdb)
{
}

bool Shell::ExecuteNextCommand()
{
    std::string commandLine = inputReader.GetLine();
    try
    {

    }
    catch (const std::exception& ex)
    {

    }
    return true;
}

void Shell::Execute()
{
    gdb.Run();
    inputReader.Start();
    bool execute = true;
    while (execute)
    {
        execute = ExecuteNextCommand();
    }
}

} } // Cm::Debugger

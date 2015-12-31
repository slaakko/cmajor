/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Debugger/Shell.hpp>
#include <Cm.Debugger/Gdb.hpp>
#include <Cm.Debugger/DebugInfo.hpp>
#include <Cm.Debugger/InputReader.hpp>
#include <Cm.Debugger/Interpreter.hpp>
#include <Cm.Debugger/GdbReply.hpp>
#include <Cm.Debugger/IdeOutput.hpp>
#include <Cm.Debugger/LineStream.hpp>
#include <iostream>

namespace Cm { namespace Debugger {

InterpreterGrammar* interpreterGrammar = nullptr;
BackTraceReplyGrammar* backTraceReplyGrammar = nullptr;

Shell::Shell(DebugInfo& debugInfo_, Gdb& gdb_, const std::string& commandFileName_) : debugInfo(debugInfo_), gdb(gdb_), inputReader(gdb, commandFileName_)
{
}

CallStack Shell::ParseBackTraceReply(const std::string& backTraceReply) const
{
    try
    {
        if (!backTraceReplyGrammar)
        {
            backTraceReplyGrammar = BackTraceReplyGrammar::Create();
        }
        return backTraceReplyGrammar->Parse(backTraceReply.c_str(), backTraceReply.c_str() + backTraceReply.length(), 0, "");
    }
    catch (const std::exception&)
    {
        return CallStack();
    }
}

bool Shell::ExecuteNextCommand()
{
    if (!interpreterGrammar)
    {
        interpreterGrammar = InterpreterGrammar::Create();
    }
    std::string commandLine;
    if (!ide)
    {
        commandLine = inputReader.GetLine();
    }
    try
    {
        CommandPtr command = nullptr;
        if (ide)
        {
            std::unique_ptr<IdeCommand> ideCommand = inputReader.GetIdeCommand();
            command = ideCommand->ToShellCommand();
        }
        else
        {
            command = interpreterGrammar->Parse(commandLine.c_str(), commandLine.c_str() + commandLine.length(), 0, std::string());
        }
        if (!command)
        {
            command = lastCommand;
        }
        try
        {
            if (!command)
            {
                throw std::runtime_error("no command to repeat");
            }
            lastCommand = command;
            command->Execute(debugInfo, gdb, inputReader, *this);
            if (debugInfo.GetState() == State::exit)
            {
                return false;
            }
        }
        catch (const CommandError& ex)
        {
            if (Cm::Debugger::ide)
            {
                Cm::Debugger::IdePrintError(ex.SequenceNumber(), ex.what());
            }
            else
            {
                ErrorLineStream()->WriteLine(std::string("error: ") + ex.what() + ".");
            }
        }
        catch (const std::exception& ex)
        {
            if (Cm::Debugger::ide)
            {
                Cm::Debugger::IdePrintError(-1, ex.what());
            }
            else
            {
                ErrorLineStream()->WriteLine(std::string("error: ") + ex.what() + ".");
            }
        }
    }
    catch (const std::exception& ex)
    {
        if (Cm::Debugger::ide)
        {
            Cm::Debugger::IdePrintError(-1, ex.what());
        }
        else
        {
            ErrorLineStream()->WriteLine(std::string("error: ") + ex.what() + ". Try \"help\".");
        }
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
        inputReader.Proceed();
        if (!ide)
        {
            std::cout << "(cmdb) ";
        }
        execute = ExecuteNextCommand();
    }
    if (ide)
    {
        IdePrintState(inputReader.QuitSequenceNumber(), "bye", 0, "", "");
    }
    else
    {
        std::cout << "bye!" << std::endl;
    }
}

} } // Cm::Debugger

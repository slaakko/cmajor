/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Debugger/InputReader.hpp>
#include <Cm.Debugger/Gdb.hpp>
#include <Cm.Debugger/DebugInfo.hpp>
#include <string>
#include <iostream>

namespace Cm { namespace Debugger {

void RunInputReader(InputReader* inputReader)
{
    inputReader->Run();
}

InputReader::InputReader(Gdb& gdb_) : gdb(gdb_), lineSet(false), exiting(false), redirecting(false), proceed(0)
{
}

void InputReader::Start()
{
    readerThread = std::thread(RunInputReader, this);
}

void InputReader::Run()
{
    while (!exiting)
    {
        std::string s;
        std::getline(std::cin, s);
        if (ide)
        {
            std::unique_ptr<IdeCommand> command;
            try
            {
                command = ParseIdeCommand(s);
            }
            catch (const std::exception& ex)
            {
                command.reset(new IdeErrorCommand(ex.what()));
            }
            bool redirect = redirecting;
            if (redirect && command->IsInputCommand())
            {
                IdeInputCommand* inputCommand = static_cast<IdeInputCommand*>(command.get());
                s = inputCommand->Input();
                s.append("\n");
                gdb.Write(s);
            }
            else
            {
                if (redirect)
                {
                    command.reset(new IdeErrorCommand("cannot issue debugging commands while redirecting input to program"));
                }
                {
                    std::unique_lock<std::mutex> lck(proceedMtx);
                    proceed = 0;
                }
                {
                    std::lock_guard<std::mutex> lck(lineMtx);
                    ideCommand.reset(command.release());
                    lineSet = true;
                }
                lineAvailable.notify_one();
                std::unique_lock<std::mutex> lck(proceedMtx);
                canProceed.wait(lck, [this] { return proceed > 0; });
            }
        }
        else
        {
            bool redirect = redirecting;
            if (redirect)
            {
                s.append("\n");
                gdb.Write(s);
            }
            else
            {
                {
                    std::unique_lock<std::mutex> lck(proceedMtx);
                    proceed = 0;
                }
                {
                    std::lock_guard<std::mutex> lck(lineMtx);
                    line = s;
                    lineSet = true;
                }
                lineAvailable.notify_one();
                std::unique_lock<std::mutex> lck(proceedMtx);
                canProceed.wait(lck, [this] { return proceed > 0; });
            }
        }
    }
}

void InputReader::Exit()
{
    exiting = true;
    Proceed();
    readerThread.join();
}

void InputReader::StartRedirecting()
{
    redirecting = true;
    Proceed();
}

void InputReader::StopRedirecting()
{
    redirecting = false;
}

void InputReader::Proceed()
{
    std::lock_guard<std::mutex> lck(proceedMtx);
    ++proceed;
    canProceed.notify_one();
}

std::string InputReader::GetLine()
{
    std::unique_lock<std::mutex> lck(lineMtx);
    lineAvailable.wait(lck, [this] { return lineSet; });
    std::string s = line;
    lineSet = false;
    return s;
}

std::unique_ptr<IdeCommand> InputReader::GetIdeCommand()
{
    std::unique_lock<std::mutex> lck(lineMtx);
    lineAvailable.wait(lck, [this] { return lineSet; });
    std::unique_ptr<IdeCommand> command = std::move(ideCommand);
    lineSet = false;
    return command;
}

} } // Cm::Debugger

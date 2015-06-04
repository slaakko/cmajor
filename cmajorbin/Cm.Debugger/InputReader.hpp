/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_DEBUGGER_INPUT_READER_INCLUDED
#define CM_DEBUGGER_INPUT_READER_INCLUDED
#include <Cm.Debugger/IdeInput.hpp>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <fstream>

namespace Cm { namespace Debugger {

class Gdb;

class InputReader
{
public:
    InputReader(Gdb& gdb_, const std::string& commandFileName_);
    void Start();
    void Run();
    void Exit();
    void StartRedirecting();
    void StopRedirecting();
    void Proceed();
    std::string GetLine();
    std::unique_ptr<IdeCommand> GetIdeCommand();
private:
    Gdb& gdb;
    std::string commandFileName;
    std::ifstream commandFile;
    std::thread readerThread;
    std::string line;
    bool lineSet;
    bool exiting;
    bool redirecting;
    std::mutex lineMtx;
    std::condition_variable lineAvailable;
    std::mutex proceedMtx;
    std::condition_variable canProceed;
    int proceed;
    std::unique_ptr<IdeCommand> ideCommand;
};

} } // Cm::Debugger

#endif // CM_DEBUGGER_INPUT_READER_INCLUDED

/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_DEBUGGER_INPUT_READER_INCLUDED
#define CM_DEBUGGER_INPUT_READER_INCLUDED
#include <thread>
#include <mutex>
#include <condition_variable>

namespace Cm { namespace Debugger {

class Gdb;

class InputReader
{
public:
    InputReader(Gdb& gdb_);
    void Start();
    void Run();
    void Exit();
    void StartRedirecting();
    void StopRedirecting();
    void Proceed();
    std::string GetLine();
private:
    Gdb& gdb;
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
};

} } // Cm::Debugger

#endif // CM_DEBUGGER_INPUT_READER_INCLUDED

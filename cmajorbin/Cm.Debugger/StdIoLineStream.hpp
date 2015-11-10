/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_DEBUGGER_STD_IO_LINE_STREAM_INCLUDED
#define CM_DEBUGGER_STD_IO_LINE_STREAM_INCLUDED
#include <Cm.Debugger/LineStream.hpp>

namespace Cm { namespace Debugger {

class StdIoLineStream : public LineStream
{
public:
    void WriteLine(const std::string& line) override;
    std::string ReadLine() override;
};

class StdErrorLineStream : public LineStream
{
public:
    void WriteLine(const std::string& line) override;
    std::string ReadLine() override;
};

} } // Cm::Debugger

#endif // CM_DEBUGGER_STD_IO_LINE_STREAM_INCLUDED

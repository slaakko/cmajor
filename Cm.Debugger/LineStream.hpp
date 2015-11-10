/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_DEBUGGER_LINE_STREAM_INCLUDED
#define CM_DEBUGGER_LINE_STREAM_INCLUDED
#include <string>

namespace Cm { namespace Debugger {

class LineStream
{
public:
    virtual ~LineStream();
    virtual void WriteLine(const std::string& line) = 0;
    virtual std::string ReadLine() = 0;
};

LineStream* IoLineStream();
void SetIoLineStream(LineStream* ioLineStream_);
LineStream* ErrorLineStream();
void SetErrorLineStream(LineStream* errorLineStream_);

} } // Cm::Debugger

#endif // CM_DEBUGGER_LINE_STREAM_INCLUDED

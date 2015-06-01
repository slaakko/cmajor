/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef CM_DEBUGGER_UTIL_INCLUDED
#define CM_DEBUGGER_UTIL_INCLUDED
#include <Cm.Core/CDebugInfo.hpp>

namespace Cm { namespace Debugger {

class Frame
{
public:
    Frame();
    Frame(int frameNumber_, const Cm::Core::SourceFileLine& sourceFileLine_);
    int FrameNumber() const { return frameNumber; }
    const Cm::Core::SourceFileLine& SourceFileLine() const { return sourceFileLine; }
private:
    int frameNumber;
    Cm::Core::SourceFileLine sourceFileLine;
};

class CallStack
{
public:
    void AddFrame(const Frame& frame);
    const std::vector<Frame>& Frames() const { return frames; }
private:
    std::vector<Frame> frames;
};

} } // Cm::Debugger

#endif // CM_DEBUGGER_UTIL_INCLUDED

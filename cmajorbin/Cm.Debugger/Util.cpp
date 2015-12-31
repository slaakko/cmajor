/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Debugger/Util.hpp>

namespace Cm { namespace Debugger {

Frame::Frame() : frameNumber(0), sourceFileLine()
{
}

Frame::Frame(int frameNumber_, const Cm::Core::SourceFileLine& sourceFileLine_) : frameNumber(frameNumber_), sourceFileLine(sourceFileLine_)
{
}

void CallStack::AddFrame(const Frame& frame)
{
    frames.push_back(frame);
}

} } // Cm::Debugger

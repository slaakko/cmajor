/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Debugger/LineStream.hpp>
#include <stdexcept>

namespace Cm { namespace Debugger {

LineStream::~LineStream()
{
}

LineStream* ioLineStream = 0;

LineStream* IoLineStream()
{
    if (!ioLineStream)
    {
        throw std::runtime_error("I/O line stream not set");
    }
    return ioLineStream;
}

void SetIoLineStream(LineStream* ioLineStream_)
{
    ioLineStream = ioLineStream_;
}

LineStream* errorLineStream = 0;

LineStream* ErrorLineStream()
{
    if (!errorLineStream)
    {
        throw std::runtime_error("Error line stream not set");
    }
    return errorLineStream;
}

void SetErrorLineStream(LineStream* errorLineStream_)
{
    errorLineStream = errorLineStream_;
}

} } // Cm::Debugger

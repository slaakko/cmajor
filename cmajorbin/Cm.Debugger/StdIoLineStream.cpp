/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Debugger/StdIoLineStream.hpp>
#include <Cm.Util/TextUtils.hpp>
#include <io.h>
#include <fcntl.h>
#include <iostream>
#include <mutex>

namespace Cm { namespace Debugger {

void StdIoLineStream::WriteLine(const std::string& line)
{
    std::cout << line << std::endl;
}

std::string StdIoLineStream::ReadLine()
{
    std::string line;
    std::getline(std::cin, line);
    return line;
}

void StdErrorLineStream::WriteLine(const std::string& line)
{
    std::cerr << line << std::endl;
}

std::string StdErrorLineStream::ReadLine()
{
    throw std::runtime_error("cannot read from StdErrorLineStream");
}

} } // Cm::Debugger

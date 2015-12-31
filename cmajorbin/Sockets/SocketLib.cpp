/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Sockets/SocketLib.hpp>
#include <Sockets/SocketApi.hpp>
#include <string>

namespace Sockets {

SocketLibraryException::SocketLibraryException(const std::string& message_) : std::runtime_error(message_)
{
}

void InitSockets()
{
    int result = init_sockets();
    if (result != 0)
    {
        throw SocketLibraryException("socket library initialization (WSAStartup) failed with error code " + std::to_string(result));
    }
}

void DoneSockets()
{
    done_sockets();
}

}   // namespace Sockets

/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef SOCKETS_SOCKET_LIB_INCLUDED
#define SOCKETS_SOCKET_LIB_INCLUDED
#include <stdexcept>

namespace Sockets {

class SocketLibraryException : public std::runtime_error
{
public:
    SocketLibraryException(const std::string& message_);
};

void InitSockets();
void DoneSockets();

}   // namespace Sockets

#endif // SOCKETS_SOCKET_LIB_INCLUDED

/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef SOCKETS_SOCKET_UTIL_INCLUDED
#define SOCKETS_SOCKET_UTIL_INCLUDED
#include <stdexcept>
#include <string>
#include <stdint.h>

namespace Sockets {

class NetworkBuffer
{
public:
    NetworkBuffer(int size_);
    NetworkBuffer(const NetworkBuffer&) = delete;
    void operator=(const NetworkBuffer&) = delete;
    NetworkBuffer(NetworkBuffer&& that);
    void operator=(NetworkBuffer&& that);
    ~NetworkBuffer();
    void* Mem() const { return mem; }
    int Size() const { return size; }
private:
    int size;
    uint8_t* mem;
};

class SocketError : public std::runtime_error
{
public:
    SocketError(const std::string& operation, int errorCode_);
    SocketError(const std::string& operation, const std::string& errorMessage, int errorCode_);
    int ErrorCode() const { return errorCode; }
private:
    int errorCode;
};

}   // namespace Sockets

#endif // SOCKETS_SOCKET_UTIL_INCLUDED

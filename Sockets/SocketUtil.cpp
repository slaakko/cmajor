/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Sockets/SocketUtil.hpp>
#include <Sockets/SocketApi.hpp>
#include <algorithm>

namespace Sockets {

NetworkBuffer::NetworkBuffer(int size_) : size(size_), mem(new uint8_t[size])
{
}

NetworkBuffer::NetworkBuffer(NetworkBuffer&& that) : size(that.size), mem(that.mem)
{
    that.size = 0;
    that.mem = nullptr;
}

void NetworkBuffer::operator=(NetworkBuffer&& that)
{
    std::swap(size, that.size);
    std::swap(mem, that.mem);
}

NetworkBuffer::~NetworkBuffer()
{
    delete[](mem);
}

class GetSocketErrorMessageContext
{
public:
    GetSocketErrorMessageContext()
    {
        begin_get_socket_error_str();
    }
    ~GetSocketErrorMessageContext()
    {
        end_get_socket_error_str();
    }
};

std::string GetSocketErrorMessage(int errorCode)
{
    std::string errorMessage;
    {
        GetSocketErrorMessageContext messageContext;
        errorMessage = get_socket_error_str(errorCode);
    }
    return errorMessage;
}

SocketError::SocketError(const std::string& operation, int errorCode_) : std::runtime_error(operation + " failed: " + GetSocketErrorMessage(errorCode_)), errorCode(errorCode_)
{
}

SocketError::SocketError(const std::string& operation, const std::string& errorMessage, int errorCode_) : std::runtime_error(operation + " failed: " + errorMessage), errorCode(errorCode_)
{
}

} // namespace Sockets

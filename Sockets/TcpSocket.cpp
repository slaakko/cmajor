/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Sockets/TcpSocket.hpp>
#include <Sockets/SocketUtil.hpp>

namespace Sockets {

TcpSocket::TcpSocket() : socket(invalidSocketHandle)
{
    socket = create_tcp_socket();
    if (socket == -1)
    {
        throw SocketError("create_tcp_socket", get_last_socket_error());
    }
}

TcpSocket::TcpSocket(const std::string& node, const std::string& service) : socket(invalidSocketHandle)
{
    std::string errorMessage;
    begin_connect();
    int getaddrinfofailed = 0;
    int result = connect_socket(node.c_str(), service.c_str(), &socket, &getaddrinfofailed);
    if (result != 0)
    {
        if (getaddrinfofailed != 0)
        {
            errorMessage = get_addrinfo_error(result);
            end_connect();
            throw SocketError("connect", errorMessage, result);
        }
        else
        {
            end_connect();
            throw SocketError("connect", result);
        }
    }
    end_connect();
}


TcpSocket::~TcpSocket()
{
    try
    {
        Close();
    }
    catch (const std::exception&)
    {
    }
}

void TcpSocket::Bind(int port)
{
    int result = bind_socket(socket, port);
    if (result == -1)
    {
        throw SocketError("bind", get_last_socket_error());
    }
}

void TcpSocket::Listen(int backlog)
{
    int result = listen_socket(socket, backlog);
    if (result == -1)
    {
        throw SocketError("listen", get_last_socket_error());
    }
}

TcpSocket TcpSocket::Accept()
{
    int peerSocket = accept_socket(socket);
    if (peerSocket == -1)
    {
        throw SocketError("accept", get_last_socket_error());
    }
    return TcpSocket(peerSocket);
}

void TcpSocket::Shutdown(ShutdownMode mode)
{
    int result = shutdown_socket(socket, mode);
    if (result == -1)
    {
        throw SocketError("shutdown", get_last_socket_error());
    }
}

void TcpSocket::Close()
{
    if (socket != invalidSocketHandle)
    {
        int result = close_socket(socket);
        if (result == -1)
        {
            throw SocketError("close", get_last_socket_error());
        }
        socket = invalidSocketHandle;
    }
}

int TcpSocket::Send(const char* buf, int len)
{
    int result = send_socket(socket, buf, len, 0);
    if (result == -1)
    {
        throw SocketError("send", get_last_socket_error());
    }
    return result;
}

void TcpSocket::Send(const std::string& s)
{
    const char* p = s.c_str();
    int len = (int)s.length();
    int numSent = Send(p, len);
    int numLeft = len - numSent;
    while (numLeft > 0)
    {
        p = p + numSent;
        numSent = Send(p, numLeft);
        numLeft = numLeft - numSent;
    }
}

int TcpSocket::Receive(char* buf, int len)
{
    int result = receive_socket(socket, buf, len, 0);
    if (result == -1)
    {
        throw SocketError("receive", get_last_socket_error());
    }
    return result;
}

std::string TcpSocket::ReceiveAll()
{
    std::string s;
    NetworkBuffer buf(65536);
    int numReceived = Receive((char*)buf.Mem(), buf.Size());
    while (numReceived > 0)
    {
        s.append(static_cast<const char*>(buf.Mem()), numReceived);
        numReceived = Receive((char*)buf.Mem(), buf.Size());
    }
    return s;
}

} // namespace Sockets

/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Cm.Debugger/SocketLineStream.hpp>
#include <Sockets/SocketUtil.hpp>

namespace Cm { namespace Debugger {

SocketLineStream::SocketLineStream(Sockets::TcpSocket&& socket_) : socket(std::move(socket_))
{
}

void SocketLineStream::WriteLine(const std::string& line)
{
    uint32_t n = uint32_t(line.length());
    int packetSize = sizeof(uint32_t) + n;
    Sockets::NetworkBuffer buffer(packetSize);
    uint8_t* p = static_cast<uint8_t*>(buffer.Mem());
    *p++ = n >> 24;
    *p++ = (n >> 16) & 0xFF;
    *p++ = (n >> 8) & 0xFF;
    *p++ = n & 0xFF;
    for (char c : line)
    {
        *p++ = uint8_t(c);
    }
    const char* start = static_cast<const char*>(buffer.Mem());
    int bytesLeft = buffer.Size();
    int bytesSent = socket.Send(start, bytesLeft);
    bytesLeft -= bytesSent;
    while (bytesLeft > 0)
    {
        start += bytesSent;
        bytesSent = socket.Send(start, bytesLeft);
        bytesLeft -= bytesSent;
    }
}

std::string SocketLineStream::ReadLine()
{
    std::string line;
    uint8_t packetLengthBytes[sizeof(uint32_t)];
    char* buf = reinterpret_cast<char*>(packetLengthBytes);
    int bytesLeft = sizeof(uint32_t);
    int bytesReceived = socket.Receive(buf, bytesLeft);
    bytesLeft -= bytesReceived;
    while (bytesLeft > 0)
    {
        buf += bytesReceived;
        bytesReceived = socket.Receive(buf, bytesLeft);
        bytesLeft -= bytesReceived;
    }
    uint32_t n = 256 * 256 * 256 * packetLengthBytes[0] + 256 * 256 * packetLengthBytes[1] + 256 * packetLengthBytes[2] + packetLengthBytes[3];
    Sockets::NetworkBuffer buffer(n);
    buf = static_cast<char*>(buffer.Mem());
    bytesLeft = n;
    bytesReceived = socket.Receive(buf, bytesLeft);
    bytesLeft -= bytesReceived;
    while (bytesLeft > 0)
    {
        buf += bytesReceived;
        bytesReceived = socket.Receive(buf, bytesLeft);
        bytesLeft -= bytesReceived;
    }
    const char* b = static_cast<char*>(buffer.Mem());
    for (uint32_t i = 0; i < n; ++i)
    {
        line.append(1, *b++);
    }
    return line;
}

} } // Cm::Debugger

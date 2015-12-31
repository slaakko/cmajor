/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Sockets/SocketApi.hpp>
#include <string>

namespace Sockets {

const int invalidSocketHandle = -1;

class TcpSocket
{
public:
    TcpSocket();
    TcpSocket(const std::string& node, const std::string& service);
    TcpSocket(int socket_) : socket(socket_) {}
    int GetSocketHandle() const { return socket; }
    TcpSocket(const TcpSocket&) = delete;
    void operator=(const TcpSocket&) = delete;
    TcpSocket(TcpSocket&& that) : socket(that.socket) { that.socket = invalidSocketHandle; }
    void operator=(TcpSocket&& that) { std::swap(socket, that.socket); }
    ~TcpSocket();
    void Bind(int port);
    void Listen(int backlog);
    TcpSocket Accept();
    void Shutdown(ShutdownMode mode);
    void Close();
    int Send(const char* buf, int len);
    void Send(const std::string& s);
    int Receive(char* buf, int len);
    std::string ReceiveAll();
private:
    int socket;
};

} // namespace Sockets

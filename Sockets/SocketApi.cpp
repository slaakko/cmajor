/*========================================================================
    Copyright (c) 2012-2016 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#include <Sockets/SocketApi.hpp>
#include <stdio.h>
#include <string.h>
#include <mutex>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>    
#elif defined(__linux) || defined(__unix) || defined(__posix)
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#else
#error unknown platform
#endif

int init_sockets()
{
#ifdef _WIN32
    WORD ver = MAKEWORD(2, 2);
    WSADATA wsaData;
    return WSAStartup(ver, &wsaData);
#endif
    return 0;
}

void done_sockets()
{
#ifdef _WIN32
    WSACleanup();
#endif
}

int get_last_socket_error()
{
#ifdef _WIN32
    return WSAGetLastError();
#elif defined(__linux) || defined(__unix) || defined(__posix)
    return get_errno();
#else
#error unknown platform
#endif
}

char socket_error_buf[1024];

std::mutex socket_error_buf_lock;

void begin_get_socket_error_str()
{
    socket_error_buf_lock.lock();
}

void end_get_socket_error_str()
{
    socket_error_buf_lock.unlock();
}

const char* get_socket_error_str(int errorCode)
{
#ifdef _WIN32
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorCode, 0, socket_error_buf, sizeof(socket_error_buf), NULL);
#elif defined(__linux) || defined(__unix) || defined(__posix)
    strncpy(socket_error_buf, strerror(errorCode), sizeof(socket_error_buf));
#else
#error unknown platform
#endif
    return socket_error_buf;
}

int create_tcp_socket()
{
    return (int)socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

int bind_socket(int socket, int port)
{
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(port);

    return bind(socket, (struct sockaddr*) &addr, sizeof(addr));
}

int listen_socket(int socket, int backlog)
{
    return listen(socket, backlog);
}

int accept_socket(int socket)
{
    return (int)accept(socket, NULL, NULL);
}

int close_socket(int socket)
{
#ifdef _WIN32
    return closesocket(socket);
#elif defined(__linux) || defined(__unix) || defined(__posix)
    return close(socket);
#else
#error unknown platform
#endif    
    return -1;
}

int shutdown_socket(int socket, enum ShutdownMode mode)
{
#ifdef _WIN32    
    int how = SD_RECEIVE;
    switch (mode)
    {
    case sd_receive: how = SD_RECEIVE; break;
    case sd_send: how = SD_SEND; break;
    case sd_both: how = SD_BOTH; break;
    }
#elif defined(__linux) || defined(__unix) || defined(__posix)
    int how = SHUT_RD;
    switch (mode)
    {
    case sd_receive: how = SHUT_RD; break;
    case sd_send: how = SHUT_WR; break;
    case sd_both: how = SHUT_RDWR; break;
    }
#else
#error unknown platform
#endif    
    return shutdown(socket, how);
}

std::mutex connect_lock;

void begin_connect()
{
    connect_lock.lock();
}

void end_connect()
{
    connect_lock.unlock();
}

int connect_socket(const char* node, const char* service, int* sckt, int* getaddrinfofailed)
{
    struct addrinfo hint;
    struct addrinfo* rp;
    struct addrinfo* res;
    memset(&hint, 0, sizeof(struct addrinfo));
    hint.ai_flags = 0;
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_protocol = IPPROTO_TCP;
    hint.ai_addrlen = 0;
    hint.ai_addr = 0;
    hint.ai_canonname = 0;
    hint.ai_next = 0;
    *getaddrinfofailed = 0;
    *sckt = -1;
    int lastError = 0;
    int result = getaddrinfo(node, service, &hint, &res);
    if (result != 0)
    {
        *getaddrinfofailed = 1;
        return result;
    }
    for (rp = res; rp != 0; rp = rp->ai_next)
    {
        *sckt = (int)socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (*sckt == -1)
        {
            continue;
        }
        result = connect(*sckt, rp->ai_addr, (int)rp->ai_addrlen);
        if (result != 0)
        {
            lastError = get_last_socket_error();
            close_socket(*sckt);
            *sckt = -1;
        }
        else
        {
            break; // connected!
        }
    }
    freeaddrinfo(res);
    if (rp == 0)
    {
        return lastError;
    }
    return 0;
}

const char* get_addrinfo_error(int errorCode)
{
    return gai_strerror(errorCode);
}

int send_socket(int socket, const char* buf, int len, int flags)
{
    return send(socket, buf, len, flags);
}

int receive_socket(int socket, char* buf, int len, int flags)
{
    return recv(socket, buf, len, flags);
}

/*========================================================================
    Copyright (c) 2012-2015 Seppo Laakko
    http://sourceforge.net/projects/cmajor/

    Distributed under the GNU General Public License, version 3 (GPLv3).
    (See accompanying LICENSE.txt or http://www.gnu.org/licenses/gpl.html)

========================================================================*/

#ifndef SOCKETS_SOCKET_API_INCLUDED
#define SOCKETS_SOCKET_API_INCLUDED

enum ShutdownMode
{
    sd_receive = 0, sd_send = 1, sd_both = 2
};

int init_sockets();
void done_sockets();
int get_last_socket_error();
void begin_get_socket_error_str();
void end_get_socket_error_str();
const char* get_socket_error_str(int errorCode);
int create_tcp_socket();
int bind_socket(int socket, int port);
int listen_socket(int socket, int backlog);
int accept_socket(int socket);
int close_socket(int socket);
int shutdown_socket(int socket, enum ShutdownMode mode);
void begin_connect();
void end_connect();
int connect_socket(const char* node, const char* service, int* sckt, int* getaddrinfofailed);
const char* get_addrinfo_error(int errorCode);
int send_socket(int socket, const char* buf, int len, int flags);
int receive_socket(int socket, char* buf, int len, int flags);

#endif // SOCKETS_SOCKET_API_INCLUDED

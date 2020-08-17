#ifndef __TCP_SOCKET_H__
#define __TCP_SOCKET_H__

#include "socket.h"

#define  SHUTDOWN_SEND  1
#define  SHUTDOWN_RECV  2
#define  SHUTDOWN_BOTH  3
/**
 *   Socket which is able to connect, send, and receive
 */
class TcpSocket : public Socket
{
public:
    /**
     *   Establish a socket connection with the given foreign
     *   address and port
     */
    void connect(const string &remoteAddress, unsigned short remotePort);

    /**
     *   Write the given buffer to this socket.  Call connect() before
     *   calling send()
     */
    void send(const void *buffer, int bufferLen);
    void send(const char *buffer, int bufferLen);
    void send(const char *str);
    void send(const string& str);


    /**
     *   Read into the given buffer up to bufferLen bytes data from this
     *   socket.  Call connect() before calling recv()
     */
    int recv(void *buffer, int bufferLen, unsigned short timeout = 0);
    int recv(char *buffer, int bufferLen, unsigned short timeout = 0);
    int recvLine(char* buffer, int bufferLen, unsigned short timeout = 0);
    void shutdown(int& type);

    /**
     *   Get the remote address.
     */
    string getRemoteAddress();

    unsigned long getRemoteIPv4Address();
    unsigned long getLocalIPv4Address();

    /**
     *   Get the remote port.
     */
    unsigned short getRemotePort() ;

public:
    TcpSocket();
    TcpSocket(const string &remoteAddress, unsigned short port);
    TcpSocket(int newConnSD);
};

#endif

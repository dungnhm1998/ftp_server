#ifdef _WIN32
#include <winsock2.h>         // For socket(), connect(), send(), and recv()
#include <ws2tcpip.h>
typedef int socklen_t;
typedef char raw_type;       // Type used for raw data on this platform
#else
#include <sys/types.h>       // For data types
#include <sys/socket.h>      // For socket(), connect(), send(), and recv()
#include <netdb.h>           // For gethostbyname()
#include <arpa/inet.h>       // For inet_addr()
#include <unistd.h>          // For close()
#include <netinet/in.h>      // For sockaddr_in
typedef void raw_type;       // Type used for raw data on this platform
#endif

#include <errno.h>             // For errno
#include "tcpsocket.h"
void fillAddr(const string&,unsigned short, sockaddr_in&);

using namespace std;

// TcpSocket Code

TcpSocket::TcpSocket() : Socket(SOCK_STREAM,IPPROTO_TCP)
{
}

TcpSocket::TcpSocket(int newConnSD) : Socket(newConnSD)
{
}

TcpSocket::TcpSocket(const string &remoteAddress, unsigned short port) : Socket(SOCK_STREAM, IPPROTO_TCP)
{
    connect(remoteAddress, port);
}


void TcpSocket::connect(const string &remoteAddress,
                        unsigned short port)
{
    // Get the address of the requested host
    sockaddr_in destAddr;
    fillAddr(remoteAddress, port, destAddr);
    // If sockDesc is INVALID_SOCKET
    if(sockDesc == -1)
    {
        // Make a new socket
        if ((sockDesc = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        {
            throw SocketException("Socket creation failed (socket())", true);
        }
    }
    // Try to connect to the given port
    if (::connect(sockDesc, (sockaddr *) &destAddr, sizeof(destAddr)) < 0)
    {
        throw SocketException("Connect failed (connect())");
    }
}


void TcpSocket::send(const void *buffer, int bufferLen)
{
    int r = ::send(sockDesc, (raw_type *) buffer, bufferLen, 0);
    if ( r < 0)
    {
        throw SocketException("Send failed (send())", true);
    } //else if (r==0) {
    //    throw SocketException("Send failed (send()) - Connection reset", true);
    //}
}

void TcpSocket::send(const char* buffer, int bufferLen)
{
    int r = ::send(sockDesc, buffer, bufferLen, 0);
    if ( r < 0)
    {
        throw SocketException("Send failed (send())", true);
    } //else if (r==0) {
    //    throw SocketException("Send failed (send()) - Connection reset", true);
    //}
}

void TcpSocket::send(const char *str)
{
    int r = ::send(sockDesc, str,strlen(str), 0);
    if ( r < 0)
    {
        throw SocketException("Send failed (send())", true);
    } //else if (r==0) {
    //    throw SocketException("Send failed (send()) - Connection reset", true);
    //}
}

void TcpSocket::send(const string& str)
{
    int r = ::send(sockDesc, (raw_type *)str.c_str(),str.size(), 0);
    if ( r < 0)
    {
        throw SocketException("Send failed (send())", true);
    } //else if (r==0) {
    //    throw SocketException("Send failed (send()) - Connection reset", true);
    //}
}

int TcpSocket::recv(void *buffer, int bufferLen, unsigned short timeout)
{
    if(timeout==0)
    {
        int rtn = ::recv(sockDesc,(raw_type *) buffer, bufferLen, 0);
        if (rtn < 0)
        {
            throw SocketException("Received failed (recv())", true);
        }
        return rtn;
    }
    else if(timeout>0)
    {
        int rtn;
        fd_set	rfds;			/* read file descriptor set	*/
        FD_ZERO(&rfds);
        timeval tv = {timeout,0};
        FD_SET(sockDesc,&rfds);
        int ret;
        if ((ret = select(sockDesc+1, &rfds, NULL, NULL,&tv)) < 0) // Error
        {
            throw SocketException("Select failed (select())", true);
        }
        else if(ret > 0) // Has new connections
        {
            if (FD_ISSET(sockDesc, &rfds))
            {
                rtn = ::recv(sockDesc,(raw_type *) buffer, bufferLen, 0);
                if(rtn<0)
                {
                    throw SocketException("Received failed (recv())", true);
                }
                return rtn;
            }
        }
    }
    return 0;
}

int TcpSocket::recv(char* buffer, int bufferLen, unsigned short timeout)
{
    if(timeout==0)
    {
        int rtn = ::recv(sockDesc, buffer, bufferLen, 0);
        if (rtn < 0)
        {
            throw SocketException("Received failed (recv())", true);
        }
        return rtn;
    }
    else if(timeout>0)
    {
        int rtn;
        fd_set	rfds;			/* read file descriptor set	*/
        FD_ZERO(&rfds);
        timeval tv = {timeout,0};
        FD_SET(sockDesc,&rfds);
        int ret;
        if ((ret = select(sockDesc+1, &rfds, NULL, NULL,&tv)) < 0) // Error
        {
            throw SocketException("Select failed (select())", true);
        }
        else if(ret > 0) // Has new connections
        {
            if (FD_ISSET(sockDesc, &rfds))
            {
                rtn = ::recv(sockDesc, buffer, bufferLen, 0);
                if(rtn<0)
                {
                    throw SocketException("Received failed (recv())", true);
                }
                return rtn;
            }
        }
    }
    return 0;
}


int TcpSocket::recvLine(char* buffer, int bufLen, unsigned short timeout)
{
    int total_byte_recv = 0;
    int byte_recv;
    if(timeout == 0)
    {
        byte_recv = ::recv(sockDesc, buffer, 1, 0);
        while(byte_recv>0)
        {
            total_byte_recv += byte_recv;
            if(total_byte_recv>=2 && buffer[total_byte_recv-1]=='\n'&& buffer[total_byte_recv-2]=='\r')
                break;
            if(total_byte_recv>=bufLen)
                break;
            byte_recv = ::recv(sockDesc,buffer+total_byte_recv, 1, 0);
        }
        if(byte_recv==-1)
        {
            throw SocketException("Received failed (recv())", true);
            return byte_recv;
        }
        else if(byte_recv==0)
        {
            return 0;
        }
        else
            return total_byte_recv;
    }
    else if(timeout > 0)
    {
        fd_set	rfds;			/* read file descriptor set	*/
        FD_ZERO(&rfds);
        timeval tv = {timeout,0};
        FD_SET(sockDesc,&rfds);
        int ret;
        if ((ret = select(sockDesc+1, &rfds, NULL, NULL,&tv)) < 0) // Error
        {
            throw SocketException("Select failed (select())", true);
        }
        else if(ret > 0) // Has new connections
        {
            if (FD_ISSET(sockDesc, &rfds))
            {
                byte_recv = ::recv(sockDesc, buffer, 1, 0);
                while(byte_recv>0)
                {
                    total_byte_recv += byte_recv;
                    if(total_byte_recv>=2 && buffer[total_byte_recv-1]=='\n'&& buffer[total_byte_recv-2]=='\r')
                       break;
                    if(total_byte_recv>=bufLen)
                       break;
                    if ((ret = select(sockDesc+1, &rfds, NULL, NULL,&tv)) < 0) // Error
                    {
                        throw SocketException("Select failed (select())", true);
                    }
                    else if(ret > 0) // Has new connections
                    {
                        if (FD_ISSET(sockDesc, &rfds))
                        {
                            byte_recv = ::recv(sockDesc,buffer+total_byte_recv, 1, 0);
                        }
                    }
                }  // end of while
                if(byte_recv==-1)
                {
                    throw SocketException("Received failed (recv())", true);
                    return byte_recv;
                }
                else if(byte_recv==0)
                {
                    return 0;
                }
                else
                    return total_byte_recv;
            }
        }
        else if(ret==0)
        {
            return -2;
        }
    } // end of if
    return 0;
}

void TcpSocket::shutdown(int& type)
{
#ifdef _WIN32
    if(type==SHUTDOWN_SEND)
        ::shutdown(this->sockDesc,SD_SEND);
    else if(type==SHUTDOWN_RECV)
        ::shutdown(this->sockDesc,SD_RECEIVE);
    else if(type==SHUTDOWN_BOTH)
        ::shutdown(sockDesc,SD_BOTH);
#else
    if(type==SHUTDOWN_SEND)
        ::shutdown(this->sockDesc,SHUT_RD);
    else if(type==SHUTDOWN_RECV)
        ::shutdown(this->sockDesc,SHUT_WR);
    else if(type==SHUTDOWN_BOTH)
        ::shutdown(sockDesc,SHUT_RDWR);
#endif // _WIN32
}

string TcpSocket::getRemoteAddress()
{
    sockaddr_in addr;
    unsigned int addr_len = sizeof(addr);

    if (getpeername(sockDesc, (sockaddr *) &addr,(socklen_t *) &addr_len) < 0)
    {
        throw SocketException("Fetch of foreign address failed (getpeername())", true);
    }
    return inet_ntoa(addr.sin_addr);
}


unsigned long TcpSocket::getRemoteIPv4Address()
{
    sockaddr_in addr;
    unsigned int addr_len = sizeof(addr);

    if (getpeername(sockDesc, (sockaddr *) &addr,(socklen_t *) &addr_len) < 0)
    {
        throw SocketException("Fetch of foreign address failed (getpeername())", true);
    }
    return addr.sin_addr.s_addr;
}

unsigned long TcpSocket::getLocalIPv4Address()
{
    sockaddr_in addr;
    unsigned int addr_len = sizeof(addr);

    if (getsockname(sockDesc, (sockaddr *) &addr,(socklen_t *) &addr_len) < 0)
    {
        throw SocketException("Fetch of foreign address failed (getsockname())", true);
    }
    return addr.sin_addr.s_addr;
}


unsigned short TcpSocket::getRemotePort()
{
    sockaddr_in addr;
    unsigned int addr_len = sizeof(addr);
    if (getpeername(sockDesc, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0)
    {
        throw SocketException("Fetch of foreign port failed (getpeername())", true);
    }
    return ntohs(addr.sin_port);
}







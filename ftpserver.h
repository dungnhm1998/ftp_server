#ifndef FTPSERVER_H_
#define FTPSERVER_H_


#include <mutex>
#include <fstream>
#include "tcpserver.h"
#include "ftpsession.h"
#include "ftpserverconfig.h"


class FTPServer: public TCPServer
{
private:
    unsigned short connTimeout = 0;
    std::mutex* logMutex = NULL;
    std::fstream* logFile = NULL;
public:
    FTPServer(unsigned short port=21);
    ~FTPServer();

protected:
    void configServer();
    void cleanServer();
    FTPServerConfig* getServerConfig() {return (FTPServerConfig*)conf;}
    void loadServerConfig(const string& confFileName);
    void startNewSession(TcpSocket* slave);
    void initCmd();
private:
    int readCmd(TcpSocket* slave, char* buffer, int buflen);
    unsigned short parseCmd(char *sCmdBuf, int len, char * cmd_argv[], int& cmd_argc);
protected:
};

#endif // FTPSERVER_H_

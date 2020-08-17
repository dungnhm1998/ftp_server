#ifndef SERVER_H__
#define SERVER_H__
// Base class for generic TCP server
#include "tcpserversocket.h"
#include "session.h"

class TCPServer
{
protected:
    ServerConfig* conf;
    TcpServerSocket master;
    bool running;
    bool logging;
    unsigned short port;
    unsigned short numCmd;
    string  cmdNameList[SERVER_CMD_MAX_NUM];
    CMD_FUNC cmdDoFunc[SERVER_CMD_MAX_NUM];
    string logFileName;
private:

public:
    TCPServer(unsigned short localPort);
    virtual ~TCPServer();
    bool start();
    void stop();
    bool restart();
    bool isRunning();
    bool getLogging() {return logging;}
    const string& getLogFileName() const {return logFileName;}

protected:
    void run();
    virtual void startNewSession(TcpSocket* slave) = 0;
    virtual void configServer() = 0;
    virtual void cleanServer() = 0;
    // virtual void loadServerConfig(const string& confFileName) = 0;
    void addCmd(const string& cmdName,CMD_FUNC f);
    void doCmd(Session* session,uint8_t cmdId, char* cmdArgv[], int cmdArgc);
    virtual void initCmd() = 0;
};

#endif // SERVER_H_INCLUDED

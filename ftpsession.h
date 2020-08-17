#ifndef FTPSESSION_H_INCLUDED
#define FTPSESSION_H_INCLUDED
#include <vector>
#include "tcpserversocket.h"
#include "session.h"
#include "ftpserverconfig.h"
#include <mutex>          // std::mutex

#define STATUS_INIT     0
#define STATUS_USERNAME     1
#define STATUS_LOGGED       2
#define STATUS_PASV_ACTIVATED       3
#define STATUS_RNFR_ACTIVATED       4
#define STATUS_DATA_CONNECTION_ACCEPTED       5



// cac lenh trong ban tin FTP request
#define USER  "USER"
#define PASS  "PASS"
#define LIST  "LIST"
#define RETR  "RETR"
#define STOR  "STOR"
#define DELE  "DELE"
#define RNFR  "RNFR"
#define RNTO  "RNTO"
#define EPSV  "EPSV"
#define CWD  "CWD"
#define PWD "PWD"

#ifndef QUIT
    #define QUIT  "QUIT"
#endif // QUIT


struct FTPSessionInfo
{
    uint8_t  status;
    string username;
    string rnfr_name;
    string currentPath;
    string rootPath;
    string data;
    TcpSocket * slaveData;
    std::mutex mutex;
};

class FTPSession : public Session
{
    FTPSessionInfo* info;
    string response;
    void reset();
    FTPServerConfig* getServerConfig(){return (FTPServerConfig*)conf;}
public:
    FTPSession(TcpSocket* slave,ServerConfig* conf);
    ~FTPSession();
    const string& getResponse() const {return response;}
    void doUser(char* cmd_argv[], int cmd_argc);
    void doPass(char* cmd_argv[], int cmd_argc);
    void doPasv(char* cmd_argv[], int cmd_argc);
    void doList(char* cmd_argv[], int cmd_argc);
    void doRetr(char* cmd_argv[], int cmd_argc);
    void doStor(char* cmd_argv[], int cmd_argc);
    void doRnFr(char* cmd_argv[], int cmd_argc);
    void doRnTo(char* cmd_argv[], int cmd_argc);
    void doDele(char* cmd_argv[], int cmd_argc);
    void doCwd(char* cmd_argv[], int cmd_argc);
    void doPwd(char* cmd_argv[], int cmd_argc);
    void doQuit(char* cmd_argv[], int cmd_argc);
    void doNoop(char* cmd_argv[], int cmd_argc);
    void doUnknown(char* cmd_argv[], int cmd_argc);
    void createDataConnection();
    // void setCurrCmd(const char* cmdName);
};



#endif // FTPSESSION_H_INCLUDED

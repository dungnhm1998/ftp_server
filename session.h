#ifndef SESSION_H_
#define SESSION_H_
#include "tcpsocket.h"
#include "serverconfig.h"

class Session;
typedef void (Session::*CMD_FUNC)(char*[], int);

#define FUNC_CAST(p_func_type) static_cast<CMD_FUNC>(p_func_type)

#define SERVER_CMD_MAX_NUM 64
#define SERVER_CMD_ARG_LEN 64
#define SERVER_CMD_ARG_NUM 16
#define SERVER_CMD_BUF_LEN 256

#define SERVER_CMD_UNKNOWN  SERVER_CMD_MAX_NUM + 1


class Session
{
protected:
    ServerConfig* conf;
    TcpSocket* slave;
    bool quitSession;
private:

public:
    Session(TcpSocket* slave,ServerConfig* conf);
    virtual ~Session();
    virtual void doUnknown(char* cmd_argv[], int cmd_argc) = 0;
    virtual void reset() = 0;
    bool isQuit();
};


#endif // SESSION_H_

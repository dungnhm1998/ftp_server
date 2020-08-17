#include "session.h"

Session::Session(TcpSocket* slave, ServerConfig* conf)
{
    this->slave = slave;
    this->quitSession = false;
    this->conf = conf;
}
Session::~Session()
{
    delete slave;
}

bool Session::isQuit()
{
    return quitSession;
}



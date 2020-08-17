#ifndef SERVERCLI_H_INCLUDED
#define SERVERCLI_H_INCLUDED

#include "cli.h"
#include "ftpserver.h"
#include <windows.h>


//typedef struct _CONSOLE_FONT_INFOEX
//{
//    ULONG cbSize;
//    DWORD nFont;
//    COORD dwFontSize;
//    UINT  FontFamily;
//    UINT  FontWeight;
//    WCHAR FaceName[LF_FACESIZE];
//}CONSOLE_FONT_INFOEX, *PCONSOLE_FONT_INFOEX;
//the function declaration begins
#ifdef __cplusplus
extern "C" {
#endif
BOOL WINAPI SetCurrentConsoleFontEx(HANDLE hConsoleOutput, BOOL bMaximumWindow, PCONSOLE_FONT_INFOEX
lpConsoleCurrentFontEx);
BOOL WINAPI GetCurrentConsoleFontEx(HANDLE hConsoleOutput,BOOL bMaximumWindow,PCONSOLE_FONT_INFOEX lpConsoleCurrentFont);
#ifdef __cplusplus
}
#endif


// typedef void (SocketCmdLineInterface::*SOCK_CLI_CMD_FUNC)(char*[], int);

class ServerCLI : public CmdLineInterface
{
private:
    FTPServer* ftp;
public:
    ServerCLI();
    ~ServerCLI();
protected:
    virtual void initCmd();
    virtual void initConsole();
private:
    void doStart(char * cmd_argv[], int cmd_argc);
    void doStop(char * cmd_argv[], int cmd_argc);
    void doStatus(char * cmd_argv[], int cmd_argc);
    void doRestart(char * cmd_argv[], int cmd_argc);
    void doHelp(char * cmd_argv[], int cmd_argc);
};

#endif // SERVERCLI_H_INCLUDED

#include <iostream>
#include "servercli.h"

ServerCLI::ServerCLI():CmdLineInterface("server>")
{
    ftp = new FTPServer();
    initConsole();
    initCmd();

    if(ftp->start())
        cout << "FTP Server started" << endl;
    else
        cout << "FTP Server failed to start" <<endl;

}
ServerCLI::~ServerCLI()
{
    delete ftp;
}

void ServerCLI::initConsole()
{
//    system("color f0");
//    //_setmode(_fileno(stdout), _O_U16TEXT);
//    SetConsoleTitleW(L"Chương trình FTP Server");
//    HANDLE hdlConsole = GetStdHandle(STD_OUTPUT_HANDLE);
//    CONSOLE_FONT_INFOEX consoleFont;
//    consoleFont.cbSize = sizeof(consoleFont);
//    //COORD font_size = {11,24};
//    GetCurrentConsoleFontEx(hdlConsole, FALSE, &consoleFont);
//    memcpy(consoleFont.FaceName, L"Consolas", sizeof(consoleFont.FaceName));
//    consoleFont.dwFontSize = {11,24};
//    SetCurrentConsoleFontEx(hdlConsole, FALSE, &consoleFont);
}

void ServerCLI::initCmd()
{
    addCmd("start",CLI_CAST(&ServerCLI::doStart));
    addCmd("stop",CLI_CAST(&ServerCLI::doStop));
    addCmd("restart",CLI_CAST(&ServerCLI::doRestart));
    addCmd("status",CLI_CAST(&ServerCLI::doStatus));
    addCmd("help", CLI_CAST(&ServerCLI::doHelp));
}

void ServerCLI::doStart(char * cmd_argv[], int cmd_argc)
{
    if(ftp->isRunning())
        cout << "FTP server is running!" << endl;
    else if(ftp->start())
        cout << "FTP Server started" << endl;
    else
        cout << "FTP Server failed to start" <<endl;
}

void ServerCLI::doStop(char * cmd_argv[], int cmd_argc)
{
    ftp->stop();
    cout << "FTP Server stopped" << endl;
}



void ServerCLI::doRestart(char * cmd_argv[], int cmd_argc)
{

    if(ftp->restart())
        cout << "FTP Server restarted" << endl;
    else
        cout << "FTP Server failed to restart" <<endl;

}

void ServerCLI::doStatus(char * cmd_argv[], int cmd_argc)
{

    if(ftp->isRunning())
        cout << "FTP Server is running\n";
    else
        cout << "FTP Server is not running\n";

}

void ServerCLI::doHelp(char * cmd_argv[], int cmd_argc)
{
    cout << "Cac lenh cua chuong trinh:" << endl;
    cout << "- start               Bat FTP server" << endl;
    cout << "- stop                Tat FTP server" << endl;
    cout << "- status              Trang thai FTP server" << endl;
    cout << "- help                Tro giup" << endl;
    cout << "- quit                Thoat" << endl;
}

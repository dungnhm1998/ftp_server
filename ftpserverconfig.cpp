#include <fstream>
#include <cstring>
#include "ftpserverconfig.h"
#include "string.h"
#include <iostream>
FTPAccount::FTPAccount()
{
}
FTPAccount::FTPAccount(string username):Account(username)
{

}

FTPAccount::~FTPAccount()
{
}

string FTPAccount::getEmailAddress() const
{
    return this->username;
}


FTPServerConfig::FTPServerConfig()
{
}

FTPServerConfig::~FTPServerConfig()
{
}


bool FTPServerConfig::isValidEmailAccount(const string& email)
{
    return isValidUser(email);
}

bool FTPServerConfig::isAuthenticatedAccount(const string& username, const string& password)
{
    return authenticate(username, password);
}

void FTPServerConfig::setConnTimeout(unsigned short timeout)
{
    connTimeout = timeout;
}

void FTPServerConfig::setMailBox(const string& mailBox)
{
    this->mailBox = mailBox;
}
const string& FTPServerConfig::getMailBox() const
{
    return mailBox;
}
void FTPServerConfig::loadServerConfig(const string& confFileName)
{

}

void FTPServerConfig::loadAccountsFromFile(const string& fileName)
{
    ifstream f(fileName,ios::in);
    if(!f.good())
    {
        throw ServerConfigException("can not open FTP account list file\n");
    }
    else
    {
        char str[128];
        char username[128];
        char password[128];
        char rootPath[256];
        char *usrName, *pass, *homeDir;
        while(!f.eof())
        {
            f.getline(str,128);
            //xử lý chuỗi để lấy ra username & password, rootPath
            if(strlen(str)>0)
            {
                //xử lý ở đây
                usrName=strtok(str," ");
                pass=strtok(NULL," ");
                homeDir=strtok(NULL, "\r\n");
                strcpy(username, usrName);
                strcpy(password, pass);
                strcpy(rootPath, homeDir);
                FTPAccount* acc = new FTPAccount();
                acc->setUserName(username);
                acc->setPassword(password);
                addAccount(acc);
                addRootPath(username, rootPath);
            }
        }
        f.close();
    }
}

void FTPServerConfig::addRootPath(const string& username, const string& rootPath)
{
    this->user_home_dir_map.insert({username, rootPath});
//    cout<<"in kq sau khi add"<<endl;
//    for (auto const& x : user_home_dir_map)
//    {
//        cout << x.first<<" "<< x.second <<endl ;
//    }
}

string FTPServerConfig::getRootpath(const string& username) const
{
    unordered_map<string, string>::const_iterator got = user_home_dir_map.find(username);
    if (got == user_home_dir_map.end())
        return NULL;//NULL
    else
        return got->second;//got->second;
}

#ifndef FTPACCOUNT_H_
#define FTPACCOUNT_H_
#include "serverconfig.h"
#include <iterator>
#include <map>
class FTPAccount: public Account
{
private:
public:

    FTPAccount();
    FTPAccount(string username);
    ~FTPAccount();

    string getEmailAddress() const;

    //const string& getDomain() const;
};

class FTPServerConfig: public ServerConfig
{
private:
    unsigned short connTimeout = 0;
    string mailBox;
    unordered_map<string, string> user_home_dir_map;

public:
    FTPServerConfig();
    ~FTPServerConfig();
    void setConnTimeout(unsigned short timeout);
    void setMailBox(const string& mailBox);
    const string& getMailBox() const;
    void loadAccountsFromFile(const string& fileName);
    void loadServerConfig(const string& confFileName);
    bool isValidEmailAccount(const string& email);
    bool isAuthenticatedAccount(const string& username, const string& password);
    string getRootpath(const string& username) const;
    void addRootPath(const string&, const string&);
};


#endif // FTPACCOUNT_H_

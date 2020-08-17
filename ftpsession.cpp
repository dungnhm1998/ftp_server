#include <cstring>
#include <algorithm>    // std::find
#include <fstream>
#include <iostream>
#include "ftpsession.h"
#include "ftpserverconfig.h"
#include "random.h"
#include <dirent.h>
#include <string.h>
#include <thread>
#include <fstream>
#include <windows.h>
FTPSession::FTPSession(TcpSocket* slave, ServerConfig* conf): Session(slave, conf) {
    this->info = new FTPSessionInfo();
    this->quitSession = false;
    this->info->status = STATUS_INIT;
    this->info->currentPath = "/";
}

void FTPSession::reset() {
    this->info->status = STATUS_INIT;
    this->info->currentPath = "/";
}

FTPSession::~FTPSession() {
    delete info;
}

void FTPSession::doUser(char* cmd_argv[], int cmd_argc) {
    //hung
    //check status hiện tại, nếu đã gửi lệnh user(status=STATUS_USERNAME) thì gửi lại phản hồi yêu cầu nhập password...
    if(cmd_argc == 2) {
        this->info->username = cmd_argv[1];

        this->info->status = STATUS_USERNAME;
        response = "331 Password required for " + (string)cmd_argv[1] + "\r\n";
    } else {
        response = "501 Syntax error\r\n";
    }
    slave->send(response);

}

void FTPSession::doPass(char* cmd_argv[], int cmd_argc) {
    //hùng
    //nếu xác thực thành công thì thay đổi ->status=STATUS_LOGGED
    if(cmd_argc <= 2) {
        if(this->info->status == STATUS_USERNAME) {
            string password = "";
            if(cmd_argc == 2) {
                password = cmd_argv[1];
            }
            // check whether user and domain name is valid or not
            FTPServerConfig* ftpConf = dynamic_cast<FTPServerConfig*>(this->conf);
            if(ftpConf->isAuthenticatedAccount(this->info->username, password)) {
                response = "230 Logged on\r\n";
                info->status = STATUS_LOGGED;
                info->rootPath = ftpConf->getRootpath(this->info->username);
            } else {
                response = "530 Login or password incorrect!\r\n";
            }
        } else {
            response = "530 Login or password incorrect!\r\n";
        }
    } else {
        response = "501 Syntax error\r\n";
    }
    slave->send(response);

}

void FTPSession::createDataConnection() {
    TcpServerSocket master;
    if (this->info->status == STATUS_LOGGED || this->info->status == STATUS_PASV_ACTIVATED) {
        try {
            //this->info->master.close();
            srand(time(NULL));
            unsigned short newPort = rand() % (65534 - 49152 + 1) + 49152;
            master.setListen(newPort);
            this->info->status = STATUS_PASV_ACTIVATED;
            response = "Entering Extended Passive Mode (|||";
            response.append(to_string(newPort));
            response.append("|).\r\n");
            slave->send(response);
            //master wait client connect to data connection port
            this->info->slaveData = master.accept();
            cout << "data connection established" << endl;
            this->info->status = STATUS_DATA_CONNECTION_ACCEPTED;

        } catch(SocketException& e) {
            //cerr << e.what() << endl;
            this->createDataConnection();//find another port
        }

    } else {
        response = "530 Please log in with USER and PASS first.\r\n";
        slave->send(response);
    }

}

void FTPSession::doPasv(char* cmd_argv[], int cmd_argc) {
    //this->info->mutex.lock();
    std::thread t(&FTPSession::createDataConnection, this);
    t.detach();
    //this->info->mutex.unlock();
}

void FTPSession::doList(char* cmd_argv[], int cmd_argc) {
    if (this->info->status == STATUS_LOGGED || this->info->status == STATUS_PASV_ACTIVATED || this->info->status == STATUS_DATA_CONNECTION_ACCEPTED) {
        if (this->info->status == STATUS_PASV_ACTIVATED || this->info->status == STATUS_DATA_CONNECTION_ACCEPTED) {
            Sleep(1000);
            if (this->info->status == STATUS_DATA_CONNECTION_ACCEPTED) {
                //slave->send("kenh control");
                struct dirent *entry;
                string path = this->info->rootPath + this->info->currentPath;
                response = "";
                DIR *dir = opendir(path.c_str());
                //string str1="";
                if (dir == NULL) {
                    return;
                }
                while ((entry = readdir(dir)) != NULL) {
                    response += (string)(entry->d_name) + "\r\n";
                }
                closedir(dir);
                this->info->slaveData->send(response);

                this->info->slaveData->close();
                response = "150 Opening data channel for directory listing of \"";
                response.append(this->info->currentPath);
                response.append("\"\r\n");
                response.append("226 Successfully transferred.\r\n");
                this->info->status = STATUS_LOGGED;
            } else {
                response = "425 Can't open data connection for transfer of  \"";
                response.append(this->info->currentPath);
                response.append("\"\r\n");
                this->info->status = STATUS_LOGGED;
            }

        } else {
            response = "503 Bad sequence of commands.\r\n";
        }
        this->info->status = STATUS_LOGGED;
    } else {
        response = "530 Please log in with USER and PASS first.\r\n";
    }
    slave->send(response);

}

void FTPSession::doRetr(char* cmd_argv[], int cmd_argc) {
    if (cmd_argc == 2) {
        if (this->info->status == STATUS_LOGGED || this->info->status == STATUS_PASV_ACTIVATED || this->info->status == STATUS_DATA_CONNECTION_ACCEPTED) {
            if (this->info->status == STATUS_PASV_ACTIVATED || this->info->status == STATUS_DATA_CONNECTION_ACCEPTED) {
                if (this->info->status == STATUS_DATA_CONNECTION_ACCEPTED) {
                    response = "";
                    string path = this->info->rootPath + this->info->currentPath + "/" + cmd_argv[1];
                    cout << path << endl;
                    //---------------
                    fstream f;
                    f.open(path, ios::in);
                    if(f) {
                        string line;
                        response = "";
                        while (!f.eof()) {
                            getline(f, line);
                            response += line + "\r\n";
                        }
                        this->info->data = response;
                        f.close();
                        this->info->slaveData->send(response);

                        response = "150 Opening data channel for file download from server of \"/";
                        response.append(cmd_argv[1]);
                        response.append("\".\r\n");
                        response.append("226 Successfully transferred \"/");
                        response.append(cmd_argv[1]);
                        response.append("\".\r\n");
                    } else {
                        response = "550 File not found.\r\n";
                    }
                    this->info->slaveData->close();
                } else {
                    response = "425 Can't open data connection for transfer of  \"";
                    response.append(this->info->currentPath);
                    response.append("\"\r\n");
                }

            } else {
                response = "503 Bad sequence of commands.\r\n";
            }
            this->info->status = STATUS_LOGGED;
        } else {
            response = "530 Please log in with USER and PASS first.\r\n";
        }

    } else {
        response = "501 Syntax error.\r\n";
    }
    slave->send(response);
}

void FTPSession::doStor(char* cmd_argv[], int cmd_argc) {
    if (cmd_argc == 2 ) {
        if (this->info->status == STATUS_LOGGED || this->info->status == STATUS_PASV_ACTIVATED || this->info->status == STATUS_DATA_CONNECTION_ACCEPTED) {
            if (this->info->status == STATUS_PASV_ACTIVATED || this->info->status == STATUS_DATA_CONNECTION_ACCEPTED) {
                if (this->info->status == STATUS_DATA_CONNECTION_ACCEPTED) {
                    response = "150 Opening data channel for upload file to server of \"/";
                    response.append(cmd_argv[1]);
                    response.append("\".\r\n");
                    slave->send(response);
                    string path = this->info->rootPath + this->info->currentPath + "/" + cmd_argv[1];
                    cout << path << endl;
                    //---------------
                    fstream f;
                    f.open(path, ios::out);
                    char buf[256];
                    int byte_recv = this->info->slaveData->recvLine(buf, 256);
                    while(byte_recv > 2) {
                        f << buf;
                        memset(buf, 0, sizeof(buf));//reset buf's value
                        byte_recv = this->info->slaveData->recvLine(buf, 256);
                    }
                    //
                    f.close();
                    //------------------

                    this->info->slaveData->close();


                    response = "226 Successfully transferred \"/";
                    response.append(cmd_argv[1]);
                    response.append("\".\r\n");
                } else {
                    response = "425 Can't open data connection for transfer of  \"";
                    response.append(this->info->currentPath);
                    response.append("\"\r\n");
                }
            } else {
                response = "503 Bad sequence of commands.\r\n";
            }
            this->info->status = STATUS_LOGGED;
        } else {
            response = "530 Please log in with USER and PASS first.\r\n";
        }
    } else {
        response = "501 Syntax error.\r\n";
    }
    slave->send(response);
}

bool checkExist(const char* path) {
    DIR*  dir = opendir(path);
    FILE* p = fopen(path, "r");
    bool res;
    if(dir == NULL && p == NULL) res = false;
    else res = true;
    fclose(p);
    closedir(dir);
    return res;

}

string backPath(string Path) {
    char* token;
    token = strtok(strdup(Path.c_str()), "/");
    string h[20];
    int n = 0;
    while( token != NULL ) {
        h[n] = token;
        token = strtok(NULL, "/");
        n++;
    }
    string myPath = "";
    if(n == 1) myPath = "/";
    else
        for(int i = 0; i < n - 1; i++) {
            myPath += "/" + h[i];
        }
    return myPath;
}

string getPath(string Path, string currentPath) {
    string myPath = "";
    int n = strlen(currentPath.c_str());
    if (strncmp(Path.c_str(), currentPath.c_str(), n) == 0) {
        myPath += Path;
    } else {
        if (strncmp(Path.c_str(), "/", 1) == 0) {
            if(strlen(Path.c_str()) == 1) myPath = "/";
            else myPath = currentPath + Path;

        } else {
            if(strncmp(Path.c_str(), "..", 2) == 0) myPath = backPath(currentPath);
            else if (currentPath == "/")
                myPath = currentPath  + Path;
            else   myPath = currentPath + "/" + Path;

        }
    }
    return myPath;
}

void FTPSession::doRnFr(char* cmd_argv[], int cmd_argc) {
    if(cmd_argc == 2) {
        if(this->info->status == STATUS_LOGGED || this->info->status == STATUS_RNFR_ACTIVATED || this->info->status == STATUS_PASV_ACTIVATED) {
            string path = this->info->rootPath + getPath((string)cmd_argv[1], this->info->currentPath);
            bool is_exist = checkExist(path.c_str());
            if(is_exist == true) {
                info->rnfr_name = path;
                info->status = STATUS_RNFR_ACTIVATED;
                response = "350 Ready for destination name " + (string)cmd_argv[1] + "\r\n";
            } else {
                response = "550 file or folder is not exists\r\n";
            }

        } else {
            response = "530 Please log in with USER and PASS first\r\n";
        }

    } else {
        response = "501 Syntax error\r\n";
    }

    slave->send(response);
}

void FTPSession::doRnTo(char* cmd_argv[], int cmd_argc) {
    if(cmd_argc == 2) {
        if(this->info->status == STATUS_RNFR_ACTIVATED) {
            string path1 = this->info->rnfr_name;
            string path2 = this->info->rootPath + getPath((string)cmd_argv[1], this->info->currentPath);
            int RET = rename(path1.c_str(), path2.c_str());
            if(RET == 0) {
                response = "250 Rename successfully!\r\n";
                info->status = STATUS_LOGGED;
            } else {
                response = "550 Rename fail, file or folder is not exists\r\n";
                info->status = STATUS_LOGGED;
            }
        } else {
            if(this->info->status == STATUS_LOGGED) {
                response = "503 Bad sequence of commands!\r\n";
            } else response = "530 Please log in with USER and PASS first\r\n";

        }
    } else {
        response = "501 Syntax error\r\n";
    }

    slave->send(response);
}


void FTPSession::doPwd(char* cmd_argv[], int cmd_argc) {
    if(this->info->status == STATUS_LOGGED) {
        response = "257 \"";
        response.append(this->info->currentPath);
        response.append("\" is current directory.\r\n");

    } else {
        response = "530 Please log in with USER and PASS first\r\n";
    }

    slave->send(response);

}

void FTPSession::doCwd(char* cmd_argv[], int cmd_argc) {
    if(cmd_argc == 2) {
        if(this->info->status == STATUS_LOGGED || this->info->status == STATUS_PASV_ACTIVATED) {
            string path = this->info->rootPath + getPath((string)cmd_argv[1], this->info->currentPath);
            DIR* dir = opendir(path.c_str());
            if(dir == NULL) {
                response = "550 folder is not exist!\r\n";
            } else {
                info->currentPath = getPath((string)cmd_argv[1], this->info->currentPath);
                response = "250 CWD successfully! " + this->info->currentPath + "\r\n";
            }
            closedir(dir);
        } else {
            response = "530 Please log in with USER and PASS first\r\n";
        }
    } else {
        response = "501 Syntax error\r\n";
    }

    slave->send(response);
}

void FTPSession::doDele(char* cmd_argv[], int cmd_argc) {
    //hùng
    if(cmd_argc == 2) {
        if(this->info->status == STATUS_LOGGED) {
            string fileName = cmd_argv[1];
            string path = this->info->rootPath + this->info->currentPath + "/" + fileName;
            if( remove(path.c_str()) != 0 ) {
                response = "550 File not found\r\n";
            } else {
                response = "250 File deleted successfully\r\n";

            }
        } else {
            response = "530 Please log in with USER and PASS first.\r\n";
        }
    } else {
        response = "501 Syntax error\r\n";
    }
    slave->send(response);

}

void FTPSession::doNoop(char* cmd_argv[], int cmd_argc) {
    response = "250 No operation\r\n";
    slave->send(response);
}

void FTPSession::doQuit(char* cmd_argv[], int cmd_argc) {
    response = "221 Good bye\r\n";
    slave->send(response);
    quitSession = true;
}
void FTPSession::doUnknown(char* cmd_argv[], int cmd_argc) {
    response = "500 Syntax error, command unrecognized\r\n";
    slave->send(response);
}



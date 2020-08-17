#include <iostream>
#include "servercli.h"

using namespace std;

int main()
{
    cout << "Hello FTP world!" << endl;
    ServerCLI cli;
    cli.run();
    return 0;
}

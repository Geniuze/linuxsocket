#include <getopt.h>
#include <stdio.h>
#include <iostream>
#include <string.h>

using namespace std;
int f_tcpserver();
int f_tcpclient();
const char *short_options = "hc:";
struct option long_options[] =
{
    {"commond", 1, NULL, 'c'},
    {"help",    0, NULL, 'h'},
    {0,0,0,0},
};

void help()
{
    cout << "help:" << endl;
    cout << "-c or --commond tcpserver/tcpclient start a tcpserver" << endl;
    cout << "-h or --help show help" << endl;
}
int main(int argc, char **argv)
{
    int c;
    while(-1 != (c = getopt_long(argc, argv, short_options, long_options, NULL)))
    {
        switch(c)
        {
        case 'c':
            if (!strcasecmp(optarg, "tcpserver") || !strcasecmp(optarg, "ts"))
            {
                f_tcpserver();
            }
            if (!strcasecmp(optarg, "tcpclient") || !strcasecmp(optarg, "tc"))
            {
                f_tcpclient();
            }
            else
            {
                help();
            }
            break;
        default:
            help();
            break;
        }
    }
    return 0;
}


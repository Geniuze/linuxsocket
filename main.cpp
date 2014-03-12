#include <getopt.h>
#include <stdio.h>
#include <iostream>
#include <string.h>

using namespace std;
int f_tcpserver();
int f_tcpclient();
int f_localtcpserver();
int f_localtcpclient();
int f_udpserver();
int f_udpclient();
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
    cout << "-c or --commond tcpserver/tcpclient/localtcpserver/localtcpclient/udpclient/udpserver start a tcpserver" << endl;
    cout << "-h or --help show help" << endl;
}
int main(int argc, char **argv)
{
    if (1 == argc)
        help();
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
            else if (!strcasecmp(optarg, "tcpclient") || !strcasecmp(optarg, "tc"))
            {
                f_tcpclient();
            }
            else if (!strcasecmp(optarg, "localtcpserver") || !strcasecmp(optarg, "lts"))
            {
                f_localtcpserver();
            }
            else if (!strcasecmp(optarg, "localtcpclient") || !strcasecmp(optarg, "ltc"))
            {
                f_localtcpclient();
            }
            else if (!strcasecmp(optarg, "udpclient") || !strcasecmp(optarg, "uc"))
            {
                f_udpclient();
            }
            else if (!strcasecmp(optarg, "udpserver") || !strcasecmp(optarg, "us"))
            {
                f_udpserver();
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


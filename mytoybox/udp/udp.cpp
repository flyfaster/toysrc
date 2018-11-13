//============================================================================
// Name        : udpclient.cpp
// Author      : Onega Zhang
// Version     :
// Copyright   : Your copyright notice
// Description : UDP server and client program
//============================================================================

#include <iostream>
using namespace std;
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <chrono>

unsigned short udp_port = 32000;

int server_main(int argc, char** argv)
{
    int sockfd, n;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len = sizeof(cliaddr);
    char mesg[1000];
    std::cout << __func__ << " recvfrom port " << udp_port << std::endl;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    servaddr.sin_port = htons(udp_port);
    bind(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr));
    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 10;
    int res = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    if (res < 0)
    {
        std::cerr << __func__ << " failed to set timeout for socket, setsockopt return "
                  << res << "\n";
        return res;
    }
//    sleep(10);
    std::cout << "start recv data on port " << udp_port << "\n";
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 200; i++)
    {
        len = sizeof(cliaddr);
        n = recvfrom(sockfd, mesg, 1000, 0, (struct sockaddr*) &cliaddr, &len);
        if (n < 0)
        {
            std::cout << __func__ << " timeout, return " << n << " for message " << i
                      << std::endl;
            continue;
        }
        mesg[n] = 0;
        printf("Received the following:\n%s\n", mesg);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> duration = end - start;
    std::chrono::milliseconds msduration =
        std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    std::cout << __func__ << " used " << msduration.count() << " milliseconds" << endl;
    return 0;
}

int client_main(int argc, char** argv)
{
    int sockfd, n;
    struct sockaddr_in servaddr, cliaddr;
    char sendline[1000];
    if (argc != 2)
    {
        printf("usage: %s <IP address>\n", argv[0]);
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(udp_port);
    for (int i = 0; i < 200; i++)
    {
        int datalen = sprintf(sendline,
                              "line%04d, make it long long long long long long long long "
                              "long long long long long long long long long long long "
                              "long long to use more space",
                              i);
        int actualsent = sendto(sockfd, sendline, strlen(sendline), 0,
                                (struct sockaddr*) &servaddr, sizeof(servaddr));
        if (actualsent < datalen)
            std::cout << __func__ << " only sent " << actualsent << " for " << sendline
                      << endl;
    }
    std::cout << __func__ << " exit now" << endl;
    return 0;
}

int main(int argc, char** argv)
{
    std::cout << argv[0] << "\t pid " << getpid() << std::endl;
    int ret = 0;
    auto start = std::chrono::high_resolution_clock::now();
    if (argc < 2) // server don't need argument
        ret = server_main(argc, argv);
    else
        ret = client_main(argc, argv); // client need ip address.
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> duration = end - start;
    std::chrono::milliseconds msduration =
        std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    std::cout << argv[0] << " used " << msduration.count() << " milliseconds" << endl;
    return ret;
}

// g++ -o udp -std=c++11 udp.cpp
// open 2 terminals
// terminal 1: ./udp
// terminal 2: ./udp 127.0.0.1
// client_main used 2~3 ms, server_main used 10112 ms
// client_main finished before server_main wake up, but server_main can receive 167 messages
// recvfrom can't timeout in 10us, it is about 1ms per call.

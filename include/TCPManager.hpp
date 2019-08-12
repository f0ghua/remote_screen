/*
 * Author:        Hutao {hutaonice@gmail.com}
 * Creation:      星期四 08/08/2019 10:09.
 * Last Revision: 星期四 08/08/2019 10:09.
 */                     

#ifndef TCPSENDER_H
#define TCPSENDER_H


#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/shm.h>

#define min(a, b) ((a)<(b)?(a):(b))
#define max(a, b) ((a)>(b)?(a):(b))

typedef int SOCKET;

class TCPManager
{
private:
    SOCKET sender, recver;
    const int targetport = 25610;
    const int pack_size = 1024;
    std::string target_address;
    bool connected;
    bool isSending;

    sockaddr_in addr;

public:
    TCPManager();
    ~TCPManager();
    bool ConnectToServer();
    inline bool isConnected(){return connected;}
    bool initServer();
    bool initClient();

    void SendBuffer(void* buffer, unsigned int size);
    void RecvBuffer(void* buffer, unsigned int* size);

    void Send_Input(unsigned char* p, int sz){
        send(sender, p, sz, 0);
    }
    long long Recv_Input(){
        long long ret = 0;
        unsigned char* p =(unsigned char*)&ret;
        unsigned char t;
        recv(sender, p, 1, 0);
        t = p[0];

        if((t>>7)&1){
            // KeyBoard Input
            recv(sender, p+1, 1, 0);
            return ret;
            if((t>>6)&1){
                //Release
            }
            else {
                //Press
            }
        }
        else{
            // Mouse Input
            if((t>>6)&1){
                //Mouse Move
                recv(sender, p+1, 4, 0);
            }
            else{
                // Mouse Button
            }
            return ret;
        }
    }
};

#endif   /* End of TCPSENDER.H */





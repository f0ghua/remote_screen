/*
 * File Type:     C/C++
 * Author:        Hutao {hutaonice@gmail.com}
 * Creation:      星期四 08/08/2019 11:08.
 * Last Revision: 星期五 09/08/2019 17:19.
 */
#include <TCPManager.hpp>
using std::cout;
using std::endl;

typedef unsigned char uchar;

TCPManager::TCPManager(){
    target_address = "49.235.201.74";
    sender = socket(AF_INET, SOCK_STREAM, 0);
    recver = socket(AF_INET, SOCK_STREAM, 0);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(target_address.c_str());
	addr.sin_port = htons(targetport);

    connected = false;
    isSending = false;
}

TCPManager::~TCPManager(){
    shutdown(sender, SHUT_RDWR);
    close(sender);
}

bool TCPManager::ConnectToServer(){
    int ret = connect(sender, (sockaddr*)&addr, sizeof(addr));
    if(ret) cout<<"connect error"<<endl;
    else cout<<"connect success"<<endl;
    connected = ret == 0;
    return ret == 0;
}

void TCPManager::SendBuffer(void* buffer, unsigned int size){
    int parts = size/pack_size + (size%pack_size > 0);
    uchar* ptr = (uchar*)buffer;
    send(sender, &size, sizeof(size), 0);
    int ret;
    while(size > 0){
        int cur_sz = min(pack_size, size);
        ret = send(sender, ptr, cur_sz, 0);
        if(ret <= 0) exit(0);
        size -= cur_sz;
        ptr += cur_sz;
    }
}

void TCPManager::RecvBuffer(void* buffer, unsigned int* size){
    recv(sender, size, sizeof(*size), 0);
    //cout<<*size<<endl;
    int parts = *size / pack_size + (*size%pack_size > 0);
    uchar* ptr = (uchar*)buffer;

    while(*size > 0){
        int cur_sz = min(pack_size, *size);
        int ret = recv(sender, ptr, cur_sz, 0);
        if(ret < 0) exit(0);
        *size -= ret;
        ptr += ret;
    }
}

bool TCPManager::initServer(){
    if(!ConnectToServer()) return false;
    send(sender, "S", 1, 0);
}

bool TCPManager::initClient(){
    if(!ConnectToServer()) return false;
    send(sender, "C", 1, 0);
}
/* EOF */


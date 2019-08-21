/*
 * File Type:     C/C++
 * Author:        Hutao {hutaonice@gmail.com}
 * Creation:      星期四 08/08/2019 11:08.
 * Last Revision: 星期二 13/08/2019 23:24.
 */
#include <TCPManager.hpp>
using std::cout;
using std::endl;
using std::cin;

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

void TCPManager::clean(){
    shutdown(sender, SHUT_RDWR);
    close(sender);
    connected = false;
}

bool TCPManager::ConnectToServer(){
    sender = socket(AF_INET, SOCK_STREAM, 0);
    int ret = connect(sender, (sockaddr*)&addr, sizeof(addr));
    if(ret) cout<<"connect error"<<endl;
    else cout<<"connect success"<<endl;
    connected = ret == 0;
    return ret == 0;
}

bool TCPManager::SendBuffer(void* buffer, unsigned int size){
    int parts = size/pack_size + (size%pack_size > 0);
    uchar* ptr = (uchar*)buffer;
    send(sender, &size, sizeof(size), 0);
    int ret;
    while(size > 0){
        int cur_sz = min(pack_size, size);
        ret = send(sender, ptr, cur_sz, 0);
        if(ret <= 0){
            connected = false;
            return false;
        }
        size -= cur_sz;
        ptr += cur_sz;
    }
    return true;
}

bool TCPManager::RecvBuffer(void* buffer, unsigned int* size){
    recv(sender, size, sizeof(*size), 0);
    //cout<<*size<<endl;
    int parts = *size / pack_size + (*size%pack_size > 0);
    uchar* ptr = (uchar*)buffer;

    while(*size > 0){
        int cur_sz = min(pack_size, *size);
        int ret = recv(sender, ptr, cur_sz, 0);
        if(ret < 0){
            connected = false;
            return false;
        }
        *size -= ret;
        ptr += ret;
    }
    return true;
}

void TCPManager::Send_Input(unsigned char* p, int sz)
{
    send(sender, p, sz, 0);
}

long long TCPManager::Recv_Input(){
    long long ret = 0;
    unsigned char* p = (unsigned char*)&ret;
    unsigned char t = 0;
    recv(sender, &t, 1, 0);
    p[0] = t;
    if((t>>7)&1){
        recv(sender, p+1, 1, 0);
    }
    else{
        if((t>>6)&1){
            recv(sender, p+1, sizeof(short)*2, 0);
        }
    }
    return ret;
}

bool TCPManager::initServer(std::string id){
    if(!ConnectToServer()) return false;
    send(sender, "S", 1, 0);
    int sz = id.length();
    send(sender, &sz, sizeof(int), 0);
    send(sender, id.c_str(), sz, 0);
    char stat = 0;
    while(stat != 'b'){
        int ret = recv(sender, &stat, 1, 0);
        if(ret < 1){
            connected = false;
            shutdown(sender, SHUT_RDWR);
            close(sender);
            return false;
        }
        if(stat == 'r'){
            cout<<"the id: "<<id<<" has been occupied."<<endl;
            exit(0);
        }
    }
    return true;
}

bool TCPManager::initClient(){
    if(!ConnectToServer()) return false;
    send(sender, "C", 1, 0);
    int n;
    recv(sender, &n, sizeof(int), 0);
    cout<<"Current online servers: "<<n<<endl;
    char *id = new char[128];
    for(int i=0;i<n;i++){
        int sz;
        recv(sender, &sz, sizeof(int), 0);
        recv(sender, id, sz, 0);
        id[sz] = 0;
        cout<<i<<" "<<id<<endl;
    }
    int ind;
    cout<<"the index of your choice:"<<endl;
    cin>>ind;
    send(sender, &ind, sizeof(int), 0);
    recv(sender, id, 1, 0);
    if(id[0] == 'n'){
        delete id;
        return false;
    }
    return true;
}
/* EOF */


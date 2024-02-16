#pragma once
#include<iostream>
#include<cstring>
#include<sys/socket.h>
#include<netinet/in.h>

using namespace std;

#define PORT 8888
#define LISTEN_NUM 20
#define MAX_MESSAGE_LENGTH 1024

class SOCKET_{
private:
    struct sockaddr_in server_addr,client_addr;
    //TCP
    int tcp_socket,tcp_conn;
    //UDP
    int udp_socket;
    //Mutual
    int len;
    int send_num,recv_num;
    char send_buff[MAX_MESSAGE_LENGTH],recv_buff[MAX_MESSAGE_LENGTH];

public:
    SOCKET_();
    int TCP_init();
    int TCP_conn();
    int TCP_recv(int);
    int TCP_send(int,const char*,string,const char*);
    void TCP_close();
    void TCP_disconn();

    int UDP_init();
    int UDP_recv();
    void UDP_send(const char*,string);

    int get_TCP_conn();
    string get_send_buff();
    string get_recv_buff();
};

SOCKET_::SOCKET_(){
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family=AF_INET;
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    server_addr.sin_port=htons(PORT);
    len=sizeof(server_addr);
}
int SOCKET_::TCP_init(){
    tcp_socket=socket(AF_INET,SOCK_STREAM,0);
    if(tcp_socket==-1){
        cout<<"tcp socket error\n";
        return 0;
    }
    int enable=1;
    if (setsockopt(tcp_socket,SOL_SOCKET,SO_REUSEADDR,&enable,sizeof(int))<0){
        cout<<"SO_REUSEADDR failed\n";
        return 0;
    }
    if(bind(tcp_socket,(struct sockaddr*)&server_addr,len)==-1){
        cout<<"tcp bind error\n";
        return 0;
    }
    if(listen(tcp_socket,LISTEN_NUM)==-1){
        cout<<"tcp listen error\n";
        return 0;
    }
    return 1;
}
int SOCKET_::TCP_conn(){
    tcp_conn=accept(tcp_socket,(struct sockaddr*)&client_addr,(socklen_t*)&len);
    if(tcp_conn==-1){
        cout<<"tcp accept error\n";
        return 0;
    }
    //cout<<"New connection\n";
    return 1;
}
int SOCKET_::TCP_recv(int conn){
    memset(&recv_buff,0,sizeof(recv_buff));
    recv_num=recv(conn,recv_buff,MAX_MESSAGE_LENGTH,0);
    if(recv_num==0){
        //cout<<"Disconnected\n";
        return 0;
    }
    recv_buff[recv_num]='\0';
    return 1;
}
int SOCKET_::TCP_send(int conn,const char* reply,string str,const char* tail){
    memset(&send_buff,0,sizeof(send_buff));
    strcpy(send_buff,reply);
    if(str!=""){
        int i=strlen(send_buff);
        int j=0;
        int t=strlen(send_buff)+str.size();
        for(;i<t;i++,j++){
            send_buff[i]=str[j];
        }
    }
    if(tail!=""){
        int i=strlen(send_buff);
        int j=0;
        int t=strlen(send_buff)+strlen(tail);
        for(;i<t;i++,j++){
            send_buff[i]=tail[j];
        }
    }
    send_num=send(conn,send_buff,strlen(send_buff),0);
    if(send_num==-1){
        cout<<"tcp send error\n";
        return 0;
    }
    return 1;
}
void SOCKET_::TCP_close(){
    close(tcp_socket);
}
void SOCKET_::TCP_disconn(){
    close(tcp_conn);
}

int SOCKET_::UDP_init(){
    udp_socket=socket(AF_INET,SOCK_DGRAM,0);
    if(udp_socket==-1){
        cout<<"udp soket error\n";
        return 0;
    }
    int enable=1;
    if (setsockopt(udp_socket,SOL_SOCKET,SO_REUSEADDR,&enable,sizeof(int))<0){
        cout<<"SO_REUSEADDR failed\n";
        return 0;
    }
    if(bind(udp_socket,(struct sockaddr*)&server_addr,len)==-1){
        cout<<"udp bind error\n";
        return 0;
    }
    return 1;
}
int SOCKET_::UDP_recv(){
    memset(&recv_buff,0,sizeof(recv_buff));
    recv_num=recvfrom(udp_socket,recv_buff,sizeof(recv_buff),0,(struct sockaddr*)&client_addr,(socklen_t*)&len);
    if(recv_num==-1){
        cout<<"udp recv error\n";
        return 0;
    }
    recv_buff[recv_num]='\0';
    return 1;
}
void SOCKET_::UDP_send(const char* reply,string str){
    memset(&send_buff,0,sizeof(send_buff));
    strcpy(send_buff,reply);
    if(str!=""){
        int i=strlen(send_buff);
        int j=0;
        int t=strlen(send_buff)+str.size();
        for(;i<t;i++,j++){
            send_buff[i]=str[j];
        }
    }
    send_num=sendto(udp_socket,send_buff,strlen(send_buff),0,(struct sockaddr*)&client_addr,len);
    if(send_num==-1){
        cout<<"udp send error\n";
    }
    return;
}

int SOCKET_::get_TCP_conn(){
    return tcp_conn;
}
string SOCKET_::get_send_buff(){
    return recv_buff;
}
string SOCKET_::get_recv_buff(){
    return recv_buff;
}

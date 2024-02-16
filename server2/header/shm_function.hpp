#pragma once
#include<iostream>
#include<cstring>
#include<string>
#include<map>
#include<sys/socket.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<fstream>

#include"game.hpp"

using namespace std;

#define invite_code_size 16
#define small_size 32
#define medium_size 128
#define buff_size 4096
#define send_size 1024
#define invitation_size 16
#define room_id_size 16
#define ans_size 4

struct invitations_{
    bool use;//register時候初始化
    char inviter[medium_size];//使用前初始化
    char inviter_email[medium_size];//使用前初始化
    char room_id[medium_size];//使用前初始化
    char in_code[medium_size];//使用前初始化
};

struct users_{
    bool reg;//一開始就初始化
    bool login_condition;//register後初始化
    bool in_room;//register後初始化
    char username[medium_size];//register後初始化再使用
    char email[medium_size];//register後初始化再使用
    char password[medium_size];//register後初始化再使用
    int invitations;//register後初始化
    invitations_ invitation[invitation_size];//每一個格子都是一個invite
    int tcp_conn;//register後初始化 & login後綁定
};

struct players_{
    bool use;//創建房間後初始化
    bool online;//有人加入後初始化在使用
    char username[medium_size];//有人加入後初始化在使用
};
struct rooms_{
    bool exist;//一開始就初始化
    bool private_room;//初始化時使用
    char manager[medium_size];//初始化再使用
    char room_id[room_id_size];//初始化再使用
    char in_code[invite_code_size];//初始化再使用 如果是private room的話
    bool playing;//初始化 玩遊戲的時候使用
    char current[medium_size];//創建的時候初始化 每一次換人都要初始化
    char ans[ans_size];//開始遊戲的時候初始化
    int rounds;//開始遊戲的時候直接設定
    players_ players[small_size];
};

struct share_mem{
    users_ user[small_size];//每個格子都是一個user的資料
    rooms_ room[small_size];//又分為public和private，其中每一個格子都是一個那個形態的房間
    int num_users;//記錄現在有幾個人註冊
    int num_rooms;//記錄現在有幾間房間
};

struct unit_room_{
    bool privat;
    bool playing;
}unit_room;
map<string,unit_room_> room_list;
map<string,unit_room_>::iterator itt;

struct unit_user_{
    bool online;
    string email;
}unit_user;
map<string,unit_user_> user_list;
map<string,unit_user_>::iterator ittt;

struct unit_invitation_{
    string inviter;
    string inviter_email;
    string in_code;
}unit_invitation;
map<string,unit_invitation_> invitation_list;
map<string,unit_invitation_>::iterator itttt;

void init_share_mem(int &shmid,key_t key){
    shmctl(shmid,IPC_RMID,NULL);
    shmid=shmget(key,sizeof(share_mem),0666|IPC_CREAT);
    share_mem*ptr=(share_mem*)shmat(shmid,NULL,0);
    users_*users=ptr->user;
    for(int i=0;i<small_size;i++){
        users[i].reg=false;
        for(int j=0;j<invitation_size;j++){
            users[i].invitation[j].use=false;
        }
    }
    rooms_*rooms=ptr->room;
    for(int i=0;i<small_size;i++){
        rooms[i].exist=false;
    }
    ptr->num_users=0;
    ptr->num_rooms=0;
    shmdt(ptr);
}

bool cmp_char_arr(char*arr1,char*arr2){
    if(strlen(arr1)!=strlen(arr2)){
        return false;
    }
    for(int i=0;i<strlen(arr1);i++){
        if(arr1[i]!=arr2[i]){
            return false;
        }
    }
    return true;
}

int register_command(string input,int shmid){
    string username="",email="",password="",format="";
    int condition=0;
    for(int i=0;i<input.length();i++){
        if(input[i]==' '){
            condition++;
            continue;
        }
        else if(input[i]=='\n'){
            break;
        }
        else{
            if(condition==0){
                continue;
            }
            else if(condition==1){
                username+=input[i];
            }
            else if(condition==2){
                email+=input[i];
            }
            else if(condition==3){
                password+=input[i];
            }
            else{
                format+=input[i];
            }
        }
    }
    if(username==""||email==""||password==""||format!=""){
        return 0;
    }
    
    share_mem*ptr=(share_mem*)shmat(shmid,NULL,0);
    users_*users=ptr->user;

    //找重複的name或是email
    for(int i=0;i<small_size;i++){
        if(!users[i].reg){
            break;
        }
        if(users[i].username==username||users[i].email==email){
            shmdt(ptr);
            return 1;
        }
    }

    //登錄進share memory
    int i;
    for(i=0;i<small_size;i++){
        if(!users[i].reg){
            break;
        }
    }

    users[i].reg=true;
    users[i].login_condition=false;
    users[i].in_room=false;
    memset(&users[i].username,0,sizeof(users[i].username));
    memset(&users[i].password,0,sizeof(users[i].password));
    memset(&users[i].email,0,sizeof(users[i].email));
    users[i].invitations=0;
    for(int j=0;j<small_size;j++){
        users[i].invitation->use=false;
    }
    users[i].tcp_conn=-1;

    for(int j=0;j<username.size();j++){
        users[i].username[j]=username[j];
    }
    for(int j=0;j<email.size();j++){
        users[i].email[j]=email[j];
    }
    for(int j=0;j<password.size();j++){
        users[i].password[j]=password[j];
    }
    

    //success
    ptr->num_users++;
    shmdt(ptr);
    return 2;
}
int login_command(string input,string &str,int shmid,bool login_condition,int tcp_conn){
    string username="",password="",format="";
    int condition=0;
    for(int i=0;i<input.size();i++){
        if(input[i]==' '){
            condition++;
        }
        else if(input[i]=='\n'){
            break;
        }
        else{
            if(condition==0){
                continue;
            }
            else if(condition==1){
                username+=input[i];
            }
            else if(condition==2){
                password+=input[i];
            }
            else{
                format+=input[i];
            }
        }
    }
    if(username==""||password==""||format!=""){
        return 0;
    }

    share_mem*ptr=(share_mem*)shmat(shmid,NULL,0);
    users_*users=ptr->user;

    //找名字存不存在
    int i;
    for(i=0;i<small_size;i++){
        if(!users[i].reg){
            shmdt(ptr);
            return 1;
        }
        if(users[i].username==username){
            break;
        }
    }
    
    //看現在的登入狀態
    if(login_condition){
        shmdt(ptr);
        return 2;
    }

    //看有沒有被其他user登入
    if(users[i].login_condition){
        shmdt(ptr);
        str=username;
        return 3;
    }

    //看密碼對不對
    if(users[i].password!=password){
        shmdt(ptr);
        return 4;
    }

    //success
    str=username;
    login_condition=true;
    users[i].login_condition=true;
    users[i].tcp_conn=tcp_conn;
    shmdt(ptr);

    fstream file;
    file.open("efs/server2.txt",ios::in);
    string num_str;
    file>>num_str;
    file.close();

    file.open("efs/server2.txt",ios::trunc);
    file.close();

    file.open("efs/server2.txt",ios::out);
    int num=stoi(num_str)+1;
    file<<to_string(num);
    file.close();

    return 5;
}
int logout_command(int shmid,bool login_condition,string room_id,string username){
    if(!login_condition){
        return 1;
    }
    if(room_id!=""){
        return 2;
    }

    share_mem*ptr=(share_mem*)shmat(shmid,NULL,0);
    users_*users=ptr->user;

    for(int i=0;i<small_size;i++){
        if(users[i].username==username){
            users[i].login_condition=false;
            users[i].tcp_conn=0;
            break;
        }
    }

    shmdt(ptr);

    fstream file;
    file.open("efs/server2.txt",ios::in);
    string num_str;
    file>>num_str;
    file.close();

    file.open("efs/server2.txt",ios::trunc);
    file.close();

    file.open("efs/server2.txt",ios::out);
    int num=stoi(num_str)-1;
    file<<to_string(num);
    file.close();

    return 3;
}
int create_public_room_command(int shmid,string input,bool login_condition,string &room_id,string username){
    string id="",format="";
    int condition=0;
    for(int i=0;i<input.size();i++){
        if(input[i]==' '){
            condition++;
            continue;
        }
        else if(input[i]=='\n'){
            break;
        }
        else{
            if(condition==0||condition==1||condition==2){
                continue;
            }
            else if(condition==3){
                id+=input[i];
            }
            else{
                format+=input[i];
            }
        }
    }
    if(id==""||format!=""){
        return 0;
    }
    
    //沒登入
    if(!login_condition){
        return 1;
    }

    //在房間裡面
    if(room_id!=""){
        return 2;
    }

    //id已經被使用
    share_mem*ptr=(share_mem*)shmat(shmid,NULL,0);
    rooms_*rooms=ptr->room;
    for(int i=0;i<small_size;i++){
        if(!rooms[i].exist){
            continue;
        }
        if(rooms[i].room_id==id){
            shmdt(ptr);
            return 3;
        }
    }

    //success
    ptr->num_rooms++;

    int i;
    for(i=0;i<small_size;i++){
        if(!rooms[i].exist){
            break;
        }
    }

    rooms[i].exist=true;
    rooms[i].private_room=false;
    memset(&rooms[i].manager,0,sizeof(rooms[i].manager));
    memset(&rooms[i].room_id,0,sizeof(rooms[i].room_id));
    memset(&rooms[i].in_code,0,sizeof(rooms[i].in_code));
    rooms[i].playing=false;
    memset(&rooms[i].current,0,sizeof(rooms[i].current));
    for(int j=0;j<small_size;j++){
        rooms[i].players[j].use=false;
    }

    for(int j=0;j<username.size();j++){
        rooms[i].manager[j]=username[j];
    }
    for(int j=0;j<id.size();j++){
        rooms[i].room_id[j]=id[j];
    }

    rooms[i].players[0].use=true;
    rooms[i].players[0].online=true;
    memset(&rooms[i].players[0].username,0,sizeof(rooms[i].players[0].username));
    for(int j=0;j<username.size();j++){
        rooms[i].players[0].username[j]=username[j];
    }

    users_*users=ptr->user;
    for(i=0;i<small_size;i++){
        if(!users[i].reg){
            break;
        }
        if(users[i].username==username){
            users[i].in_room=true;
            break;
        }
    }

    room_id=id;
    shmdt(ptr);
    return 4;
}
int create_privat_room_command(int shmid,string input,bool login_condition,string &room_id,string username){
    string id="",invit_code="",format="";
    int condition=0;
    for(int i=0;i<input.size();i++){
        if(input[i]==' '){
            condition++;
            continue;
        }
        else if(input[i]=='\n'){
            break;
        }
        else{
            if(condition==0||condition==1||condition==2){
                continue;
            }
            else if(condition==3){
                id+=input[i];
            }
            else if(condition==4){
                invit_code+=input[i];
            }
            else{
                format+=input[i];
            }
        }
    }
    if(id==""||invit_code==""||format!=""){
        return 0;
    }

    //沒登入
    if(!login_condition){
        return 1;
    }

    //在房間裡面
    if(room_id!=""){
        return 2;
    }

    //id已經被使用
    share_mem*ptr=(share_mem*)shmat(shmid,NULL,0);
    rooms_*rooms=ptr->room;
    for(int i=0;i<small_size;i++){
        if(!rooms[i].exist){
            continue;
        }
        if(rooms[i].room_id==id){
            shmdt(ptr);
            return 3;
        }
    }

    //success
    ptr->num_rooms++;
    
    int i;
    for(i=0;i<small_size;i++){
        if(!rooms[i].exist){
            break;
        }
    }

    rooms[i].exist=true;
    rooms[i].private_room=true;
    memset(&rooms[i].manager,0,sizeof(rooms[i].manager));
    memset(&rooms[i].room_id,0,sizeof(rooms[i].room_id));
    memset(&rooms[i].in_code,0,sizeof(rooms[i].in_code));
    rooms[i].playing=false;
    memset(&rooms[i].current,0,sizeof(rooms[i].current));
    for(int j=0;j<small_size;j++){
        rooms[i].players[j].use=false;
    }

    for(int j=0;j<username.size();j++){
        rooms[i].manager[j]=username[j];
    }
    for(int j=0;j<id.size();j++){
        rooms[i].room_id[j]=id[j];
    }
    for(int j=0;j<invit_code.size();j++){
        rooms[i].in_code[j]=invit_code[j];
    }

    rooms[i].players[0].use=true;
    rooms[i].players[0].online=true;
    memset(&rooms[i].players[0].username,0,sizeof(rooms[i].players[0].username));
    for(int j=0;j<username.size();j++){
        rooms[i].players[0].username[j]=username[j];
    }

    users_*users=ptr->user;
    for(i=0;i<small_size;i++){
        if(!users[i].reg){
            break;
        }
        if(users[i].username==username){
            users[i].in_room=true;
            break;
        }
    }

    room_id=id;
    shmdt(ptr);
    return 4;
}
int list_rooms_command(int shmid,string &str){
    sleep(1);
    share_mem*ptr=(share_mem*)shmat(shmid,NULL,0);
    if(ptr->num_rooms==0){
        shmdt(ptr);
        return 0;
    }

    rooms_*rooms=ptr->room;
    for(int i=0;i<small_size;i++){
        if(!rooms[i].exist){
            continue;
        }
        unit_room.privat=rooms[i].private_room;
        unit_room.playing=rooms[i].playing;
        room_list[rooms[i].room_id]=unit_room;
    }
    int i;
    for(i=1,itt=room_list.begin();itt!=room_list.end();i++,itt++){
        str+=('0'+i);
        if(!itt->second.privat){
            str+=". (Public) Game Room ";
        }
        else{
            str+=".(Private) Game Room ";
        }
        str+=(itt->first+" ");
        if(!itt->second.playing){
            str+="is open for players\n";
        }
        else{
            str+="has started playing\n";
        }
    }
    
    room_list.clear();
    shmdt(ptr);
    return 1;
}
int list_users_command(int shmid,string &str){
    share_mem*ptr=(share_mem*)shmat(shmid,NULL,0);
    if(ptr->num_users==0){
        shmdt(ptr);
        return 0;
    }

    users_*users=ptr->user;
    for(int i=0;i<small_size;i++){
        if(!users[i].reg){
            break;
        }
        unit_user.email=users[i].email;
        unit_user.online=users[i].login_condition;
        user_list[users[i].username]=unit_user;
    }
    int i;
    for(i=1,ittt=user_list.begin();ittt!=user_list.end();i++,ittt++){
        str+=('0'+i);
        str+=". ";
        str+=ittt->first;
        str+=("<"+ittt->second.email+"> ");
        if(!ittt->second.online){
            str+="Offline\n";
        }
        else{
            str+="Online\n";
        }
    }

    user_list.clear();
    shmdt(ptr);
    return 1;
}
int join_room_command(string input,int shmid,bool login_condition,string &room_id,string username){
    string id="",format="";
    int condition=0;
    for(int i=0;i<input.length();i++){
        if(input[i]==' '){
            condition++;
            continue;
        }
        else if(input[i]=='\n'){
            break;
        }
        else{
            if(condition==0||condition==1){
                continue;
            }
            else if(condition==2){
                id+=input[i];
            }
            else{
                format+=input[i];
            }
        }
    }
    if(id==""||format!=""){
        return 0;
    }
    
    //沒登入
    if(!login_condition){
        return 1;
    }

    //在房間裡面了
    if(room_id!=""){
        return 2;
    }

    share_mem*ptr=(share_mem*)shmat(shmid,NULL,0);
    rooms_*rooms=ptr->room;

    //找id
    bool flag=false;
    int i;
    for(i=0;i<small_size;i++){
        if(rooms[i].room_id==id){
            flag=true;
            break;
        }
    }
    
    //id不存在
    if(!flag){
        room_id=id;
        shmdt(ptr);
        return 3;
    }

    //room是private
    if(rooms[i].private_room){
        shmdt(ptr);
        return 4;
    }

    //room已經開始遊戲
    if(rooms[i].playing){
        shmdt(ptr);
        return 5;
    }

    //broadcast到房間裡面的成員
    users_*users=ptr->user;
    players_*players=ptr->room[i].players;
    for(int i=0;i<small_size;i++){
        if(!players[i].use){
            break;
        }
        if(!players[i].online){
            continue;
        }
        for(int j=0;j<small_size;j++){
            if(!users[j].reg){
                break;
            }
            if(cmp_char_arr(players[i].username,users[j].username)){
                char send_buff[send_size];
                memset(&send_buff,0,sizeof(send_buff));
                string str1="Welcome, ";
                string str2=" to game!\n";
                int k;
                for(k=0;k<str1.size();k++){
                    send_buff[k]=str1[k];
                }
                for(int l=0;l<username.size();l++,k++){
                    send_buff[k]=username[l];
                }
                for(int l=0;l<str2.size();l++,k++){
                    send_buff[k]=str2[l];
                }
                int send_num=send(users[j].tcp_conn,send_buff,strlen(send_buff),0);
                if(send_num==-1){
                    cout<<"broadcast to players error:"<<players[i].username<<"\n";
                    continue;
                }
                break;
            }
        }
    }

    //success
    room_id=id;
    int j;
    for(j=0;j<small_size;j++){
        if(!players[j].use){
            break;
        }
    }
    players[j].use=true;
    players[j].online=true;
    memset(&players[j].username,0,sizeof(players[j].username));
    for(int k=0;k<username.size();k++){
        players[j].username[k]=username[k];
    }

    for(i=0;i<small_size;i++){
        if(!users[i].reg){
            break;
        }
        if(users[i].username==username){
            users[i].in_room=true;
            break;
        }
    }

    shmdt(ptr);
    return 6;
}
int invite_command(string input,int shmid,bool login_condition,string room_id,string username,string &invitee_name_email){
    string invitee_email="",format="";
    int condition=0;
    for(int i=0;i<input.length();i++){
        if(input[i]==' '){
            condition++;
            continue;
        }
        else if(input[i]=='\n'){
            break;
        }
        else{
            if(condition==0){
                continue;
            }
            else if(condition==1){
                invitee_email+=input[i];
            }
            else{
                format+=input[i];
            }
        }
    }
    if(invitee_email==""||format!=""){
        return 0;
    }
    
    //沒登入
    if(!login_condition){
        return 1;
    }

    //沒有在房間裡面
    if(room_id==""){
        return 2;
    }

    //你不是manager
    share_mem*ptr=(share_mem*)shmat(shmid,NULL,0);
    rooms_*rooms=ptr->room;
    //先找room
    int i;
    for(i=0;i<small_size;i++){
        if(!rooms[i].exist){
            continue;
        }
        if(rooms[i].room_id==room_id){
            break;
        }
    }
    //確認manager
    string manager="";
    string in_code="";
    for(int j=0;j<strlen(rooms[i].manager);j++){
        manager+=rooms[i].manager[j];
    }
    for(int j=0;j<strlen(rooms[i].in_code);j++){
        in_code+=rooms[i].in_code[j];
    }
    if(manager!=username){
        shmdt(ptr);
        return 3;
    }

    //invitee沒登入
    bool flag=false;
    users_*users=ptr->user;
    for(i=0;i<small_size;i++){
        if(!users[i].reg){
            continue;
        }
        string email="";
        for(int j=0;j<strlen(users[i].email);j++){
            email+=users[i].email[j];
        }
        if(email==invitee_email){
            flag=true;
            break;
        }
    }
    //找不到email
    if(!flag){
        shmdt(ptr);
        return 5;
    }
    //沒登入
    if(!users[i].login_condition){
        shmdt(ptr);
        return 4;
    }

    //送邀請訊息給invitee
    //i是invitee的users[]
    char send_buff[send_size];
    memset(&send_buff,0,sizeof(send_buff));
    string str1="You receive invitation from ";
    int j;
    for(j=0;j<str1.size();j++){
        send_buff[j]=str1[j];
    }
    for(int k=0;k<username.size();j++,k++){
        send_buff[j]=username[k];
    }
    send_buff[j]='<';
    j++;
    string inviter_email="";
    for(int k=0;k<small_size;k++){
        if(!users[k].reg){
            continue;
        }
        if(users[k].username==username){
            for(int l=0;l<strlen(users[k].email);l++){
                inviter_email+=users[k].email[l];
            }
            break;
        }
    }
    for(int k=0;k<inviter_email.size();k++,j++){
        send_buff[j]=inviter_email[k];
    }
    send_buff[j]='>';
    j++;
    send_buff[j]='\n';
    int send_num=send(users[i].tcp_conn,send_buff,strlen(send_buff),0);
    if(send_num==-1){
        cout<<"Send invitation to invitee error\n";
    }

    //添加invitation到share_mem 的users的buffer
    users[i].invitations++;
    invitations_*invitations=users[i].invitation;
    for(j=0;j<invitation_size;j++){
        if(!invitations[j].use){
            break;
        }
    }
    invitations[j].use=true;
    for(int k=0;k<username.size();k++){
        invitations[j].inviter[k]=username[k];
    }
    for(int k=0;k<inviter_email.size();k++){
        invitations[j].inviter_email[k]=inviter_email[k];
    }
    for(int k=0;k<room_id.size();k++){
        invitations[j].room_id[k]=room_id[k];
    }
    for(int k=0;k<in_code.size();k++){
        invitations[j].in_code[k]=in_code[k];
    }
    
    //success
    string invitee_name="";
    for(j=0;j<strlen(users[i].username);j++){
        invitee_name+=users[i].username[j];
    }
    invitee_name_email=(invitee_name+"<"+invitee_email+">");

    shmdt(ptr);
    return 6;
}
int list_invitations_command(int shmid,string username,string &str){
    share_mem*ptr=(share_mem*)shmat(shmid,NULL,0);
    users_*users=ptr->user;
    //找user
    int i;
    for(i=0;i<small_size;i++){
        if(!users[i].reg){
            break;
        }
        if(username==users[i].username){
            break;
        }
    }

    if(users[i].invitations==0){
        shmdt(ptr);
        return 0;
    }

    invitations_*invitations=users[i].invitation;
    rooms_*rooms=ptr->room;
    for(int j=0;j<invitation_size;j++){
        if(!invitations[j].use){
            break;
        }
        string inviter="";
        for(int k=0;k<strlen(invitations[j].inviter);k++){
            inviter+=invitations[j].inviter[k];
        }
        string inviter_email="";
        for(int k=0;k<strlen(invitations[j].inviter_email);k++){
            inviter_email+=invitations[j].inviter_email[k];
        }
        string room_id="";
        for(int k=0;k<strlen(invitations[j].room_id);k++){
            room_id+=invitations[j].room_id[k];
        }
        string in_code="";
        for(int k=0;k<strlen(invitations[j].in_code);k++){
            in_code+=invitations[j].in_code[k];
        }

        //確認room還存不存在
        bool flag=false;
        for(int k=0;k<small_size;k++){
            if(!rooms[k].exist){
                continue;
            }
            if(rooms[k].room_id==room_id){
                flag=true;
                break;
            }
        }
        if(!flag){
            continue;
        }

        unit_invitation.inviter=inviter;
        unit_invitation.inviter_email=inviter_email;
        unit_invitation.in_code=in_code;
        invitation_list[room_id]=unit_invitation;
    }
    int j;
    for(j=1,itttt=invitation_list.begin();itttt!=invitation_list.end();j++,itttt++){
        str+=('0'+j);
        str+=(". "+itttt->second.inviter+"<"+itttt->second.inviter_email+"> invite you to join game room "+itttt->first+", invitation code is "+itttt->second.in_code+"\n");
    }

    invitation_list.clear();
    shmdt(ptr);
    return 1;
}
int accept_command(string input,int shmid,bool login_condition,string &room_id,string username){
    string inviter_email="",invitation_code="",format="";
    int condition=0;
    for(int i=0;i<input.length();i++){
        if(input[i]==' '){
            condition++;
            continue;
        }
        else if(input[i]=='\n'){
            break;
        }
        else{
            if(condition==0){
                continue;
            }
            else if(condition==1){
                inviter_email+=input[i];
            }
            else if(condition==2){
                invitation_code+=input[i];
            }
            else{
                format+=input[i];
            }
        }
    }
    if(inviter_email==""||invitation_code==""||format!=""){
        return 0;
    }

    //沒登入
    if(!login_condition){
        return 1;
    }

    //已經在房間裡面
    if(room_id!=""){
        return 2;
    }

    //先看有沒有來自 inviter_email 的邀請 去確認自己是否有被邀請
    share_mem*ptr=(share_mem*)shmat(shmid,NULL,0);
    users_*users=ptr->user;
    int i;
    //找到自己
    for(i=0;i<small_size;i++){
        if(!users[i].reg){
            break;
        }
        if(users[i].username==username){
            break;
        }
    }
    invitations_*invitations=users[i].invitation;
    bool flag=false;
    int j;
    //找inviter的email
    for(j=0;j<invitation_size;j++){
        if(!invitations[j].use){
            break;
        }
        if(invitations[j].inviter_email==inviter_email){
            flag=true;
            break;
        }
    }
    if(!flag){
        shmdt(ptr);
        return 3;
    }
    //找房間還存不存在
    //i是自己的編號
    //j是invitation的編號
    //用invitation裡面的room_id確認還存不存在
    rooms_*rooms=ptr->room;
    int k;
    flag=false;
    for(k=0;k<small_size;k++){
        if(!rooms[k].exist){
            continue;
        }
        if(cmp_char_arr(rooms[k].room_id,invitations[j].room_id)){
            flag=true;
            break;
        }
    }
    if(!flag){
        shmdt(ptr);
        return 3;
    }

    //invitation code錯誤
    if(invitations[j].in_code!=invitation_code){
        shmdt(ptr);
        return 4;
    }

    //遊戲已經開始，不可以加入
    if(rooms[k].playing){
        shmdt(ptr);
        return 5;
    }
    
    //success
    //i是user的編號
    //j是invitation的編號
    //k是room的編號
    //先broadcast到房間裡面的人
    players_*players=rooms[k].players;
    for(int l=0;l<small_size;l++){
        if(!players[l].use){
            continue;
        }
        if(!players[l].online){
            continue;
        }
        for(int m=0;m<small_size;m++){
            if(!users[m].reg){
                break;
            }
            if(cmp_char_arr(players[l].username,users[m].username)){
                char send_buff[send_size];
                memset(&send_buff,0,sizeof(send_buff));
                string str1="Welcome, ";
                string str2=" to game!\n";
                int o;
                for(o=0;o<str1.size();o++){
                    send_buff[o]=str1[o];
                }
                for(int p=0;p<username.size();p++,o++){
                    send_buff[o]=username[p];
                }
                for(int p=0;p<str2.size();p++,o++){
                    send_buff[o]=str2[p];
                }
                int send_num=send(users[m].tcp_conn,send_buff,strlen(send_buff),0);
                if(send_num==-1){
                    cout<<"broadcast to players error:"<<players[l].username<<"\n";;
                }
                break;
            }
        }
    }
    //加進去房間的shm
    int l;
    for(l=0;l<small_size;l++){
        if(!players[l].use){
            break;
        }
    }
    players[l].use=true;
    players[l].online=true;
    memset(&players[l].username,0,sizeof(players[l].username));
    for(int m=0;m<username.size();m++){
        players[l].username[m]=username[m];
    }

    for(int z=0;z<strlen(invitations[j].room_id);z++){
        room_id+=invitations[j].room_id[z];
    }

    for(i=0;i<small_size;i++){
        if(!users[i].reg){
            break;
        }
        if(users[i].username==username){
            users[i].in_room=true;
            break;
        }
    }
    users[i].invitations--;
    shmdt(ptr);
    return 6;
}
int leave_room_command(int choose,int shmid,bool login_condition,string room_id,string username){
    //沒登入
    if(!login_condition){
        return 1;
    }

    //沒房間
    if(room_id==""){
        return 2;
    }

    int i;//rooms

    share_mem*ptr=(share_mem*)shmat(shmid,NULL,0);
    users_*users=ptr->user;
    rooms_*rooms=ptr->room;

    //先找房間
    for(i=0;i<small_size;i++){
        if(!rooms[i].exist){
            continue;
        }
        if(rooms[i].room_id==room_id){
            break;
        }
    }
    //確認是不是manager
    //success 1
    if(rooms[i].manager==username){
        //broadcast到房間裡面的其他人
        players_*players=rooms[i].players;
        players[0].online=false;//自己(manager)
        for(int j=1;j<small_size,choose!=0;j++){//自己不用
            if(!players[j].use){
                break;
            }
            if(!players[j].online){
                continue;
            }
            for(int k=0;k<small_size;k++){
                if(!users[k].reg){
                    break;
                }
                if(cmp_char_arr(players[j].username,users[k].username)){
                    char send_buff[send_size];
                    memset(&send_buff,0,sizeof(send_buff));
                    string str1="Game room manager leave game room ";
                    string str2=", you are forced to leave too\n";
                    int l;
                    for(l=0;l<str1.size();l++){
                        send_buff[l]=str1[l];
                    }
                    for(int m=0;m<room_id.size();m++,l++){
                        send_buff[l]=room_id[m];
                    }
                    for(int m=0;m<str2.size();m++,l++){
                        send_buff[l]=str2[m];
                    }
                    int send_num=send(users[k].tcp_conn,send_buff,strlen(send_buff),0);
                    if(send_num==-1){
                        cout<<"broadcast to players error:"<<players[j].username<<"\n";
                        continue;
                    }
                    users[k].in_room=false;
                    break;
                }
            }
            players[j].online=false;
        }
        rooms[i].exist=false;
        //更新自己的in_room狀態
        for(int k=0;k<small_size;k++){
            if(!users[k].reg){
                break;
            }
            if(users[k].username==username){
                users[i].in_room=false;
                break;
            }
        }
        ptr->num_rooms--;
        shmdt(ptr);
        return 3;
    }
    else{
        //success 2
        if(rooms[i].playing){
            //broadcast到房間裡面的其他人
            players_*players=rooms[i].players;
            rooms[i].playing=false;
            for(int j=0;j<small_size,choose!=0;j++){
                if(!players[j].use){
                    break;
                }
                if(!players[j].online){
                    continue;
                }
                if(players[j].username==username){
                    players[j].online=false;
                    continue;
                }
                for(int l=0;l<small_size;l++){
                    if(!users[l].reg){
                        break;
                    }
                    if(cmp_char_arr(players[j].username,users[l].username)){
                        char send_buff[send_size];
                        memset(&send_buff,0,sizeof(send_buff));
                        string str1=" leave game room ";
                        string str2=", game ends\n";
                        int m;
                        for(m=0;m<username.size();m++){
                            send_buff[m]=username[m];
                        }
                        for(int o=0;o<str1.size();o++,m++){
                            send_buff[m]=str1[o];
                        }
                        for(int o=0;o<room_id.size();o++,m++){
                            send_buff[m]=room_id[o];
                        }
                        for(int o=0;o<str2.size();o++,m++){
                            send_buff[m]=str2[o];
                        }
                        int send_num=send(users[l].tcp_conn,send_buff,strlen(send_buff),0);
                        if(send_num==-1){
                            cout<<"broadcast to players error:"<<players[j].username<<"\n";
                        }
                    }
                }
            }
            for(int j=0;j<small_size;j++){
                if(!users[j].reg){
                    break;
                }
                if(users[j].username==username){
                    users[j].in_room=false;
                    break;
                }
            }
            shmdt(ptr);
            return 4;
        }
        //success 3
        else{
            players_*players=rooms[i].players;
            for(int j=0;j<small_size,choose!=0;j++){
                if(!players[j].use){
                    break;
                }
                if(!players[j].online){
                    continue;
                }
                if(players[j].username==username){
                    players[j].online=false;
                    continue;
                }
                for(int l=0;l<small_size;l++){
                    if(!users[l].reg){
                        break;
                    }
                    if(cmp_char_arr(players[j].username,users[l].username)){
                        char send_buff[send_size];
                        memset(&send_buff,0,sizeof(send_buff));
                        string str1=" leave game room ";
                        int m;
                        for(m=0;m<username.size();m++){
                            send_buff[m]=username[m];
                        }
                        for(int o=0;o<str1.size();o++,m++){
                            send_buff[m]=str1[o];
                        }
                        for(int o=0;o<room_id.size();o++,m++){
                            send_buff[m]=room_id[o];
                        }
                        send_buff[m]='\n';
                        int send_num=send(users[l].tcp_conn,send_buff,strlen(send_buff),0);
                        if(send_num==-1){
                            cout<<"broadcast to players error:"<<players[j].username<<"\n";
                        }
                    }
                }
            }
            for(int j=0;j<small_size;j++){
                if(!users[j].reg){
                    break;
                }
                if(users[j].username==username){
                    users[j].in_room=false;
                    break;
                }
            }
            shmdt(ptr);
            return 5;
        }
    }
    return 0;
}
int start_game_command(string input,int shmid,bool login_condition,string room_id,string username){
    string rounds="",ans="",format="";
    int condition=0;
    for(int i=0;i<input.length();i++){
        if(input[i]==' '){
            condition++;
            continue;
        }
        else if(input[i]=='\n'){
            break;
        }
        else{
            if(condition==0||condition==1){
                continue;
            }
            else if(condition==2){
                rounds+=input[i];
            }
            else if(condition==3){
                ans+=input[i];
            }
            else{
                format+=input[i];
            }
        }
    }
    if(rounds==""||format!=""){
        return 0;
    }

    //沒登入
    if(!login_condition){
        return 1;
    }

    //沒有房間
    if(room_id==""){
        return 2;
    }

    // i -> room

    share_mem*ptr=(share_mem*)shmat(shmid,NULL,0);
    rooms_*rooms=ptr->room;

    //不是manager
    //用room_id找房間
    int i;
    for(i=0;i<small_size;i++){
        if(!rooms[i].exist){
            continue;
        }
        if(rooms[i].room_id==room_id){
            break;
        }
    }
    if(rooms[i].manager!=username){
        shmdt(ptr);
        return 3;
    }

    //遊戲已經開始
    if(rooms[i].playing){
        shmdt(ptr);
        return 4;
    }

    //不是4個數字
    if(ans!=""&&!check_four(ans)){
        shmdt(ptr);
        return 5;
    }

    //success
    if(ans==""){
        ans=provide_ans();
    }
    memset(&rooms[i].ans,0,sizeof(rooms[i].ans));
    for(int j=0;j<4;j++){
        rooms[i].ans[j]=ans[j];
    }
    rooms[i].playing=true;
    memset(&rooms[i].current,0,sizeof(rooms[i].current));
    for(int j=0;j<username.size();j++){
        rooms[i].current[j]=username[j];
    }
    rooms[i].rounds=stoi(rounds);

    //broadcast
    players_*players=rooms[i].players;
    users_*users=ptr->user;
    for(int j=0;j<small_size;j++){
        if(!players[j].use){
            break;
        }
        if(!players[j].online){
            continue;
        }
        for(int k=0;k<small_size;k++){
            if(!users[k].reg){
                break;
            }
            if(cmp_char_arr(users[k].username,players[j].username)){
                char send_buff[buff_size];
                memset(&send_buff,0,sizeof(send_buff));
                string str1="Game start! Current player is ";
                int l;
                for(l=0;l<str1.size();l++){
                    send_buff[l]=str1[l];
                }
                for(int m=0;m<username.size();m++,l++){
                    send_buff[l]=username[m];
                }
                send_buff[l]='\n';
                int send_num=send(users[k].tcp_conn,send_buff,strlen(send_buff),0);
                if(send_num==-1){
                    cout<<"broadcast to players error:"<<players[j].username<<"\n";
                    continue;
                }
                break;
            }
        }
    }
    
    shmdt(ptr);
    return 6;
}
int guess_command(string input,int shmid,bool login_condition,string room_id,string username,string &current){
    string guess_num="",format="";
    int condition=0;
    for(int i=0;i<input.length();i++){
        if(input[i]==' '){
            condition++;
            continue;
        }
        else if(input[i]=='\n'){
            break;
        }
        else{
            if(condition==0){
                continue;
            }
            else if(condition==1){
                guess_num+=input[i];
            }
            else{
                format+=input[i];
            }
        }
    }
    if(guess_num==""||format!=""){
        return 0;
    }

    //沒登入
    if(!login_condition){
        return 1;
    }

    //沒房間
    if(room_id==""){
        return 2;
    }
    
    // i -> room

    share_mem*ptr=(share_mem*)shmat(shmid,NULL,0);
    rooms_*rooms=ptr->room;

    //沒開始
    int i;
    for(i=0;i<small_size;i++){
        if(!rooms[i].exist){
            continue;
        }
        if(rooms[i].room_id==room_id){
            break;
        }
    }

    if(!rooms[i].playing){
        //是manager
        if(rooms[i].manager==username){
            shmdt(ptr);
            return 31;
        }
        //不是manager
        else{
            shmdt(ptr);
            return 32;
        }
    }

    //現在不是你 急三小
    if(rooms[i].current!=username){
        current=rooms[i].current;
        shmdt(ptr);
        return 4;
    }

    //為什麼不好好輸入4個數字==
    if(!check_four(guess_num)){
        shmdt(ptr);
        return 5;
    }

    //success
    string answer="";
    for(int j=0;j<strlen(rooms[i].ans);j++){
        answer+=rooms[i].ans[j];
    }
    string result=gaming(guess_num,answer);
    players_*players=rooms[i].players;
    users_*users=ptr->user;

    //success -> BINGO
    if(result=="4A0B"){
        string s1=" guess \'";
        string s2="\' and got Bingo!!! ";
        string s3=" wins the game, game ends\n";
        for(int j=0;j<small_size;j++){
            if(!players[j].use){
                break;
            }
            if(!players[j].online){
                continue;
            }
            for(int k=0;k<small_size;k++){
                if(!users[k].reg){
                    break;
                }
                if(cmp_char_arr(users[k].username,players[j].username)){
                    char send_buff[buff_size];
                    memset(&send_buff,0,sizeof(send_buff));
                    int l;
                    for(l=0;l<username.size();l++){
                        send_buff[l]=username[l];
                    }
                    for(int m=0;m<s1.size();m++,l++){
                        send_buff[l]=s1[m];
                    }
                    for(int m=0;m<guess_num.size();m++,l++){
                        send_buff[l]=guess_num[m];
                    }
                    for(int m=0;m<s2.size();m++,l++){
                        send_buff[l]=s2[m];
                    }
                    for(int m=0;m<username.size();m++,l++){
                        send_buff[l]=username[m];
                    }
                    for(int m=0;m<s3.size();m++,l++){
                        send_buff[l]=s3[m];
                    }
                    int send_num=send(users[k].tcp_conn,send_buff,strlen(send_buff),0);
                    break;
                }
            }
        }
        rooms[i].playing=false;
        return 6;
    }

    // j -> now user players[]

    int j;
    bool flag=true;
    for(j=0;j<small_size;j++){
        if(!players[j].use){
            flag=false;
            break;
        }
        if(!players[j].online){
            continue;
        }
        if(players[j].username==username){
            break;
        }
    }
    
    // k -> next players

    int k;
    if(!flag||j==(small_size-1)){
        rooms[i].rounds--;
        k=0;
    }
    else{
        flag=false;
        for(k=j+1;k<small_size;k++){
            if(!players[k].use){
                break;
            }
            if(!players[k].online){
                continue;
            }
            else{
                flag=true;
                break;
            }
        }
        if(!flag){
            rooms[i].rounds--;
            k=0;
        }
    }
    //success -> NO CHANCE
    if(rooms[i].rounds==0){
        //broadcast
        string s1=" guess \'";
        string s2="\' and got \'";
        string s3="\'\nGame ends, no one wins\n";
        for(int k=0;k<small_size;k++){
            if(!players[k].use){
                break;
            }
            if(!players[k].online){
                continue;
            }
            for(int o=0;o<small_size;o++){
                if(!users[o].reg){
                    break;
                }
                if(cmp_char_arr(users[o].username,players[k].username)){
                    char send_buff[buff_size];
                    memset(&send_buff,0,sizeof(send_buff));
                    int l;
                    for(l=0;l<username.size();l++){
                        send_buff[l]=username[l];
                    }
                    for(int m=0;m<s1.size();m++,l++){
                        send_buff[l]=s1[m];
                    }
                    for(int m=0;m<guess_num.size();m++,l++){
                        send_buff[l]=guess_num[m];
                    }
                    for(int m=0;m<s2.size();m++,l++){
                        send_buff[l]=s2[m];
                    }
                    for(int m=0;m<result.size();m++,l++){
                        send_buff[l]=result[m];
                    }
                    for(int m=0;m<s3.size();m++,l++){
                        send_buff[l]=s3[m];
                    }
                    int send_num=send(users[o].tcp_conn,send_buff,strlen(send_buff),0);
                    break;
                }
            }
        }
        rooms[i].playing=false;
    }
    //success -> NOT BINGO
    else{
        memset(&rooms[i].current,0,sizeof(rooms[i].current));
        for(int l=0;l<strlen(players[k].username);l++){
            rooms[i].current[l]=players[k].username[l];
        }
        string s1=" guess \'";
        string s2="\' and got \'";
        string s3="\'\n";
        for(k=0;k<small_size;k++){
            if(!players[k].use){
                break;
            }
            if(!players[k].online){
                continue;
            }
            for(int o=0;o<small_size;o++){
                if(!users[o].reg){
                    break;
                }
                if(cmp_char_arr(users[o].username,players[k].username)){
                    char send_buff[buff_size];
                    memset(&send_buff,0,sizeof(send_buff));
                    int l;
                    for(l=0;l<username.size();l++){
                        send_buff[l]=username[l];
                    }
                    for(int m=0;m<s1.size();m++,l++){
                        send_buff[l]=s1[m];
                    }
                    for(int m=0;m<guess_num.size();m++,l++){
                        send_buff[l]=guess_num[m];
                    }
                    for(int m=0;m<s2.size();m++,l++){
                        send_buff[l]=s2[m];
                    }
                    for(int m=0;m<result.size();m++,l++){
                        send_buff[l]=result[m];
                    }
                    for(int m=0;m<s3.size();m++,l++){
                        send_buff[l]=s3[m];
                    }
                    int send_num=send(users[o].tcp_conn,send_buff,strlen(send_buff),0);
                    break;
                }
            }
        }
    }
    shmdt(ptr);
    return 6;
}
string state_command(){
    fstream server1,server2,server3;
    server1.open("efs/server1.txt",ios::in);
    server2.open("efs/server2.txt",ios::in);
    server3.open("efs/server3.txt",ios::in);

    string s1,s2,s3;
    server1>>s1;
    server2>>s2;
    server3>>s3;

    server1.close();
    server2.close();
    server3.close();

    string ret="Server1: ";
    ret+=(s1+"\n"+"Server2: "+s2+"\n"+"Server3: "+s3+"\n");
    return ret;
}
void exit_command(int shmid,bool login_condition,string room_id,string username){
    leave_room_command(0,shmid,login_condition,room_id,username);
    share_mem*ptr=(share_mem*)shmat(shmid,NULL,0);
    users_*users=ptr->user;
    for(int i=0;i<small_size;i++){
        if(users[i].username==username){
            users[i].login_condition=false;
            users[i].tcp_conn=0;
            break;
        }
    }
    shmdt(ptr);
    if(login_condition){
        fstream file;
        file.open("efs/server2.txt",ios::in);
        string num_str;
        file>>num_str;
        file.close();

        file.open("efs/server2.txt",ios::trunc);
        file.close();

        file.open("efs/server2.txt",ios::out);
        int num=stoi(num_str)-1;
        file<<to_string(num);
        file.close();
    }
}

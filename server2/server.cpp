#include<iostream>
#include<vector>
#include<thread>
#include<unistd.h>

#include"header/command.hpp"
#include"header/shm_function.hpp"
#include"header/socket.hpp"

using namespace std;

key_t key=ftok("share_mem",66);
int shmid=shmget(key,sizeof(share_mem),0666|IPC_CREAT);

vector<pthread_t> pool;
int conn[128];

SOCKET_ SOCKET;

void* tcp_thread(void*tcp_conn_){
    int tcp_conn=*(int*)tcp_conn_;
    bool login_condition=false;
    string username="";
    string room_id="";
    while(true){
        if(!SOCKET.TCP_recv(tcp_conn)){
            exit_command(shmid,login_condition,room_id,username);
            return NULL;
        }
        // cout<<SOCKET.get_recv_buff()<<"\n";
        
        share_mem*ptr=(share_mem*)shmat(shmid,NULL,0);
        users_*users=ptr->user;
        for(int i=0;i<small_size;i++){
            if(!users[i].reg){
                break;
            }
            string tmp="";
            for(int j=0;j<strlen(users[i].username);j++){
                tmp+=users[i].username[j];
            }
            if(tmp==username&&!users[i].in_room){
                room_id="";
                break;
            }
        }
        shmdt(ptr);

        int check_num=check(SOCKET.get_recv_buff());
        //login
        if(check_num==2){
            int feedback=login_command(SOCKET.get_recv_buff(),username,shmid,login_condition,tcp_conn);
            if(feedback==0){
                SOCKET.TCP_send(tcp_conn,login_format,"",space);
            }
            else if(feedback==1){
                SOCKET.TCP_send(tcp_conn,login_fail_1_username_not_found,"",space);
            }
            else if(feedback==2){
                SOCKET.TCP_send(tcp_conn,login_fail_2_already_login,username,n);
            }
            else if(feedback==3){
                SOCKET.TCP_send(tcp_conn,login_fail_3_someone_login,username,n);
                username="";
            }
            else if(feedback==4){
                SOCKET.TCP_send(tcp_conn,login_fail_4_wrong_password,"",space);
            }
            else if(feedback==5){
                login_condition=true;
                SOCKET.TCP_send(tcp_conn,login_success,username,n);
            }
        }
        //logout
        else if(check_num==3){
            int feedback=logout_command(shmid,login_condition,room_id,username);
            if(feedback==1){
                SOCKET.TCP_send(tcp_conn,logout_fail_1_not_logged_in,"",space);
            }
            else if(feedback==2){
                SOCKET.TCP_send(tcp_conn,logout_fail_2_in_game_room_1,room_id,logout_fail_2_in_game_room_2);
            }
            else if(feedback==3){
                SOCKET.TCP_send(tcp_conn,logout_success,username,n);
                username="";
                login_condition=false;
            }
        }
        //create public room
        else if(check_num==4){
            int feedback=create_public_room_command(shmid,SOCKET.get_recv_buff(),login_condition,room_id,username);
            if(feedback==0){
                SOCKET.TCP_send(tcp_conn,create_public_format,"",space);
            }
            else if(feedback==1){
                SOCKET.TCP_send(tcp_conn,create_public_fail_1_not_logged_in,"",space);
            }
            else if(feedback==2){
                SOCKET.TCP_send(tcp_conn,create_public_fail_2_already_in_room_1,room_id,create_public_fail_2_already_in_room_2);
            }
            else if(feedback==3){
                SOCKET.TCP_send(tcp_conn,create_public_fail_3_room_id_used,"",space);
            }
            else if(feedback==4){
                SOCKET.TCP_send(tcp_conn,create_public_success,room_id,n);
            }
        }
        //create private room
        else if(check_num==5){
            int feedback=create_privat_room_command(shmid,SOCKET.get_recv_buff(),login_condition,room_id,username);
            if(feedback==0){
                SOCKET.TCP_send(tcp_conn,create_privat_format,"",space);
            }
            else if(feedback==1){
                SOCKET.TCP_send(tcp_conn,create_privat_fail_1_not_logged_in,"",space);
            }
            else if(feedback==2){
                SOCKET.TCP_send(tcp_conn,create_privat_fail_2_already_in_room_1,room_id,create_privat_fail_2_already_in_room_2);
            }
            else if(feedback==3){
                SOCKET.TCP_send(tcp_conn,create_privat_fail_3_room_id_used,"",space);
            }
            else if(feedback==4){
                SOCKET.TCP_send(tcp_conn,create_privat_success,room_id,n);
            }
        }
        //join room
        else if(check_num==8){
            int feedback=join_room_command(SOCKET.get_recv_buff(),shmid,login_condition,room_id,username);
            if(feedback==0){
                SOCKET.TCP_send(tcp_conn,join_room_format,"",space);
            }
            else if(feedback==1){
                SOCKET.TCP_send(tcp_conn,join_room_fail_1_not_logged_in,"",space);
            }
            else if(feedback==2){
                SOCKET.TCP_send(tcp_conn,join_room_fail_2_in_room_1,room_id,join_room_fail_2_in_room_2);
            }
            else if(feedback==3){
                SOCKET.TCP_send(tcp_conn,join_room_fail_3_id_not_exist_1,room_id,join_room_fail_3_id_not_exist_2);
                room_id="";
            }
            else if(feedback==4){
                SOCKET.TCP_send(tcp_conn,join_room_fail_4_private_room,"",space);
            }
            else if(feedback==5){
                SOCKET.TCP_send(tcp_conn,join_room_fail_5_game_started,"",space);
            }
            else if(feedback==6){
                SOCKET.TCP_send(tcp_conn,join_room_success,room_id,n);
            }
        }
        //invite
        else if(check_num==9){
            string invitee_name_email;
            int feedback=invite_command(SOCKET.get_recv_buff(),shmid,login_condition,room_id,username,invitee_name_email);
            if(feedback==0){
                SOCKET.TCP_send(tcp_conn,invite_format,"",space);
            }
            else if(feedback==1){
                SOCKET.TCP_send(tcp_conn,invite_fail_1_not_logged_in,"",space);
            }
            else if(feedback==2){
                SOCKET.TCP_send(tcp_conn,invite_fail_2_not_join_any_game_room,"",space);
            }
            else if(feedback==3){
                SOCKET.TCP_send(tcp_conn,invite_fail_3_not_manager,"",space);
            }
            else if(feedback==4){
                SOCKET.TCP_send(tcp_conn,invite_fail_4_invitee_not_logged_in,"",space);
            }
            else if(feedback==5){
                SOCKET.TCP_send(tcp_conn,invite_fail_5_email_not_found,"",space);
            }
            else if(feedback==6){
                SOCKET.TCP_send(tcp_conn,invite_success,invitee_name_email,n);
            }
        }
        //list invitations
        else if(check_num==10){
            string invitation_list="";
            int feedback=list_invitations_command(shmid,username,invitation_list);
            if(feedback==0){
                SOCKET.TCP_send(tcp_conn,list_invitations_no_invitations,"",space);
            }
            else if(feedback==1){
                SOCKET.TCP_send(tcp_conn,list_invitations_invitation,invitation_list,space);
            }
        }
        //accept
        else if(check_num==11){
            int feedback=accept_command(SOCKET.get_recv_buff(),shmid,login_condition,room_id,username);
            if(feedback==0){
                SOCKET.TCP_send(tcp_conn,accept_format,"",space);
            }
            else if(feedback==1){
                SOCKET.TCP_send(tcp_conn,accept_fail_1_not_logged_in,"",space);
            }
            else if(feedback==2){
                SOCKET.TCP_send(tcp_conn,accept_fail_2_already_in_game_room_1,room_id,accept_fail_2_already_in_game_room_2);
            }
            else if(feedback==3){
                SOCKET.TCP_send(tcp_conn,accept_fail_3_invitation_not_exist,"",space);
            }
            else if(feedback==4){
                SOCKET.TCP_send(tcp_conn,accept_fail_4_invitation_code_incorrect,"",space);
            }
            else if(feedback==5){
                SOCKET.TCP_send(tcp_conn,accept_fail_5_game_started,"",space);
            }
            else if(feedback==6){
                SOCKET.TCP_send(tcp_conn,accept_success,room_id,n);
            }
        }
        //leave room
        else if(check_num==12){
            int feedback=leave_room_command(1,shmid,login_condition,room_id,username);
            if(feedback==0){
                cout<<"leave error\n";
            }
            else if(feedback==1){
                SOCKET.TCP_send(tcp_conn,leave_room_fail_1_not_logged_in,"",space);
            }
            else if(feedback==2){
                SOCKET.TCP_send(tcp_conn,leave_room_fail_2_not_in_room,"",space);
            }
            else if(feedback==3){
                SOCKET.TCP_send(tcp_conn,leave_room_success_1_are_manager,room_id,n);
                room_id="";
            }
            else if(feedback==4){
                SOCKET.TCP_send(tcp_conn,leave_room_success_2_not_manager_game_started_1,room_id,leave_room_success_2_not_manager_game_started_2);
                room_id="";
            }
            else if(feedback==5){
                SOCKET.TCP_send(tcp_conn,leave_room_success_3_not_manager_game_not_started,room_id,n);
                room_id="";
            }
        }
        //start game
        else if(check_num==13){
            string current;
            int feedback=start_game_command(SOCKET.get_recv_buff(),shmid,login_condition,room_id,username);
            if(feedback==0){
                SOCKET.TCP_send(tcp_conn,start_game_format,"",space);
            }
            else if(feedback==1){
                SOCKET.TCP_send(tcp_conn,start_game_fail_1_not_logged_in,"",space);
            }
            else if(feedback==2){
                SOCKET.TCP_send(tcp_conn,start_game_fail_2_not_in_room,"",space);
            }
            else if(feedback==3){
                SOCKET.TCP_send(tcp_conn,start_game_fail_3_not_manager,"",space);
            }
            else if(feedback==4){
                SOCKET.TCP_send(tcp_conn,start_game_fail_4_game_already_started,"",space);
            }
            else if(feedback==5){
                SOCKET.TCP_send(tcp_conn,start_game_fail_5_four_digit_number,"",space);
            }
            else if(feedback==6){

            }
        }
        //guess
        else if(check_num==14){
            string current;
            int feedback=guess_command(SOCKET.get_recv_buff(),shmid,login_condition,room_id,username,current);
            if(feedback==0){
                SOCKET.TCP_send(tcp_conn,guess_format,"",space);
            }
            else if(feedback==1){
                SOCKET.TCP_send(tcp_conn,guess_fail_1_not_logged_in,"",space);
            }
            else if(feedback==2){
                SOCKET.TCP_send(tcp_conn,guess_fail_2_not_in_room,"",space);
            }
            else if(feedback==31){
                SOCKET.TCP_send(tcp_conn,guess_fail_3_are_manager,"",space);
            }
            else if(feedback==32){
                SOCKET.TCP_send(tcp_conn,guess_fail_3_not_manager,"",space);
            }
            else if(feedback==4){
                SOCKET.TCP_send(tcp_conn,guess_fail_4_not_your_turn,current,n);
            }
            else if(feedback==5){
                SOCKET.TCP_send(tcp_conn,guess_fail_5_four_digit_number,"",space);
            }
            else if(feedback==6){

            }
        }
        //state
        else if(check_num==15){
            string output=state_command();
            SOCKET.TCP_send(tcp_conn,space,output,space);
        }
        //invalid input
        else{
            SOCKET.TCP_send(tcp_conn,invalidinput,"",space);
        }
    }
}

int main(){    
    if(!SOCKET.TCP_init()){
        return 0;
    }
    if(!SOCKET.UDP_init()){
        return 0;
    }

    init_share_mem(shmid,key);

    fstream file;

    file.open("efs/server2.txt",ios::trunc);
    file.close();

    file.open("efs/server2.txt",ios::out);
    int num=0;
    file<<to_string(num);
    file.close();

    cout<<"\n*****Server Waiting*****\n";
    pid_t pid,child_pid;

    pid=fork();
    //UDP
    if(pid==0){
        SOCKET.TCP_close();
        while(true){
            if(!SOCKET.UDP_recv()){
                continue;
            }
            // cout<<SOCKET.get_recv_buff()<<"\n";
                
            int check_num=check(SOCKET.get_recv_buff());
            //register
            if(check_num==1){
                int feedback=register_command(SOCKET.get_recv_buff(),shmid);
                if(feedback==0){
                    SOCKET.UDP_send(register_format,"");
                }
                else if(feedback==1){
                    SOCKET.UDP_send(register_fail_1,"");
                }
                else if(feedback==2){
                    SOCKET.UDP_send(register_success,"");
                }
            }
            //list rooms
            else if(check_num==6){
                string room_list="";
                int feedback=list_rooms_command(shmid,room_list);
                if(feedback==0){
                    SOCKET.UDP_send(list_rooms_no_room,"");
                }
                else if(feedback==1){
                    SOCKET.UDP_send(list_rooms_room,room_list);
                }
            }
            //list users
            else if(check_num==7){
                string user_list="";
                int feedback=list_users_command(shmid,user_list);
                if(feedback==0){
                    SOCKET.UDP_send(list_users_no_user,"");
                }
                else if(feedback==1){
                    SOCKET.UDP_send(list_users_user,user_list);
                }
            }
            //invalid input
            else{
                SOCKET.UDP_send(invalidinput,"");
            }
        }
    }
    //TCP
    else if(pid>0){
        for(int t=0;;t++){
            if(!SOCKET.TCP_conn()){
                continue;
            }
            int tcp_conn=SOCKET.get_TCP_conn();
            conn[t]=tcp_conn;
            pthread_t th;
            pool.push_back(th);
            pthread_create(&pool[t],NULL,tcp_thread,&conn[t]);
        }
    }
    else{
        cout<<"fork() error\n";
    }
    return 0;
}
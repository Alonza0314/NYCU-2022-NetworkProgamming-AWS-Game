#pragma once
using namespace std;

const char* regist="register";
const char* login="login";
const char* logout="logout";
const char* create_public_room="create public room";
const char* create_privat_room="create private room";
const char* list_rooms="list rooms";
const char* list_users="list users";
const char* join_room="join room";
const char* invite="invite";
const char* list_invitations="list invitations";
const char* acceptt="accept";
const char* leave_room="leave room";
const char* start_game="start game";
const char* guess="guess";
const char* state="status";

const char* invalidinput="invalid input\n";
const char* space="";
const char* n="\n";

const char* register_format="Usage: register <username> <email> <password>\n";
const char* register_success="Register Successfully\n";
const char* register_fail_1="Username or Email is already used\n";

const char* login_format="login <username> <password>\n";
const char* login_success="Welcome, ";
const char* login_fail_1_username_not_found="Username does not exist\n";
const char* login_fail_2_already_login="You already logged in as ";
const char* login_fail_3_someone_login="Someone already logged in as ";
const char* login_fail_4_wrong_password="Wrong password\n";

const char* logout_success="Goodbye, ";
const char* logout_fail_1_not_logged_in="You are not logged in\n";
const char* logout_fail_2_in_game_room_1="You are already in game room ";
const char* logout_fail_2_in_game_room_2=", please leave game room\n";

const char* create_public_format="create public room <game room id>\n";
const char* create_public_success="You create public game room ";
const char* create_public_fail_1_not_logged_in="You are not logged in\n";
const char* create_public_fail_2_already_in_room_1="You are already in game room ";
const char* create_public_fail_2_already_in_room_2=", please leave game room\n";
const char* create_public_fail_3_room_id_used="Game room ID is used, choose another one\n";

const char* create_privat_format="create private room <game_room_id> <invitation code>\n";
const char* create_privat_success="You create private game room ";
const char* create_privat_fail_1_not_logged_in="You are not logged in\n";
const char* create_privat_fail_2_already_in_room_1="You are already in game room ";
const char* create_privat_fail_2_already_in_room_2=", please leave game room\n";
const char* create_privat_fail_3_room_id_used="Game room ID is used, choose another one\n";

const char* list_rooms_no_room="List Game Rooms\nNo Rooms\n";
const char* list_rooms_room="List Game Rooms\n";

const char* list_users_no_user="List Users\nNo Users\n";
const char* list_users_user="List Users\n";

const char* join_room_format="join room <game room id>\n";
const char* join_room_success="You join game room ";
const char* join_room_welcom_1="Welcome, ";
const char* join_room_welcom_2=" to game!\n";
const char* join_room_fail_1_not_logged_in="You are not logged in\n";
const char* join_room_fail_2_in_room_1="You are already in game room ";
const char* join_room_fail_2_in_room_2=", please leave game room\n";
const char* join_room_fail_3_id_not_exist_1="Game room ";
const char* join_room_fail_3_id_not_exist_2=" is not exist\n";
const char* join_room_fail_4_private_room="Game room is private, please join game by invitation code\n";
const char* join_room_fail_5_game_started="Game has started, you can't join now\n";

const char* invite_format="invite <invitee email>\n";
const char* invite_success="You send invitation to ";
const char* invite_fail_1_not_logged_in="You are not logged in\n";
const char* invite_fail_2_not_join_any_game_room="You did not join any game room\n";
const char* invite_fail_3_not_manager="You are not private game room manager\n";
const char* invite_fail_4_invitee_not_logged_in="Invitee not logged in\n";
const char* invite_fail_5_email_not_found="Email not found\n";

const char* list_invitations_no_invitations="List invitations\nNo Invitations\n";
const char* list_invitations_invitation="List invitations\n";

const char* accept_format="accept <inviter email> <invitation code>\n";
const char* accept_success="You join game room ";
const char* accept_fail_1_not_logged_in="You are not logged in\n";
const char* accept_fail_2_already_in_game_room_1="You are already in game room ";
const char* accept_fail_2_already_in_game_room_2=", please leave game room\n";
const char* accept_fail_3_invitation_not_exist="Invitation not exist\n";
const char* accept_fail_4_invitation_code_incorrect="Your invitation code is incorrect\n";
const char* accept_fail_5_game_started="Game has started, you can't join now\n";

const char* leave_room_success_1_are_manager="You leave game room ";
const char* leave_room_success_2_not_manager_game_started_1="You leave game room ";
const char* leave_room_success_2_not_manager_game_started_2=", game ends\n";
const char* leave_room_success_3_not_manager_game_not_started="You leave game room ";
const char* leave_room_fail_1_not_logged_in="You are not logged in\n";
const char* leave_room_fail_2_not_in_room="You did not join any game room\n";

const char* start_game_format="start game <number of rouds> <guess number>\n";
const char* start_game_fail_1_not_logged_in="You are not logged in\n";
const char* start_game_fail_2_not_in_room="You did not join any game room\n";
const char* start_game_fail_3_not_manager="You are not game room manager, you can't start game\n";
const char* start_game_fail_4_game_already_started="Game has started, you can't start again\n";
const char* start_game_fail_5_four_digit_number="Please enter 4 digit number with leading zero\n";

const char* guess_format="guess <guess number>\n";
const char* guess_fail_1_not_logged_in="You are not logged in\n";
const char* guess_fail_2_not_in_room="You did not join any game room\n";
const char* guess_fail_3_are_manager="You are game room manager, please start game first\n";
const char* guess_fail_3_not_manager="Game has not started yet\n";
const char* guess_fail_4_not_your_turn="Please wait..., current player is ";
const char* guess_fail_5_four_digit_number="Please enter 4 digit number with leading zero\n";

int check(string str){
    string tmp="";
    for(int i=0;i<str.length();i++){
        tmp+=str[i];
        if(tmp==regist){
            return 1;
        }
        else if(tmp==login){
            return 2;
        }
        else if(tmp==logout){
            return 3;
        }
        else if(tmp==create_public_room){
            return 4;
        }
        else if(tmp==create_privat_room){
            return 5;
        }
        else if(tmp==list_rooms){
            return 6;
        }
        else if(tmp==list_users){
            return 7;
        }
        else if(tmp==join_room){
            return 8;
        }
        else if(tmp==invite){
            return 9;
        }
        else if(tmp==list_invitations){
            return 10;
        }
        else if(tmp==acceptt){
            return 11;
        }
        else if(tmp==leave_room){
            return 12;
        }
        else if(tmp==start_game){
            return 13;
        }
        else if(tmp==guess){
            return 14;
        }
        else if(tmp==state){
            return 15;
        }
    }
    return 0;
}
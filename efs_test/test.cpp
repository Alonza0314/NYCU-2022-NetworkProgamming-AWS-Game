#include<iostream>
#include<fstream>
#include<string>

using namespace std;

int main(){
    fstream file;
    
    file.open("efs/test.txt",ios::in);
    if(!file.is_open()){
        cout<<"fuck\n";
        return 0;
    }
    string num_str;
    file>>num_str;
    file.close();

    file.open("efs/test.txt",ios::trunc);
    file.close();

    file.open("efs/test.txt",ios::out);
    int num=stoi(num_str)+1;
    file<<to_string(num);
    file.close();

    return 0;
}
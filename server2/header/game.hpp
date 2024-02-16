#pragma once
#include<ctime>
#include<cstring>

bool check_four(string ans){
    if(ans.size()!=4){
        return false;
    }
    for(int i=0;i<4;i++){
        if(ans[i]<'0'||ans[i]>'9'){
            return false;
        }
    }
    return true;
}

string provide_ans(){
    string tmp_ans="";
    bool record[10];
    int tmp;
    tmp_ans="";
    memset(record,0,sizeof(record));
    srand(time(NULL));
    for(int i=0;i<4;i++){
        tmp=rand()%10;
        while(record[tmp]){
            tmp=rand()%10;
        }
        tmp_ans+=to_string(tmp);
        record[tmp]=1;
    }
    return tmp_ans;
}

string gaming(string guess,string answer){
    bool flag=false;
    int A=0;
    int B=0;
    bool record[4]={false,false,false,false};
    bool arr[4]={false,false,false,false};
    for(int i=0;i<4;i++){
        if(guess[i]==answer[i]){
            A++;
            record[i]=true;
            arr[i]=true;
        }
    }
    for(int i=0;i<4;i++){
        if(arr[i]){
            continue;
        }
        for(int j=0;j<4;j++){
            if(record[j]||j==i){
                continue;
            }
            if(guess[i]==answer[j]){
                B++;
                record[j]=true;
                break;
            }
        }
    }
    return (to_string(A)+"A"+to_string(B)+"B");
}

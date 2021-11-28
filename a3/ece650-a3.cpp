#include <vector>
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <stdio.h>
#include <string.h>

using namespace std;

std::vector<int> parseArg(int argc, char **argv){
    int s=0, n=0, l=0 ,c=0, ds=10, dn=5, dl=5, dc=20;
    std::vector<int> vec;
    for (int i = 1; i < argc; i+=2) {
        std::string str = argv[i];
        int val = std::atoi(argv[i+1]);
        if (str=="-s"){
            s = val;
        }else if(str=="-n"){
            n = val;
        }else if(str=="-l"){
            l = val;
        }else if(str=="-c"){
            c = val;
        }
    }
    if (s==0){
        s = ds;
    }
    if (n==0){
        n = dn;
    }
    if (l==0){
        l = dl;
    }
    if(c==0){
        c = dc;
    }
    vec.push_back(s);
    vec.push_back(n);
    vec.push_back(l);
    vec.push_back(c);
    return vec;
}


int main (int argc, char **argv) {
    int s, n, l ,c;
    std::vector<int> vec;
    std::vector<int> res = parseArg(argc, argv);
    s = res[0];
    n = res[1];
    l = res[2];
    c = res[3];


    // p1[0] for a1 read, p1[1] for rgen write
    int p1[2];
    // p2[0] for a2 read, p2[1] for a1 and readline write
    int p2[2];
    pipe(p1);
    pipe(p2);

    pid_t pid1 = fork();
    // fork readline process
    if (pid1 == 0){
        // setup write
        close(1);
        dup(p2[1]);
        char* argv1[2];
        argv1[0] = (char*)"./readline_process";
        argv1[1] = nullptr;
        execv(argv1[0],argv1);
        exit(1);
    }

    pid_t pid2 = fork();
    // fork a1
    if (pid2== 0){
        // setup read
        close(0);
        dup(p1[0]);
        close(p1[0]);
        // setup write
        close(1);
        dup(p2[1]);
        close(p2[1]);
        char* argv2[3];
        argv2[0] = (char*)"/usr/bin/python3";
        argv2[1] = (char*)"./ece650-a1.py";
        argv2[2] = nullptr;
        execv(argv2[1],argv2);
        exit(1);
    }

    pid_t pid3 = fork();
    // fork a2
    if (pid3 == 0){
        // setup read
        close(0);
        dup(p2[0]);
        close(p2[0]);
        char* argv3[2];
        argv3[0] = (char*)"./ece650-a2";
        argv3[1] = nullptr;
        execv(argv3[0],argv3);
        exit(1);
    }

    pid_t pid4 = fork();
    // fork rgen
    if (pid4 == 0){
        // exec
        close(1);
        dup(p1[1]);
        close(p1[1]);
        char* argv4[6];
        argv4[0] = (char*)"./rgen";
        argv4[1] = (char*)std::to_string(s).c_str();
        argv4[2] = (char*)std::to_string(n).c_str();
        argv4[3] = (char*)std::to_string(l).c_str();
        argv4[4] = (char*)std::to_string(c).c_str();
        argv4[5] = nullptr;
        execv(argv4[0], argv4);
        exit(1);
    }


    pid_t id = wait(0);
    if (id==pid1){
        kill(pid2, SIGTERM);
        kill(pid3, SIGTERM);
        kill(pid4, SIGTERM);
    }else if (id == pid4){
        kill(pid1, SIGTERM);
        kill(pid2, SIGTERM);
        kill(pid3, SIGTERM);
    }

    return 0;
}


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
    std::cout << "Replace with your solution for the main "
              << "driver program of Assignment 3\n";
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

    // fork readline process
    if (fork() == 0){
        // setup write
        close(1);
        dup(p2[1]);
        char* argv1[2];
        argv1[0] = (char*)"/usr/bin/readline_process";
        argv1[1] = nullptr;
        int res1 = execv("/usr/bin/readline_process",argv1);
        int x = 3;
    }

    // fork a1
    if (fork()== 0){
        // setup read
        close(0);
        dup(p1[0]);
        close(p1[0]);
        // setup write
        close(1);
        dup(p2[1]);
        close(p2[1]);
        char* argv2[3];
        argv2[0] = (char*)"python3";
        argv2[1] = (char*)"/usr/bin/ece650-a1.py";
        argv2[2] = nullptr;
        int res2 = execv("/usr/bin/ece650-a1.py",argv2);
        int x =3;
    }

    // fork a2
    if (fork() == 0){
        // setup read
        close(0);
        dup(p2[0]);
        close(p2[0]);
        char* argv3[2];
        argv3[0] = (char*)"/usr/bin/ece650-a2";
        argv3[1] = nullptr;
        int res3 = execv("/usr/bin/ece650-a2",argv3);
        int x = 3;
    }

    // fork rgen
    if (fork() == 0){
        // exec
        close(1);
        dup(p1[1]);
        close(p1[1]);
        char* argv4[5];
        argv4[0] = (char*)"/usr/bin/rgen";
        argv4[1] = (char*)std::to_string(s).c_str();
        argv4[2] = (char*)std::to_string(n).c_str();
        argv4[3] = (char*)std::to_string(l).c_str();
        argv4[4] = (char*)std::to_string(c).c_str();
        int res4 = execv("/usr/bin/rgen", argv4);
        int x =5;
    }

    int id = wait(0);
//    if (id==pid1){
//        kill(pid2, SIGTERM);
//        kill(pid3, SIGTERM);
//        kill(pid4, SIGTERM);
//    }else if (id == pid4){
//        kill(pid1, SIGTERM);
//        kill(pid2, SIGTERM);
//        kill(pid3, SIGTERM);
//    }

    return 0;
}


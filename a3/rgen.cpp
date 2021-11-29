//
// Created by fanzekun on 2021-11-27.
//

#include <unordered_map>
#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fstream>


static const char alpha[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N',
                             'O','P','Q','R','S','T','U','V','W','X','Y','Z',
                               'a','b','c','d','e','f','g','h','i','j','k','l','m','n',
                               'o','p','q','r','s','t','u','v','w','x','y','z'};

const int MAX_ATTETMP = 25;

typedef struct {
    int x;
    int y;
} coordinate;

typedef struct {
    coordinate c1;
    coordinate c2;
} segment;

std::unordered_map<std::string, std::vector<segment>> map;


// assume that s1, s2'length > 0
bool isOverlap(segment s1, segment s2){
    // seg1
    coordinate c1 = s1.c1;
    coordinate c2 = s1.c2;
    // seg2
    coordinate c3 = s2.c1;
    coordinate c4 = s2.c2;

    // c1==c3 and c2==c4
    if (c1.x==c3.x && c1.y==c3.y && c2.x==c4.x && c2.y==c4.y){
        return true;
    }
    // c1==c4 and c2==c3
    if (c1.x==c4.x && c1.y==c4.y && c2.x==c3.x && c2.y==c3.y){
        return true;
    }

    // case1: s1,s2 are both horizontal
    if (c1.x==c2.x && c3.x==c4.x){
        // c3 is between c1 and c2
        // or c4 is between c1 and c2
        if (c1.x == c3.x && ((c3.y-c1.y)*(c3.y-c2.y)<0 || (c4.y-c1.y)*(c4.y-c2.y)<0)){
            return true;
        }else{
            return false;
        }
    }
    if (c1.x==c2.x && c3.x != c4.x){
        return false;
    }
    if (c1.x != c2.x && c3.x==c4.x){
        return false;
    }
    // case2: s1,s2's slope are the same
    int slope1 = (c2.y-c1.y) / (c2.x-c1.x);
    int slope2 = (c4.y-c3.y) / (c4.x-c3.x);
    int b1 = c2.y-slope1*c2.x;
    int b2 = c4.y-slope2*c4.x;
    if (slope1 == slope2 && b1==b2 && ((c3.y-c1.y)*(c3.y-c2.y)<0 || (c4.y-c1.y)*(c4.y-c2.y)<0)){
        return true;
    }
    return false;
}

bool isNonZeroLength(segment s1){
    coordinate c1 = s1.c1;
    coordinate c2 = s1.c2;
    if (c1.x==c2.x && c1.y==c2.y){
        return false;
    }
    return true;
}

unsigned int genRandom(std::ifstream& urandom, int upper){
    unsigned int num = 100;
    urandom.read((char *)&num, sizeof(int));
    return num % upper;
}


std::string genStreetName(std::ifstream& urandom){
    int size = sizeof(alpha);
    int len = genRandom(urandom, 18) + 3;
    std::string res;
    while(true){
        for (int i = 0; i < len; ++i) {
            res+= alpha[genRandom(urandom, size)];
        }
        if (map.find(res) != map.end()){
            res.clear();
        }else{
            return res;
        }
    }
}


coordinate genCord(int c, std::ifstream& urandom){
    int x = genRandom(urandom, 2*c+1) - c;
    int y = genRandom(urandom, 2*c+1) - c;
    coordinate cord;
    cord.x = x;
    cord.y = y;
    return cord;
}

segment genSegment(segment pre, int c, std::ifstream& urandom){
    segment seg;
    seg.c1.x = pre.c2.x;
    seg.c1.y = pre.c2.y;
    while(true){
        seg.c2 = genCord(c, urandom);
        if (isNonZeroLength(seg)){
            return seg;
        }
    }
}

segment genSegment(int c, std::ifstream& urandom){
    segment seg;
    while(true){
        seg.c1 = genCord(c, urandom);
        seg.c2 = genCord(c, urandom);
        if (isNonZeroLength(seg)){
            return seg;
        }
    }
}

// whether overlap with other segments
bool isOverlapWithOtherSegments(segment seg){
    for (auto& it: map) {
        // Do stuff
        std::vector<segment> vec = it.second;
        for (size_t i = 0; i < vec.size(); ++i) {
            segment s = vec[i];
            if (isOverlap(s, seg)){
                return true;
            }
        }
    }
    return false;
}


// whether form cycle in current street
bool isFormCycle(const std::string& name, segment seg){
    std::vector<segment> vec = map[name];
    if (vec.empty()){
        return false;
    }
    segment s = vec[0];
    if (s.c1.x==seg.c2.x && s.c1.y==seg.c2.y){
        return true;
    }
    return false;
}

bool isSegmentVaild(const std::string& name, segment seg){
    if (isOverlapWithOtherSegments(seg)){
        return false;
    }
    if (isFormCycle(name, seg)){
        return false;
    }
    return true;
}

int genStreet(const std::string& name, int n, int c, std::ifstream& urandom){
    // num of segments in this street
    int num_of_seg = genRandom(urandom, n) + 1;
    int count = 0;
    for (int i = 0; i < num_of_seg; ++i) {
        while(count < MAX_ATTETMP){
            segment seg;
            if (i==0){
                seg = genSegment(c, urandom);
            }else{
                seg = genSegment(map[name].back(),c, urandom);
            }
            if (isSegmentVaild(name, seg)){
                map[name].push_back(seg);
                count = 0;
                break;
            }
            count++;
        }
        // fail to generate vaild segment
        if (count == MAX_ATTETMP){
            // do stuff
            return -1;
        }
    }
    return 0;
}



int createGraph(int s, int n, int c, std::ifstream& urandom){
    int num_of_street = genRandom(urandom, s-1) + 2;
    for (int i = 0; i < num_of_street; ++i) {
        const std::string &name = genStreetName(urandom);
        std::vector<segment> segs;
        map[name] = segs;
        if(genStreet(name, n ,c, urandom)==-1){
            return -1;
        }
    }
    return 0;
}

std::string segvec2str(std::vector<segment> vec){
    std::string res;
    std::vector<coordinate> list;
    list.push_back(vec[0].c1);
    list.push_back(vec[0].c2);
    for (size_t i = 1; i < vec.size(); ++i) {
        list.push_back(vec[i].c2);
    }
    for (size_t i = 0; i < list.size(); ++i) {
        res += (" ("+std::to_string(list[i].x)+","+std::to_string(list[i].y)+")");
    }
    return res;
}



void add(){
    for (auto& it : map){
        std::string name = it.first;
        std::string coordinate_str = segvec2str(it.second);
        std::string add_command;
        add_command += "add ";
        add_command += "\"";
        add_command+= name;
        add_command += "\"";
        add_command+=coordinate_str;
        std::cout << add_command << std::endl;
    }
}

void rm(){
    for (auto& it : map){
        std::string name = it.first;
        std::string rm_command;
        rm_command += "rm ";
        rm_command += "\"";
        rm_command+= name;
        rm_command += "\"";
        std::cout << rm_command << std::endl;
    }
}

void gg(){
    std::cout << "gg" << std::endl;
}


int main(int argc, char **argv) {
    int s = std::atoi(argv[1]);
    int n = std::atoi(argv[2]);
    int l = std::atoi(argv[3]);
    int c = std::atoi(argv[4]);
    // open /dev/urandom to read
    std::ifstream urandom("/dev/urandom");
    // check that it did not fail
    if (urandom.fail()) {
        std::cerr << "Error: cannot open /dev/urandom" << std::endl;
        exit(1);
    }
    while(true){
        if(createGraph(s, n, c, urandom) != 0){
            std::cerr << "Error: failed to generate valid input for 25 simultaneous attempts" << std::endl;
            exit(1);
        }
        add();
        gg();
        sleep(genRandom(urandom, l-4) + 5);
        rm();
        map.clear();
    }
}
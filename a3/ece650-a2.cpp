// Compile with c++ ece650-a2cpp -std=c++11 -o ece650-a2
#include <iostream>
#include <sstream>
#include <vector>
#include <deque>
#include <regex>
#include <stdlib.h>

std::vector<std::vector<int>> adjlist;
std::regex v_reg("(V [0-9]+)");
std::regex e_reg("(E \\{(<[0-9]+,[0-9]+>,?)*\\})");
std::regex s_reg("(s [0-9]+ [0-9]+)");
std::regex single_edge_reg("(<[0-9]+,[0-9]+>)");

// 1: wait for specify number of vertices
// 2: wait for specify every edge
// 3: wait for input source and destination
// 4: a cycle has been completed
int state = 1;
int num_of_vertex = 0;


void handlerResult(std::vector<int> res);

std::vector<int> find_shortest_path(int src, int dst);


int main(int argc, char **argv) {
    while (!std::cin.eof()) {
        std::string input_str;
        std::getline(std::cin, input_str);

        if (std::regex_match(input_str, v_reg) && state != 2) {
            std::cout << input_str << std::endl;
            adjlist.clear();
            num_of_vertex = atoi(input_str.substr(2, input_str.size()).c_str());
            for (int i = 0; i < num_of_vertex + 1; ++i) {
                std::vector<int> list{};
                adjlist.push_back(list);
            }
            state = 2;
        } else if (std::regex_match(input_str, e_reg) && state == 2) {
            std::cout << input_str << std::endl;
            std::sregex_iterator cur(input_str.begin(), input_str.end(), single_edge_reg);
            std::sregex_iterator last;
            while (cur != last) {
                std::smatch smatch = *cur;
                std::string single_edge = smatch.str();
                size_t index = single_edge.find(',');
                int src = atoi(single_edge.substr(1, index).c_str());
                int dst = atoi(single_edge.substr(index + 1, single_edge.size() - 1).c_str());
                if (src < 1 || src > num_of_vertex || dst < 1 || dst > num_of_vertex) {
                    std::cerr << "Error: Vertex does not exist" << std::endl;
                    adjlist.clear();
                    break;
                }
                if (src != dst) {
                    adjlist[src].push_back(dst);
                    adjlist[dst].push_back(src);
                }
                cur++;
            }
            state = 3;
        } else if (std::regex_match(input_str, s_reg) && (state == 3 || state == 4)) {
            size_t index1 = input_str.find_first_of(' ');
            size_t index2 = input_str.find_last_of(' ');
            int src = atoi(input_str.substr(index1 + 1, index2).c_str());
            int dst = atoi(input_str.substr(index2 + 1, input_str.size()).c_str());
            if (src < 1 || src > num_of_vertex || dst < 1 || dst > num_of_vertex) {
                std::cerr << "Error: Vertex does not exist" << std::endl;
            } else if (src != dst) {
                std::vector<int> res = find_shortest_path(src, dst);
                handlerResult(res);
            }
            state = 4;
        }
    }
    return 0;
}


std::vector<int> find_shortest_path(int src, int dst) {
    bool visited[num_of_vertex + 1];
    int precessor[num_of_vertex + 1];
    for (int i = 0; i < num_of_vertex + 1; ++i) {
        visited[i] = false;
        precessor[i] = -1;
    }
    std::deque<int> q;
    q.push_back(src);
    visited[src] = true;
    while (!q.empty()) {
        int &u = q.front();
        q.pop_front();
        for (size_t i = 0; i < adjlist[u].size(); ++i) {
            int v = adjlist[u][i];
            if (!visited[v]) {
                visited[v] = true;
                precessor[v] = u;
                if (v == dst) {
                    break;
                }
                q.push_back(v);
            }
        }
    }
    std::vector<int> res;
    if (!visited[dst]) {
        return res;
    } else {
        int vertex = dst;
        while (vertex != -1) {
            res.push_back(vertex);
            vertex = precessor[vertex];
        }
        return res;
    }
}

void handlerResult(std::vector<int> res) {
    if (res.empty()) {
        std::cerr << "Error: No path exists" << std::endl;
        return;
    }
    std::string str;
    while (!res.empty()) {
        str += std::to_string(res.back());
        res.pop_back();
        if (!res.empty()) {
            str += "-";
        }
    }
    std::cout << str << std::endl;
}

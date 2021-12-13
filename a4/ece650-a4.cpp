// Compile with c++ ece650-a2cpp -std=c++11 -o ece650-a2
#include <iostream>
#include <sstream>
#include <vector>
#include <deque>
#include <regex>
#include <stdlib.h>
#include <memory>
#include "./minisat/minisat/core/SolverTypes.h"
#include "./minisat/minisat/core/Solver.h"
#include "./minisat/minisat/mtl/Vec.h"


std::vector<std::vector<int>> adjlist;
std::regex v_reg("(V [0-9]+)");
std::regex e_reg("(E \\{(<[0-9]+,[0-9]+>,?)*\\})");
std::regex single_edge_reg("(<[0-9]+,[0-9]+>)");

// 1: wait for specify number of vertices
// 2: wait for specify every edge
// 3: wait for input source and destination
// 4: a cycle has been completed
int state = 1;
int num_of_vertex = 0;

void compute();

std::vector<int> compute_vertex_cover(int size);

int main(int argc, char **argv) {
    while (!std::cin.eof()) {
        std::string input_str;
        std::getline(std::cin, input_str);

        if (std::regex_match(input_str, v_reg) && state != 2) {
            adjlist.clear();
            num_of_vertex = atoi(input_str.substr(2, input_str.size()).c_str());
            for (int i = 0; i < num_of_vertex; ++i) {
                std::vector<int> list{};
                adjlist.push_back(list);
            }
            state = 2;
        } else if (std::regex_match(input_str, e_reg) && state == 2) {
            std::sregex_iterator cur(input_str.begin(), input_str.end(), single_edge_reg);
            std::sregex_iterator last;
            while (cur != last) {
                std::smatch smatch = *cur;
                std::string single_edge = smatch.str();
                size_t index = single_edge.find(',');
                int src = atoi(single_edge.substr(1, index).c_str());
                int dst = atoi(single_edge.substr(index + 1, single_edge.size() - 1).c_str());
                if (src < 1 || src > num_of_vertex || dst < 1 || dst > num_of_vertex) {
                    std::cout << "Error: Vertex does not exist" << std::endl;
                    adjlist.clear();
                    break;
                }
                if (src != dst) {
                    adjlist[src - 1].push_back(dst - 1);
                    adjlist[dst - 1].push_back(src - 1);
                }
                cur++;
            }
            compute();
            state = 3;
        }
    }
    return 0;
}

void compute() {
    std::vector<int> result;
    int max = num_of_vertex / 2;
    for (int i = 1; i <= max; ++i) {
        result = compute_vertex_cover(i);
        if (!result.empty()) {
            break;
        }
    }

    std::string str;
    int len = result.size();
    for (int i = 0; i < len; ++i) {
        str += std::to_string(result[i]);
        if (i < len) {
            str += " ";
        }
    }
    std::cout << str << std::endl;
}

std::vector<int> compute_vertex_cover(int size) {
    std::vector<int> result;
    std::unique_ptr<Minisat::Solver> solver(new Minisat::Solver());

    Minisat::Lit input[num_of_vertex][size];
    for (int i = 0; i < num_of_vertex; ++i) {
        for (int j = 0; j < size; ++j) {
            input[i][j] = Minisat::mkLit(solver->newVar());
        }
    }
    // type 1
    for (int i = 0; i < size; ++i) {
        Minisat::vec<Minisat::Lit> temp_clause{};
        for (int j = 0; j < num_of_vertex; ++j) {
            temp_clause.push(input[j][i]);
        }
        solver->addClause(temp_clause);
    }

    // type 2
    for (int i = 0; i < num_of_vertex; ++i) {
        for (int j = 0; j < size-1; ++j) {
            for (int k = j + 1; k < size; ++k) {
                solver->addClause(~input[i][j], ~input[i][k]);
            }
        }
    }

    // type 3
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < num_of_vertex-1; ++j) {
            for (int k = j + 1; k < num_of_vertex; ++k) {
                solver->addClause(~input[j][i], ~input[k][i]);
            }
        }
    }

    // type 4
    for (int i = 0; i < num_of_vertex; ++i) {
        int adj_len = (int) adjlist[i].size();
        for (int j = 0; j < adj_len; ++j) {
            int neighbor = adjlist[i][j];
            if (i < neighbor) {
                Minisat::vec<Minisat::Lit> temp_clause{};
                for (int k = 0; k < size; ++k) {
                    temp_clause.push(input[i][k]);
                    temp_clause.push(input[neighbor][k]);
                }
                solver->addClause(temp_clause);
            }
        }
    }
    bool res = solver->solve();

    if (res) {
        for (int i = 0; i < num_of_vertex; ++i) {
            for (int j = 0; j < size; ++j) {
                if (Minisat::toInt(solver->modelValue(input[i][j])) == 0) {
                    result.push_back(i + 1);
                    break;
                }
            }
        }
    }
    return result;
}






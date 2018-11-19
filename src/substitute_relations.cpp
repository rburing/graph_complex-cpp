#include <iostream>
#include <fstream>
#include <ginac/ginac.h>
#include "graph_sum.hpp"
using namespace std;
using namespace GiNaC;

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        cout << "Usage: " << argv[0] << " <graph-sum-filename> <relations-filename>\n";
        return 1;
    }
    string graph_sum_filename(argv[1]), relations_filename(argv[2]);

    ifstream relations_file(relations_filename);
    lst relations;
    parser coefficient_reader;
    for (string lhs, rhs; getline(relations_file, lhs, '=') && relations_file.ignore(1) && getline(relations_file, rhs); )
    {
        relations.append(coefficient_reader(lhs) == coefficient_reader(rhs));
    }

    ifstream graph_sum_file(graph_sum_filename);
    GraphSum<ex> graph_sum = GraphSum<ex>::from_istream(graph_sum_file, [&coefficient_reader](std::string s) -> ex { return coefficient_reader(s); });
    for (auto const& term : graph_sum)
    {
        ex coeff = term.second.subs(relations);
        if (coeff == 0)
            continue;
        cout << term.first << "    " << coeff << "\n";
    }
}

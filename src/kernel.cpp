#include <iostream>
#include <fstream>
#include <ginac/ginac.h>
#include "graph_sum.hpp"
using namespace std;
using namespace GiNaC;

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        cout << "Usage: " << argv[0] << " <graph-sum-filename>\n";
        return 1;
    }

    string graph_sum_filename(argv[1]);
    ifstream graph_sum_file(graph_sum_filename);
    parser coefficient_reader;
    GraphSum<ex> graph_sum = GraphSum<ex>::from_istream(graph_sum_file, [&coefficient_reader](std::string s) -> ex { return coefficient_reader(s); });
    // assume input is reduced modulo skew-symmetry
    lst vars;
    for (auto const& pair : coefficient_reader.get_syms())
        vars.append(pair.second);
    lst eqns;
    for (auto const& term : graph_sum)
        eqns.append(term.second==0);
    for (auto const& eqn : lsolve(eqns, vars)) // , solve_algo::markowitz);
        cout << eqn << "\n";
}

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
    graph_sum.reduce_mod_skew();
    size_t equations = graph_sum.size();
    lst vars;
    for (auto const& pair : coefficient_reader.get_syms())
        vars.append(pair.second);
    size_t i = 0;
    matrix A(equations, vars.nops());
    for (auto const& term : graph_sum)
    {
        size_t j = 0;
        for (ex const& var : vars)
        {
            A(i,j++) = term.second.coeff(var);
        }
        ++i;
    }
    size_t rank = A.rank();
    cout << "Rank: " << rank << "\n"
         << "Nullity: " << vars.nops() - rank << "\n";
}

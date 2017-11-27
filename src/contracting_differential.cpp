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
    GraphSum<ex> d_graph_sum = graph_sum.contracting_differential();
    d_graph_sum.reduce_mod_skew();
    cout << d_graph_sum;
}

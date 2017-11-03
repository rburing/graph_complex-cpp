#include <iostream>
#include "graph.hpp"
#include "graph_sum.hpp"
#include <sstream>
using namespace std;

int main()
{
    Graph edge(3, { {0, 1}, {1, 2} });
    cout << "Graph: " << edge << "\n";
    cout << "Automorphisms: " << edge.automorphism_group() << " (" << edge.automorphism_group().size() << " in total)\n";
    cout << "Graph is " << (edge.is_zero() ? "zero" : "not zero") << "\n";
    int parity = edge.label_canonically();
    cout << "Canonical labeling: " << edge << "\n";
    cout << "Parity of induced edge permutation: " << parity << "\n";

    GraphSum<int> zero_sum({ { edge, 1 } });
    zero_sum.reduce_mod_skew();
    cout << "Zero sum is " << ((zero_sum == 0) ? "zero" : "not zero") << ".\n";

    Graph tetrahedron(4, { {0, 1}, {0, 2}, {0, 3}, {1, 2}, {1, 3}, {2, 3} });
    Graph fivewheel(6, { {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 0},
                         {0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5} });
    Graph roof(6, { {0, 1}, {1, 2}, {2, 3}, {3, 0}, {3, 4},
                    {0, 4}, {4, 5}, {2, 5}, {1, 5}, {0, 2} });

    GraphSum<int> tetrahedral_flow({ { tetrahedron, 1 } });
    tetrahedral_flow.reduce_mod_skew();
    GraphSum<int> fivewheel_flow({ { fivewheel, 2 }, { roof, 5} });
    fivewheel_flow.reduce_mod_skew();

    cout << "Tetrahedral flow:\n" << tetrahedral_flow;
    cout << "5-wheel flow:\n" << fivewheel_flow;

    cout << "Contracting differential of 5-wheel flow:\n";
    GraphSum<int> d_fivewheel_flow = fivewheel_flow.contracting_differential();
    d_fivewheel_flow.reduce_mod_skew();
    cout << (d_fivewheel_flow == 0 ? "Vanishes" : "Does not vanish") << ".\n";

    Graph human(7, { {0,1},{2,0},{2,1},{3,0},{3,4},{4,2},{5,4},{5,1},{6,3},{6,4},{6,5} });
    GraphSum<int> human_cocycle({ { human, 1} });
    cout << "Contracting differential of human:\n";
    GraphSum<int> d_human = human_cocycle.contracting_differential();
    d_human.reduce_mod_skew();
    cout << d_human;

    Graph test;
    cout << test << "\n";
    stringstream ss;
    ss << "2 2   0 1  1 0";
    ss >> test;
    cout << test << "\n";

    GraphSum<int> test_sum;
    stringstream sum_stream;
    sum_stream << "3 2   0 1  1 2    1\n3 2   0 1  1 2    1";
    sum_stream >> test_sum;
    cout << test_sum;
    return 0;
}

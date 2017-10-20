#include <iostream>
#include "graph.hpp"
using namespace std;

int main()
{
    Graph edge(3, { {0, 1}, {1, 2} });
    cout << "Graph: " << edge << "\n";
    cout << "Automorphisms: " << edge.automorphism_group() << " (" << edge.automorphism_group().size() << " in total)\n";
    cout << "Graph is " << (edge.is_zero() ? "zero" : "not zero") << "\n";
    return 0;
}

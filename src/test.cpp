#include <iostream>
#include "graph.hpp"
using namespace std;

int main()
{
    Graph edge(3, { {0, 1}, {1, 2} });
    cout << edge << "\n";
    return 0;
}

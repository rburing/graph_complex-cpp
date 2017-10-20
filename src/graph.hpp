#include <vector>
#include <set>
#include <ostream>

class Graph
{
public:
    typedef char Vertex;
    typedef std::pair<Vertex, Vertex> Edge;
    Graph(size_t vertices, std::vector<Edge> edges);
    typedef std::vector<Vertex> Automorphism; // a (particular type of) vertex permutation
    typedef std::set<Automorphism> AutomorphismGroup;
    AutomorphismGroup automorphism_group();
    bool is_zero();
private:
    friend std::ostream& operator<<(std::ostream& os, const Graph& graph);
    size_t d_vertices;
    std::vector<Edge> d_edges;
};

std::ostream& operator<<(std::ostream& os, const Graph::Vertex vertex);
std::ostream& operator<<(std::ostream& os, const Graph::Automorphism& sigma);
std::ostream& operator<<(std::ostream& os, const Graph::AutomorphismGroup& automorphisms);

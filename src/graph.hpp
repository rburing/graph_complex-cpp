#include <vector>
#include <ostream>

class Graph
{
public:
    typedef char Vertex;
    typedef std::pair<Vertex, Vertex> Edge;
    Graph(size_t vertices, std::vector<Edge> edges);
private:
    friend std::ostream& operator<<(std::ostream& os, const Graph& graph);
    size_t d_vertices;
    std::vector<Edge> d_edges;
};

std::ostream& operator<<(std::ostream& os, const Graph::Vertex vertex);

#include "graph.hpp"

std::ostream& operator<<(std::ostream& os, const Graph::Vertex vertex)
{
    os << (size_t)vertex;
    return os;
}

Graph::Graph(size_t vertices, std::vector<Edge> edges) : d_vertices(vertices), d_edges(edges) {}

std::ostream& operator<<(std::ostream& os, const Graph& graph)
{
    os << "{ ";
    size_t count = 0;
    for (Graph::Edge edge: graph.d_edges)
    {
        os << "{ " << edge.first << ", " << edge.second << " }";
        if (++count != graph.d_edges.size())
            os << ",";
        os << " ";
    }
    os << "}";
    return os;
}

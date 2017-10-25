#ifndef INCLUDED_GRAPH_H_
#define INCLUDED_GRAPH_H_

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
    AutomorphismGroup automorphism_group() const;
    bool is_zero() const;
    int label_canonically();
    bool operator<(const Graph& other) const;
    std::vector<Edge> edges() const;
private:
    friend std::ostream& operator<<(std::ostream& os, const Graph& graph);
    size_t d_vertices;
    std::vector<Edge> d_edges;
};

std::ostream& operator<<(std::ostream& os, const Graph::Vertex vertex);
std::ostream& operator<<(std::ostream& os, const Graph::Automorphism& sigma);
std::ostream& operator<<(std::ostream& os, const Graph::AutomorphismGroup& automorphisms);

#endif

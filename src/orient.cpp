#include <iostream>
#include <fstream>
#include <string>
#include <ginac/ginac.h>
#include "graph_sum.hpp"
using namespace std;
using namespace GiNaC;

// TODO: Refactor

int main(int argc, char* argv[])
{
    if (argc != 2 && argc != 3)
    {
        cout << "Usage: " << argv[0] << " <graph-sum-filename> [--insert-jacobiator]\n";
        return 1;
    }

    bool insert_jacobiator = argc == 3 && string(argv[2]) == "--insert-jacobiator";

    //cerr << "# option: insert-jacobiator = " << (insert_jacobiator ? "yes" : "no") << "\n";

    string graph_sum_filename(argv[1]);
    ifstream graph_sum_file(graph_sum_filename);
    parser coefficient_reader;
    GraphSum<ex> graph_sum = GraphSum<ex>::from_istream(graph_sum_file, [&coefficient_reader](std::string s) -> ex { return coefficient_reader(s); });
    typedef std::tuple<ex, Graph, std::vector<int>, std::vector<bool> > Term;
    for (const GraphSum<ex>::Term& pair: graph_sum)
    {
        const Graph& graph = pair.first;
        const std::vector<Graph::Edge>& edges = graph.edges();
        ex coeff = pair.second;
        size_t jacobi_vertex = 0;
        do
        {
            std::vector<int> degrees(graph.vertices(), 2);
            if (insert_jacobiator)
                degrees[jacobi_vertex] = 3;
            std::vector<bool> orientations(edges.size(), false);
            std::vector<Term> terms = { Term { coeff, graph, degrees, orientations } };
            size_t edge_index = 0;
            for (const Graph::Edge& edge : edges)
            {
                std::vector<Term> left_terms;
                std::vector<Term> right_terms;
                for (Term& term : terms)
                {
                    Graph left_graph = std::get<1>(term);
                    std::vector<int> left_degrees = std::get<2>(term);
                    std::vector<bool> left_orientation = std::get<3>(term);
                    left_orientation[edge_index] = false;
                    if (left_degrees[edge.second] >= 1)
                    {
                        int left_factor = left_degrees[edge.second];
                        int temp_sum = 0;
                        for (int u = 0; u != edge.second; ++u)
                            temp_sum += left_degrees[u];
                        int left_koszul_sign = temp_sum % 2 == 0 ? 1 : -1;
                        left_degrees[edge.second]--;
                        left_terms.push_back(Term { std::get<0>(term) * left_factor * left_koszul_sign, left_graph, left_degrees, left_orientation });
                    }
                    Graph right_graph = std::get<1>(term);
                    std::vector<int> right_degrees = std::get<2>(term);
                    std::vector<bool> right_orientation = std::get<3>(term);
                    right_orientation[edge_index] = true;
                    if (right_degrees[edge.first] >= 1)
                    {
                        int right_factor = right_degrees[edge.first];
                        int temp_sum = 0;
                        for (int u = 0; u != edge.first; ++u)
                            temp_sum += right_degrees[u];
                        int right_koszul_sign = temp_sum % 2 == 0 ? 1 : -1;
                        right_degrees[edge.first]--;
                        right_terms.push_back(Term { std::get<0>(term) * right_factor * right_koszul_sign, right_graph, right_degrees, right_orientation });
                    }
                }
                terms = left_terms;
                terms.reserve(left_terms.size() + right_terms.size());
                terms.insert(terms.end(), right_terms.begin(), right_terms.end());
                edge_index++;
            }
            for (Term& term : terms)
            {
                Graph& graph = std::get<1>(term);
                int external_vertices = 0;
                std::vector<int>& degrees = std::get<2>(term);
                for (size_t j = 0; j != graph.vertices(); ++j)
                    external_vertices += degrees[j];
                size_t new_vertices = graph.vertices() + external_vertices;
                if (insert_jacobiator)
                    ++new_vertices;
                std::vector<Graph::Edge> oriented_edges = graph.edges();
                std::vector<bool>& orientation = std::get<3>(term);
                for (size_t j = 0; j != oriented_edges.size(); ++j)
                {
                    oriented_edges[j].first += external_vertices;
                    oriented_edges[j].second += external_vertices;
                    if (!orientation[j])
                    {
                        oriented_edges[j] = { oriented_edges[j].second, oriented_edges[j].first };
                    }
                }
                std::vector<Graph::Edge> ground_edges;
                Graph::Vertex ground_vertex = 0;
                for (Graph::Vertex v = 0; v != graph.vertices(); ++v)
                {
                    for (int j = 0; j != degrees[v]; ++j)
                        ground_edges.push_back({ v + external_vertices, ground_vertex++ });
                }
                oriented_edges.reserve(oriented_edges.size() + ground_edges.size());
                oriented_edges.insert(oriented_edges.end(), ground_edges.begin(), ground_edges.end());
                int jacobi_shift = 0;
                do
                {
                    std::vector<Graph::Edge> new_edges = oriented_edges;
                    // Insert Jacobiator
                    if (insert_jacobiator)
                    {
                        size_t extra_vertex = new_vertices - 1;
                        // Jacobiator is to be inserted at internal vertex: jacobi_vertex
                        std::vector<size_t> jacobi_edges;
                        std::vector<Graph::Vertex> jacobi_targets;
                        size_t edge_index = 0;
                        for (Graph::Edge e : new_edges)
                        {
                            if (e.first == external_vertices + jacobi_vertex)
                            {
                                jacobi_edges.push_back(edge_index);
                                jacobi_targets.push_back(e.second);
                            }
                            ++edge_index;
                        }
                        Graph::Edge new_edge = { extra_vertex, external_vertices + jacobi_vertex };
                        new_edges.push_back(new_edge);
                        // Change target
                        new_edges[jacobi_edges[2]].first = extra_vertex;
                        // Shift targets
                        for (size_t i = 0; i != 3; ++i)
                            new_edges[jacobi_edges[i]].second = jacobi_targets[(i + jacobi_shift) % 3];
                    }
                    if (insert_jacobiator)
                        cout << "1    ";
                    cout << external_vertices << " " << (new_vertices - external_vertices) << " 1   ";
                    for (Graph::Vertex v = external_vertices; v != new_vertices; ++v)
                    {
                        // Find outgoing edges. NB: Edge ordering is used here.
                        int outdegree = 0;
                        for (Graph::Edge e : new_edges)
                        {
                            if (e.first == v)
                            {
                                cout << e.second << " ";
                                outdegree += 1;
                            }
                        }
                        cout << "  ";
                    }
                    cout << "   ";
                    if (insert_jacobiator)
                        cout << external_vertices + jacobi_vertex << " " << (new_vertices - 1) << "    ";
                    ex magic_constant = 1;
                    if (insert_jacobiator)
                        magic_constant = ex(-2)/27; // constant c depending on all conventions,
                                                    // which makes [[P, Or(\gamma)]] = c * ( Or(\gamma)([[P,P]], ..., P) + ... + Or(\gamma)(P, ..., [[P,P]]) )
                                                    // for graph cocycles \gamma
                    cout << magic_constant * std::get<0>(term) << "\n";
                } while (insert_jacobiator && ++jacobi_shift != 3);
            }
        } while (insert_jacobiator && ++jacobi_vertex != graph.vertices());
    }
}

#include "graph_sum.hpp"
#include <string>
#include <sstream>
#include <algorithm>

template<class T>
std::ostream& operator<<(std::ostream& os, const std::pair<Graph, T>& term)
{
    os << term.first << "    " << term.second << "\n";
    return os;
}

template<class T>
std::ostream& operator<<(std::ostream& os, const GraphSum<T>& graph_sum)
{
    for (const typename GraphSum<T>::Term& term : graph_sum)
        os << term;
    return os;
}

template <class T>
std::istream& operator>>(std::istream& is, GraphSum<T>& graph_sum)
{
    typename GraphSum<T>::Term term;
    for (std::string line; getline(is, line); )
    {
        if (line.length() == 0 || line[0] == '#') // also skip comments
            continue;
        std::stringstream ss(line);
        ss >> term.first >> term.second;
        graph_sum[term.first] += term.second;
    }
    return is;
}

template <class T>
void GraphSum<T>::reduce_mod_skew()
{
    GraphSum<T> result;
    for (typename GraphSum<T>::Term term : *this)
    {
        term.second *= term.first.label_canonically();
        result[term.first] += term.second;
    }
    for (typename GraphSum<T>::iterator it = result.begin(); it != result.end(); )
        if (it->second == 0 || it->first.is_zero())
            it = result.erase(it);
        else
            ++it;
    std::swap(*this, result);
}

template <class T>
bool GraphSum<T>::operator==(const GraphSum<T> &other) const
{
    GraphSum<T> difference = *this - other;
    difference.reduce_mod_skew();
    return difference.size() == 0;
}

template <class T>
bool GraphSum<T>::operator==(int other) const
{
    if (other != 0)
        return false;
    GraphSum<T> difference = *this;
    difference.reduce_mod_skew();
    return difference.size() == 0;
}

template <class T>
bool GraphSum<T>::operator!=(const GraphSum<T> &other) const
{
    return !(*this == other);
}

template <class T>
bool GraphSum<T>::operator!=(int other) const
{
    return !(*this == other);
}

template <class T>
GraphSum<T>& GraphSum<T>::operator+=(const GraphSum<T>& rhs)
{
    for (const typename GraphSum<T>::Term& term : rhs)
        (*this)[term.first] += term.second;
    return *this;
}

template <class T>
GraphSum<T> operator+(GraphSum<T> lhs, const GraphSum<T> &rhs)
{
    lhs += rhs;
    return lhs;
}

template <class T>
GraphSum<T>& GraphSum<T>::operator-=(const GraphSum<T>& rhs)
{
    for (const typename GraphSum<T>::Term& term : rhs)
        (*this)[term.first] -= term.second;
    return *this;
}

template <class T>
GraphSum<T> operator-(GraphSum<T> lhs, const GraphSum<T>& rhs)
{
    lhs -= rhs;
    return lhs;
}

template <class T>
GraphSum<T> operator*(T lhs, GraphSum<T> rhs)
{
    for (auto& term : rhs)
        term.second *= lhs;
    return rhs;
}

template <class T>
GraphSum<T> GraphSum<T>::contracting_differential()
{
    GraphSum<T> differential;
    for (const typename GraphSum<T>::Term& term : *this)
    {
        //std::cerr << term.first << "\n";
        size_t edge_count = 0;
        for (const typename Graph::Edge& edge : term.first.edges())
        {
            int sign = edge_count++ % 2 == 0 ? 1 : -1;
            //std::cerr << "Contracting { " << edge.first << ", " << edge.second << "}: sign " << sign << "\n";
            std::vector<Graph::Edge> new_edges = term.first.edges();
            // redirect everything from edge.second to edge.first
            for (typename Graph::Edge& new_edge : new_edges)
            {
                if (new_edge.first == edge.second) new_edge.first = edge.first;
                if (new_edge.second == edge.second) new_edge.second = edge.first;
            }
            // delete the newly created loop
            for (auto it = new_edges.begin(); it != new_edges.end(); )
                if (it->first == edge.first && it->second == edge.first)
                    it = new_edges.erase(it);
                else
                    ++it;
            // shift vertex labeling down, and sort edges
            for (typename Graph::Edge& new_edge : new_edges)
            {
                if (new_edge.first >= edge.second) --new_edge.first;
                if (new_edge.second >= edge.second) --new_edge.second;
                if (new_edge.first > new_edge.second)
                    new_edge = {new_edge.second, new_edge.first};
            }
            // skip graphs with double edges
            bool double_edges = false;
            std::map<Graph::Edge, size_t> edge_counts;
            for (typename Graph::Edge& new_edge : new_edges)
            {
                if (++edge_counts[new_edge] == 2)
                {
                    double_edges = true;
                    break;
                }
            }
            Graph new_graph(term.first.vertices() - 1, new_edges);
            //std::cerr << "New graph (before relabeling): " << new_graph << "\n";
            //std::cerr << "Double edges? " << (double_edges ? "Yes" : "No") << "\n";
            if (double_edges)
                continue;
            sign *= new_graph.label_canonically();
            differential[new_graph] += sign*term.second;
            //std::cerr << "New graph (after relabeling): " << new_graph << "    " << sign*term.second << "\n";
        }
    }
    return differential;
}

template <class T>
GraphSum<T> GraphSum<T>::expanding_differential()
{
    GraphSum<T> differential;
    for (const typename GraphSum<T>::Term& term : *this)
    {
        std::map<Graph::Vertex, size_t> degrees = term.first.degree_sequence();
	Graph::Vertex new_vertex = term.first.vertices();
        for (auto& vertex_degree : degrees)
	{
            if (vertex_degree.second <= 3)
                continue;
            Graph::Vertex vertex = vertex_degree.first;
	    size_t degree = vertex_degree.second;
            // Blow up this vertex of valency > 3
            std::vector<Graph::Edge> new_edges = term.first.edges();
            new_edges.reserve(new_edges.size() + 1); // make room for new edge and prevent reallocation
            std::vector<Graph::Vertex*> incident_edges(degree);
            size_t edge_count = 0;
            for (Graph::Edge& edge : new_edges)
            {
                if (edge.first == vertex || edge.second == vertex)
                    incident_edges[edge_count++] = edge.first == vertex ? &edge.first : &edge.second;
            }
            new_edges.push_back({ vertex, new_vertex });
            // Decompose valency = n + m with n, m >= 2
	    for (size_t n = 2; n != degree - 1; ++n)
	    {
                // Choose n edges to redirect to the new vertex of the new edge
                std::string bitmask(n, 1);
                bitmask.resize(degree, 0);
                do {
                    for (size_t i = 0; i != degree; ++i)
		        *incident_edges[i] = bitmask[i] ? new_vertex : vertex;
                    Graph graph(term.first.vertices()+1, new_edges);
		    differential[graph] += term.second;
                } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
            }
            // Do not create graphs with vertices of valency < 3
	}
    }
    return differential;
}

template <class T>
GraphSum<T> GraphSum<T>::from_istream(std::istream& is, std::function<T(std::string)> const& parser)
{
    GraphSum<T> graph_sum;
    for (std::string line; getline(is, line); )
    {
        if (line.length() == 0 || line[0] == '#') // also skip comments
            continue;
        Graph graph;
        std::stringstream ss(line);
        ss >> graph;
        std::string coefficient_str;
        ss >> coefficient_str;
        T coefficient = parser(coefficient_str);
        graph_sum[graph] += coefficient;
    }
    return graph_sum;
}

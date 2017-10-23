#ifndef INCLUDED_GRAPH_SUM_H_
#define INCLUDED_GRAPH_SUM_H_

#include "graph.hpp"
#include <map>
#include <ostream>

template<class T> class GraphSum;
template<class T> std::ostream& operator<<(std::ostream&, const GraphSum<T>&);

template<class T>
class GraphSum: public std::map< Graph, T >
{
    using std::map< Graph, T >::map; // inherit constructors
    using std::map< Graph, T >::operator[]; // inherit subscript operator

    public:
    typedef std::pair<Graph, T> Term;
    GraphSum<T>& operator+=(const GraphSum<T>& rhs);
    GraphSum<T>& operator-=(const GraphSum<T>& rhs);
    GraphSum<T>& operator=(const GraphSum<T>&) = default;
    void reduce_mod_skew();
    bool operator==(const GraphSum<T>& other) const;
    bool operator==(int other) const;
    bool operator!=(const GraphSum<T>& other) const;
    bool operator!=(int other) const;

    private:
    friend std::ostream& operator<< <>(std::ostream& os, const GraphSum<T>& graph_sum);
};

template <class T>
GraphSum<T> operator+(GraphSum<T> lhs, const GraphSum<T>& rhs);
template <class T>
GraphSum<T> operator-(GraphSum<T> lhs, const GraphSum<T>& rhs);
template <class T>
GraphSum<T> operator*(T lhs, GraphSum<T> rhs);

#include "graph_sum.tpp"

#endif
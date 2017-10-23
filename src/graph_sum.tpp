#include "graph_sum.hpp"

template<class T>
std::ostream& operator<<(std::ostream& os, const GraphSum<T>& graph_sum)
{
    for (const typename GraphSum<T>::Term& term : graph_sum)
        os << term.first << "    " << term.second << "\n";
    return os;
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

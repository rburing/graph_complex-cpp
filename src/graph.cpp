#include "graph.hpp"
#include "permutation.hpp"
#include "nauty.h"
#include "naugroup.h"
#include <functional>
#include <algorithm>

std::ostream& operator<<(std::ostream& os, const Graph::Vertex vertex)
{
    os << (size_t)vertex;
    return os;
}

Graph::Graph(size_t vertices, std::vector<Edge> edges) : d_vertices(vertices), d_edges(edges)
{
    // store edges as { source, target } with source <= target:
    for (Edge& edge : d_edges)
        if (edge.first > edge.second)
            edge = {edge.second, edge.first};
}

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

std::ostream& operator<<(std::ostream& os, const Graph::Automorphism& sigma)
{
    os << "{ ";
    for (size_t i = 0, count = 0; i != sigma.size(); ++ i)
    {
        os << sigma[i];
        if (++count != sigma.size())
            os << ",";
        os << " ";
    }
    os << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Graph::AutomorphismGroup& automorphisms)
{
    os << "{ ";
    size_t count = 0;
    for (const Graph::Automorphism& sigma : automorphisms)
    {
        os << sigma;
        if (++count != automorphisms.size())
            os << ",";
        os << " ";
    }
    os << "}";
    return os;
}

Graph::AutomorphismGroup Graph::automorphism_group() const
{
    /* The following is adapted from nautyex3.c */

    DYNALLSTAT(graph,g,g_sz);
    DYNALLSTAT(int,lab,lab_sz);
    DYNALLSTAT(int,ptn,ptn_sz);
    DYNALLSTAT(int,orbits,orbits_sz);
    static DEFAULTOPTIONS_GRAPH(options);
    statsblk stats;

    int n = d_vertices, m = SETWORDSNEEDED(n);
    grouprec *group;

 /* The following causes nauty to call two procedures which
        store the group information as nauty runs. */
        
    options.userautomproc = groupautomproc;
    options.userlevelproc = grouplevelproc;

    nauty_check(WORDSIZE,m,n,NAUTYVERSIONID);

    DYNALLOC2(graph,g,g_sz,m,n,"malloc");
    DYNALLOC1(int,lab,lab_sz,n,"malloc");
    DYNALLOC1(int,ptn,ptn_sz,n,"malloc");
    DYNALLOC1(int,orbits,orbits_sz,n,"malloc");

    EMPTYGRAPH(g,m,n);
    for (std::pair<Graph::Vertex, Graph::Vertex> edge : d_edges)
        ADDONEEDGE(g, edge.first, edge.second, m);

    densenauty(g,lab,ptn,orbits,&options,&stats,m,n,NULL);

 /* Get a pointer to the structure in which the group information
    has been stored.  If you use TRUE as an argument, the
    structure will be "cut loose" so that it won't be used
    again the next time nauty() is called.  Otherwise, as
    here, the same structure is used repeatedly. */
        
    group = groupptr(FALSE);

 /* Expand the group structure to include a full set of coset
    representatives at every level.  This step is necessary
    if allgroup() is to be called. */
        
    makecosetreps(group);

 /* Add all automorphisms to a set (jumping through hoops
    to get a C-style callback function). */

    std::set<Graph::Automorphism> automorphisms;
    static std::function<void(int*, int)> add_automorphism_bounce;
    auto add_automorphism = [](int* aut, int n)
    {
        add_automorphism_bounce(aut, n);
    };
    add_automorphism_bounce = [&automorphisms](int* aut, int n)
    {
        automorphisms.insert(Graph::Automorphism(aut, aut + n));
    };
    allgroup(group, add_automorphism);

    return automorphisms;
}

bool Graph::is_zero() const
{
    for (Automorphism sigma : automorphism_group())
    {
        std::vector<Graph::Edge> new_edges = d_edges;
        std::vector<char> induced_edge_permutation(d_edges.size());
        size_t count = 0;
        for (Graph::Edge& edge : new_edges)
        {
            edge.first = sigma[edge.first];
            edge.second = sigma[edge.second];
            if (edge.first > edge.second)
                edge = {edge.second, edge.first};
            induced_edge_permutation[count++] = std::distance(d_edges.begin(), std::find(d_edges.begin(), d_edges.end(), edge));
        }
        if (parity(induced_edge_permutation) == -1)
            return true;
    }
    return false;
}

int Graph::label_canonically()
{
    DYNALLSTAT(int,lab,lab_sz);
    DYNALLSTAT(int,invlab,invlab_sz);
    DYNALLSTAT(int,ptn,ptn_sz);
    DYNALLSTAT(int,orbits,orbits_sz);
    DYNALLSTAT(int,map,map_sz);
    DYNALLSTAT(graph,g1,g1_sz);
    DYNALLSTAT(graph,cg1,cg1_sz);
    static DEFAULTOPTIONS_GRAPH(options);
    statsblk stats;

    options.getcanon = TRUE;

    int n = d_vertices, m = SETWORDSNEEDED(n), i;
    nauty_check(WORDSIZE,m,n,NAUTYVERSIONID);

    DYNALLOC1(int,lab,lab_sz,n,"malloc");
    DYNALLOC1(int,invlab,invlab_sz,n,"malloc");
    DYNALLOC1(int,ptn,ptn_sz,n,"malloc");
    DYNALLOC1(int,orbits,orbits_sz,n,"malloc");
    DYNALLOC1(int,map,map_sz,n,"malloc");

    // construct the nauty graph:
    DYNALLOC2(graph,g1,g1_sz,n,m,"malloc");
    EMPTYGRAPH(g1,m,n);
    for (i = 0; (size_t)i < d_edges.size(); ++i)
        ADDONEEDGE(g1,d_edges[i].first,d_edges[i].second,m);

    // get a canonical labeling of it:
    DYNALLOC2(graph,cg1,cg1_sz,n,m,"malloc")
    densenauty(g1,lab,ptn,orbits,&options,&stats,m,n,cg1);

    // extract the list of edges from the canonical labeling, ordering them lexicographically:
    std::vector<Edge> new_edges(d_edges.size());
    size_t edge_count = 0;
    for (size_t k = 0; k < m*(size_t)n; ++k)
    {
        setword neighbors = cg1[k]; // bit at position l is 1 if there is an edge from k to l
        unsigned int neighbor = 0;
        while (neighbors)
        {
            TAKEBIT(neighbor, neighbors); // get the position of the first 1-bit, and set that bit to 0
            if (k < neighbor) // avoid duplicates
                new_edges[edge_count++] = {k, neighbor};
        }
    }

    /*
    std::cerr << "Old edges: " << *this << "\n";
    std::cerr << "New edges: " << Graph(d_vertices, new_edges) << "\n";
    */

    // compute the inverse of the isomorphism
    for (int k = 0; k < n; ++k)
        invlab[lab[k]] = k;

    /*
    std::cout << "Isomorphism:";
    for (i = 0; i < n; ++i)
        std::cout << " " << invlab[i];
    std::cout << "\n";
    */

    // calculate permutation of edges induced by the isomorphism:
    std::vector<char> induced_edge_permutation(d_edges.size());
    size_t count = 0;
    for (Graph::Edge edge : d_edges)
    {
        edge.first = invlab[(int)edge.first];
        edge.second = invlab[(int)edge.second];
        if (edge.first > edge.second)
            edge = {edge.second, edge.first};
        induced_edge_permutation[count++] = std::distance(new_edges.begin(), std::find(new_edges.begin(), new_edges.end(), edge));
    }

    /*
    std::cerr << "Induced edge permutation: ";
    for (size_t i = 0; i != induced_edge_permutation.size(); ++i)
        std::cerr << (int)induced_edge_permutation[i] << " ";
    std::cerr << "\n";
    */

    d_edges = new_edges;

    return parity(induced_edge_permutation);
}

bool Graph::operator<(const Graph& other) const
{
    return std::tie(this->d_vertices, this->d_edges) < std::tie(other.d_vertices, other.d_edges);
}

std::vector<Graph::Edge> Graph::edges() const
{
    return d_edges;
}

size_t Graph::vertices() const
{
    return d_vertices;
}

#include "graph.hpp"
#include "nauty.h"
#include "naugroup.h"
#include <functional>

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

Graph::AutomorphismGroup Graph::automorphism_group()
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

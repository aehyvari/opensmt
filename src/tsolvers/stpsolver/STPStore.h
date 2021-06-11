#ifndef OPENSMT_STPSTORE_H
#define OPENSMT_STPSTORE_H

#include <cstdint>
#include <vector>
#include <Number.h>
#include <SolverTypes.h>
#include <PtStructs.h>
// implementations of template functions #included below class definitions

struct VertexRef {
    uint32_t x;

    inline bool operator==(VertexRef other) const { return x == other.x; }

    inline bool operator!=(VertexRef other) const { return x != other.x; }
};

struct VertexRefHash {
    uint32_t operator() (const VertexRef& v) const { return v.x; }
};

struct EdgeRef {
    uint32_t x;

    inline bool operator==(EdgeRef other) const { return x == other.x; }

    inline bool operator!=(EdgeRef other) const { return x != other.x; }
};

static VertexRef VertRef_Undef = VertexRef{INT32_MAX};
static EdgeRef EdgeRef_Undef = EdgeRef{INT32_MAX};

template<class T>
struct Edge {
    VertexRef from, to;     // vertices of this edge
    EdgeRef neg;            // the logical negation of this edge
    T cost;                 // cost of this edge

    uint32_t setTime;       // timestamp of when this was assigned as true (0 if it wasn't assigned)

    Edge() = default;

    Edge(Edge &&other) = default;

    Edge(VertexRef from, VertexRef to, EdgeRef neg, T cost, uint32_t setTime)
            : from(from), to(to), neg(neg), cost(cost), setTime(setTime) {}

    Edge &operator=(Edge &&other) = default;

    // copy semantics are disabled (we want edges to only exist in a store)
    Edge(const Edge &other) = delete;

    Edge &operator=(const Edge &other) = delete;
};

template<class T>
class STPStore {
private:
    uint32_t vertices;                  // number of created vertices (a vertex doesn't actually carry any information)
    std::vector<Edge<T>> edges;         // list of all created edges

public:
    STPStore() : vertices(1) {}         // the 'zero' vertex is created implicitly
    VertexRef createVertex();

    EdgeRef createEdge(VertexRef from, VertexRef to, T cost);

    size_t vertexNum() const { return vertices; }

    size_t edgeNum() const { return edges.size(); }

    Edge<T> &getEdge(EdgeRef e) { return edges[e.x]; }

    const Edge<T> &getEdge(EdgeRef e) const { return edges[e.x]; }

    EdgeRef getNegation(EdgeRef e) { return edges[e.x].neg; }

    static VertexRef zero() { return VertexRef{0}; }

    void setNegation(EdgeRef a, EdgeRef b);

    void clear();
};

#include "STPStore_implementations.hpp"

#endif //OPENSMT_STPSTORE_H

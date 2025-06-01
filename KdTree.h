#ifndef __KdTree_h
#define __KdTree_h

#include "KNN.h"
#include "Utils.h"
#include <functional>
#include <utility>

namespace tcii::p1
{ // begin namespace tcii::p1

using namespace cg;

// PointHolder = array de pontos
template <size_t D, IsReal R, IsPointArray<Vec, R, D> A>
class PointHolder
{
public:
  using Point = Vec<D, R>;

  PointHolder(A&& points):
    _points{std::move(points)}
  {
    // do nothing
  }

  auto& points() const
  {
    return _points;
  }

private:
  A _points;

}; // PointHolder

template <size_t D, typename R, typename A>
class KdTree: public PointHolder<D, R, A>
{
public:
  using Base = PointHolder<D, R, A>;
  using Bounds = cg::Bounds<D, R>;
  using Point = typename Base::Point;
  using PointFunc = std::function<bool(const A&, unsigned)>;
  using KNN = p1::KNN<R>;

  // definindo mínimos e máximos da KD-tree
  const struct Params
  {
    unsigned maxPointsPerNode;
    unsigned minPointsPerNode;
    unsigned maxDepth;

  } params;

  // maxPointsPerNode = 20
  // minPointsPerNode = 5
  // maxDepth = 8
  // otimização ou código inútil?
  static constexpr Params dflParams()
  {
    return {20, 5, 8};
  }

  // construtor a partir de points
  KdTree(A&& points, const Params& params = dflParams());

  // KdTree não tem construtor de cópia e nem op= de cópia
  KdTree(const KdTree&) = delete;
  KdTree& operator =(const KdTree&) = delete;

  // construtor de movimento
  KdTree(KdTree&& other) noexcept:
    Base{std::move(other)},
    _root{std::exchange(other._root, nullptr)},
    _nodeCount{other._nodeCount},
    _leafCount{other._leafCount},
    params{other.params}
  {
    // do nothing
  }

  // não tem que deletar a árvore inteira...?
  ~KdTree()
  {
    delete _root;
  }

  // limite da kdtree = limite do nó raiz
  // faz sentido, já que o nó raiz tem todos os pontos
  auto& bounds() const
  {
    return _root->bounds;
  }

  auto nodeCount() const
  {
    return _nodeCount;
  }

  auto leafCount() const
  {
    return _leafCount;
  }

  // achar k vizinhos de acordo com um filtro e um ponto central
  KNN findNeighbors(const Point& point,
    unsigned k,
    PointFunc filter = {}) const;
  
  // achar k vizinhos de acordo com um filtro e um índice do ponto central
  auto findNeighbors(unsigned index, unsigned k, PointFunc filter = {}) const
  {
    assert(index < this->points().size());
    return findNeighbors(this->points()[index], k, filter);
  }

  // aplicar a função f para cada vizinho de point, em um raio limitado e de acordo com um filtro
  void forEachNeighbor(const Point& point,
    R radius,
    PointFunc f,
    PointFunc filter = {}) const;

  // aplicar a função f para cada vizinho do ponto no index, em um raio limitado e de acordo com um filtro
  void forEachNeighbor(unsigned index,
    R radius,
    PointFunc f,
    PointFunc filter = {}) const
  {
    assert(index < this->points().size());
    forEachNeighbor(this->points()[index], radius, f, filter);
  }

private:
  struct Node
  {
    Bounds bounds;

    Node(const Bounds& bounds, unsigned depth):
      bounds{bounds}
    {
      // TODO
    }

  }; // Node

  Node* _root{};
  unsigned _nodeCount{};
  unsigned _leafCount{};

}; // KdTree

// construção da KdTree
template <size_t D, typename R, typename A>
KdTree<D, R, A>::KdTree(A&& points, const Params& params):
  Base{std::move(points)},
  params{params}
{
  _root = new Node{computeBounds<D, R>(this->points()), 0};
  _nodeCount = _leafCount = 1;
  // TODO
}

// implementação de findNeighbors
template <size_t D, typename R, typename A>
auto
KdTree<D, R, A>::findNeighbors(const Point& point,
  unsigned k,
  PointFunc filter) const -> KNN
{
  KNN knn{k};

  // TODO
  return knn;
}

// implementação de forEachNeighbor
template <size_t D, typename R, typename A>
void
KdTree<D, R, A>::forEachNeighbor(const Point& point,
  R radius,
  PointFunc f,
  PointFunc filter) const
{
  f(this->points(), 0);
  // TODO
}

template <typename R, typename A> using KdTree3 = KdTree<3, R, A>;

} // end namespace tcii::p1

#endif // __KdTree_h

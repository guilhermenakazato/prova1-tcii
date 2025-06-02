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

  auto begin() const {
    return _points;
  }

  auto end() const {
    return _points + points.size();
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
    // unsigned depth;
    // Node* _children[2];
    // unsigned _firstPoint;
    // unsigned _pointCount;

    Node(const Bounds& bounds, unsigned depth):
      bounds{bounds}, depth{depth}
    {
      // TODO
      // factory pattern!
      /*
        se tem como dividir e ainda não chegou no final (depth=8), então divida
        ao dividir, mantenha o pai como Branch e o filho como Leaf
        chamar recursivamente o método de construção da kdtree?
      */
    }

  }; // Node

  // struct Branch: Node {
  //   Node* _children = new Node[2];

  //   Branch() {

  //   }
  // };

  // struct Leaf: Node {
  //   unsigned _firstPoint;
  //   unsigned _pointCount;
  // };

  /*
    Um ramo possui um ponteiro para seus dois n´os filhos (que podem ser alocados 
    sequencialmente em memória dinâmica). Uma folha contém um índice usado para 
    determinar qual é o primeiro ponto do subconjunto de pontos do nó e um contador 
    do número de pontos.

    Branch: Node {
      _leftChild
      _rightChild
    }

    Leaf: Node {
      _firstPoint
      _pointCounter
    }
  */

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
  
  /*
    pensamentos aleatórios
    talvez, ao invés de _root ser Node, ser um Branch
    assim, terá children
    usar estratégia de lookahead de compiladores
    criar sempre Branch, menos quando não for possível realizar a divisão
    quando não é possível dividir?
    quando o Node satisfaz minPoints <= pointCount <= maxPoints
    depthNode == maxDepth
    uma função recursiva seria útil

    buildTree(Node* currentNode, ?unsigned i?) {
      if(i == 2) {
        return;
      } else {
        if(qtdePontosNo > maxPoints && floor(qtdePontosNo/2) > minPoints && currentNode->depth + 1 < maxDepth)
          dividir
        senao 
          i++;
      }
    }
  */

  // TODO
  // provavelmente tudo errado
  // dividir somente se o nível for menor que o máximo e se tiver mais pontos que o permitido
  // nó auxiliar pra não perder o começo da árvore
  // Node* aux{_root};
  // while(aux->depth < params.maxDepth && aux->_pointCount > params.maxDepth) {
  //   _points.
  // } 

  // _root->_children = aux->_children;
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

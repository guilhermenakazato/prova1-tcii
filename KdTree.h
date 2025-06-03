#ifndef __KdTree_h
#define __KdTree_h

#include "KNN.h"
#include "Utils.h"
#include <functional>
#include <utility>
#include <span>

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

  // auto begin() const {
  //   return _points;
  // }

  // auto end() const {
  //   return _points + points.size();
  // }

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
  static constexpr Params dflParams()
  {
    return {2, 1, 8};
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
    unsigned depth;
    bool isLeaf;

    union {
      Node* children[2];
      struct {
        size_t pointCount;
        unsigned firstPoint;
      } leafData;
    } nodeData;

    Node(const Bounds& bounds, unsigned depth, size_t pointCount, unsigned firstPoint):
      bounds{bounds}, 
      depth{depth}
    {
      isLeaf = true;
      nodeData.leafData.pointCount = pointCount;
      nodeData.leafData.firstPoint = firstPoint;
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
  std::cout.precision(3);
  cout << "Original points" << endl;
  for(auto i = this->points().begin(); i != this->points().end(); i++) {
    cout << *i << " ";
  }
  cout << endl;

  auto size = this->points().size();
  _root = new Node{computeBounds<D, R>(this->points()), 0, size, 0};
  _nodeCount = _leafCount = 1;
  
  auto canBeDivided = [params](Node leaf) {
    if (!leaf.isLeaf) return false;
    auto leafPointCount = leaf.nodeData.leafData.pointCount;
    return leafPointCount > params.maxPointsPerNode && leaf.depth < params.maxDepth;
  };
  
  // dimension tem que ser genérico dps!!
  int dimension = 3;
  vector<unsigned> indexVector;
  indexVector.reserve(size);

  // inicializando vetor de índices
  // lembre-se que ele é útil pois não podemos alterar a ordem dos pontos! 
  for(int i = 0; i < size; i++) {
    indexVector[i] = i;
  }

  // definindo a função pra poder fazer recursivo...
  std::function<void(size_t, size_t, unsigned int, Node&)> buildTree;
  buildTree = [&](size_t begin, size_t end, unsigned currentDepth, Node& currentNode) {
    Node& aux = currentNode;

    while(canBeDivided(aux)) {
      size_t median = (begin + end) / 2;
      
      std::nth_element(
        indexVector.begin() + begin, 
        indexVector.begin() + median, 
        indexVector.begin() + end + 1, 
        [this, currentDepth, dimension](unsigned a, unsigned b) {
          return this->points()[a][currentDepth % dimension] < this->points()[b][currentDepth % dimension];
        }
      );

      currentNode.isLeaf = false;
      
      // span pega intervalo [a;b[
      // TODO: completar isso para que os filhos esejam de acordo com indexVector
      span leftIndexes{indexVector.begin() + begin, indexVector.begin() + median + 1};
      span rightIndexes{indexVector.begin() + median + 1, indexVector.begin() + end + 1};

      // vector<typeof()>

      size_t leftPointCount = median - begin + 1;
      size_t rightPointCount = end - median;
      currentDepth++;
    
      // cout << "Left children: " << leftPointCount << ", depth: " << currentDepth << endl;
      // for(auto& e : left) {
      //   cout << e << " ";
      // } 
      // cout << endl;
     
      // cout << "Right children: " << rightPointCount << ", depth: " << currentDepth << endl;
      // for(auto& e : right) {
      //   cout << e << " ";
      // } 
      // cout << endl;

      currentNode.nodeData.children[0] = new Node{computeBounds<D, R>(left), currentDepth, leftPointCount, indexVector[begin]}; 
      currentNode.nodeData.children[1] = new Node{computeBounds<D, R>(right), currentDepth, rightPointCount, indexVector[median + 1]};
      _nodeCount += 2;
      _leafCount++;
      
      buildTree(begin, median, currentDepth, *currentNode.nodeData.children[0]);
      buildTree(median + 1, end, currentDepth, *currentNode.nodeData.children[1]);
    }
  };

  buildTree(0, size - 1, 0, *_root);
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

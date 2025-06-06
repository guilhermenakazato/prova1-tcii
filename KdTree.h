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
        size_t firstPoint;
      } leafData;
    } nodeData;

    Node(const Bounds& bounds, unsigned depth, size_t pointCount, size_t firstPoint):
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
  vector<unsigned> indexVector;
}; // KdTree

// construção da KdTree
template <size_t D, typename R, typename A>
KdTree<D, R, A>::KdTree(A&& points, const Params& params):
  Base{std::move(points)},
  params{params}
{
  auto size = this->points().size();
  _root = new Node{computeBounds<D, R>(this->points()), 0, size, 0};
  _nodeCount = _leafCount = 1;
  
  auto canBeDivided = [params](Node leaf) {
    if (!leaf.isLeaf) return false;
    auto leafPointCount = leaf.nodeData.leafData.pointCount;
    return leafPointCount > params.maxPointsPerNode && leaf.depth < params.maxDepth;
  };
  
  size_t dimension = D;

  // inicializando vetor de índices
  // lembre-se que ele é útil pois não podemos alterar a ordem dos pontos! 
  indexVector.reserve(size);
  for(int i = 0; i < size; i++) {
    indexVector[i] = i;
  }
 
  // definindo a função pra poder fazer recursivo... (que feio!)
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
      span leftIndexes{indexVector.begin() + begin, indexVector.begin() + median + 1};
      span rightIndexes{indexVector.begin() + median + 1, indexVector.begin() + end + 1};
      
      size_t leftPointCount = median - begin + 1;
      size_t rightPointCount = end - median;

      PointVector<D,R> left(leftPointCount);
      PointVector<D,R> right(rightPointCount);

      auto populateChildPointVector = [this](std::span<unsigned> indexVector, PointVector<D,R>& vector) {
        transform(
          indexVector.begin(),
          indexVector.end(),
          vector.begin(),
          [&](int i) {
            return this->points()[i];
          }
        );
      };

      populateChildPointVector(leftIndexes, left);
      populateChildPointVector(rightIndexes, right);
      currentDepth++;

      // to com duvidas na modelagem nessa parte.....
      // ver com o Jun dps...... antes tava indexVector[begin] e indexVector[median+1]
      currentNode.nodeData.children[0] = new Node{computeBounds<D, R>(left), currentDepth, leftPointCount, begin}; 
      currentNode.nodeData.children[1] = new Node{computeBounds<D, R>(right), currentDepth, rightPointCount, median + 1};
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
  /**
   * um ponto pode ter, no máximo, n - 1 vizinhos.
   * a heap e o KNN parecem funcionar perfeitamente mesmo se k >= n
   * porém, não consegui fazer o sort funcionar corretamente
   * vale a pena arrumar esse "bug" ou será que só o assert já tá bom?
   */

  assert(k < this->points().size());
  using namespace std;
  KNN knn{k};
  
  std::function<void(const Node&)> traverse;
  traverse = [&](const Node& currentNode) {
    if(currentNode.isLeaf) {
      unsigned firstPoint = currentNode.nodeData.leafData.firstPoint;
      size_t pointCount = currentNode.nodeData.leafData.pointCount;
      
      for(int i = 0; i < pointCount; i++) {
        int elementIndex = indexVector[firstPoint + i];
        Point currentPoint = this->points()[elementIndex];
                
        // se o ponto no índice atual for o próprio ponto recebido pela função, ignorar
        if(point == currentPoint)
        continue;

        if(filter && !filter(this->points(), elementIndex))
          continue;
        
        knn.add(distance<D,R>(point, currentPoint), elementIndex);
      }
    } else {
      // algoritmo do prof. Pagliosa
      int dimension = currentNode.depth % D;
      R d0 = distance<D, R>(point, currentNode.nodeData.children[0]->bounds);
      R d1 = distance<D, R>(point, currentNode.nodeData.children[1]->bounds);
      
      /**
       * só ignorar o lado se a distância do ponto pro bound do nó for maior que 
       * a pior distância em KNN.neighbours() e KNN estiver cheio
       * tá certo isso que eu disse...?
       */
      if (d0 < d1) {
        if (d0 > knn.getMaxDist() && knn.isFull())
          return;
        traverse(*currentNode.nodeData.children[0]);
        if (d1 > knn.getMaxDist() && knn.isFull())
          return;
        traverse(*currentNode.nodeData.children[1]);
      } else {
        if (d1 > knn.getMaxDist() && knn.isFull()) 
          return;
        traverse(*currentNode.nodeData.children[0]);
        if (d0 > knn.getMaxDist() && knn.isFull())
          return;
        traverse(*currentNode.nodeData.children[1]);
      }
    }

  };

  traverse(*_root);
  
  knn.sort();
  for(int i = 0; i < knn.neighbors().size(); i++) {
    int neighborIndex = knn.neighbors()[i].second;

    cout << "Vizinho " << (i + 1) << ": " << this->points()[neighborIndex] << endl;
    cout << "Distância do vizinho pro ponto: " << knn.neighbors()[i].first << endl << endl;
  }

  return knn;
}

/**
 * não tem que ser em ordem de acordo com a distância, né?
 */
// implementação de forEachNeighbor
template <size_t D, typename R, typename A>
void
KdTree<D, R, A>::forEachNeighbor(const Point& point,
  R radius,
  PointFunc f,
  PointFunc filter) const
{
  using namespace std;
  
  std::function<void(const Node&)> traverse;
  traverse = [&](const Node& currentNode) {
    if(currentNode.isLeaf) {
      unsigned firstPoint = currentNode.nodeData.leafData.firstPoint;
      size_t pointCount = currentNode.nodeData.leafData.pointCount;
      
      for(int i = 0; i < pointCount; i++) {
        int elementIndex = indexVector[firstPoint + i];
        Point currentPoint = this->points()[elementIndex];
        
        // se o ponto no índice atual for o próprio ponto recebido pela função, ignorar
        if(point == currentPoint)
        continue;
        
        // só os pontos que satisfazem a condição de filter são aceitos
        if(filter && !filter(this->points(), elementIndex)) {
          cout << "Entrei!" << endl;
          continue;
        }
        
        cout << "Ponto a ser checado: " << currentPoint << endl;
        if(distance<D,R>(point, currentPoint) <= radius) {
          if(!f(this->points(), elementIndex))
            return;
        }

      }
    } else {
      // algoritmo do prof. Pagliosa
      int dimension = currentNode.depth % D;
      R d0 = distance<D, R>(point, currentNode.nodeData.children[0]->bounds);
      R d1 = distance<D, R>(point, currentNode.nodeData.children[1]->bounds);
      
      /**
       * só ignorar o lado se a distância do ponto pro bound do nó for maior que 
       * a pior distância em KNN.neighbours() e KNN estiver cheio
       * tá certo isso que eu disse...?
       */
      if (d0 < d1) {
        if (d0 > radius)
          return;
        traverse(*currentNode.nodeData.children[0]);
        if (d1 > radius)
          return;
        traverse(*currentNode.nodeData.children[1]);
      } else {
        if (d1 > radius) 
          return;
        traverse(*currentNode.nodeData.children[0]);
        if (d0 > radius)
          return;
        traverse(*currentNode.nodeData.children[1]);
      }
    }

  };

  traverse(*_root);
}

template <typename R, typename A> using KdTree3 = KdTree<3, R, A>;

} // end namespace tcii::p1

#endif // __KdTree_h

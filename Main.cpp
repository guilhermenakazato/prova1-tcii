#include "KdTree.h"
#include "Utils.h"
#include <iostream>

using namespace tcii::p1;

int
main()
{
  constexpr size_t D{3};
  using R = float;
  using A = PointVector<D, R>;

  // criando uma KdTree com n pontos
  constexpr size_t np{4};
  KdTree<D, R, A> tree{prand<D, R>(np)};
  auto& points = tree.points();
  auto& bounds = tree.bounds();

  std::cout.precision(3);
  std::cout << "Tree bounds: " << bounds << '\n';
  std::cout << "Points: " << points.size() << '\n';

  R d{};

  for (unsigned i = 0; i < np; ++i)
    d += distance(points[i], bounds);
  std::cout << "Distance sum: " << d << '\n';

  // 10 vizinhos do ponto no índice 0 e imprimindo aqueles em um raio de 0.5
  (void)tree.findNeighbors(0, 10);
  tree.forEachNeighbor(0,
    2.f,
    [](const A& points, unsigned index)
    {
      std::cout << points[index] << '\n';
      return true;
    });
    
  puts("Press any key to exit...");
  (void)getchar();
  return 0;
}

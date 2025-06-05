#include "KdTree.h"
#include "Utils.h"
#include "KNN.h"
#include "ParticleSystem.h"
#include "NameableObject.h"
#include "Renderer.h"
#include <cstdio>
#include <iostream>

using namespace tcii::p1;
using namespace tcii::physx;
using namespace tcii::cg;

int
main()
{
  constexpr size_t D{3};
  using R = float;
  using A = PointVector<D, R>;

  // criando uma KdTree com n pontos
  constexpr size_t np{5};
  KdTree<D, R, A> tree{prand<D, R>(np)};
  auto& points = tree.points();
  auto& bounds = tree.bounds();

  std::cout.precision(3);
  std::cout << "Tree bounds: " << bounds << '\n';
  std::cout << "Points: " << points.size() << '\n';

  // pra q serve isso
  R d{};
  for (unsigned i = 0; i < np; ++i)
    d += distance(points[i], bounds);
  std::cout << "Distance sum: " << d << '\n';

  // n vizinhos do ponto no índice 0 e imprimindo aqueles em um raio 
  (void)tree.findNeighbors(0, 3);
  tree.forEachNeighbor(0, 
   2.f,
   [](const A& points, unsigned index)
   {
     std::cout << points[index] << '\n';

     return true;
   });

  //auto ps = ParticleSystem<Vec3f>::New();

  //ps->setParticleBuffer(1000);

  //auto pb = ps->particles();

  //pb->add(Vec3f{ 1.0f, 1.5f, 2.0f }, Vec3f{ 123.0f, 456.0f, 678.0f });
  //pb->add(Vec3f{ 2.5f, 3.0f, 3.5f }, Vec3f(111.0f, 222.0f, 444.0f));
  //pb->add(Vec3f{ 4.0f, 4.5f, 5.0f }, Vec3f(333.0f, 666.0f, 999.0f));

  //auto pc = pb->particleCount();

  //printf("%s (capacity: %d)\n", ps->name(), pb->capacity());
  //std::cout << "Particles: " << pc << "\nBounds: ";
  //(pc ? std::cout << ps->bounds() : std::cout << "<empty>") << '\n';

  //ParticleArray<Vec3f> pa(*ps); // constructor
  //std::cout << "Testing ParticleArray:\n";
  //for (auto i = 0u; i < pa.size(); i++) // .size() and operator[]()
  //{
  //    std::cout << "Particle Position: " << std::get<0>(pa[i]) << " of RGB " << std::get<1>(pa[i]) << '\n'; // Talvez corrigir o jeito que chama
  //}

  //Renderer renderer;
  //ps->render(renderer);

	// KNN<float> heap{7};
  //   heap.add(100.0f, 0);
	// heap.add(50.0f, 1);
	// heap.add(75.0f, 4);
	// heap.add(25.0f, 6);
	// heap.add(125.0f, 2);
	// heap.add(200.0f, 5);
	// heap.add(150.0f, 3);

	// auto aux = heap.neighbors();

	// for (size_t i = 0; i < 7; ++i) {
	// 	auto pair = aux[i];
	//     std::cout << "KNN[" << i << "] = " << pair.first << " " << pair.second << '\n';
	// }
		
  puts("Press any key to exit...");
  (void)getchar();
  return 0;
}

#include "KdTree.h"
#include "Utils.h"
#include "KNN.h"
#include "ParticleSystem.h"
#include "NameableObject.h"
#include "Renderer.h"
#include "Utils.h"
#include "Vec2.h"
#include "Bounds2.h"
#include <cstdio>
#include <iostream>

using namespace tcii::p1;
using namespace tcii::physx;
using namespace tcii::cg;

int
main()
{
	constexpr size_t D{ 3 };
	using R = float;
	using A = PointVector<D, R>;

	// criando uma KdTree com n pontos
	constexpr size_t np{ 5 };
	KdTree<D, R, A> tree{ prand<D, R>(np) };
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
	(void)tree.findNeighbors(0, 4);
	tree.forEachNeighbor(
		0,
		2.f,
		[](const A& points, unsigned index)
		{
			return points[index][0] > 0.350f;
		},
		[](const A& points, unsigned index) {
			return points[index][1] > 0.300f;
		}	
	);


	/////////////////////////////////////////////////////// Particle System
	//auto ps = ParticleSystem<Vec3f>::New();

	//ps->setParticleBuffer(1000);

	//auto pb = ps->particles();

	//pb->add(Vec3f{ 1.0f, 1.5f, 2.0f }, Vec3f{ 123.0f, 456.0f, 678.0f});
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
	//	std::cout << "Particle Position: " << std::get<0>(pa[i]) << " of RGB " << std::get<1>(pa[i]) << '\n'; // Talvez corrigir o jeito que chama
	//}
	//std::cout << "Filter on particle 0 (should be true): " << filter(pa, 0, { 123.0f, 456.0f, 678.0f }) << std::endl;
	//std::cout << "Filter on particle 1 (should be false): " << filter(pa, 1, { 123.0f, 456.0f, 678.0f }) << std::endl;

	//for (size_t i = 0; i < 7; ++i) 
	//{
	//		auto pair = aux[i];
	//	std::cout << "KNN[" << i << "] = " << pair.first << " " << pair.second << '\n';
	//}

	//Renderer renderer;
	//ps->render(renderer);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////// Heap
	  // KNN<float> heap{7};
	  // heap.add(100.0f, 0);
	  // heap.add(50.0f, 1);
	  // heap.add(75.0f, 4);
	  // heap.add(25.0f, 6);
	  // heap.add(125.0f, 2);
	  // heap.add(200.0f, 5);
	  // heap.add(150.0f, 3);

	  // auto aux = heap.neighbors();
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////


	/////////////////////////////////////////////////////// Vec2 Bounds 2
	//std::cout << "===Vec2===\n";
	//Vec2f v2a{ 1.0f, 2.0f };
	//Vec2f v2b{ 3.0f, 4.0f };
	//Vec2f v2c = v2a + v2b;
	//Vec2f v2d = v2b - v2a;
	//Vec2f v2e = 2.0f * v2a;
	//Vec2f v2f = v2a * 3.0f;
	//float v2len = v2a.length();

	//std::cout << "[(1,2)]Vec2f v2a: " << v2a << std::endl;
	//std::cout << "[(3,4)]Vec2f v2b: " << v2b << std::endl;
	//std::cout << "[(4,6)]v2a + v2b: " << v2c << std::endl;
	//std::cout << "[(2,2)]v2b - v2a: " << v2d << std::endl;
	//std::cout << "[(2,4)]2.0 * v2a: " << v2e << std::endl;
	//std::cout << "[(3,6)]v2a * 3.0: " << v2f << std::endl;
	//std::cout << "[(2.63067)]v2a.length(): " << v2len << std::endl;
	//std::cout << "[(true)]v2a == v2a: " << (v2a == v2a) << std::endl;
	//std::cout << "[(false)]v2a == v2b: " << (v2a == v2b) << std::endl;

	//std::cout << "\n===Bounds2===\n";
	//Bounds2f b2;
	//b2.setEmpty();
	//b2.inflate(v2a);
	//b2.inflate(v2b);
	//std::cout << "[(1,2) (3,4)]inflate(v2a) inflate(v2b)" << b2 << std::endl;
	//std::cout << "[1,2]min(): " << b2.min() << std::endl;
	//std::cout << "[3,4]max(): " << b2.max() << std::endl;
	//std::cout << "[true]contains(1,2): " << b2.contains(v2a) << std::endl;
	//std::cout << "[false]contains(Vec2f{0.0f, 0.0f}): " << b2.contains(Vec2f{ 0.0f, 0.0f }) << std::endl;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	puts("Press any key to exit...");
	(void)getchar();
	return 0;
}
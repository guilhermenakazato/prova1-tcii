#include "ParticleKdTree.h"
#include <iostream>

using namespace tcii::p1;
using namespace tcii::cg;
using namespace tcii::physx;

// imprimindo partículas.... (D = 3 fixo)
void printing_particles(const ParticleArray<Vec3f>& arr, Vec3f center) {
    std::cout << "\nParticle[index] [(position), (color)] in (distance)\n";
    for (auto i = 0u; i < arr.size(); i++) {
        std::cout << "Particle[" << i << "]" 
            << " [" << arr[i] << ", " << arr.color(i) << "] in (" 
			<< distance<3, float>(center, arr[i]) << ")\n";
    }
}

template <size_t D, IsReal R, IsPointArray<Vec, R, D> A>
void printing_points(KdTree<D,R,A>& tree) {
    std::cout << "\nPoint[index] [(position)] in (distance)\n";
    for (auto i = 0u; i < tree.points().size(); i++) {
        std::cout << "Point[" << i << "]" 
            << " [" << tree.points()[i] << "] in (" 
			<< distance<D, R>(tree.points()[0], tree.points()[i]) << ")\n";;
    }
}

// imprimindo KNN
void printing_knn(const ParticleArray<Vec3f>& arr, const KNN<float>& knn) {
    std::cout << "\nFound " << knn.size() << " neighbors.\n";
    for (auto i = 0u; i < knn.size(); i++) {
        std::cout << "Neighbor[" << i << "]" << " ["
            << arr[knn.neighbors()[i].second] << ", " << arr.color(knn.neighbors()[i].second) <<
            "] in (" << knn.neighbors()[i].first << ")\n";
    }
}

template <size_t D, typename R, typename A>
inline
void printing_knn(auto& tree, const KNN<R>& knn) {
    std::cout << "\nFound " << knn.size() << " neighbors.\n";
    for (auto i = 0u; i < knn.size(); i++) {
        std::cout << "Neighbor[" << i << "]" << " ["
            << tree.points()[knn.neighbors()[i].second] <<
            "] in (" << knn.neighbors()[i].first << ")\n";
    }
}

// imprimindo o que tá no vetor within
void printing_within(const std::vector<unsigned> within, const ParticleArray<Vec3f>& arr, Vec3f center, float radius) {
    std::cout << "\n" << within.size() << " particles within radius "
        << radius << " around center " << center << ".\n\n";

    for (auto i = 0u; i < within.size(); i++) {
        std::cout << "Particle[" << within[i] << "]"
            << " [" << arr[within[i]] << ", "
            << arr.color(within[i]) << "] in ("
            << distance<3, float>(center, arr[within[i]]) << ")\n";
    }
}

// Função auxiliar para criar partículas dentro de um determinado intervalo
void make_particles(ParticleSystem<Vec3f>* ps, float min, float max, size_t n) {
    auto pb = ps->particles();
    for (auto i = 0u; i < n; i++) {
        Vec3f pos = prand<3, float>(min, max);
        Vec3f color = prand<3, float>(0.0f, 1.0f);
        pb->add(pos, color);
    }
}

// Função auxiliar para colorir partículas com uma cor específica
// A coloração é apenas para testes em partículas criadas aleatoriamente
void color_particles(ParticleSystem<Vec3f>* ps, size_t n, Vec3f color) {
    auto pb = ps->particles();
    for (auto i = 0u; i < n; i++) {
		if(i % 2 == 1) pb->setColor(i, color);
    }
}

// Função auxiliar, explorada em sala de aula, para criar partículas de distância exata de um centro
void strict_radius_particles(ParticleSystem<Vec3f>* ps, size_t n, Vec3f center, float radius) {
    auto pb = ps->particles();
    for (auto i = 0u; i < n; i++) {
        Vec3f dir = prand<3, float>(-1.0f, 1.0f).versor();
        Vec3f color = prand<3, float>(0.0f, 1.0f);
        pb->add(center + radius * dir, color);
    }
}

// Função auxiliar, explorada em sala de aula, para criar partículas com distância aleatória dentro de um raio
void loose_radius_particles(ParticleSystem<Vec3f>* ps, size_t n, Vec3f center, float radius) {
    auto pb = ps->particles();
    for (auto i = 0u; i < n; i++) {
        Vec3f dir = prand<3, float>(-1.0f, 1.0f).versor();
        Vec3f color = prand<3, float>(0.0f, 1.0f);
        pb->add(center + srand<float>(0.0f,radius) * dir, color);
    }
}

/**
 * Teste para vetores de pontos simples
 */

// Nessa função, deixamos fixada a procura de vizinhos para o ponto no índice 0
template <size_t dimension, IsReal realNumber>
void simpleKdTree(unsigned particles, unsigned neighbors, realNumber radius) {
    constexpr size_t D{dimension};
	using R = realNumber;
	using A = PointVector<D, R>;
	unsigned np{particles};

    KdTree<D,R,A> tree{prand<D,R>(np)};
    printing_points<D,R,A>(tree);
    printing_knn<D,R,A>(tree, tree.findNeighbors(0, neighbors));
}

// Para os seguintes testes, a busca por vizinhos foi realizada em {0,0,0} 
// que, não necessariamente, é posição de uma partícula. 

/*
Função de Teste(n partículas, k vizinhos)
- Partículas aleatórias
- Sem filtro, sem raio
*/
void random_nofilter_noradius(int n_particles, int k_neighbors, Vec3f chosen_origin) {
    auto ps = ParticleSystem<Vec3f>::New();
    ps->setParticleBuffer(n_particles);
    make_particles(ps, -1.0f, 1.0f, n_particles);
	ParticleArray<Vec3f> particles(*ps);
    KdTree3<float, ParticleArray<Vec3f>> tree(std::move(particles));

    auto knn = tree.findNeighbors(chosen_origin, k_neighbors);

    printing_particles(particles, chosen_origin);

    printing_knn(particles, knn);
}

/*
Função de Teste(n partículas, k vizinhos, origem, raio)
- Partículas aleatórias
- Sem filtro, com raio
- Strict para distância exatamente raio
- Loose para distância <= raio
- Assume-se que o ponto de origem para a busca é o mesmo que o centro da esfera
*/
void random_nofilter_radius(int n_particles, int k_neighbors, Vec3f chosen_origin, float radius) {
    auto ps = ParticleSystem<Vec3f>::New();
    ps->setParticleBuffer(n_particles);

    strict_radius_particles(ps, n_particles, chosen_origin, radius);
    //loose_radius_particles(ps, n_particles, chosen_origin, radius);

    ParticleArray<Vec3f> particles(*ps);
    KdTree3<float, ParticleArray<Vec3f>> tree(std::move(particles));

	printing_particles(particles, chosen_origin);
    
    std::vector<unsigned> within;

    // Por tratar-se de pontos flutuantes, define-se um erro de 1e-4f
    tree.forEachNeighbor(chosen_origin, radius + 1e-4f, [&](const ParticleArray<Vec3f>&, unsigned i) {
        within.push_back(i);
        return true;
    });

    printing_within(within, particles, chosen_origin, radius);

    // Exemplo de busca por vizinhos restritos ao raio
    auto ps_within = ParticleSystem<Vec3f>::New();
    ps_within->setParticleBuffer(within.size());
    auto pb_within = ps_within->particles();
    
    for (auto i = 0u; i < within.size(); i++) {
        pb_within->add(particles[i], particles.color(i));
    }

    ParticleArray<Vec3f> particles_within(*ps_within);
    KdTree3<float, ParticleArray<Vec3f>> tree_within(std::move(particles_within));

    auto knn = tree_within.findNeighbors(chosen_origin, k_neighbors);
    printing_knn(particles_within, knn);
    // Talvez o exemplo não seja muito prático. 
}

/*
Função de Teste(n partículas, k vizinhos, origem, cor)
- Partículas aleatórias
- Com filtro, sem raio
- Pela natureza aleatória ao definir uma cor, colore-se uma parte das partículas com uma cor específica 
*/
void random_filter_noradius(int n_particles, int k_neighbors, Vec3f chosen_origin, Vec3f set_color) {
    auto ps = ParticleSystem<Vec3f>::New();
    ps->setParticleBuffer(n_particles);
    make_particles(ps, -1.0f, 1.0f, n_particles);
    ParticleArray<Vec3f> particles(*ps);
    KdTree3<float, ParticleArray<Vec3f>> tree(std::move(particles));

    color_particles(ps, n_particles, set_color);

    auto color_filter = [&](const ParticleArray<Vec3f>& arr, unsigned i) {
        return arr.color(i) == set_color;
        };

    auto knn = tree.findNeighbors(chosen_origin, k_neighbors, color_filter);

    printing_particles(particles, chosen_origin);

    printing_knn(particles, knn);
}

/*
Função de Teste(n partículas, k vizinhos, origem, raio, cor)
- Partículas aleatórias
- Com filtro, com raio
- Strict para distância exatamente raio
- Loose para distância <= raio
- Pela natureza aleatâria ao definir uma cor, colore-se uma parte das partículas com uma cor específica
*/
void random_filter_radius(int n_particles, int k_neighbors, Vec3f chosen_origin, float radius, Vec3f set_color) {
    auto ps = ParticleSystem<Vec3f>::New();
    ps->setParticleBuffer(n_particles);

    //strict_radius_particles(ps, n_particles, chosen_origin, radius);
    loose_radius_particles(ps, n_particles, chosen_origin, radius);

    ParticleArray<Vec3f> particles(*ps);
    KdTree3<float, ParticleArray<Vec3f>> tree(std::move(particles));
    color_particles(ps, n_particles, set_color);

    printing_particles(particles, chosen_origin);

    std::vector<unsigned> within;

    // Por tratar-se de pontos flutuantes, define-se um erro de 1e-4f
    tree.forEachNeighbor(chosen_origin, radius + 1e-4f, [&](const ParticleArray<Vec3f>&, unsigned i) {
        within.push_back(i);
        return true;
        });

    printing_within(within, particles, chosen_origin, radius);

    // Exemplo de busca por vizinhos restritos ao raio
    auto ps_within = ParticleSystem<Vec3f>::New();
    ps_within->setParticleBuffer(within.size());
    auto pb_within = ps_within->particles();

    for (auto i = 0u; i < within.size(); i++) {
        pb_within->add(particles[i], particles.color(i));
    }

    ParticleArray<Vec3f> particles_within(*ps_within);
    KdTree3<float, ParticleArray<Vec3f>> tree_within(std::move(particles_within));

    auto color_filter = [&](const ParticleArray<Vec3f>& arr, unsigned i) {
        return arr.color(i) == set_color;
        };

    auto knn = tree_within.findNeighbors(chosen_origin, k_neighbors, color_filter);
    printing_knn(particles_within, knn);
    // Talvez o exemplo não seja muito prático. 
}

inline void menu() {
    cout << "\nEscolha uma das opções:" << endl;
    cout << "1 - Testar Particle KdTree sem filtro e sem raio" << endl;
    cout << "2 - Testar Particle KdTree sem filtro e com raio" << endl;
    cout << "3 - Testar Particle KdTree com filtro e sem raio" << endl;
    cout << "4 - Testar Particle KdTree com filtro e com raio" << endl;
    cout << "5 - Testar a KdTree de pontos simples" << endl;
    cout << "6 - Sair" << endl;
    cout << "Escolha: "; 
}

inline void testFunctions(int opt, int particles, int neighbors) {
    switch(opt) {
        case 1: 
            random_nofilter_noradius(particles, neighbors, Vec3f{ 0.0f, 0.0f, 0.0f });
            break;
        case 2:
            random_nofilter_radius(particles, neighbors, Vec3f{ 0.0f,0.0f,0.0f }, 2.0f);
            break;
        case 3: 
            random_filter_noradius(particles, neighbors, Vec3f{ 0.0f,0.0f,0.0f }, Vec3f{ 0.0f, 1.0f, 0.0f });
            break;
        case 4: 
            random_filter_radius(particles, neighbors, Vec3f{ 0.0f,0.0f,0.0f }, 2.0f, Vec3f{ 0.0f, 1.0f, 0.0f });
            break;
        case 5:
            simpleKdTree<2, float>(particles, neighbors, 2.f);
            break;
        case 6: 
        // Unreachable.
            cout << "Tchau!" << endl;
            break;
        default: 
            cout << "Escolha uma opção válida." << endl;
            break;
    }
}

int main() {
    int opt = 0;

    while(true) {
        int neighbors, particles;

        menu();
        cin >> opt;

        if(opt == 6) {
            cout << "Tchau!" << endl;
            break;
        }

        cout << "\nEscolha a quantidade de partículas: ";
        cin >> particles; 
        (void) getchar();
        cout << "Escolha a quantidade de vizinhos: ";
        cin >> neighbors;
        (void) getchar();

        if(particles < neighbors) {
            cout << "Não é possível ter um número de vizinhos maior que o número de partículas, tente novamente." << endl;
        } else {
            testFunctions(opt, particles, neighbors);
        }
    }

    return 0;
}
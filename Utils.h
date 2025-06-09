#ifndef __Utils_h
#define __Utils_h

#include "Bounds3.h"
#include "Bounds2.h"

#include "Concepts.h"
#include <vector>

class ParticleKdTree;
namespace tcii::p1
{ // begin namespace tcii::p1
using namespace cg;

// apenas calculando os limites da caixa usando todos os pontos recebidos
template <size_t D, IsReal R, IsPointArray<Vec, R, D> A>
auto
computeBounds(const A& points)
{
  Bounds<D, R> b;

  for (size_t n = points.size(), i = 0; i < n; ++i)
    b.inflate(points[i]);
  return b;
}

template <size_t D, IsReal R>
auto
distance(const Vec<D, R>& p, const Bounds<D, R>& bounds)
{
  R d{};

  for (size_t i = 0; i < D; ++i)
    if (auto t = p[i] - bounds[0][i]; t < 0)
      d += t * t;
    else if (auto t = p[i] - bounds[1][i]; t > 0)
      d += t * t;
  return (R)std::sqrt(d);
}

template<size_t D, IsReal R>
auto 
distance(const Vec<D,R>& a, const Vec<D,R>& b) {
  R d{};

  for (size_t i = 0; i < D; ++i)
    d += std::pow(a[i] - b[i], 2);
  
  return (R)std::sqrt(d);
}

// retorna um número real entre 0 e 1 
template <IsReal R>
inline auto
srand()
{
  return (R)((double)rand() / RAND_MAX);
}

// retorna um número entre min e max
template <IsReal R>
inline auto
srand(R min, R max)
{
  return min + srand<R>() * (max - min);
}

// retorna um ponto de dimensão D e precisão R com os valores de D (x,y,z,....) estando entre min e max
template <size_t D, IsReal R>
inline auto
prand(R min, R max)
{
  Vec<D, R> p;

  for (size_t i = 0; i < D; ++i)
    p[i] = srand(min, max);
  return p;
}

template <size_t D, IsReal R>
using PointVector = std::vector<Vec<D, R>>;

// retorna um vetor com pontos de dimensão D e precisão R com os valores de D (x,y,z,....) estando entre min e max
template <size_t D, IsReal R>
inline auto
prand(size_t n, R min = 0, R max = 1)
{
  assert(n > 0);

  PointVector<D, R> v(n);

  for (size_t i = 0; i < n; ++i)
    v[i] = prand<D>(min, max);
  return v;
}

// especialização pra 3D
template <typename R> using Points3 = PointVector<3, R>;

} // end namespace tcii::p1

#endif // __Utils_h
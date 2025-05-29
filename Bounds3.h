#ifndef __Bounds3_h
#define __Bounds3_h

#include "Vec3.h"

namespace tcii::cg
{ // begin namespace tcii::cg

// declaração do template
template <size_t D, typename real> class Bounds;

// especialização para D=3, Bounds recebe algum tipo que é 'real'
template <typename real>
class Bounds<3, real>
{
public:
  using vec3 = Vec3<real>;

  // construtor determinando que Bounds começa vazio
  Bounds()
  {
    setEmpty();
  }

  // apenas um overload de op[] pra pegar um atributo
  auto& operator [](size_t i) const
  {
    assert(i >= 0 && i < 2);
    return (&_p1)[i];
  }

  // devolve o ponto mínimo da caixa limitante
  auto& min() const
  {
    return _p1;
  }

  // devolve o ponto máximo da caixa limitante
  auto& max() const
  {
    return _p2;
  }

  // só checando se o ponto tá fora da caixa
  // ex: se estiver mais à esquerda do ponto mínimo ou mais à direita do ponto máximo, então a caixa não contém o ponto.
  // segue a mesma lógica para y e para z
  bool contains(const vec3& p) const	
  {
    if (p.x < _p1.x || p.x > _p2.x)
      return false;
    if (p.y < _p1.y || p.y > _p2.y)
      return false;
    if (p.z < _p1.z || p.z > _p2.z)
      return false;
    return true;
  }

  // p1 = max (era pra ser min)
  // p2 = min (era pra ser max)
  // isso gera min > max 
  // o que gera uma caixa de volume nulo e, consequentemente, vazia
  void setEmpty()
  {
    _p1.x = _p1.y = _p1.z = +std::numeric_limits<real>::max();
    _p2.x = _p2.y = _p2.z = -std::numeric_limits<real>::max();
  }

  // alterando os limites de acordo com um ponto dado
  void inflate(const vec3& p)
  {
    if (p.x < _p1.x)
      _p1.x = p.x;
    if (p.x > _p2.x)
      _p2.x = p.x;
    if (p.y < _p1.y)
      _p1.y = p.y;
    if (p.y > _p2.y)
      _p2.y = p.y;
    if (p.z < _p1.z)
      _p1.z = p.z;
    if (p.z > _p2.z)
      _p2.z = p.z;
  }

  // só usa o inflate(const vec3& p)
  void inflate(const Bounds& bounds)
  {
    inflate(bounds._p1);
    inflate(bounds._p2);
  }

private:
  vec3 _p1;
  vec3 _p2;

}; // Bounds3

// alias com template
template <typename real> using Bounds3 = Bounds<3, real>;

// cout lol
template <typename real>
std::ostream&
operator <<(std::ostream& os, const Bounds3<real>& b)
{
  os << "min" << b.min() << " max" << b.max();
  return os;
}

// especialização do alias para float
using Bounds3f = Bounds3<float>;

} // end namespace tcii::cg

#endif // __Bounds3_h

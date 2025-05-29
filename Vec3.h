#ifndef __Vec3_h
#define __Vec3_h

#include <cassert>
#include <cmath>
#include <iostream>

namespace tcii::cg
{ // begin namespace tcii::cg

// checa se o tipo recebido é um floating-point em tempo de >COMPILAÇÃO<
#define ASSERT_REAL(T, msg) static_assert(std::floating_point<T>, msg)

// forward declaration, provavelmente para possibilitar a especialização sem ter que definir o corpo da struct
template <size_t D, typename real> struct Vec;

// especialização para D=3
template <typename real>
struct Vec<3, real>
{

  // macro
  ASSERT_REAL(real, "Vec3: floating-point type expected");

  // alias
  using value_type = real;

  // acho que ele esqueceu de usar value_type lol
  real x;
  real y;
  real z;

  // pegando um valor de Vec que pode ser alterado no futuro.
  // aparentemente, é aritmética de ponteiro. Pega o endereço de x, que é o primeiro valor da classe, e soma i. 
  // se i = 0, pega x; se i = 1, pega y; se z = 2, pega z. 
  // isso assume que eles estão armazenados contiguamente na memória...
  // é bom fazer um static_assert
  auto& operator [](size_t i)
  {
    assert(i >= 0 && i < 3);
    return (&x)[i];
  }

  // remove o const do this e transforma em Vec*, enviando para a função op[] não const
  auto operator [](size_t i) const
  {
    return const_cast<Vec*>(this)->operator [](i);
  }

  // forward declaration
  real length() const;

  // vetor unitário de Vec
  auto versor() const
  {
    return ((real)1 / length()) * *this;
  }

}; // Vec3

template <typename real> using Vec3 = Vec<3, real>;

// apenas umas definições de operações entre vetores.....
// soma de vetores
template <typename real>
inline Vec3<real>
operator +(const Vec3<real>& u, const Vec3<real>& v)
{
  return {u.x + v.x, u.y + v.y, u.z + v.z};
}

// subtração de vetores
template <typename real>
inline Vec3<real>
operator -(const Vec3<real>& u, const Vec3<real>& v)
{
  return {u.x - v.x, u.y - v.y, u.z - v.z};
}

// multiplicação de um vetor por um escalar
template <typename real>
inline Vec3<real>
operator *(real s, const Vec3<real>& v)
{
  return {s * v.x, s * v.y, s * v.z};
}

// msm coisa, só tá mudando a ordem pra mostrar q obedece a propriedade comutativa
template <typename real>
inline Vec3<real>
operator *(const Vec3<real>& v, real s)
{
  return s * v;
}

// definindo o cálculo do tamanho de um vetor (raiz quadrada da soma dos quadrados das coordenadas)
template <typename real>
inline real
Vec<3, real>::length() const
{
  return std::sqrt(x * x + y * y + z * z);
}

// fancy cout
template <typename T>
std::ostream&
operator <<(std::ostream& os, const Vec3<T>& v)
{
  os << '(' << v.x << ',' << v.y << ',' << v.z << ')';
  return os;
}

// especialiazação de Vec3
using Vec3f = Vec3<float>;

} // end namespace tcii::cg

#endif // __Vec3_h

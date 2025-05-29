#ifndef __Concepts_h
#define __Concepts_h

#include <concepts>

// concepts servem para adicionar condições ao uso de templates
namespace tcii::p1
{ // begin namespace tcii::p1
using namespace std;
// checa se os dois tipos são iguais
template <typename T, typename U>
concept SameAs = std::same_as<T, U>;

// checa se R é real
template <typename R>
concept IsReal = std::floating_point<R>;

// usa o requires para definir alguma condição
// a condição é que o x[i] tem que ser convertível para o tipo E
// { x[i] } ==> verifica se x[i] é uma condição válida, ou seja, checa se o tipo C definiu um operator []
// aparentemente x e i são valores aleatórios gerados pelo compilador.....
template <typename C, typename E>
concept IsIndexable = requires (C x, size_t i)
{
  { x[i] } -> std::convertible_to<E>;
};

// P = ponto, D = dimensão, R = tipo do ponto
// P = P<D,R>
// algo é um ponto se: R é real, o tipo de P é igual ao tipo de R, se é indexável e se as operações do ponto mantêm o tipo do ponto 
template <template <size_t, typename> typename P, typename R, size_t D>
concept IsPoint = IsReal<R> &&
  SameAs<R, typename P<D, R>::value_type> &&
  IsIndexable<P<D, R>, R> &&
requires (P<D, R> p, R r)
{
  { p + p } -> SameAs<P<D, R>>;
  { p - p } -> SameAs<P<D, R>>;
  { p * r } -> SameAs<P<D, R>>;
  { r * p } -> SameAs<P<D, R>>;
};

// A = tipo do array, P = ponto, D = dimensão, R = tipo do ponto
// P = P<D,R>
// algo é um array de pontos se: é um ponto, é indexável, se é possível construir por movimento, se tem um método size e se o tipo do tamanho é size_t
template <typename A,
  template <size_t, typename> typename P,
  typename R,
  size_t D>
concept IsPointArray = IsPoint<P, R, D> &&
  IsIndexable<A, P<D, R>> &&
  std::move_constructible<A> &&
requires (A a, size_t i)
{
  { a.size() } -> SameAs<size_t>;
};

} // end namespace tcii::p1

#endif // __Concepts_h

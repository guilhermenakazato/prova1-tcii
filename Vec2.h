#ifndef __Vec2_h
#define __Vec2_h

#include <cassert>
#include <cmath>
#include <iostream>

/*
Autor(es): Guilherme Fernandes Nakazato ; Felipe Jun Takahashi
*/

namespace tcii::cg
{ // begin namespace tcii::cg

#define ASSERT_REAL(T, msg) static_assert(std::floating_point<T>, msg)

    template <size_t D, typename real> struct Vec;

    template <typename real>
    struct Vec<2, real> // Ajustado para 2 dimens�es
    {
        ASSERT_REAL(real, "Vec2: floating-point type expected"); // Nome Vec2

        using value_type = real;

        real x;
        real y; // Sem coordenada z

        auto& operator [](size_t i)
        {
            assert(i >= 0 && i < 2); // Ajustado para i < 2
            return (&x)[i];
        }

        auto operator [](size_t i) const
        {
            return const_cast<Vec*>(this)->operator [](i);
        }

        real length() const;

        auto versor() const
        {
            return ((real)1 / length()) * *this;
        }

    }; // Nome Vec2 

	template <typename real> using Vec2 = Vec<2, real>; // Ajustado para 2 dimens�es

    template <typename real>
    inline Vec2<real> // Tipo Vec2
        operator +(const Vec2<real>& u, const Vec2<real>& v) // Tipo Vec2
    {
        return { u.x + v.x, u.y + v.y}; // z removido
    }

    template <typename real>
    inline bool
		operator ==(const Vec2<real>& u, const Vec2<real>& v) // Tipo Vec2
    {
		for (size_t i = 0; i < 2; i++) { // Ajustado para 2 dimens�es
            if (u[i] != v[i])
                return false;
        }

        return true;
    }

    template <typename real>
	inline Vec2<real> // Tipo Vec2
		operator -(const Vec2<real>& u, const Vec2<real>& v) // Tipo Vec2
    {
		return { u.x - v.x, u.y - v.y}; // z removido
    }

    template <typename real>
	inline Vec2<real> // Tipo Vec2
		operator *(real s, const Vec2<real>& v) // Tipo Vec2
    {
		return { s * v.x, s * v.y }; // z removido
    }

    template <typename real>
    inline Vec2<real> // Tipo Vec2
		operator *(const Vec2<real>& v, real s) // Tipo Vec2
    {
        return s * v;
    }

    template <typename real>
    inline real
        Vec<2, real>::length() const // Ajustado para 2 dimens�es
    {
		return std::sqrt(x * x + y * y); // z removido
    }

    template <typename T>
    std::ostream&
        operator <<(std::ostream& os, const Vec2<T>& v) // Tipo Vec2
    {
        os << '(' << v.x << ',' << v.y << ')'; // z removido
        return os;
    }

    using Vec2f = Vec2<float>; // Nome Vec2f e Tipo Vec2

} // end namespace tcii::cg

#endif // __Vec2_h
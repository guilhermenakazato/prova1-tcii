#ifndef __Bounds2_h
#define __Bounds2_h

#include "Vec2.h"

/*
Autor(es): Guilherme Fernandes Nakazato ; Felipe Jun Takahashi
*/

namespace tcii::cg
{ // begin namespace tcii::cg

    template <size_t D, typename real> class Bounds;

    template <typename real>
    class Bounds<2, real> // Bounds2
    {
    public:
        using vec2 = Vec2<real>; // Vec2

        Bounds()
        {
            setEmpty();
        }

        auto& operator [](size_t i) const
        {
            assert(i >= 0 && i < 2); // Ajustado para 2 dimensões
            return (&_p1)[i];
        }

        auto& min() const
        {
            return _p1;
        }

        auto& max() const
        {
            return _p2;
        }

        bool contains(const vec2& p) const //vec2
        {
            if (p.x < _p1.x || p.x > _p2.x)
                return false;
            if (p.y < _p1.y || p.y > _p2.y)
                return false; // z removido
            return true;
        }

        void setEmpty()
        {
            _p1.x = _p1.y = +std::numeric_limits<real>::max();
            _p2.x = _p2.y = -std::numeric_limits<real>::max(); // z removido
        }

        void inflate(const vec2& p) //vec2
        {
            if (p.x < _p1.x)
                _p1.x = p.x;
            if (p.x > _p2.x)
                _p2.x = p.x;
            if (p.y < _p1.y)
                _p1.y = p.y;
            if (p.y > _p2.y)
				_p2.y = p.y; // z removido
        }

        void inflate(const Bounds& bounds)
        {
            inflate(bounds._p1);
            inflate(bounds._p2);
        }

    private:
        vec2 _p1; //vec2
        vec2 _p2; //vec2

    }; // Bounds2

    template <typename real> using Bounds2 = Bounds<2, real>; // Bounds2

    template <typename real>
    std::ostream&
		operator <<(std::ostream& os, const Bounds2<real>& b) // Bounds2
    {
        os << "min" << b.min() << " max" << b.max();
        return os;
    }

    using Bounds2f = Bounds2<float>; // Bounds2

} // end namespace tcii::cg

#endif // __Bounds2_h
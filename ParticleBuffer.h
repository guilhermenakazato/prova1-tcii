#ifndef __ParticleBuffer_h
#define __ParticleBuffer_h

#include "Bounds3.h"
#include "SharedObject.h"
#include "SoA.h"
#include "Renderer.h"

namespace tcii::physx
{ // begin namespace tcii::physx

using namespace cg;

template <typename... Fields> class ParticleSystem;

// Estrutura MyAllocator vista em sala de aula, utilizada como argumento para SoA
struct MyAllocator {
    template<typename T>
    static auto 
    allocate(unsigned count) 
    {
		return new T[count];
	}

    template <typename T>
    static void 
    free(T* ptr) 
    {
        delete[] ptr;
	}
};

template <typename... Fields>
class ParticleBuffer: public SharedObject
{
public:
  auto 
  particleCount() const
  {
    return _particleCount;
  }

  auto 
  capacity() const
  {
      return _capacity;
  }

  void 
  add(const Vec3f& p, const Fields&... fields)
  {
      if (!(_particleCount >= _capacity)) 
      {
          _soa.template set(_particleCount++, p, fields...);
      }
  }

  void 
  clear()
  {
	  _particleCount = 0;
  }

  // Adicionada para facilitar a implementacao de [] em ParticleArray
  auto 
  get(unsigned index) const
  {
      return _soa.tuple(index);
  }

  Bounds3f bounds() const;
  void render(Renderer&) const;

private:
  unsigned _particleCount{};
  unsigned _capacity{};

  SoA<MyAllocator, unsigned, Vec3f, Fields...> _soa;

  ParticleBuffer(unsigned capacity)
      : _particleCount(0), 
        _capacity(capacity), 
        _soa(capacity)
  {}

  friend ParticleSystem<Fields...>;

}; // ParticleBuffer

template <typename... Fields>
Bounds3f
ParticleBuffer<Fields...>::bounds() const
{
  Bounds3f b;
  for(auto i = 0u;i < _particleCount;i++) 
  {
      b.inflate(_soa.template get<0>(i));       
  }
  
  return b;
}

template <typename... Fields>
void
ParticleBuffer<Fields...>::render(Renderer& renderer) const
{
  for(auto i = 0u;i<_particleCount; i++) 
  {
      renderer.draw("Particle", _soa.template get<0>(i));
  }
}

} // end namespace tcii::physx

#endif // __ParticleBuffer_h

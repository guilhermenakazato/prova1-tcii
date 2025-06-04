#ifndef __ParticleArray_h__
#define __ParticleArray_h__

#include "ParticleBuffer.h"
#include "ParticleSystem.h"
#include <tuple>

template <typename... Fields>
class ParticleArray { // Particle Array como visto em sala de aula
public: 

	ParticleArray(tcii::physx::ParticleSystem<Fields...>& ps)
		: _particles(ps.particles()) 
	{}

	ParticleArray(ParticleArray&&) noexcept = default;

	size_t 
	size() const 
	{
		return _particles ? _particles->particleCount() : 0;
	}

	// Verifica se existem particlas, retorna a particla na posicao i se sim, caso contrario retorna uma tupla vazia
	auto 
	operator[](size_t i) const 
	{
		return _particles ? _particles->get(i) : std::tuple<tcii::cg::Vec3f, Fields...>{};
	}


private:
	tcii::physx::ObjectPtr<tcii::physx::ParticleBuffer<Fields...>> _particles;
};


#endif
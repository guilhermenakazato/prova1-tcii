#ifndef __KNN_h
#define __KNN_h

#include "Concepts.h"
#include <vector>
#include <algorithm>

namespace tcii::p1
{ // begin namespace tcii::p1

template <IsReal R>
class KNN
{
public:
  KNN(unsigned k) : _k(k) {}

  // Similar ao Shift, adaptado do trabalho que fizemos em ED
  void
  add(R distance, unsigned index) 
  {
      if (heap.size() < _k)
      {
          // A heap n�o est� cheia, colocamos um candidato
          heap.emplace_back(distance, index);

          // A partir do momento que temos todos os candidatos, montamos a heap
          if (heap.size() == _k)
              std::make_heap(heap.begin(), heap.end(), cmp);
      }

      else if (distance < heap.front().first) // O novo candidato � melhor que o pior
      {
          // Remove o pior elemento (propriedade Max Heap)
          std::pop_heap(heap.begin(), heap.end(), cmp);

          // Colocamos o novo candidato em uma posi��o para chamar o Heapify
          heap.back() = { distance, index };

          // Mant�m a propriedade da Heap
          std::push_heap(heap.begin(), heap.end(), cmp);
      }
  }

  // Auxiliar para verificar se a heap est� cheia
  bool isFull() const { return heap.size() == _k; }

  // Auxiliar para obter a pior dist�ncia
  R getMaxDist() const { return heap.front().first; }

  // Retorna refer�ncia constante ao heap (n�o ordenado)
  const std::vector<std::pair<R, unsigned>>& neighbors() const 
  {
      return heap;
  }

private:
    unsigned _k;

    // Nossa heap � um vector de dados que armazeam a dist�ncia e o �ndice de um ponto
    std::vector<std::pair<R, unsigned>> heap;

    static bool cmp(const std::pair<R, unsigned>& a, const std::pair<R, unsigned>& b) { return a.first < b.first; }
};

} // end namespace tcii::p1

#endif // __KNN_h

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
          // A heap não está cheia, colocamos um candidato
          heap.emplace_back(distance, index);

          // A partir do momento que temos todos os candidatos, montamos a heap
          if (heap.size() == _k)
              std::make_heap(heap.begin(), heap.end(), cmp);
      }

      else if (distance < heap.front().first) // O novo candidato é melhor que o pior
      {
          // Remove o pior elemento (propriedade Max Heap)
          std::pop_heap(heap.begin(), heap.end(), cmp);

          // Colocamos o novo candidato em uma posiçãoo para chamar o Heapify
          heap.back() = { distance, index };

          // Mantém a propriedade da Heap
          std::push_heap(heap.begin(), heap.end(), cmp);
      }
  }

  // Auxiliar para verificar se a heap está cheia
  bool isFull() const { return heap.size() == _k; }

  // Auxiliar para obter a pior distância
  R getMaxDist() const { return !heap.empty() ? heap.front().first : 999999; }

  auto size() const { return heap.size(); }

  // Retorna referência constante ao heap (não ordenado)
  const std::vector<std::pair<R, unsigned>>& neighbors() const 
  {
      return heap;
  }
  
  void sort() {
    std::sort_heap(heap.begin(), heap.end(), cmp);
  }

private:
    unsigned _k;

    // Nossa heap é um vector de dados que armazeam a distância e o índice de um ponto
    std::vector<std::pair<R, unsigned>> heap;

    static bool cmp(const std::pair<R, unsigned>& a, const std::pair<R, unsigned>& b) { return a.first < b.first; }
};

} // end namespace tcii::p1

#endif // __KNN_h
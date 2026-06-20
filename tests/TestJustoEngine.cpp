
#include "doctest.h"
#include "JustoEngine.hpp"
#include <memory>

namespace SCosa {

  class PublicJusto : public JustoEngine {
  public:
    PublicJusto(int seed, float root, int maxSize) :
      JustoEngine(seed, root, maxSize) {}
    static void testReduceFraction(int64_t& num, int64_t& den) {
      reduceFraction(num, den);
    }
    void testNext(int nSamples, const float* trigger, const float* mutate,
		  const float* numeratorIn, const float* denominatorIn,
		  float* frequency, float* numeratorOut, float* denominatorOut,
		  float *distanceOut) {
      next(nSamples, trigger, mutate, numeratorIn, denominatorIn, frequency,
	   numeratorOut, denominatorOut, distanceOut);
    }
  };

  TEST_CASE("JustoEngine::reduceFraction") {
    
    SUBCASE("simple fraction") {
      int64_t numerator = 4;
      int64_t denominator = 6;
      PublicJusto::testReduceFraction(numerator, denominator);
      CHECK(numerator == 2);
      CHECK(denominator == 3);
    }
    
    SUBCASE("complex fraction") {
      int64_t numerator = 2 * 2 * 3 * 3 * 7 * 5 * 5;
      int64_t denominator = 2 * 2 * 3 * 7 * 5;
      PublicJusto::testReduceFraction(numerator, denominator);
      CHECK(numerator == 3 * 5);
      CHECK(denominator == 1);
    }
    
    SUBCASE("already reduced fraction") {
      int64_t numerator = 5;
      int64_t denominator = 4;
      PublicJusto::testReduceFraction(numerator, denominator);
      CHECK(numerator == 5);
      CHECK(denominator == 4);
    }
    
    SUBCASE("large values") {
      int64_t numerator = 10000000000;
      int64_t denominator = 5000000000;
      PublicJusto::testReduceFraction(numerator, denominator);
      CHECK(numerator == 2);
      CHECK(denominator == 1);
    }
  }

  class StateJusto {
  public:
    int nSamples;
    std::unique_ptr<float[]> triggerIn;
    std::unique_ptr<float[]> mutateIn;
    std::unique_ptr<float[]> numeratorIn;
    std::unique_ptr<float[]> denominatorIn;
    std::unique_ptr<float[]> frequencyOut;
    std::unique_ptr<float[]> numeratorOut;
    std::unique_ptr<float[]> denominatorOut;
    std::unique_ptr<float[]> distanceOut;
    PublicJusto justo;
    StateJusto(int nSamples, int seed, float root, int maxSize) :
      nSamples(nSamples),
      triggerIn(std::make_unique<float[]>(nSamples)),
      mutateIn(std::make_unique<float[]>(nSamples)),
      numeratorIn(std::make_unique<float[]>(nSamples)),
      denominatorIn(std::make_unique<float[]>(nSamples)),
      frequencyOut(std::make_unique<float[]>(nSamples)),
      numeratorOut(std::make_unique<float[]>(nSamples)),
      denominatorOut(std::make_unique<float[]>(nSamples)),
      distanceOut(std::make_unique<float[]>(nSamples)),
      justo(seed, root, maxSize) {
      for (int i = 0; i < nSamples; i++) {
	triggerIn[i] = (1+i) % 2;
	mutateIn[i] = 1;
	numeratorIn[i] = 1;
	denominatorIn[i] = 1;
      }
    };
    void testNext(int n) {
      justo.testNext(n, triggerIn.get(), mutateIn.get(),
		     numeratorIn.get(), denominatorIn.get(),
		     frequencyOut.get(), numeratorOut.get(),
		     denominatorOut.get(), distanceOut.get());
    }
  };

  TEST_CASE("JustoEngine::next") {

    SUBCASE("startup") {
      StateJusto justo(1, 1, 440.0f, 1);
      justo.testNext(1);
    }
    
    SUBCASE("generate") {
      constexpr int n = 6;
      StateJusto justo(n, 1, 440.0f, n/2);
      for (int i = 0; i < n; i++) justo.mutateIn[i] = 0;
      justo.testNext(n);
      CHECK(justo.numeratorOut[0] == 1);
      CHECK(justo.denominatorOut[0] == 1);
      CHECK(justo.numeratorOut[2] == 2);
      CHECK(justo.denominatorOut[0] == 1);
      CHECK(justo.numeratorOut[4] == 6);
      CHECK(justo.denominatorOut[0] == 1);
    }
    
    SUBCASE("mutate") {
      constexpr int n = 6;
      StateJusto justo(n, 1, 440.0f, n/2);
      for (int i = 0; i < n; i++) justo.mutateIn[i] = 0;
      justo.testNext(n);
      CHECK(justo.numeratorOut[0] == 1);
      CHECK(justo.denominatorOut[0] == 1);
      CHECK(justo.numeratorOut[2] == 2);
      CHECK(justo.denominatorOut[0] == 1);
      CHECK(justo.numeratorOut[4] == 6);
      CHECK(justo.denominatorOut[0] == 1);
      for (int i = 0; i < n; i++) justo.mutateIn[i] = 1;
      justo.testNext(n);
      CHECK(justo.numeratorOut[0] == 1);
      CHECK(justo.denominatorOut[0] == 1);
      CHECK(justo.numeratorOut[2] == 2);
      CHECK(justo.denominatorOut[0] == 1);
      CHECK(justo.numeratorOut[4] == 7);  // this is worse instead of better
      CHECK(justo.denominatorOut[0] == 1);
    }
    
  }
  
}

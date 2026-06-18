
#include "doctest.h"
#include "JustoEngine.hpp"

namespace SCosa {

  // expose protected methods
  class TestableJustoEngine : public JustoEngine {
  public:
    TestableJustoEngine(int seed, float root, int maxSize) : JustoEngine(seed, root, maxSize) {}
    static void testReduceFraction(int64_t& num, int64_t& den) {
      reduceFraction(num, den);
    }
  };

  TEST_CASE("JustoEngine - Fraction Reduction Math Check") {
    
    SUBCASE("It should cleanly reduce standard fractions") {
      int64_t numerator = 4;
      int64_t denominator = 6;
      TestableJustoEngine::testReduceFraction(numerator, denominator);
      CHECK(numerator == 2);
      CHECK(denominator == 3);
    }
    
    SUBCASE("Handle complex fractions") {
      int64_t numerator = 2 * 2 * 3 * 3 * 7 * 5 * 5;
      int64_t denominator = 2 * 2 * 3 * 7 * 5;
      TestableJustoEngine::testReduceFraction(numerator, denominator);
      CHECK(numerator == 3 * 5);
      CHECK(denominator == 1);
    }
    
    SUBCASE("It should keep already optimized fractions intact") {
      int64_t numerator = 5;
      int64_t denominator = 4;
      TestableJustoEngine::testReduceFraction(numerator, denominator);
      CHECK(numerator == 5);
      CHECK(denominator == 4);
    }
    
    SUBCASE("It should handle large integer values smoothly") {
      int64_t numerator = 10000000000;
      int64_t denominator = 5000000000;
      TestableJustoEngine::testReduceFraction(numerator, denominator);
      CHECK(numerator == 2);
      CHECK(denominator == 1);
    }
  }
  
}

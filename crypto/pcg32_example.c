/*
 * PCG32 Example
 * https://www.pcg-random.org/
 */
#include <stdint.h> // for uint32_t and uint64_t types
#include <stdio.h>

// ===== PCG RNG state =====
// PCG uses two 64-bit numbers to store its state:
static uint64_t pcg_state = 0x853c49e6748fea9bULL; // internal RNG state
static uint64_t pcg_inc =
    0xda3e39cb94b95bdbULL; // "stream selector" (must be odd)

uint32_t pcg_rand();

// ===== Seed the RNG =====
void pcg_srand(uint64_t seed, uint64_t seq) {
  // Reset state
  pcg_state = 0;
  // Set the increment (sequence), must be odd
  pcg_inc = (seq << 1u) | 1u;

  // Advance the generator once (to mix initial state)
  pcg_rand();

  // Add the user-provided seed to the state
  pcg_state += seed;

  // Advance again to finalize initialization
  pcg_rand();
}

// ===== Generate a 32-bit random number =====
uint32_t pcg_rand() {
  // Save the old state
  uint64_t oldstate = pcg_state;

  // Update the state using a linear congruential step
  pcg_state = oldstate * 6364136223846793005ULL + pcg_inc;

  // Extract the high-quality random bits
  uint32_t xorshifted = (uint32_t)(((oldstate >> 18u) ^ oldstate) >> 27u);

  // Determine a rotation amount from the top bits of oldstate
  uint32_t rot = oldstate >> 59u;

  // Rotate bits to improve randomness
  return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

// ===== Example usage =====

int main() {
  // Seed the RNG (seed = 42, sequence = 54)
  pcg_srand(42, 54);

  // Generate 10 random numbers
  for (int i = 0; i < 10; i++)
    printf("%u\n", pcg_rand());
}

#ifndef WALKPERC_OPTS_H
#define WALKPERC_OPTS_H

const bool using_saftey_checks = true;

#define PATCHWALK_CHECK_RANGE(var,min,max)                              \
  { if(using_saftey_checks) {                                           \
      auto varval = var;                                             \
      if ((varval) < (min) || (varval) > (max)) {                      \
       std::cerr << "*** " << #var " = " << varval << ". Out of range (" << min << "," << max << ")\n"; \
       abort(); }                                                      \
    }                                                                  \
  }

// Use vectors instead of dynamic C arrays
// No measurable speed difference in 10 minute runs
#define USE_VECTORS_HK
#define USE_VECTORS_PERC2D

/*
minstd_rand0 -- Discovered in 1969 by Lewis, Goodman and Miller, adopted
  as "Minimal standard" in 1988 by Park and Miller

minstd_rand -- Newer "Minimum standard", recommended by Park, Miller,
and Stockmeyer in 1993

mt19937  -- 32-bit Mersenne Twister by Matsumoto and Nishimura, 1998

mt19937_64 -- 64-bit Mersenne Twister by Matsumoto and Nishimura, 2000

ranlux24_base -- std::subtract_with_carry_engine<uint_fast32_t, 24, 10, 24>

ranlux48_base -- std::subtract_with_carry_engine<uint_fast64_t, 48, 5, 12>

ranlux24 -- std::discard_block_engine<ranlux24_base, 223, 23>
     24-bit RANLUX generator by Martin Lüscher and Fred James, 1994

ranlux48 -- std::discard_block_engine<ranlux48_base, 389, 11>
    48-bit RANLUX generator by Martin Lüscher and Fred James, 1994

knuth_b -- std::shuffle_order_engine<minstd_rand0, 256>

*/

#define MINSTD_RAND0    0
#define MINSTD_RAND     1
#define MT19937         2
#define MT19937_64      3
#define RANLUX24_BASE   4
#define RANLUX48_BASE   5
#define RANLUX24        6
#define RANLUX48        7
#define KNUTH_B         8

#define RNG_GEN_PAIR  MT19937_64

#if RNG_GEN_PAIR == MINSTD_RAND0
#define PERC_2D_SITE_GENERATOR minstd_rand0
#define WALK_2D_GENERATOR minstd_rand0

#elif  RNG_GEN_PAIR == MINSTD_RAND
#define PERC_2D_SITE_GENERATOR minstd_rand
#define WALK_2D_GENERATOR minstd_rand

#elif  RNG_GEN_PAIR == MT19937
#define PERC_2D_SITE_GENERATOR mt19937
#define WALK_2D_GENERATOR mt19937

#elif  RNG_GEN_PAIR == MT19937_64
#define PERC_2D_SITE_GENERATOR mt19937_64
#define WALK_2D_GENERATOR mt19937_64

#elif  RNG_GEN_PAIR == RANLUX24_BASE
#define PERC_2D_SITE_GENERATOR ranlux24_base
#define WALK_2D_GENERATOR ranlux24_base

#elif  RNG_GEN_PAIR == RANLUX48_BASE
#define PERC_2D_SITE_GENERATOR ranlux48_base
#define WALK_2D_GENERATOR ranlux48_base

#elif  RNG_GEN_PAIR == RANLUX24
#define PERC_2D_SITE_GENERATOR ranlux24
#define WALK_2D_GENERATOR ranlux24

#elif  RNG_GEN_PAIR == RANLUX48
#define PERC_2D_SITE_GENERATOR ranlux48
#define WALK_2D_GENERATOR ranlux48

#elif  RNG_GEN_PAIR == KNUTH_B
#define PERC_2D_SITE_GENERATOR knuth_b
#define WALK_2D_GENERATOR knuth_b
#endif

#define WALKPERC_OPTS_STRINGIFY(a) WALKPERC_OPTS_STRINGIFY_(a)
#define WALKPERC_OPTS_STRINGIFY_(a) #a

#endif

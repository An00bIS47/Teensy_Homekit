#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include <Arduino.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>


#include "mbedtls/entropy_poll.h"
// #include "EntropyWrapper.h"

#ifndef MBEDTLS_ENTROPY_HARDWARE_ALT
#error "MBEDTLS_ENTROPY_HARDWARE_ALT should always be used for Hardare entropy"
#endif


#define TRNG_ENT_COUNT 16

uint32_t gWDT_trng_priorT[16];
uint8_t gWDT_trng_indexT=16;
uint32_t retValT;
uint32_t share_entropy;

// This function returns a unsigned char (8-bit) with the number of unsigned long values
// in the entropy pool
uint8_t available(void)
{
#ifdef ARDUINO_SAM_DUE
  return(TRNG->TRNG_ISR & TRNG_ISR_DATRDY);
#elif defined(__MK66FX1M0__) || defined (__MK64FX512__)
  return  (RNG_SR & RNG_SR_OREG_LVL(0xF));
#elif defined(__IMXRT1062__)
  if (gWDT_trng_indexT < 16) return (16 - gWDT_trng_indexT);
  uint32_t m = TRNG_MCTL;
  if (m & TRNG_MCTL_ENT_VAL) {
  } else if (m & TRNG_MCTL_ERR) {
    TRNG_MCTL = m;
  } else {
    return 0;
  }
  for (int i=0; i < 16; i++) {
    gWDT_trng_priorT[i] = *(&TRNG_ENT0 + i); // copy 512 bits from TRNG
  }
  gWDT_trng_indexT = 0;
  return 16;
#else
  return(gWDT_pool_count);
#endif
}

// This function returns a uniformly distributed random integer in the range
// of [0,0xFFFFFFFF] as long as some entropy exists in the pool and a 0
// otherwise.  To ensure a proper random return the available() function
// should be called first to ensure that entropy exists.
//
// The pool is implemented as an 8 value circular buffer
uint32_t randomTeensy(void)
{
#ifdef ARDUINO_SAM_DUE
  while (! (TRNG->TRNG_ISR & TRNG_ISR_DATRDY))
    ;
  retVal = TRNG->TRNG_ODATA;
#elif defined(__MK66FX1M0__) || defined (__MK64FX512__)
  while ((RNG_SR & RNG_SR_OREG_LVL(0xF)) == 0); // wait
  retVal = RNG_OR;
#elif defined(__IMXRT1062__)
  while (!available()) ; // wait
  return gWDT_trng_priorT[gWDT_trng_indexT++];
#else
  uint8_t waiting;
  while (gWDT_pool_count < 1)
    waiting += 1;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    retVal = gWDT_entropy_pool[gWDT_pool_start];
    gWDT_pool_start = (gWDT_pool_start + 1) % WDT_POOL_SIZE;
    --gWDT_pool_count;
  }
#endif
  return(retValT);
}

// This function returns one byte of a single 32-bit entropy value, while preserving the remaining bytes to
// be returned upon successive calls to the method.  This makes best use of the available entropy pool when
// only bytes size chunks of entropy are needed.  Not available to public use since there is a method of using
// the default random method for the end-user to achieve the same results.  This internal method is for providing
// that capability to the random method, shown below
uint8_t random8T(void)
{
  static uint8_t byte_positionT=0;
  uint8_t retVal8 = 0;

  if (byte_positionT == 0) {
    share_entropy = randomTeensy();
    retVal8 = share_entropy >> 24;
  } else if (byte_positionT == 1) {
      retVal8 = share_entropy >> 16;
  } else if (byte_positionT == 2) {
      retVal8 = share_entropy >> 8;
  } else if (byte_positionT == 3) {
      retVal8 = share_entropy;
  }
  byte_positionT++;

//   retVal8 = share_entropy.int8[byte_positionT++];
  byte_positionT = byte_positionT % 4;
  return(retVal8);
}

void init(){
  CCM_CCGR6 |= CCM_CCGR6_TRNG(CCM_CCGR_ON);
  TRNG_MCTL = TRNG_MCTL_RST_DEF | TRNG_MCTL_PRGM; // reset to program mode
  TRNG_SCMISC = TRNG_SCMISC_RTY_CT(TRNG_DEFAULT_RETRY_COUNT) | 
                TRNG_SCMISC_LRUN_MAX(TRNG_DEFAULT_RUN_MAX_LIMIT);
  TRNG_SCML = TRNG_SCML_MONO_MAX(TRNG_DEFAULT_MONOBIT_MAXIMUM) |
                TRNG_SCML_MONO_RNG(TRNG_DEFAULT_MONOBIT_MAXIMUM - TRNG_DEFAULT_MONOBIT_MINIMUM);
  TRNG_SCR1L = TRNG_SCR1L_RUN1_MAX(TRNG_DEFAULT_RUNBIT1_MAXIMUM) |
                TRNG_SCR1L_RUN1_RNG(TRNG_DEFAULT_RUNBIT1_MAXIMUM - TRNG_DEFAULT_RUNBIT1_MINIMUM);
  TRNG_SCR2L = TRNG_SCR2L_RUN2_MAX(TRNG_DEFAULT_RUNBIT2_MAXIMUM) |
                TRNG_SCR2L_RUN2_RNG(TRNG_DEFAULT_RUNBIT2_MAXIMUM - TRNG_DEFAULT_RUNBIT2_MINIMUM);
  TRNG_SCR3L = TRNG_SCR3L_RUN3_MAX(TRNG_DEFAULT_RUNBIT3_MAXIMUM) |
                TRNG_SCR3L_RUN3_RNG(TRNG_DEFAULT_RUNBIT3_MAXIMUM - TRNG_DEFAULT_RUNBIT3_MINIMUM);
  TRNG_SCR4L = TRNG_SCR4L_RUN4_MAX(TRNG_DEFAULT_RUNBIT4_MAXIMUM) |
                TRNG_SCR4L_RUN4_RNG(TRNG_DEFAULT_RUNBIT4_MAXIMUM - TRNG_DEFAULT_RUNBIT4_MINIMUM);
  TRNG_SCR5L = TRNG_SCR5L_RUN5_MAX(TRNG_DEFAULT_RUNBIT5_MAXIMUM) |
                TRNG_SCR5L_RUN5_RNG(TRNG_DEFAULT_RUNBIT5_MAXIMUM - TRNG_DEFAULT_RUNBIT5_MINIMUM);
  TRNG_SCR6PL = TRNG_SCR6PL_RUN6P_MAX(TRNG_DEFAULT_RUNBIT6PLUS_MAXIMUM) |
                TRNG_SCR6PL_RUN6P_RNG(TRNG_DEFAULT_RUNBIT6PLUS_MAXIMUM -
                        TRNG_DEFAULT_RUNBIT6PLUS_MINIMUM);
  TRNG_PKRMAX = TRNG_DEFAULT_POKER_MAXIMUM;
  TRNG_PKRRNG = TRNG_DEFAULT_POKER_MAXIMUM - TRNG_DEFAULT_POKER_MINIMUM;
  TRNG_FRQMAX = TRNG_DEFAULT_FREQUENCY_MAXIMUM;
  TRNG_FRQMIN = TRNG_DEFAULT_FREQUENCY_MINIMUM;
  TRNG_SDCTL = TRNG_SDCTL_ENT_DLY(TRNG_DEFAULT_ENTROPY_DELAY) |
                TRNG_SDCTL_SAMP_SIZE(TRNG_DEFAULT_SAMPLE_SIZE);
  TRNG_SBLIM = TRNG_DEFAULT_SPARSE_BIT_LIMIT;

  TRNG_MCTL = TRNG_MCTL_SAMP_MODE(2); // start run mode
  TRNG_ENT15; // discard any stale data
}

int mbedtls_hardware_poll( void *data,
                           unsigned char *output, size_t len, size_t *olen )
{    

    init();
    for (int i=0; i < len; i++){
        output[i] = random8T();
    }
    
    *olen = len;

    return 0;
}

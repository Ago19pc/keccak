#pragma once
#include <math.h>
#include <stdint.h>
#include <stdio.h>

//#if (defined HIGH_PERFORMANCE_X86_64)

#if defined(__arm__) || defined(__aarch64__)
static inline
uint64_t getTime(void) {
	uint64_t value;
	asm volatile ("mrs %0, cntvct_el0" : "=r"(value));
	return value;
}
#elif defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
static inline
uint64_t getTime(void) {
  unsigned long long result;
    __asm__ __volatile__(
        "rdtscp;"
        "shl $32, %%rdx;"
        "or %%rdx, %%rax"
        : "=a"(result)
        :
        : "%rcx", "%rdx");
  return result;
}
#endif



typedef struct {
     double mean;
     double M2;
     long count;
} welford_t;

static inline
void welford_init(welford_t* state){
    state->mean = 0.0;
    state->M2 = 0.0;
    state->count = 0;
    return;
}

static inline
void welford_update(welford_t* state, double sample){
    //printf("current mean: %f\n", state->mean);
    //printf("sample: %f\n", sample);
    double delta, delta2;
    state->count = state->count + 1;
    delta = sample - state->mean;
    state->mean += delta / (long double)(state->count);
    delta2 = sample - state->mean;
    state->M2 += delta * delta2;
    //printf("new mean: %f\n", state->mean);
}

static inline
double welch_t_statistic(const welford_t state1,
                         const welford_t state2){
 long double num, den, var1, var2;
 var1 = state1.M2/(double)(state1.count-1);
 var2 = state2.M2/(double)(state2.count-1);

 num = state1.mean - state2.mean;
 den = sqrtl(var1/(double) state1.count + var2/(double) state2.count );

 return num/den;
}

static inline
void welford_print(const welford_t state){
     printf("%.2f,%.2f",
              state.mean,
              sqrt(state.M2/(double)(state.count-1)));
}

/*
static inline
void welford_print_tex(const welford_t state){
     printf("$%.2lf$ ($%.2lf$)",
              state.mean,
              sqrtl(state.M2/(double)(state.count-1)));
}
*/

static inline
long double welford_stddev(const welford_t state){
     return sqrtl(state.M2/(double)(state.count-1));
}

static inline
long double welford_mean(const welford_t state){
     return state.mean;
}

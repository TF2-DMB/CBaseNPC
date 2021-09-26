#ifdef __linux__

#include <features.h>

#if __GLIBC__ > 2 || ( __GLIBC__ == 2 && ( __GLIBC_MINOR__ > 30 ) )

#include <math.h>

extern "C" {
	double __exp_finite(double x) { return exp(x); }
	double __exp2_finite(double x) { return exp2(x); }
	double __acos_finite(double x) { return acos(x); }
	double __asin_finite(double x) { return asin(x); }
	double __atan2_finite(double x, double y) { return atan2(x, y); }
	double __log2_finite(double x) { return log2(x); }
	double __log10_finite(double x) { return log10(x); }
	double __log_finite(double x) { return log(x); }
	double __pow_finite(double x, double y) { return pow(x, y); }

	float __expf_finite(float x) { return expf(x); }
	float __exp2f_finite(float x) { return exp2f(x); }
	float __acosf_finite(float x) { return acosf(x); }
	float __asinf_finite(float x) { return asinf(x); }
	float __atan2f_finite(float x, float y) { return atan2f(x, y); }
	float __log2f_finite(float x) { return log2f(x); }
	float __log10f_finite(float x) { return log10f(x); }
	float __logf_finite(float x) { return logf(x); }
	float __powf_finite(float x, float y) { return powf(x, y); }
}

#endif // #if __GLIBC__ > 2
#endif // #ifdef __linux__
#ifndef ENGINE_MATH_MATHUTILS_H_
#define ENGINE_MATH_MATHUTILS_H_

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

//to have backwards compatibility, please remove at some point
#define DEG_TO_RAD(x) ((x)*(M_PI/180.0))
#define RAD_TO_DEG(x) ((x)*(180.0/M_PI))

//general purpose macro functions
#define degToRad(x) ((x)*(M_PI/180.0))
#define radToDeg(x) ((x)*(180.0/M_PI))
#define getMin(A, B) ((A)<(B)?(A):(B))
#define getMax(A, B) ((A)>(B)?(A):(B))
#define getClamp(value, min, max) (getMin((max), getMax((value), (min))))
#define interpolateLinear(P, A, B) ((P)*((B)-(A)) + (A))

#endif /*ENGINE_MATH_MATHUTILS_H_*/

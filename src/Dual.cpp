#include "Dual.h"

// I should really make these all inline and move them into the header.

#define forSlots for (int i = 0; i < kSlots; i++)

namespace PistolsAtDawn {

Float4 operator + (const float c, const Float4 & x) { return x + c; }
Float4 operator - (const float c, const Float4 & x) { return _mm_sub_ps(_mm_set_ps1(c), x.q); }
Float4 operator * (const float c, const Float4 & x) { return x * c; }
Float4 operator / (const float c, const Float4 & x) { return _mm_mul_ps(_mm_set_ps1(c), _mm_rcp_ps(x.q)); }
Float4 operator+(const Float4 & x) { return x; }
Float4 operator-(const Float4 & x) { return Float4(_mm_sub_ps(_mm_setzero_ps(), x.q)); }

// *****************************

template <int kSlots> Dual_Float<kSlots> operator+(const float lhs, const Dual_Float<kSlots> & rhs) { return rhs + lhs; }
template <int kSlots> Dual_Float<kSlots> operator-(const float lhs, const Dual_Float<kSlots> & rhs) { return -rhs + lhs; }
template <int kSlots> Dual_Float<kSlots> operator*(const float lhs, const Dual_Float<kSlots> & rhs) { return rhs * lhs; }
template <int kSlots> Dual_Float<kSlots> operator/(const float lhs, const Dual_Float<kSlots> & rhs) { Dual_Float<kSlots> out; out.data.x = lhs / rhs.data.x; forSlots{ out.data.dx[i] = lhs / rhs.data.dx[i]; } return out; }
template <int kSlots, typename T> bool operator< (const T lhs, const Dual_Float<kSlots> & rhs) { return lhs <  rhs.data.x; }
template <int kSlots, typename T> bool operator<=(const T lhs, const Dual_Float<kSlots> & rhs) { return lhs <= rhs.data.x; }
template <int kSlots, typename T> bool operator> (const T lhs, const Dual_Float<kSlots> & rhs) { return lhs >  rhs.data.x; }
template <int kSlots, typename T> bool operator>=(const T lhs, const Dual_Float<kSlots> & rhs) { return lhs >= rhs.data.x; }
template <int kSlots, typename T> bool operator==(const T lhs, const Dual_Float<kSlots> & rhs) { return lhs == rhs.data.x; }
template <int kSlots, typename T> bool operator!=(const T lhs, const Dual_Float<kSlots> & rhs) { return lhs != rhs.data.x; }
template <int kSlots> Dual_Float<kSlots> operator+(const Dual_Float<kSlots> & x) { return Dual_Float<kSlots>(x); }
template <int kSlots> Dual_Float<kSlots> operator-(const Dual_Float<kSlots> & x) { Dual_Float<kSlots> y{ -x.data.x }; forSlots{ y.data.dx[i] = -x.data.dx[i]; } return y; }
template <int kSlots> Dual_Float<kSlots> sin(const Dual_Float<kSlots> & x) { Dual_Float<kSlots> out{ sin(x.data.x) }; const float c = cos(x.data.x); for (int i = 0; i < kSlots; i++) { out.data.dx[i] = x.data.dx[i] * c; } return out; }
template <int kSlots> Dual_Float<kSlots> cos(const Dual_Float<kSlots> & x) { Dual_Float<kSlots> out{ cos(x.data.x) }; const float ns = -sin(x.data.x); forSlots{ out.data.dx[i] = x.data.dx[i] * ns; } return out; }
// template <int kSlots> Dual_Float<kSlots> tan(const Dual_Float<kSlots> & x) { Dual_Float<kSlots> out{ asin(x.data.x) }; const float t = sec( x.data.x ); const float t2 = t * t; forSlots{ out.data.dx[i] = x.data.dx[i] * t2; } return out; }
template <int kSlots> Dual_Float<kSlots> asin(const Dual_Float<kSlots> & x) { Dual_Float<kSlots> out{ asin(x.data.x) }; const float t = 1.f / sqrt(1.f - x.data.x*x.data.x); forSlots{ out.data.dx[i] = x.data.dx[i] * t; } return out; }
template <int kSlots> Dual_Float<kSlots> acos(const Dual_Float<kSlots> & x) { Dual_Float<kSlots> out{ acos(x.data.x) }; const float t = -1.f / sqrt(1.f - x.data.x*x.data.x); forSlots{ out.data.dx[i] = x.data.dx[i] * t; } return out; }
template <int kSlots> Dual_Float<kSlots> atan(const Dual_Float<kSlots> & x) { Dual_Float<kSlots> out{ atan(x.data.x) }; const float t = 1.f / (1.f + x.data.x*x.data.x); forSlots{ out.data.dx[i] = x.data.dx[i] * t; } return out; }
template <int kSlots> Dual_Float<kSlots> exp(const Dual_Float<kSlots> & x) { Dual_Float<kSlots> out{ exp(x.data.x) }; /*const Float4 e = out.data.x;*/ forSlots{ out.data.dx[i] = x.data.dx[i] * out.data.x; } return out; }
template <int kSlots> Dual_Float<kSlots> log(const Dual_Float<kSlots> & x) { Dual_Float<kSlots> out{ log(x.data.x) }; const Float4 inv = _mm_rcp_ps(_mm_set1_ps(x.data.x)); forSlots{ out.data.dx[i] = x.data.dx[i] * inv; } return out; }

} // namespace PistolsAtDawn

#undef forSlots

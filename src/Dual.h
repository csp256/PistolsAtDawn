#pragma once 

//#include <x86intrin.h>
#include "intrin.h"

#include <cstring>
#include <iostream>
#include "math.h"

namespace PistolsAtDawn {

#define forSlots for (int i = 0; i < kSlots; i++)

struct Float4
{
	__m128 q;
	Float4() : q{ 0 } {}
	Float4(const __m128 q) : q{ q } {}
	Float4(const float x[4]) : q{ x[0],x[1],x[2],x[3] } {}
	Float4(const float a, const float b, const float c, const float d) : q{ a,b,c,d } {}
	// Float4(const int x) : q{ 0 } { Track(x); }
	void Track(const uint64_t x) { float pf[4]; memcpy(pf, &q, sizeof(q)); pf[x] = 1.f; memcpy(&q, pf, sizeof(q)); }
	// void Track(const uint64_t x) { union u { __m128 p; float pf[4]; }; ((u*) &q )->pf[x] = 1.0f; /* oh, is this not working right?? */ }
	float Partial(const uint64_t x) const { float pf[4]; memcpy(pf, &q, sizeof(q)); return pf[x]; }
	// float Partial(const uint64_t x) const { union u { __m128 p; float pf[4]; }; return ((u*)&q)->pf[x]; }
	void Print() const { std::cout << "{ " << Partial(0) << " " << Partial(1) << " " << Partial(2) << " " << Partial(3) << " } "; }
	void Reset(void) { q = _mm_setzero_ps(); }
	// float    operator[](const int i) const { float uf[4]; memcpy(uf, &q, sizeof(q)); return uf[i]; }
	Float4 & operator+=(const Float4 & rhs) { q = _mm_add_ps(q, rhs.q); return *this; }
	Float4 & operator-=(const Float4 & rhs) { q = _mm_sub_ps(q, rhs.q); return *this; }
	Float4 & operator*=(const Float4 & rhs) { q = _mm_mul_ps(q, rhs.q); return *this; }
	Float4 & operator/=(const Float4 & rhs) { q = _mm_div_ps(q, rhs.q); return *this; }
	Float4   operator+ (const Float4 & rhs) const { return Float4(*this) += rhs; }
	Float4   operator- (const Float4 & rhs) const { return Float4(*this) -= rhs; }
	Float4   operator* (const Float4 & rhs) const { return Float4(*this) *= rhs; }
	Float4   operator/ (const Float4 & rhs) const { return Float4(*this) /= rhs; }
	Float4 & operator+=(const float rhs) { q = _mm_add_ps(q, _mm_set_ps1(rhs)); return *this; }
	Float4 & operator-=(const float rhs) { q = _mm_sub_ps(q, _mm_set_ps1(rhs)); return *this; }
	Float4 & operator*=(const float rhs) { q = _mm_mul_ps(q, _mm_set_ps1(rhs)); return *this; }
	Float4 & operator/=(const float rhs) { q = _mm_mul_ps(q, _mm_rcp_ps(_mm_set1_ps(rhs))); return *this; }
	/*
	Though the above gets good codegen, technically rcp is an approximation.
	Intel says:
	The relative error for this approximation is:
	|Relative Error| ≤ 1.5 ∗ 2^−12
	You can use the below implementation instead, if it makes you feel better.
	Also, search for other places the "reciprocal and multiply" idiom is used!
	Though you should note that this *ONLY* impacts the partial derivatives.
	(Primal part never has data dependency on dual part.)
	*/
	// Float4 & operator/=(const float rhs) { q = _mm_div_ps(q, _mm_set_ps1(rhs)); return *this; }
	Float4   operator+ (const float rhs) const { return Float4(*this) += rhs; }
	Float4   operator- (const float rhs) const { return Float4(*this) -= rhs; }
	Float4   operator* (const float rhs) const { return Float4(*this) *= rhs; }
	Float4   operator/ (const float rhs) const { return Float4(*this) /= rhs; }
	Float4 & operator+=(const __m128 rhs) { q = _mm_add_ps(q, rhs); return *this; }
	Float4 & operator-=(const __m128 rhs) { q = _mm_sub_ps(q, rhs); return *this; }
	Float4 & operator*=(const __m128 rhs) { q = _mm_mul_ps(q, rhs); return *this; }
	Float4 & operator/=(const __m128 rhs) { q = _mm_div_ps(q, rhs); return *this; }
	Float4   operator+ (const __m128 rhs) const { return Float4(*this) += rhs; }
	Float4   operator- (const __m128 rhs) const { return Float4(*this) -= rhs; }
	Float4   operator* (const __m128 rhs) const { return Float4(*this) *= rhs; }
	Float4   operator/ (const __m128 rhs) const { return Float4(*this) /= rhs; }
	Float4 & operator= (const Float4 & rhs) { q = rhs.q; return *this; }

}; // struct Float4 

Float4 operator + (const float c, const Float4 & x);
Float4 operator - (const float c, const Float4 & x);
Float4 operator * (const float c, const Float4 & x);
Float4 operator / (const float c, const Float4 & x);
Float4 operator + (const Float4 & x);
Float4 operator - (const Float4 & x);

/* ************************************************************************************************************************
Data Type
************************************************************************************************************************* */

struct DummyType {};
namespace Internal {

template <int kSlots, int kZero>
struct Data 
{
	Float4 dx[kSlots];
	float x;
	// char padding[12];
	Data() {}
	Data(const float & x) : x{ x } {  }
	Data(const Data & rhs) : x{ rhs.x } { forSlots{ dx[i] = rhs.dx[i]; } }
	void Track(const uint64_t i) { dx[i / 4].Track(i % 4); }
};

template <int kZero>
struct Data<kZero, kZero> 
{
	union {
		DummyType dx[1];
		float x;
		// DummyType padding[1];
	};
	Data() {}
	Data(const float & x) : x{ x } {}
	Data(const Data & x) : x{ x.x } {}
};

} // namespace Internal

  
/* ************************************************************************************************************************
Dual Type
************************************************************************************************************************* */


template <int kSlots_>
struct Dual_Float 
{
	typedef float InternalType;
	static constexpr int kSlots = kSlots_;
	Internal::Data<kSlots, 0> data;
	static constexpr float ln2 = 0.6931471805599453f;
	Dual_Float() {}
	Dual_Float(const float x) : data{ x } {}
	Dual_Float(const float x, const int slot) : data{ x } { data.Track(slot); }
	Dual_Float(const Dual_Float & d) : data{ d.data } {}
	void Print(void) const { std::cout << data.x << " { "; forSlots{ data.dx[i].Print(); } std::cout << "}" << std::endl; }
	void Track(const uint64_t slot) { data.Track(slot); }
	float Primal() const { return data.x; }
	float & PrimalReference() { return data.x; }
	float Partial(const uint64_t slot) const { const Float4 & dx = data.dx[slot / 4]; return dx.Partial(slot % 4); }
	Dual_Float & operator+=(const Dual_Float & rhs) { forSlots{ data.dx[i] += rhs.data.dx[i]; } data.x += rhs.data.x; return *this; };
	Dual_Float & operator-=(const Dual_Float & rhs) { forSlots{ data.dx[i] -= rhs.data.dx[i]; } data.x -= rhs.data.x; return *this; };
	Dual_Float & operator*=(const Dual_Float & rhs) { forSlots{ data.dx[i] = data.dx[i] * rhs.data.x + rhs.data.dx[i] * data.x; } data.x *= rhs.data.x; return *this; }
	Dual_Float & operator/=(const Dual_Float & rhs) { const float inv = 1.f / rhs.data.x, inv2 = inv * inv; forSlots{ data.dx[i] = (data.dx[i] * rhs.data.x - rhs.data.dx[i] * data.x) * inv2; } data.x *= inv; return *this; }
	Dual_Float   operator+ (const Dual_Float & rhs) const { return Dual_Float(*this) += rhs; }
	Dual_Float   operator- (const Dual_Float & rhs) const { return Dual_Float(*this) -= rhs; }
	Dual_Float   operator* (const Dual_Float & rhs) const { return Dual_Float(*this) *= rhs; }
	Dual_Float   operator/ (const Dual_Float & rhs) const { return Dual_Float(*this) /= rhs; }
	Dual_Float & operator+=(const float rhs) { data.x += rhs; return *this; }
	Dual_Float & operator-=(const float rhs) { data.x -= rhs; return *this; }
	Dual_Float & operator*=(const float rhs) { forSlots{ data.dx[i] *= rhs; } data.x *= rhs; return *this; }
	Dual_Float & operator/=(const float rhs) { const float inv = 1.f / rhs; forSlots{ data.dx[i] *= inv; } data.x *= inv; return *this; }
	Dual_Float   operator+ (const float rhs) const { return Dual_Float(*this) += rhs; }
	Dual_Float   operator- (const float rhs) const { return Dual_Float(*this) -= rhs; }
	Dual_Float   operator* (const float rhs) const { return Dual_Float(*this) *= rhs; }
	Dual_Float   operator/ (const float rhs) const { return Dual_Float(*this) /= rhs; }
	bool operator< (const Dual_Float & rhs) const { return data.x <  rhs.data.x; }
	bool operator<=(const Dual_Float & rhs) const { return data.x <= rhs.data.x; }
	bool operator> (const Dual_Float & rhs) const { return data.x >  rhs.data.x; }
	bool operator>=(const Dual_Float & rhs) const { return data.x >= rhs.data.x; }
	bool operator==(const Dual_Float & rhs) const { return data.x == rhs.data.x; }
	bool operator!=(const Dual_Float & rhs) const { return data.x != rhs.data.x; }
	Dual_Float & operator=(const Dual_Float & rhs) { data.x = rhs.data.x; forSlots{ data.dx[i] = rhs.data.dx[i]; } return *this; }
	bool operator< (const float rhs) const { return data.x <  rhs; }
	bool operator<=(const float rhs) const { return data.x <= rhs; }
	bool operator> (const float rhs) const { return data.x >  rhs; }
	bool operator>=(const float rhs) const { return data.x >= rhs; }
	bool operator==(const float rhs) const { return data.x == rhs; }
	bool operator!=(const float rhs) const { return data.x != rhs; }
	Dual_Float & operator=(const float rhs) { data.x = rhs; forSlots{ data.dx[i].Reset(); } return *this; }
	bool operator< (const int rhs) const { return data.x <  rhs; }
	bool operator<=(const int rhs) const { return data.x <= rhs; }
	bool operator> (const int rhs) const { return data.x >  rhs; }
	bool operator>=(const int rhs) const { return data.x >= rhs; }
	bool operator==(const int rhs) const { return data.x == rhs; }
	bool operator!=(const int rhs) const { return data.x != rhs; }
	Dual_Float & operator=(const int rhs) { data.x = (float)rhs; forSlots{ data.dx[i].Reset(); } return *this; }
	// operator float() const { return (float) data.x; } 
	// operator double() const { return (double) data.x; } 

}; // struct Dual_Float

template <int kSlots> Dual_Float<kSlots> operator+(const float lhs, const Dual_Float<kSlots> & rhs);
template <int kSlots> Dual_Float<kSlots> operator-(const float lhs, const Dual_Float<kSlots> & rhs);
template <int kSlots> Dual_Float<kSlots> operator*(const float lhs, const Dual_Float<kSlots> & rhs);
template <int kSlots> Dual_Float<kSlots> operator/(const float lhs, const Dual_Float<kSlots> & rhs);
template <int kSlots, typename T> bool operator< (const T lhs, const Dual_Float<kSlots> & rhs);
template <int kSlots, typename T> bool operator<=(const T lhs, const Dual_Float<kSlots> & rhs);
template <int kSlots, typename T> bool operator> (const T lhs, const Dual_Float<kSlots> & rhs);
template <int kSlots, typename T> bool operator>=(const T lhs, const Dual_Float<kSlots> & rhs);
template <int kSlots, typename T> bool operator==(const T lhs, const Dual_Float<kSlots> & rhs);
template <int kSlots, typename T> bool operator!=(const T lhs, const Dual_Float<kSlots> & rhs);
template <int kSlots> Dual_Float<kSlots> operator+(const Dual_Float<kSlots> & x);
template <int kSlots> Dual_Float<kSlots> operator-(const Dual_Float<kSlots> & x);

/* ************************************************************************************************************************
Special functions
************************************************************************************************************************* */

template <int kSlots> Dual_Float<kSlots> sin(const Dual_Float<kSlots> & x);
template <int kSlots> Dual_Float<kSlots> cos(const Dual_Float<kSlots> & x);
// template <int kSlots> Dual_Float<kSlots> tan(const Dual_Float<kSlots> & x) { Dual_Float<kSlots> out{ asin(x.data.x) }; const float t = sec( x.data.x ); const float t2 = t * t; forSlots{ out.data.dx[i] = x.data.dx[i] * t2; } return out; }
template <int kSlots> Dual_Float<kSlots> asin(const Dual_Float<kSlots> & x);
template <int kSlots> Dual_Float<kSlots> acos(const Dual_Float<kSlots> & x);
template <int kSlots> Dual_Float<kSlots> atan(const Dual_Float<kSlots> & x);
template <int kSlots> Dual_Float<kSlots> exp(const Dual_Float<kSlots> & x);
template <int kSlots> Dual_Float<kSlots> log(const Dual_Float<kSlots> & x);

template <int kSlots> inline Dual_Float<kSlots> sqrt(const Dual_Float<kSlots> & x) { Dual_Float<kSlots> out{ sqrtf(x.data.x) }; const float hx = 0.5f * out.data.x; forSlots{ out.data.dx[i] = x.data.dx[i] * hx; } return out; }

/* Still missing one of the pow's! */
template <int kSlots> inline Dual_Float<kSlots> pow(const Dual_Float<kSlots> & x, const float k) { Dual_Float<kSlots> out{ powf(x.data.x, k) }; const Float4 c = k * out.data.x * _mm_rcp_ps(_mm_set1_ps(x.data.x)); forSlots{ out.data.dx[i] = c * x.data.dx[i]; } return out; }
//template <int kSlots> inline Dual_Float<kSlots> pow(const Dual_Float<kSlots> & x, const Dual_Float<kSlots> & k) { Dual_Float<kSlots> out = Dual_Float<kSlots>{ pow(x.data.x, k.data.x) }; const float c = k.data.x * out.data.x / x.data.x; const float d = out.data.x * log(x.data.x); forSlots{ out.data.dx[i] = c * x.data.dx[i] + d * k.data.dx[i]; } return out; }

template struct Dual_Float<1>;

} // namespace PistolsAtDawn

#undef forSlots

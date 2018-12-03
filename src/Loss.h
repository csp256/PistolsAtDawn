#pragma once
#include "Dual.h"

namespace PistolsAtDawn {

// Implements Barron's loss function 
// https://arxiv.org/pdf/1701.03077.pdf
class Loss
{
	using Dual = PistolsAtDawn::Dual_Float<1>;
public:
	const float a;
	const float c;

private:
	float inv_c2, inv_c2b, d_exp;
public:
	Loss(const float a = 0.f, const float c = 1.f, const float eps = 1e-5)
		: a{ a }
		, c{ c }
	{
		const auto b = std::abs(2.f - a) + eps;
		const auto c2 = c * c;
		const auto d = (a < 0) ? a - eps : a + eps;
		inv_c2 = 1.f / c2;
		inv_c2b = inv_c2 / b;
		d_exp = 0.5f * d - 1.f;
	}

	Dual
	operator () (const float x_) const;
};

}
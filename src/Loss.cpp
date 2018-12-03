#include "stdafx.h"
#include "Loss.h"

namespace PistolsAtDawn {

// Implements dp/dx as the primal part, and d^2p/dx^2 in the first dual slot
Loss::Dual
Loss::operator () (const float x_) const
{
	//Dual x = Dual(x_, 0);
	//return x * inv_c2 * pow(x * x * inv_c2b + 1.f, d_exp);
	return Dual(x_);
}

}
#pragma once

#include "Dual.h"
#include <Eigen/Dense>

template <typename Float>
struct Sphere
{
	const Float radius;

	Sphere() 
		: radius{ 1.f }
	{}

	Sphere(const double radius) 
		: radius{ (Float) radius }
	{}

	inline
	Float
	operator() (
		const Eigen::Matrix<Float, -1, 1> & parameters_,
		const Eigen::Matrix<Float, -1, 1> & datum_) const
	{
		
		const auto R2 = (parameters_ - datum_).squaredNorm();
		const auto R = sqrt(R2);
		const auto dr = Float(R) - radius;
		return dr;
	}
};

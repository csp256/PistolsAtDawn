#include "stdafx.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include "SyntheticData.h"

Eigen::MatrixXd
GeneratePointsOnSphereSurface(
	const int samples,
	const Eigen::Vector3d center,
	const double radius,
	const double noise)
{
	std::vector<Eigen::Vector3d> temp;

	std::mt19937_64 rng(0);
	std::normal_distribution<double> normal(0, noise);

	const auto offset = 2. / samples;
	const auto increment = M_PI * (3. - sqrt(5.));

	for (int i = 0, k = 0; i < samples; i++) {
		const auto y = ((i * offset) - 1) + (offset / 2);
		const auto r = sqrt(1 - y*y);
		const auto phi = i * increment;
		const auto x = cos(phi) * r;
		const auto z = sin(phi) * r;
		const auto xx = x * radius + center[0];
		const auto yy = y * radius + center[1];
		const auto zz = z * radius + center[2];
		if (std::isfinite(xx) && std::isfinite(yy) && std::isfinite(zz)) {
			temp.emplace_back(xx + normal(rng), yy + normal(rng), zz + normal(rng));
		}
	}

	Eigen::MatrixXd out;
	out.resize(3, temp.size());
	for (unsigned int i = 0; i < temp.size(); i++) {
		out.col(i) = temp[i];
	}

	return out;
}

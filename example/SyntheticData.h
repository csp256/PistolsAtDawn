#pragma once

#include <Eigen/Dense>
#include <vector>
#include <random>

Eigen::MatrixXd
GeneratePointsOnSphereSurface(
	const int samples,
	const Eigen::Vector3d center,
	const double radius,
	const double noise = 0.);

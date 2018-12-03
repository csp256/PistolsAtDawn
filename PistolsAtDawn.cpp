#include "Sphere.h"
#include "Optimization.h"
#include "SyntheticData.h"

int main()
{
	using Dual = PistolsAtDawn::Dual_Float<1>;

	const int data_points = 100;
	const Eigen::Vector3d sphere_center = Eigen::Vector3d(2, 3, 5);
	const double radius = 1.;
	const double noise = 0.1;

	const Eigen::MatrixXd data = GeneratePointsOnSphereSurface(data_points, sphere_center, radius, noise);

	Eigen::VectorXd parameters = Eigen::Vector3d::Zero();

	auto sphere = Sphere<Dual>(radius);

	std::cout << "First Residual:  " << (parameters - sphere_center).norm() << std::endl;

	PistolsAtDawn::Optimizer<double> opt;
	opt.Optimize(sphere, parameters, data);
	
	std::cout << "Final Residual:  " << (parameters - sphere_center).norm() << std::endl;

	return 0;
}

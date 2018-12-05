#include "Sphere.h"
#include "SyntheticData.h"
#include "Optimization.h"

/*

This example fits a sphere of constant radius to a 
noisy set of 3d points.

In order to fit a different model to different data, 
all the user must do is create a different class 
which implements the same operator() as the Sphere
class does in this example.

On each iteration of the optimization the Optimizer
object loops over the data and calls that operator()
function with the current model parameters and one
column of data matrix. (Each column represents a
different data point.)

*/

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

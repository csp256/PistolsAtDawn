#pragma once

#include <chrono>
#include <Eigen/Dense>
#include "Dual.h"
#include "Loss.h"

namespace PistolsAtDawn {

template <typename Type = double>
struct Optimizer
{
	static constexpr int kSlots = 1;

	using Dual = Dual_Float<kSlots>;
	using Duel = Dual_Float<kSlots>;
	using PistolsAtDawn = Dual_Float<kSlots>;

	Eigen::Matrix<Dual, -1, -1> parameters_;
	Eigen::Matrix<Dual, -1,  1> datum_;
	Eigen::Matrix<Type, -1,  1> state;
	Eigen::Matrix<Type,  1, -1> J_row;

	int iteration = 0;
	int max_iterations;
	Type learning_rate;
	Type measurement_error;
	Loss barron_loss;

	void 
	Reset(const int num_data, const int num_parameters);

	void
	LoadDefaults();

	Optimizer()
		: barron_loss(0.f, 1.f)
	{ LoadDefaults(); }

	// See "Bundle adjustment, a modern synthesis", page 17, equation 10 
	template <typename F>
	inline 
	void
	Optimize(
			F & f,
			Eigen::VectorXd & parameters,
			const Eigen::MatrixXd & data)
	{
        Reset(data.cols(), parameters.rows());

		for (int i = 0; i < parameters_.rows(); i++) {
			parameters_(i, 0) = Dual((float) parameters[i], i); 
		}

		Eigen::Matrix<Type, -1, -1> H;
		Eigen::Matrix<Type, -1,  1> g;
		H.resize(parameters.rows(), parameters.rows());
		g.resize(parameters.rows(), 1);
		
		for (int count = 0; count < max_iterations; count++) {
			for (int i = 0; i < parameters.rows(); i++) {
				parameters_(i, 0).data.x = (float) parameters[i];
			}

			H.setZero();
			g.setZero();
			Type DeltaZ;
			const Type W = 1 / (measurement_error * measurement_error);

			for (int i = 0; i < data.cols(); i++) {
				const auto & datum_ = data.col(i).template cast<Dual>();
				Dual f_ = f(parameters_.col(0), datum_);
				DeltaZ = f_.Primal();
				for (int j = 0; j < parameters.rows(); j++) {
					J_row[j] = (Type)f_.Partial(j);
				}
				const auto W_DeltaZ = W * DeltaZ;
				const auto DeltaZ_W_DeltaZ = DeltaZ * W_DeltaZ;

				auto InnerTerm = [&]()
				{
					const bool negative_curvature =
						(loss.Partial(0) * DeltaZ_W_DeltaZ) < (-0.5 * loss.Primal());

					if (negative_curvature) {
						// Unsafe to apply acceleration term without rescaling (see Triggs, eq 11)
						// so we just set it to 0 as an approximation (I think this is what Ceres does?)
						return loss.Primal() * W;
					} else {
						// Can safely add acceleration term because we have positive curvature
						return loss.Primal() * W + 2. * loss.Partial(0) * W_DeltaZ * W_DeltaZ;
					}
				};
				
				const auto loss = barron_loss((float) DeltaZ_W_DeltaZ);
				g -= loss.Primal() * J_row.transpose() * W * DeltaZ;
				H += J_row.transpose() * InnerTerm() * J_row;
			} // loop over data

			Eigen::BDCSVD<Eigen::Matrix<Type, -1, -1>> solver(H, Eigen::ComputeFullU | Eigen::ComputeFullV);

			parameters.col(0) += (learning_rate * solver.solve(g)).eval();

			//std::cout << parameters.transpose() << std::endl;
		} // main optimization loop
		
	} // Optimizer::Optimize()

}; // struct Optimizer

} // namespace PistolsAtDawn

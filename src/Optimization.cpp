#include "Optimization.h"

namespace PistolsAtDawn {

template <typename T>
void 
Optimizer<T>::Reset(const int num_data, const int num_parameters)
{
	parameters_.resize(num_parameters, 1);
	parameters_.setZero();
	datum_.resize(num_data, 1);
	datum_.setZero();
	state.resize(num_data, 1);
	state.setZero();
	J_row.resize(1, num_parameters);
	J_row.setZero();

	iteration = 0;
}

template <typename T>
void
Optimizer<T>::LoadDefaults()
{
	max_iterations = 50;
	learning_rate = 1.0;
	measurement_error = 1.0; 
}

template struct Optimizer<double>;

} // namespace PistolsAtDawn

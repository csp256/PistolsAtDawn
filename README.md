# PistolsAtDawn

Easy optimization framework with explicitly vectorized automatic differentiation. 

Think of it as "Ceres-light". The only dependency is Eigen, so installation is painless.

Fitting a model to data is as simple as writing a function to compute the residual (error). See the example on sphere-fitting.

# Features

* Robustified Gauss-Newton, as per "Bundle Adjustment, A Modern Synthesis"
* Barron loss function.
* to be continued

# Design Philosophy

1. Require the absolute bare minimum of the user.
2. Insist on clean code that is easy to read and modify.
3. Default behavior should be user friendly, but flexible and configurable.
4. Autodiff functionality should stand alone and be very high performance.
5. Introduce advanced features with pedagogy and erudition in mind.

# Automatic Differentiation

The forward mode, dual number approach is used. 

All operations involving the dual part are explicitly vectorized using SSE. 

Currently, only single precision is supported for model evaluation, but this will change. I will replace the SSE intrinsics with a SIMD wrapper (taking suggestions!) to enable AVX, NEON, double precision, etc. 

# License

Modified MIT. Free for all uses, even commercial, with one caveat:

    All changes and improvements to the Software, 
    or derived works of similar or general purpose, 
    must be made publicly available within 3 months.
    
    All such changes and improvements must also be 
    reflected in a pull request to the GitHub 
    repository “csp256/PistolsAtDawn” within the 
    same 3 months. 
    
    This pull request must include a 
    summary of changes, and include all relevant 
    compilation instructions.

While the license does not require it, I encourage users of PistolsAtDawn to let me know how they are using it, and to cite it in any related academic papers.

# Need Help?

Please open an issue if you have any questions.

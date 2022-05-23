/** \file */
#ifndef PROXSUITE_QP_SPARSE_HELPERS_HPP
#define PROXSUITE_QP_SPARSE_HELPERS_HPP

#include <Eigen/Sparse>
#include <veg/vec.hpp>
#include <qp/sparse/fwd.hpp>

namespace proxsuite {
namespace qp {
namespace sparse {

/*!
* Update the proximal parameters of the results object.
*
* @param rho_new primal proximal parameter
* @param mu_eq_new dual equality proximal parameter
* @param mu_in_new dual inequality proximal parameter
* @param results solver result 
*/
template <typename T>
void update_proximal_parameters(
		Results<T>& results,
		tl::optional<T> rho_new,
		tl::optional<T> mu_eq_new,
		tl::optional<T> mu_in_new) {

	if (rho_new != tl::nullopt) {
		results.info.rho = rho_new.value();
	}
	if (mu_eq_new != tl::nullopt) {
		results.info.mu_eq = mu_eq_new.value();
		results.info.mu_eq_inv = T(1) / results.info.mu_eq;
	}
	if (mu_in_new != tl::nullopt) {
		results.info.mu_in = mu_in_new.value();
		results.info.mu_in_inv = T(1) / results.info.mu_in;
	}
}
/*!
* Warm start the results primal and dual variables.
*
* @param x_wm primal proximal parameter
* @param y_wm dual equality proximal parameter
* @param z_wm dual inequality proximal parameter
* @param results solver result 
* @param settings solver settings 
*/
template <typename T>
void warm_start(
		tl::optional<VecRef<T>> x_wm,
		tl::optional<VecRef<T>> y_wm,
		tl::optional<VecRef<T>> z_wm,
		Results<T>& results,
		Settings<T>& settings) {

	isize n_eq = results.y.rows();
	isize n_in = results.z.rows();
	if (n_eq!=0){
		if (n_in!=0){
			if(x_wm != tl::nullopt && y_wm != tl::nullopt && z_wm != tl::nullopt){
					results.x = x_wm.value().eval();
					results.y = y_wm.value().eval();
					results.z = z_wm.value().eval();
			}
		}else{
			// n_in= 0
			if(x_wm != tl::nullopt && y_wm != tl::nullopt){
					results.x = x_wm.value().eval();
					results.y = y_wm.value().eval();
			}
		}
	}else if (n_in !=0){
		// n_eq = 0
		if(x_wm != tl::nullopt && z_wm != tl::nullopt){
					results.x = x_wm.value().eval();
					results.z = z_wm.value().eval();
		}
	} else {
		// n_eq = 0 and n_in = 0
		if(x_wm != tl::nullopt ){
					results.x = x_wm.value().eval();
		}
	}	

	settings.initial_guess = InitialGuessStatus::WARM_START;

}

template <typename T, typename I, typename P>
void qp_setup(
		QpView<T, I> qp,
		Results<T>& results,
		Model<T, I>& data,
		Workspace<T, I>& work,
		Settings<T>& settings,
		P& precond) {
	isize n = qp.H.nrows();
	isize n_eq = qp.AT.ncols();
	isize n_in = qp.CT.ncols();

	if (results.x.rows() != n) {
		results.x.resize(n);
		results.x.setZero();
	}
	if (results.y.rows() != n_eq) {
		results.y.resize(n_eq);
		results.y.setZero();
	}
	if (results.z.rows() != n_in) {
		results.z.resize(n_in);
		results.z.setZero();
	}
	if (results.active_constraints.len() != n_in) {
		results.active_constraints.resize(n_in);
		for (isize i = 0; i < n_in; ++i) {
			results.active_constraints[i] = false;
		}
	}

	switch (settings.initial_guess) {
				case InitialGuessStatus::UNCONSTRAINED_INITIAL_GUESS: {
					work._.setup_impl(
						qp,
						results,
						data,
						precond,
						P::scale_qp_in_place_req(veg::Tag<T>{}, n, n_eq, n_in));
                    break;
                }
                case InitialGuessStatus::EQUALITY_CONSTRAINED_INITIAL_GUESS:{
					work._.setup_impl(
						qp,
						results,
						data,
						precond,
						P::scale_qp_in_place_req(veg::Tag<T>{}, n, n_eq, n_in));
                    break;
                }
                case InitialGuessStatus::COLD_START:{
					// keep solutions but restart workspace and results
					//qpwork.cleanup(); equivalent ?
					results.cold_start();
					work._.setup_impl(
						qp,
						results,
						data,
						precond,
						P::scale_qp_in_place_req(veg::Tag<T>{}, n, n_eq, n_in));
                    break;
                }
                case InitialGuessStatus::NO_INITIAL_GUESS:{
					//qpwork.cleanup(); equivalent ?
					results.cleanup(); 
					work._.setup_impl(
						qp,
						results,
						data,
						precond,
						P::scale_qp_in_place_req(veg::Tag<T>{}, n, n_eq, n_in));
                    break;
                }
				case InitialGuessStatus::WARM_START:{
					//qpwork.cleanup();
					results.cleanup(); 
					work._.setup_impl(
						qp,
						results,
						data,
						precond,
						P::scale_qp_in_place_req(veg::Tag<T>{}, n, n_eq, n_in));
                    break;
                }
                case InitialGuessStatus::WARM_START_WITH_PREVIOUS_RESULT:{
                    // keep workspace and results solutions except statistics
					results.cleanup_statistics();
                    break;
                }
	}
	// initial guess done in the solve for the moment
}

} // namespace sparse
} // namespace qp
} // namespace proxsuite

#endif /* end of include guard PROXSUITE_QP_SPARSE_HELPERS_HPP */
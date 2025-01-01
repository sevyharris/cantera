/**
 *  @file PreconditionerBase.h Declarations for the class
 *   PreconditionerBase which is a virtual base class for
 *   preconditioning systems.
 */

// This file is part of Cantera. See License.txt in the top-level directory or
// at https://cantera.org/license.txt for license and copyright information.

#ifndef PRECONDITIONERBASE_H
#define PRECONDITIONERBASE_H

#include "cantera/base/ctexceptions.h"

namespace Cantera
{

/**
 * Specifies the side of the system on which the preconditioner is applied. Not all
 * methods are supported by all integrators.
 */
enum class PreconditionerSide {
    NO_PRECONDITION, //! No preconditioning
    LEFT_PRECONDITION, //! Left side preconditioning
    RIGHT_PRECONDITION, //! Right side preconditioning
    BOTH_PRECONDITION //! Left and right side preconditioning
};

//! PreconditionerBase serves as an abstract type to extend different preconditioners
class PreconditionerBase
{
public:
    PreconditionerBase() {}

    virtual ~PreconditionerBase() {}

    virtual const string type() const = 0;

    //! Set a value at the specified row and column of the jacobian triplet vector
    //! @param row row in the jacobian matrix
    //! @param col column in the jacobian matrix
    //! @param value value of the element at row and col
    virtual void setValue(size_t row, size_t col, double value) {
        throw NotImplementedError("PreconditionerBase::setValue");
    }

    //! Adjust the state vector based on the preconditioner, e.g., Adaptive
    //! preconditioning uses a strictly positive composition when preconditioning which
    //! is handled by this function
    //! @param state a vector containing the state to be updated
    virtual void stateAdjustment(vector<double>& state) {
        throw NotImplementedError("PreconditionerBase::stateAdjustment");
    }

    //! Get preconditioner application side for CVODES
    string preconditionerSide() const {
        return m_precon_side;
    }

    virtual void setPreconditionerSide(const string& preconSide) {
        m_precon_side = preconSide;
    }

    //! Update the diagonal terms in the Jacobian by using the transient mask.
    //! @param rdt  Reciprocal of the time step [1/s]
    //! @param mask  Mask for transient terms: 1 if transient, 0 if algebraic.
    virtual void updateTransient(double rdt, int* mask) {
        throw NotImplementedError("PreconditionerBase::updateTransient");
    }

    //! Solve a linear system Ax=b where A is the preconditioner
    //! @param[in] stateSize length of the rhs and output vectors
    //! @param[in] rhs_vector right hand side vector used in linear system
    //! @param[out] output output vector for solution
    virtual void solve(const size_t stateSize, double* rhs_vector, double* output) {
        throw NotImplementedError("PreconditionerBase::solve");
    };

    //! Perform preconditioner specific post-reactor setup operations such as factorize.
    //! @deprecated To be removed after %Cantera 3.2. Use updatePreconditioner() and
    //!     factorize() instead.
    virtual void setup() {
        throw NotImplementedError("PreconditionerBase::setup");
    };

    //! Reset preconditioner parameters as needed
    virtual void reset() {
        throw NotImplementedError("PreconditionerBase::reset");
    };

    //! Called during setup for any processes that need
    //! to be completed prior to setup functions used in sundials.
    //! @param networkSize the number of variables in the associated reactor network
    virtual void initialize(size_t networkSize) {
        throw NotImplementedError("PreconditionerBase::initialize");
    };

    //! Used to provide system bandwidth for implementations that use banded matrix
    //! storage. Ignored if not needed.
    virtual void setBandwidth(size_t bw) {}

    //! Print preconditioner contents
    virtual void printPreconditioner() {
        throw NotImplementedError("PreconditionerBase::printPreconditioner");
    };

    //! Transform Jacobian vector and write into
    //! preconditioner, P = (I - gamma * J)
    virtual void updatePreconditioner() {
        throw NotImplementedError("PreconditionerBase::updatePreconditioner");
    }

    //! Set gamma used in preconditioning
    //! @param gamma used in M = I - gamma*J
    virtual void setGamma(double gamma) {
        m_gamma = gamma;
    };

    //! Get gamma used in preconditioning
    virtual double gamma() {
        return m_gamma;
    };

    //! Set the absolute tolerance in the solver outside of the network initialization
    //! @param atol the specified tolerance
    virtual void setAbsoluteTolerance(double atol) {
        m_atol = atol;
    }

    //! Get latest status of linear solver. Zero indicates success. Meaning of non-zero
    //! values is solver dependent.
    virtual int info() const {
        throw NotImplementedError("PreconditionerBase::info");
    }

    //! Elapsed CPU time spent computing the Jacobian elements.
    double elapsedTime() const {
        return m_elapsed;
    }
    void updateElapsed(double evalTime) {
        m_elapsed += evalTime;
    }

    //! Number of Jacobian evaluations.
    int nEvals() const {
        return m_nevals;
    }
    void incrementEvals() {
        m_nevals++;
    }

    //! Number of times 'incrementAge' has been called since the last evaluation
    int age() const {
        return m_age;
    }

    //! Increment the Jacobian age.
    void incrementAge() {
        m_age++;
    }

    //! Set the Jacobian age.
    void setAge(int age) {
        m_age = age;
    }

protected:
    //! Factorize the system matrix. This method should be called at the end of
    //! implementations of updatePreconditioner() and updateTransient().
    virtual void factorize() {
        throw NotImplementedError("preconditionerBase::factorize");
    }

    //! Dimension of the preconditioner
    size_t m_dim;

    //! gamma value used in M = I - gamma*J
    double m_gamma = 1.0;

    //! bool saying whether or not the preconditioner is initialized
    bool m_init = false;

    //! Absolute tolerance of the ODE solver
    double m_atol = 0;

    double m_elapsed = 0.0; //!< Elapsed CPU time taken to compute the Jacobian
    int m_nevals = 0; //!< Number of Jacobian evaluations.

    int m_age = 100000; //!< Age of the Jacobian (times incrementAge() has been called)

    string m_precon_side = "none";
};

}
#endif

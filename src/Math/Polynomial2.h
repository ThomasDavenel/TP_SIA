#ifndef _Rennes1_Polynom2_H
#define _Rennes1_Polynom2_H

#include <math.h>
#include <Math/Interval.h>
#include <array>

namespace Math
{
	/// <summary>
	/// A second degree polynomial with solvers.
	/// </summary>
	template <class Float>
	class Polynomial2 
	{
	protected:
		/** \brief Les coefficients du polynome */
		Float m_coefficients[3];

	public:
		/// <summary>
		/// Access to coefficients.
		/// </summary>
		/// <param name="index">The index.</param>
		/// <returns></returns>
		Float operator[] (int index) const
		{
			return m_coefficients[index];
		}

		/// <summary>
		/// Access to the coefficients.
		/// </summary>
		/// <param name="index">The index.</param>
		/// <returns></returns>
		Float & operator[] (int index)
		{
			return m_coefficients[index];
		}

		/// <summary>
		/// Construction of a aX^2+bX+c polynomial.
		/// </summary>
		/// <param name="a">a.</param>
		/// <param name="b">The b.</param>
		/// <param name="c">The c.</param>
		Polynomial2(Float a = 0, Float b = 0, Float c = 0)
		{
			(*this)[0] = c;
			(*this)[1] = b;
			(*this)[2] = a;
		}


		/// <summary>
		/// Evaluates the polynomial.
		/// </summary>
		/// <param name="value">The value.</param>
		/// <returns></returns>
		Float operator() (Float value) const
		{
			return (m_coefficients[2] * value + m_coefficients[1])*value + m_coefficients[0];
		}

		/// <summary>
		/// Returns the delta of this polynomial.
		/// </summary>
		/// <returns></returns>
		Float delta() const
		{
			return m_coefficients[1] * m_coefficients[1] - m_coefficients[0] * m_coefficients[2] * Float(4.0);
		}

		/// <summary>
		/// Solves P(x)=0.
		/// </summary>
		/// <param name="sol">The solutions.</param>
		/// <returns> The number of solutions</returns>
		int solutions(std::array<Float, 2> & sol) const
		{
			Float d = delta();
			if (d < 0.0)
			{
				return 0;
			}
			Float divider = Float(1.0) / (Float(2.0)*m_coefficients[2]);
			Float coefDivider = m_coefficients[1] * divider;
			if (d == 0)
			{
				sol[0] = (-coefDivider);
				return 1;
			}
			Float sqrtDeltaDivider = sqrt(d) * divider;
			sol[0]=(-coefDivider - sqrtDeltaDivider);
			sol[1]=(-coefDivider + sqrtDeltaDivider);
			return 2;
		}

		/// <summary>
		/// Returns the parameters value of the polynomial extremum (minimum or maximum)
		/// </summary>
		/// <returns></returns>
		Float extremumRoot() const
		{
			return -m_coefficients[1] / (m_coefficients[2] * Float(2.0));
		}

		/// <summary>
		/// Returns the parameter value of the minimum value of the polynomial in the provided interval.
		/// </summary>
		/// <param name="interval">The interval.</param>
		/// <returns></returns>
		Float minimumRoot(Math::Interval<Float> const & interval) const
		{
			std::pair<Float, Float> result;
			result = std::min(std::make_pair((*this)(interval.inf()), interval.inf()), std::make_pair((*this)(interval.sup()), interval.sup()));
			// If the extremum is in the interval, wa have to take it into consideration
			Float eRoot = extremumRoot();
			if (interval.in(eRoot))
			{
				result = std::min(result, std::make_pair((*this)(eRoot), eRoot));
			}
			return result.second;
		}

		/// <summary>
		/// Returns the parameter value of the maximum value of the polynomial in the provided interval.
		/// </summary>
		/// <param name="interval">The interval.</param>
		/// <returns></returns>
		Float maximumRoot(Math::Interval<Float> const & interval) const
		{
			std::pair<Float, Float> result;
			result = std::max(std::make_pair((*this)(interval.inf()), interval.inf()), std::make_pair((*this)(interval.sup()), interval.sup()));
			// If the extremum is in the interval, we have to take it into consideration
			Float eRoot = extremumRoot();
			if (interval.in(eRoot))
			{
				result = std::max(result, std::make_pair((*this)(eRoot), eRoot));
			}
			return result.second;
		}
	};

}


#endif

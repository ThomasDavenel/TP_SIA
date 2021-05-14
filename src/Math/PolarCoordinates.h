#pragma once

#include <Math/Vector.h>

namespace Math
{
	/// <summary>
	/// Polar coordinates
	/// </summary>
	template <typename FloatType = float>
	class PolarCoordinates
	{
	private:
		FloatType m_angle;
		FloatType m_norm;

	public:
		/// <summary>
		/// The 2 dimensional vector type.
		/// </summary>
		using Vector2 = Math::Vector<FloatType, 2>;

		/// <summary>
		/// Initializes a new instance of the <see cref="PolarCoordinates"/> class.
		/// </summary>
		/// <param name="angle">The angle.</param>
		/// <param name="norm">The norm.</param>
		PolarCoordinates(FloatType angle = FloatType(0.0), FloatType norm = FloatType(1.0))
			: m_angle(angle), m_norm(norm)
		{}

		/// <summary>
		/// Initializes a new instance of the <see cref="PolarCoordinates"/> class.
		/// </summary>
		/// <param name="v">The vector.</param>
		PolarCoordinates(const Vector2 & v)
			: m_angle(atan2(v[1], v[0])), m_norm(v.norm())
		{}

		/// <summary>
		/// Sets the angle.
		/// </summary>
		/// <param name="angle">The angle.</param>
		void setAngle(FloatType angle) { m_angle = angle; }

		/// <summary>
		/// Gets the angle.
		/// </summary>
		/// <returns></returns>
		FloatType getAngle() const { return m_angle; }

		/// <summary>
		/// Sets the norm.
		/// </summary>
		/// <param name="norm">The norm.</param>
		void setNorm(FloatType norm) { m_norm = norm; }

		/// <summary>
		/// Gets the norm.
		/// </summary>
		/// <returns></returns>
		FloatType getNorm() const { return m_norm; }

		/// <summary>
		/// Rotates with the specified angle.
		/// </summary>
		/// <param name="angle">The angle.</param>
		/// <returns></returns>
		PolarCoordinates rotate(float angle) const { return PolarCoordinates(m_angle + angle, m_norm); }

		/// <summary>
		/// Scales the norm with the specified value.
		/// </summary>
		/// <param name="value">The value.</param>
		/// <returns></returns>
		PolarCoordinates scale(float value) const { return PolarCoordinates(m_angle, m_norm*value); }

		PolarCoordinates addNorm(float value) const { return PolarCoordinates(m_angle, m_norm + value); }

		/// <summary>
		/// Converts the polar coordinates into a vector
		/// </summary>
		/// <returns>
		/// The vector
		/// </returns>
		operator Vector2 () { return Math::makeVector(FloatType(cos(m_angle)), FloatType(sin(m_angle)))*m_norm; }

		/// <summary>
		/// Gets the vector associated with the polar coordinates.
		/// </summary>
		/// <returns></returns>
		Vector2 getVector() const { return (Vector2)(*this); }
	};
}
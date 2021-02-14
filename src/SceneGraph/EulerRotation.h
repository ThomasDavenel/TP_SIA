#ifndef _SceneGraph_EulerRotation_H
#define _SceneGraph_EulerRotation_H

#include <SceneGraph/Transform.h>
#include <Math/Constant.h>

namespace SceneGraph
{
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \class	EulerRotation
	///
	/// \brief	Euler rotation.
	///
	/// \author	F. Lamarche, Université de Rennes 1
	/// \date	17/02/2016
	////////////////////////////////////////////////////////////////////////////////////////////////////
	class EulerRotation : public Transform
	{
	protected:
		/// \brief	The angle around X axis.
		float m_angleX ;
		/// \brief	The angle around Y axis.
		float m_angleY ;
		/// \brief	The angle around Z axis.
		float m_angleZ ;

	public:

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	EulerRotation::EulerRotation(float angleX, float angleY, float angleZ)
		///
		/// \brief	Constructor.
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	17/02/2016
		///
		/// \param	angleX	The angle around x axis.
		/// \param	angleY	The angle around y axis.
		/// \param	angleZ	The angle around z axis.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		EulerRotation(float angleX, float angleY, float angleZ)
			: m_angleX(angleX), m_angleY(angleY), m_angleZ(angleZ)
		{}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	float EulerRotation::getAngleX() const
		///
		/// \brief	Gets the X angle.
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	17/02/2016
		///
		/// \return	The X angle.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		float getAngleX() const { return m_angleX ; }

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	float EulerRotation::getAngleY() const
		///
		/// \brief	Gets the Y angle.
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	17/02/2016
		///
		/// \return The Y angle.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		float getAngleY() const { return m_angleY ; }

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	float EulerRotation::getAngleZ() const
		///
		/// \brief	Gets the Z angle.
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	17/02/2016
		///
		/// \return	The angle z coordinate.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		float getAngleZ() const { return m_angleZ ; }

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	void EulerRotation::setAngleX(float value)
		///
		/// \brief	Sets X angle.
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	17/02/2016
		///
		/// \param	value The value.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		void setAngleX(float value) { m_angleX = value ; }

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	void EulerRotation::setAngleY(float value)
		///
		/// \brief	Sets Y angle.
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	17/02/2016
		///
		/// \param	value	The value.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		void setAngleY(float value) { m_angleY = value ; }

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	void EulerRotation::setAngleZ(float value)
		///
		/// \brief	Sets Z angle.
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	17/02/2016
		///
		/// \param	value	The value.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		void setAngleZ(float value) { m_angleZ = value ; }

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	virtual void EulerRotation::draw()
		///
		/// \brief	Draws this node.
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	17/02/2016
		////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual void draw()
		{
			storeMatrix() ;
			glRotatef((float)(m_angleX*180.0/Math::pi), 1.0f, 0.0f, 0.0f) ;
			glRotatef((float)(m_angleY*180.0/Math::pi), 0.0f, 1.0f, 0.0f) ;
			glRotatef((float)(m_angleZ*180.0/Math::pi), 0.0f, 0.0f, 1.0f) ;
			Group::draw() ;
			restoreMatrix() ;
		}
	};
}

#endif
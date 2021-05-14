#pragma once

#include <stdext/hash_combine.h>
#include <HelperGl/Color.h>
#include <HelperGl/Material.h>
#include <functional>

namespace std
{
	/// <summary>
	/// Hash function for HelperGl::Color
	/// </summary>
	template <>
	struct hash<HelperGl::Color>
	{
		size_t operator() (const HelperGl::Color & color) const
		{
			const GLfloat * tmp = color.getBuffer();
			return stdext::hash_combine(tmp[0], tmp[1], tmp[2], tmp[3]);
		}
	};

	/// <summary>
	/// Equal to operator for HelperGl::Color
	/// </summary>
	template <>
	struct equal_to<HelperGl::Color>
	{
		bool operator() (const HelperGl::Color & color1, const HelperGl::Color & color2) const
		{
			for (size_t cpt = 0; cpt < 4; ++cpt)
			{
				if (color1.getBuffer()[cpt] != color2.getBuffer()[cpt]) { return false; }
			}
			return true;
		}
	};

	/// <summary>
	/// Hash function for HelperGl::Material
	/// </summary>
	template <>
	struct hash<HelperGl::Material>
	{
		size_t operator() (const HelperGl::Material & material) const
		{
			return stdext::hash_combine(material.getAmbiant(), material.getDiffuse(), material.getEmission(), material.getSpecular(), material.getShininess());
		}
	};

	/// <summary>
	/// Equal to function for HelperGl::Material
	/// </summary>
	template <>
	struct equal_to<HelperGl::Material>
	{
		bool operator() (const HelperGl::Material & mat1, const HelperGl::Material & mat2) const
		{
			return std::equal_to<HelperGl::Color>()(mat1.getAmbiant(), mat2.getAmbiant()) &&
				std::equal_to<HelperGl::Color>()(mat1.getDiffuse(), mat2.getDiffuse()) &&
				std::equal_to<HelperGl::Color>()(mat1.getSpecular(), mat2.getSpecular()) &&
				std::equal_to<HelperGl::Color>()(mat1.getEmission(), mat2.getEmission()) &&
				mat1.getShininess() == mat2.getShininess();
		}
	};

}
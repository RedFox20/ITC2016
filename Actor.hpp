#pragma once
#include "StaticMesh.hpp"

namespace itc
{
	////////////////////////////////////////////////////////////////////////////

	class Actor
	{
	public:
		vec3 Position;
		vec3 Rotation;
		vec3 Scale;

		unique_ptr<BMDModel> Model;
		sf::Texture Texture;

	public:
		Actor();
		~Actor();

		void affineTransform(mat4& outModelViewProj, const mat4& viewProj) const;

		void draw(Shader& shader, const mat4& viewProj) const;
	};

	////////////////////////////////////////////////////////////////////////////
}

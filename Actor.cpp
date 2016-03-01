#include "Actor.hpp"

namespace itc
{
	////////////////////////////////////////////////////////////////////////////

	Actor::Actor()
	{
	}

	Actor::~Actor()
	{
	}

	void Actor::affineTransform(mat4& outModelViewProj, const mat4& viewProj) const
	{
		mat4 affine;
		mat4::from_position(affine, Position);
		affine.scale(Scale);
		affine.multiply(mat4::from_rotation(mat4{}, Rotation));
		outModelViewProj = viewProj;
		outModelViewProj.multiply(affine);	}

	void Actor::draw(Shader& shader, const mat4& viewProj) const
	{
		mat4 modelViewProj;
		affineTransform(modelViewProj, viewProj);

		shader.bind(u_Transform, modelViewProj);
		shader.bind(u_DiffuseTex, Texture);

		
	}

	////////////////////////////////////////////////////////////////////////////
}



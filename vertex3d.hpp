#pragma once
#include "GLEW/glew.h"
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include "types3d.hpp"

namespace itc
{
	////////////////////////////////////////////////////////////////////////////////
	using namespace std;

	struct vertex3d
	{
		vec3 pos;
		vec2 tex;
		vec3 norm;
	};

	
	////////////////////////////////////////////////////////////////////////////////

	/** @brief shader uniform slots */
	typedef enum ShaderUniform
	{
		u_Transform,    // uniform matrix transform;      model-view-project matrix
		u_DiffuseTex,   // uniform sampler2D diffuseTex;  diffuse texture
		u_SpecularTex,  // uniform sampler2D specularTex; specular texture
		u_NormalTex,    // uniform sampler2D normalTex;   normal texture
		u_ShadowTex,    // uniform sampler2D shadowTex;   shadow texture
		u_OccludeTex,   // uniform sampler2D occludeTex;  occlusion texture for fake SSAO
		u_DiffuseColor, // uniform vec4 diffuseColor;     diffuse color 
		u_OutlineColor, // uniform vec4 outlineColor;     background or outline color
		u_MaxUniforms,  // uniform counter
	} ShaderUniform;


	/** @brief shader attribute slots */
	typedef enum ShaderAttr
	{
		a_Position,      // attribute vec3 position;  position (vec2 XY or vec3 XYZ)
		a_Normal,        // attribute vec3 normal;    normal 
		a_Coord,         // attribute vec2 coord;     texture coordinate 0
		a_Coord2,        // attribute vec2 coord2;    texture coordinate 1
		a_Vertex,        // attribute vec4 vertex;    additional generic 4D vertex
		a_Color,         // attribute vec4 color;     per-vertex coloring
		a_MaxAttributes, // attribute counter
	} ShaderAttr;

	////////////////////////////////////////////////////////////////////////////////

	struct Vertex3dBuffer
	{
		enum { a_Pos = a_Position, a_Tex = a_Coord, a_Norm = a_Normal, };
		GLuint arrayObj;    // vertex array object
		GLuint vertexBuf;   // vertex buffer
		GLuint indexBuf;    // element buffer
		GLuint vertexCount; // # of verts
		GLuint indexCount;  // # of indices

		Vertex3dBuffer();
		~Vertex3dBuffer();
		void create(const vertex3d* verts, int numVerts,
					const index_t* indices, int numIndices);
		void draw();
	};


	////////////////////////////////////////////////////////////////////////////////

	struct Shader
	{
		GLuint program;    // linked glProgram
		char vs_path[120]; // vert shader path
		char fs_path[120]; // frag shader path
		time_t vs_mod;     // last modified time of vert shader file
		time_t fs_mod;     // last modified time of frag shader file
		char uniforms[u_MaxUniforms];     // uniform locations
		bool attributes[a_MaxAttributes]; // attribute present? true/false

		Shader();

		/** @brief Loads shader from {shaderName}.frag and {shaderName}.vert */
		bool loadShader(const string& shaderName);
		bool reload();
		bool hotload();

		void loadUniforms();
		void bind();
		void unbind();
		void bind(ShaderUniform u_uniformSlot, const mat4& matrix);
		void bind(ShaderUniform u_uniformSlot, int glTex2DSlot, unsigned glTexture);
		void bind(ShaderUniform u_uniformSlot, int glTex2DSlot, const sf::Texture& texture);
		void bind(ShaderUniform u_uniformSlot, const vec2& value);
		void bind(ShaderUniform u_uniformSlot, const vec3& value);
		void bind(ShaderUniform u_uniformSlot, const vec4& value);
	};
}

////////////////////////////////////////////////////////////////////////////////
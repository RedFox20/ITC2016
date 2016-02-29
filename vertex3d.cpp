#include "vertex3d.hpp"
#include <cstring>
#include <sys/stat.h>

namespace itc
{
	////////////////////////////////////////////////////////////////////////////////

	Vertex3dBuffer::Vertex3dBuffer() 
		: arrayObj(0), vertexBuf(0), indexBuf(0), vertexCount(0), indexCount(0)
	{
	}
	Vertex3dBuffer::~Vertex3dBuffer()
	{
		if (vertexBuf) glDeleteBuffers(1, &vertexBuf);
		if (indexBuf)  glDeleteBuffers(1, &indexBuf);
		if (arrayObj)  glDeleteVertexArrays(1, &arrayObj);
	}
	void Vertex3dBuffer::create(const vertex3d* vertices, int numVertices, 
								const index_t* indices, int numIndices)
	{
		vertexCount = numVertices;
		indexCount  = numIndices;

		glGenVertexArrays(1, &arrayObj);
		glBindVertexArray(arrayObj);     // bind VAO to start recording
		{
			// create and fill index buffer
			glGenBuffers(1, &indexBuf);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuf);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(index_t), indices, GL_STATIC_DRAW);
			// create & fill vertex buffer
			glGenBuffers(1, &vertexBuf);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
			glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(vertex3d), vertices, GL_STATIC_DRAW);
			// set VAO vertex attributes
			glVertexAttribPointer(a_Pos, 3, GL_FLOAT, 0, sizeof(vertex3d), (void*)0);
			glEnableVertexAttribArray(a_Pos);
			glVertexAttribPointer(a_Tex, 2, GL_FLOAT, 0, sizeof(vertex3d), (void*)(3*sizeof(float)));
			glEnableVertexAttribArray(a_Tex);
			glVertexAttribPointer(a_Norm, 3, GL_FLOAT, 0, sizeof(vertex3d), (void*)(5*sizeof(float)));
			glEnableVertexAttribArray(a_Norm);
		}
		glBindVertexArray(0);
	}
	void Vertex3dBuffer::draw()
	{
		glBindVertexArray(arrayObj);
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	////////////////////////////////////////////////////////////////////////////////

	static const char* UniformMap[] = {
		"transform",     // u_Transform
		"diffuseTex",    // u_DiffuseTex
		"specularTex",   // u_SpecularTex
		"normalTex",     // u_NormalTex
		"shadowTex",     // u_ShadowTex
		"occludeTex",    // u_OccludeTex
		"diffuseColor",  // u_DiffuseColor
		"outlineColor",  // u_OutlineColor
	};
	static const char* AttributeMap[] = {
		"position",      // a_Position
		"normal",        // a_Normal
		"coord",         // a_Coord
		"coord2",        // a_Coord2
		"vertex",        // a_Vertex
		"color",         // a_Color
	};

	static const char* uniform_name(ShaderUniform uniformSlot) {
		if (0 <= uniformSlot && uniformSlot < u_MaxUniforms)
			return UniformMap[uniformSlot];
		return "u_invalid";
	}

	#if DEBUG
	static void checkShaderLog(int program)
	{
		int logLength = 0;
		GLboolean prog = glIsProgram(program);
		if (prog) glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
		else      glGetShaderiv(program,  GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 0) {
			char* log = (char*)alloca(logLength);
			if (prog) glGetProgramInfoLog(program, logLength, &logLength, log);
			else      glGetShaderInfoLog(program,  logLength, &logLength, log);
			fprintf(stderr, "%s\n", log);
		}
	}
	#else
	#define checkShaderLog(x) /*do nothing*/
	#endif
	static GLuint compileShader(const char* shMem, int size, const char* idstr, GLenum type)
	{
		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &shMem, &size);
		glCompileShader(shader);
		checkShaderLog(shader); // this can be a warning
		int status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (!status) {
			fprintf(stderr, "shader_load(): failed to compile '%s'\n", idstr);
			glDeleteShader(shader);
			return 0;
		}
		return shader;
	}
	static GLuint compileShaderFile(const char* shFile, time_t* modified, GLenum type)
	{
		FILE* f = fopen(shFile, "rb");
		if (!f) {
			fprintf(stderr, "shader_load(): failed to load file '%s'\n", shFile);
			return 0;
		}

		struct stat s; fstat(fileno(f), &s);
		*modified   = s.st_mtime;
		size_t size = s.st_size;

		char* shMem = (char*)(size <= 65536 ? alloca(size) : malloc(size));
		fread(shMem, size, 1, f);
		fclose(f);

		GLuint shader = compileShader(shMem, (int)size, shFile, type);
		if (size > 65536) free(shMem);
		return shader;
	}

	static time_t time_modified(const char* file) {
		struct stat st;
		return stat(file, &st) == 0 ? st.st_mtime : 0;
	}

	////////////////////////////////////////////////////////////////////////////////

	Shader::Shader() : program(0), vs_mod(0), fs_mod(0)
	{
		vs_path[0] = fs_path[0] = '\0';
	}

	bool Shader::loadShader(const string & shaderName)
	{
		snprintf(vs_path, sizeof(vs_path), "%s.vert", shaderName.data());
		snprintf(fs_path, sizeof(fs_path), "%s.frag", shaderName.data());
		vs_mod = 0;
		fs_mod = 0;
		memset(uniforms,   -1,    sizeof(uniforms));
		memset(attributes, false, sizeof(attributes));
		return reload();
	}

	bool Shader::reload()
	{
		GLuint vs = compileShaderFile(vs_path, &vs_mod, GL_VERTEX_SHADER);
		GLuint fs = compileShaderFile(fs_path, &fs_mod, GL_FRAGMENT_SHADER);

		int status = vs && fs; // ok
		if (status) {
			glDeleteProgram(this->program);
			GLuint program = this->program = glCreateProgram();
			glAttachShader(program, vs);
			glAttachShader(program, fs);

			// bind our hardcoded attribute locations:
			for (int i = 0; i < a_MaxAttributes; ++i)
				glBindAttribLocation(program, i, AttributeMap[i]);

			glLinkProgram(program);
			glValidateProgram(program);
			checkShaderLog(program); // this can be a warning
			glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
			if (status) loadUniforms();
			else fprintf(stderr, "shader::reload(%s/.frag) failed\n", vs_path);
		}
		glDeleteShader(vs);
		glDeleteShader(fs);
		return !!status;
	}

	bool Shader::hotload()
	{
		if (time_modified(vs_path) != vs_mod) return reload();
		if (time_modified(fs_path) != fs_mod) return reload();
		return false;
	}

	void Shader::loadUniforms()
	{
		// brute force load all supported uniform and attribute locations
		int numActive;
		glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numActive);
		for (int i = 0; numActive && i < u_MaxUniforms; ++i) {
			int loc = glGetUniformLocation(program, UniformMap[i]);
			if (loc != -1) --numActive;
			uniforms[i] = (char)loc; // always write result (incase of shader reload)
		}
		glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &numActive);
		for (int i = 0; i < a_MaxAttributes; ++i) {
			int loc = glGetAttribLocation(program, AttributeMap[i]);
			attributes[i] = loc != -1; // always write result (incase of shader reload)
		}
		numActive = 0;
	}

	void Shader::bind()
	{
		glUseProgram(program);
	}

	void Shader::unbind()
	{
		glUseProgram(0);
	}


	static void check_uniform(const Shader& s, const char* where, ShaderUniform u_uniformSlot) {
		if (s.uniforms[u_uniformSlot] == -1)
			fprintf(stderr, "%s: uniform '%s' not found", where, uniform_name(u_uniformSlot));
	}
	void Shader::bind(ShaderUniform u_uniformSlot, const mat4& matrix)
	{
		check_uniform(*this, "shader_bind_mat()", u_uniformSlot);
		glUniformMatrix4fv(uniforms[u_uniformSlot], 1, GL_FALSE, matrix.m);
	}
	void Shader::bind(ShaderUniform u_uniformSlot, int glTex2DSlot, unsigned glTexture)
	{
		check_uniform(*this, "shader_bind_tex()", u_uniformSlot);
		glBindTexture(GL_TEXTURE_2D + glTex2DSlot, glTexture);
		glUniform1i(uniforms[u_DiffuseTex], glTex2DSlot); // GL_TEXTURE_2D [glTex2DSlot]
	}
	void Shader::bind(ShaderUniform u_uniformSlot, int glTex2DSlot, const sf::Texture& texture)
	{
		return bind(u_uniformSlot, glTex2DSlot, texture.getNativeHandle());
	}
	void Shader::bind(ShaderUniform u_uniformSlot, const vec2& value)
	{
		check_uniform(*this, "shader_bind_vec2()", u_uniformSlot);
		glUniform2fv(u_uniformSlot, 1, &value.x);
	}
	void Shader::bind(ShaderUniform u_uniformSlot, const vec3& value)
	{
		check_uniform(*this, "shader_bind_vec3()", u_uniformSlot);
		glUniform3fv(u_uniformSlot, 1, &value.x);
	}
	void Shader::bind(ShaderUniform u_uniformSlot, const vec4& value)
	{
		check_uniform(*this, "shader_bind_vec4()", u_uniformSlot);
		glUniform4fv(u_uniformSlot, 1, &value.x);
	}

	////////////////////////////////////////////////////////////////////////////////
}


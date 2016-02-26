#pragma once
#define _USE_MATH_DEFINES
#include <cmath>

namespace itc
{
	////////////////////////////////////////////////////////////////////////////////

	float radf(float degrees);

	////////////////////////////////////////////////////////////////////////////////

	typedef unsigned int index_t; // vertex index type 

	// 2D position vector - typical UV container or 2D coordinate
	struct vec2
	{
		static const vec2 ZERO; // Represents vec2 {0,0}
		float x, y;

		float len()   const { return sqrtf(x*x + y*y); }
		float sqlen() const { return x*x + y*y; }
		vec2& normalize() {
			float sqr = x*x + y*y;
			float inv = 1.0f / sqrtf(sqr);
			x *= inv, y *= inv;
			return *this;
		}
		vec2 normalized() const {
			float sqr = x*x + y*y;
			float inv = 1.0f / sqrtf(sqr);
			return vec2 { x*inv, y*inv };
		}
	}; 

	////////////////////////////////////////////////////////////////////////////////

	// 3D position vector - typical XYZ container
	struct vec3
	{
		static const vec3 ZERO;  // Represents vec3 {0,0,0}
		static const vec3 XAXIS; // Represents vec3 {1,0,0}
		static const vec3 YAXIS; // Represents vec3 {0,1,0}
		static const vec3 ZAXIS; // Represents vec3 {0,0,1}
		float x, y, z;

		vec3() {}
		vec3(float x, float y, float z) : x(x), y(y), z(z) {}
		vec3 cross(const vec3& b) const { return vec3(y*b.z - b.y*z, z*b.x - b.z*x, x*b.y - b.x*y); }
		float dot(const vec3& b) const { return x*b.x + y*b.y + z*b.z; }
		float len(vec3 v)   const { return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z); }
		float sqlen(vec3 v) const { return v.x*v.x + v.y*v.y + v.z*v.z; }
		vec3& normalize() {
			float sqr = x*x + y*y + z*z;
			float inv = 1.0f / sqrtf(sqr);
			x*=inv, y*=inv, z*=inv;
			return *this;
		}
		vec3 normalized() const {
			float sqr = x*x + y*y + z*z;
			float inv = 1.0f / sqrtf(sqr);
			return vec3(x*inv, y*inv, z*inv);
		}
	}; 

	inline vec3 operator+(const vec3& a, const vec3& b) { return vec3(a.x+b.x, a.y+b.y, a.z+b.z); }
	inline vec3 operator-(const vec3& a, const vec3& b) { return vec3(a.x-b.x, a.y-b.y, a.z-b.z); }
	inline vec3 operator*(const vec3& a, const vec3& b) { return vec3(a.x*b.x, a.y*b.y, a.z*b.z); }
	inline vec3 operator/(const vec3& a, const vec3& b) { return vec3(a.x/b.x, a.y/b.y, a.z/b.z); }
	inline vec3 operator+(const vec3& a, float v) { return vec3(a.x+v, a.y+v, a.z+v); }
	inline vec3 operator-(const vec3& a, float v) { return vec3(a.x-v, a.y-v, a.z-v); }
	inline vec3 operator*(const vec3& a, float v) { return vec3(a.x*v, a.y*v, a.z*v); }
	inline vec3 operator/(const vec3& a, float v) { return vec3(a.x/v, a.y/v, a.z/v); }

	////////////////////////////////////////////////////////////////////////////////

	// 4D float vector - used for Quaternions and RGBA colors
	struct vec4
	{
		static const vec4 ZERO; // Represents vec4 {0,0,0,0}
		float x, y, z, w;

		void set(float X, float Y, float Z, float W) { x=X,y=Y,z=Z,w=W; }


	};

	inline vec4 operator+(const vec4& a, const vec4& b) { return vec4{a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w}; }
	inline vec4 operator-(const vec4& a, const vec4& b) { return vec4{a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w}; }
	inline vec4 operator*(const vec4& a, const vec4& b) { return vec4{a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w}; }
	inline vec4 operator/(const vec4& a, const vec4& b) { return vec4{a.x/b.x, a.y/b.y, a.z/b.z, a.w/b.w}; }
	inline vec4 operator+(const vec4& a, float v) { return vec4{a.x+v, a.y+v, a.z+v, a.w+v}; }
	inline vec4 operator-(const vec4& a, float v) { return vec4{a.x-v, a.y-v, a.z-v, a.w-v}; }
	inline vec4 operator*(const vec4& a, float v) { return vec4{a.x*v, a.y*v, a.z*v, a.w*v}; }
	inline vec4 operator/(const vec4& a, float v) { return vec4{a.x/v, a.y/v, a.z/v, a.w/v}; }

	////////////////////////////////////////////////////////////////////////////////


	// creates a quaternion rotation from an euler angle (degrees), rotation axis must be specified
	vec4 quat_angle_axis(float angle, const vec3& axis);

	// creates a quaternion rotation from an euler angle (degrees), rotation axis must be specified
	vec4 quat_angle_axis(float angle, float x, float y, float z);

	// creates a quaternion rotation from euler XYZ (degrees) rotation 
	vec4 quat_from_rotation(const vec3& rotation);

	// rotates quaternion p with extra rotation p
	// q = additional rotation we wish to rotate with
	// p = original rotation
	vec4 quat_mul(const vec4& q, const vec4& p);

	////////////////////////////////////////////////////////////////////////////////
	// A 4x4 matrix for affine transformations

	typedef struct _mat4_row_vis
	{
		float x,y,z,w;
	} _mat4_row_vis;

	struct mat4
	{
		union {
			struct {
				_mat4_row_vis vis0, vis1, vis2, vis3;
			};
			struct {
				float m00, m01, m02, m03; // row0  0-3
				float m10, m11, m12, m13; // row1  0-3
				float m20, m21, m22, m23; // row2  0-3
				float m30, m31, m32, m33; // row3  0-3
			};
			float m[16]; // 4x4 float matrix
			struct {
				vec4 r0, r1, r2, r3; // rows 0-3
			};
			vec4 r[4]; // rows 0-3
		};

		mat4() {}
		mat4(float m00, float m01, float m02, float m03, 
			 float m10, float m11, float m12, float m13,
			 float m20, float m21, float m22, float m23,
			 float m30, float m31, float m32, float m33)
			: m00(m00), m01(m01), m02(m02), m03(m03),
			  m10(m10), m11(m11), m12(m12), m13(m13),
			  m20(m20), m21(m21), m22(m22), m23(m23),
			  m30(m30), m31(m31), m32(m32), m33(m33) {}

		// initializes a new identity matrix
		mat4& identity();

		// multiplies two matrices:  ma = ma * mb
		mat4& multiply(const mat4& m);

		// transforms 3D vector v with this matrix and return the resulting vec4
		vec4 multiply(const vec3& v) const;

		// transforms 4D vector v with this matrix and returns the resulting vec4
		vec4 multiply(const vec4& v) const;

		// translates object transformation matrix by given offset
		mat4& translate(const vec3& offset);

		// rotates an object transformation matrix by given degree angle around a given axis
		mat4& rotate(float angleDegs, const vec3& axis);

		// rotates an object transformation matrix by given degree angle around a given axis
		mat4& rotate(float angleDegs, float x, float y, float z);

		// scales an object transformation matrix by a given scale factor
		mat4& scale(const vec3& scale);

		// creates an ortographic projection matrix
		mat4& ortho(float left, float right, float bottom, float top);

		// creates a perspective projection matrix
		mat4& perspective(float fov, float width, float height, float zNear, float zFar);

		// creates a lookat view/camera matrix
		mat4& lookat(const vec3& eye, const vec3& center, const vec3& up);

		// creates a translated matrix from XYZ position
		static mat4& from_position(mat4& out, const vec3& position);

		// creates a rotated matrix from euler XYZ rotation
		static mat4& from_rotation(mat4& out, const vec3& rotation);

		// creates a scaled matrix from XYZ scale
		static mat4& from_scale(mat4& out, const vec3& scale);
	};

	// global identity matrix for easy initialization
	extern const struct mat4 IDENTITY;

	////////////////////////////////////////////////////////////////////////////////
}

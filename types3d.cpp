#include "types3d.hpp"

namespace itc
{
	const vec2 vec2::ZERO = { 0.0f, 0.0f };
	const vec3 vec3::ZERO = { 0.0f, 0.0f, 0.0f };
	const vec4 vec4::ZERO = { 0.0f, 0.0f, 0.0f, 0.0f };
	const vec3 vec3::XAXIS; // Represents vec3 {1,0,0}
	const vec3 vec3::YAXIS; // Represents vec3 {0,1,0}
	const vec3 vec3::ZAXIS; // Represents vec3 {0,0,1}

	////////////////////////////////////////////////////////////////////////////////

	float radf(float degrees) {
		return (degrees * (float)M_PI) / 180.0f; // rads=(degs*PI)/180
	}

	// creates a quaternion rotation from an euler angle (degrees), rotation axis must be specified
	vec4 quat_angle_axis(float angle, const vec3& axis)
	{
		const float r = radf(angle) * 0.5f;
		const float s = sinf(r);
		return vec4{axis.x * s, axis.y * s, axis.z * s, cosf(r)};
	}

	// creates a quaternion rotation from an euler angle (degrees), rotation axis must be specified
	vec4 quat_angle_axis(float angle, float x, float y, float z)
	{
		return quat_angle_axis(angle, {x,y,z});
	}

	// creates a quaternion rotation from euler XYZ (degrees) rotation 
	vec4 quat_from_rotation(const vec3& rotation)
	{
		vec4 q = quat_angle_axis(rotation.x, 1.0f, 0.0f, 0.0f);
		     q = quat_mul(quat_angle_axis(rotation.y, 0.0f, 1.0f, 0.0f), q);
		return   quat_mul(quat_angle_axis(rotation.z, 0.0f, 0.0f, 1.0f), q);
	}

	// rotates quaternion p with extra rotation p
	// q = additional rotation we wish to rotate with
	// p = original rotation
	vec4 quat_mul(const vec4& q, const vec4& p)
	{
		return vec4{
			q.w*p.w - q.x*p.x - q.y*p.y - q.z*p.z,
			q.w*p.x + q.x*p.w + q.y*p.z - q.z*p.y,
			q.w*p.y + q.y*p.w + q.z*p.x - q.x*p.z,
			q.w*p.z + q.z*p.w + q.x*p.y - q.y*p.x
		};
	}

	////////////////////////////////////////////////////////////////////////////////

	const mat4 IDENTITY = {
		1, 0, 0, 0, 
		0, 1, 0, 0, 
		0, 0, 1, 0, 
		0, 0, 0, 1,
	};

	mat4& mat4::identity()
	{
		return (*this = IDENTITY);
	}

	mat4& mat4::multiply(const mat4& mb)
	{
		const vec4 a0 = r0;
		const vec4 a1 = r1;
		const vec4 a2 = r2;
		const vec4 a3 = r3;
		const vec4 b0 = mb.r0;
		const vec4 b1 = mb.r1;
		const vec4 b2 = mb.r2;
		const vec4 b3 = mb.r3;
		r0 = (a0*b0.x + a1*b0.y) + (a2*b0.z + a3*b0.w);
		r1 = (a0*b1.x + a1*b1.y) + (a2*b1.z + a3*b1.w);
		r2 = (a0*b2.x + a1*b2.y) + (a2*b2.z + a3*b2.w);
		r3 = (a0*b3.x + a1*b3.y) + (a2*b3.z + a3*b3.w);
		return *this;
	}

	vec4 mat4::multiply(const vec3& v) const
	{
		return vec4{
			(m00*v.x) + (m10*v.y) + (m20*v.z) + m30,
			(m01*v.x) + (m11*v.y) + (m21*v.z) + m31,
			(m02*v.x) + (m12*v.y) + (m22*v.z) + m32,
			(m03*v.x) + (m13*v.y) + (m23*v.z) + m33
		};
	}
	vec4 mat4::multiply(const vec4& v) const
	{
		return vec4{
			(m00*v.x) + (m10*v.y) + (m20*v.z) + (m30*v.w),
			(m01*v.x) + (m11*v.y) + (m21*v.z) + (m31*v.w),
			(m02*v.x) + (m12*v.y) + (m22*v.z) + (m32*v.w),
			(m03*v.x) + (m13*v.y) + (m23*v.z) + (m33*v.w)
		};
	}

	mat4& mat4::translate(const vec3& offset)
	{
		r3 = multiply(offset);
		return *this;
	}

	static vec4 _mat4_rotrow(mat4& m, const vec3& rot)
	{
		return (m.r0 * rot.x) + (m.r1 * rot.y) + (m.r2 * rot.z);
	}
	mat4& mat4::rotate(float angleDegs, const vec3& rotationAxis)
	{
		float rad = radf(angleDegs);
		float c = cosf(rad);
		vec3 axis  = (rotationAxis).normalized();
		vec3 temp  = axis * (1.0f - c);
		vec3 sAxis = axis * sinf(rad);
		vec3 r0 = { c + temp.x*axis.x, 
						temp.x*axis.y + sAxis.z,
						temp.x*axis.z - sAxis.y, };
		vec3 r1 = {     temp.y*axis.x - sAxis.z,
					c + temp.y*axis.y, 
						temp.y*axis.z + sAxis.x, };
		vec3 r2 = {     temp.z*axis.x + sAxis.y,
						temp.z*axis.y - sAxis.x, 
					c + temp.z*axis.z };
		this->r0 = _mat4_rotrow(*this, r0);
		this->r1 = _mat4_rotrow(*this, r1);
		this->r2 = _mat4_rotrow(*this, r2);
		return *this;
	}
	mat4& mat4::rotate(float angleDegs, float x, float y, float z)
	{
		return rotate(angleDegs, {x,y,z});
	}

	mat4& mat4::scale(const vec3& scale)
	{
		m00 *= scale.x;
		m11 *= scale.y;
		m22 *= scale.z;
		return *this;
	}

	mat4& mat4::ortho(float left, float right, float bottom, float top)
	{
		const float rl = right - left;
		const float tb = top - bottom;
		m00 = 2.0f / rl, m01 = 0.0f, m02 = 0.0f,  m03 = 0.0f;
		m10 = 0.0f, m11 = 2.0f / tb, m12 = 0.0f,  m13 = 0.0f;
		m20 = 0.0f, m21 = 0.0f,      m22 = -1.0f, m23 = 0.0f;
		m30 = -(right+left)/rl, m31 = -(top+bottom)/tb, m32 = 0.0f, m33 = 1.0f;
		return *this;
	}

	mat4& mat4::perspective(float fov, float width, float height, float zNear, float zFar)
	{
		const float rad2 = radf(fov) * 0.5f;
		const float h = cosf(rad2) / sinf(rad2);
		const float w = (h * height) / width;
		const float range = zFar - zNear;
		m00 = w, m01 = 0, m02 = 0, m03 = 0;
		m10 = 0, m11 = h, m12 = 0, m13 = 0;
		m20 = 0, m21 = 0, m22 = -(zFar + zNear) / range, m23 = -1;
		m30 = 0, m31 = 0, m32 = (-2.0f * zFar * zNear) / range, m33 = 1;
		return *this;
	}

	mat4& mat4::lookat(const vec3& eye, const vec3& center, const vec3& up) // create a lookat mat4
	{
		const vec3 f = (center - eye).normalized();
		const vec3 s = f.cross(up.normalized()).normalized();
		const vec3 u = s.cross(f);
		m00 = s.x, m01 = u.x, m02 = -f.x, m03 = 0.0f;
		m10 = s.y, m11 = u.y, m12 = -f.y, m13 = 0.0f;
		m20 = s.z, m21 = u.z, m22 = -f.z, m23 = 0.0f;
		m30 = -s.dot(eye), m31 = -u.dot(eye), m32 = f.dot(eye), m33 = 1.0f;
		return *this;
	}

	// creates a translated matrix from XYZ position
	mat4& mat4::from_position(mat4& m, const vec3& pos)
	{
		return m.identity().translate(pos);
	}

	mat4& mat4::from_rotation(mat4& m, const vec3& rotation)
	{
		vec4 q = quat_from_rotation(rotation);
		m.m00 = 1 - 2 * q.y * q.y - 2 * q.z * q.z;
		m.m01 = 2 * q.x * q.y + 2 * q.w * q.z;
		m.m02 = 2 * q.x * q.z - 2 * q.w * q.y;
		m.m03 = 0.0f;
		m.m10 = 2 * q.x * q.y - 2 * q.w * q.z;
		m.m11 = 1 - 2 * q.x * q.x - 2 * q.z * q.z;
		m.m12 = 2 * q.y * q.z + 2 * q.w * q.x;
		m.m13 = 0.0f;
		m.m20 = 2 * q.x * q.z + 2 * q.w * q.y;
		m.m21 = 2 * q.y * q.z - 2 * q.w * q.x;
		m.m22 = 1 - 2 * q.x * q.x - 2 * q.y * q.y;
		m.m23 = 0.0f;
		m.m30 = 0.0f;
		m.m31 = 0.0f;
		m.m32 = 0.0f;
		m.m33 = 1.0f;
		return m;
	}

	mat4& mat4::from_scale(mat4& m, const vec3& sc)
	{
		m.m00 = sc.x, m.m01 = 0.0f, m.m02 = 0.0f, m.m03 = 0.0f;
		m.m10 = 0.0f, m.m11 = sc.y, m.m12 = 0.0f, m.m13 = 0.0f;
		m.m20 = 0.0f, m.m21 = 0.0f, m.m22 = sc.z, m.m23 = 0.0f;
		m.m30 = 0.0f, m.m31 = 0.0f, m.m32 = 0.0f, m.m33 = 1.0f;
		return m;
	}


}
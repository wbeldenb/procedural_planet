#include "apFloat/apfloat.h"
#include "apfloat/apint.h"
#include "apfloat/ap.h"
#include "apfloat/apcplx.h"
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

vec3 toSpherical(vec3 xyz) {
	apfloat r = sqrt(pow(xyz.x, 2) + pow(xyz.y, 2) + pow(xyz.z, 2));
	apfloat theta = acos(xyz.z / r);
	apfloat phi = atan2(xyz.y, xyz.x);
	
	return vec3(r, theta, phi);
}

vec3 toCartesian(vec3 rtp) {
	apfloat x = rtp.x * sin(rtp.y) * cos(rtp.z);
	apfloat y = rtp.x * sin(rtp.y) * sin(rtp.z);
	apfloat z = rtp.x * sin(rtp.y);

	//mat4 m = mat4(1.f);

	return vec3(x, y, z);
}
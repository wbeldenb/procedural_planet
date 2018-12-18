#pragma once

#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "apFloat/apfloat.h"
#include "apfloat/apint.h"
#include "apfloat/ap.h"
#include "apfloat/apcplx.h"

using namespace glm;

class apvec3 {
	apfloat x, y, z;
	apfloat r, g, b;
};

class apmat4x4 {
	//values stored in matrix
	apfloat val[4][4];

	//operator overrides
	apmat4x4 operator + (const apmat4x4 other) {
		apmat4x4 temp;

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				temp.val[i][j] = val[i][j] + other.val[i][j];
			}
		}

		return temp;
	}

	apmat4x4 operator - (const apmat4x4 other) {
		apmat4x4 temp;

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				temp.val[i][j] = val[i][j] - other.val[i][j];
			}
		}

		return temp;
	}

	apmat4x4 operator * (const int other) {
		apmat4x4 temp;

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				temp.val[i][j] = val[i][j] * other;
			}
		}

		return temp;
	}

	apmat4x4 operator * (const float other) {
		apmat4x4 temp;

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				temp.val[i][j] = val[i][j] * other;
			}
		}

		return temp;
	}

	apmat4x4 operator * (const apfloat other) {
		apmat4x4 temp;

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				temp.val[i][j] = val[i][j] * other;
			}
		}

		return temp;
	}

	bool operator == (const apmat4x4 other) {
		for (int i = 0; i < 16; i++) {
			if (val[i] != other.val[i])
				return false;
		}

		return true;
	}

	bool operator != (const apmat4x4 other) {
		for (int i = 0; i < 16; i++) {
			if (val[i] != other.val[i])
				return true;
		}

		return false;
	}

	apmat4x4 operator * (const apmat4x4 other) {
		apmat4x4 temp;
		apfloat num;

		//maybe change to strassen's if this is gonna be painfully slow
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				num = apfloat(0);
				for (int k = 0; k < 4; k++) {
					num += val[i][k] * other.val[k][j];
				}

				temp.val[i][j] = num;
			}
		}

		return temp;
	}

	vec3 operator * (const vec3 v) {
		apfloat x = v.x * val[0][0] + v.y * val[0][1] + v.z * val[0][2];
		apfloat y = v.x * val[1][0] + v.y * val[1][1] + v.z * val[1][2];
		apfloat z = v.x * val[2][0] + v.y * val[2][1] + v.z * val[2][2];

		return vec3(x, y, z);
	}
};

apmat4x4 rotate_apmat4x4(vec3 offset);
apmat4x4 scale_apmat4x4(apfloat scale);
apmat4x4 translate_apmat4x4(apfloat angle, vec3 axis);
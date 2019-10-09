/*procedural planet base code
by William Belden Brown 2018-2019
with some code snippets from Ian Thomas Dunn, Zoe Wood, and Christian Eckhardt
*/

#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <math.h>
#include <algorithm>    
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>

#include "stb_image.h"
#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "Shape.h"
#include "double_algebra.h"

//namespace declarations
using namespace std;
using namespace glm;
using namespace ttmath;

//planet constant for easy changing
#define PLANET_RADIUS 1000

//calculate time offsets
double get_last_elapsed_time() {
	static double lasttime = glfwGetTime();
	double actualtime = glfwGetTime();
	double difference = actualtime - lasttime;
	lasttime = actualtime;
	return difference;
}

//camera class for fps camera with 6 degrees of movement
class camera {
private:
	double_vec_ pos;
	vec3 rot;
	bigFloat posX, posY, posZ, scale, rate;

public:
	double deltaX, deltaY, prevX, prevY;
	bool w, a, s, d, moved;
	float theta, phi;

	camera() {
		pos = double_vec_(0, 0, 0);
		rot = vec3(0.0);
		w = a = s = d = false;
		deltaX = deltaY = prevX = prevY = 0;
		theta = phi = 0;
	}

	double_vec_ getPos() {
		return pos;
	}

	vec3 getRot() {
		return rot;
	}

	mat4 process(double fTime) {
		float speedFB = 0;
		float speedLR = 0;
		float fwdspeed = 45;

		if (w)
			speedFB = fwdspeed * fTime;
		else if (s)
			speedFB = -fwdspeed * fTime;
		if (a)
			speedLR = fwdspeed * fTime;
		else if (d)
			speedLR = -fwdspeed * fTime;

		float xangle = 0;
		if (deltaX > 0)
			xangle = 0.6 * fTime;
		else if (deltaX < 0)
			xangle = -0.6 * fTime;

		float yangle = 0;
		if (deltaY > 0)
			yangle = 0.6 * fTime;
		else if (deltaY < 0)
			yangle = -0.6 * fTime;

		rot += vec3(phi, theta, 0.0);
		mat4 R = rotate(mat4(1), theta, vec3(0, 1, 0));
		mat4 Rz = rotate(mat4(1), 0.f, vec3(0, 0, 1));
		mat4 Rx = rotate(mat4(1), phi, vec3(1, 0, 0));
		vec4 dir = vec4(speedLR, 0, speedFB, 1);
		R = Rz * Rx * R;
		dir = dir * R;
		pos += double_vec_(dir.x, dir.y, dir.z);
		mat4 T = translate(mat4(1), downscale());
		return R * T;
	}

	vec3 downscale() {
		bigFloat x, y, z;
		x = bigFloat(pos.x);
		y = bigFloat(pos.y);
		z = bigFloat(pos.z);
		return glm::vec3(x.ToFloat(), y.ToFloat(), z.ToFloat());
	}
};

//global fps camera
camera myCam;

//application class to handle rendering and frame updates
class Application : public EventCallbacks {
private:
	////////////////////
	//planet constants//
	////////////////////
	const double_vec_ planetCenter = double_vec_(0, 0, -(PLANET_RADIUS + 100));
	//extreme point for LOD refactor testing
	double_vec_ extPointVertN, extPointVertS, extPointHorzE, extPointHorzW;
	//number of subdivions to great a n-gon to create planet sphere
	const int numSides = 100;
	//angle between center point of the planet for any two adjacent points on the planets equator
	const bigFloat deltaTheta = 2 * PI / numSides;
	//planet circumference
	const bigFloat planetCircum = 2 * PI * PLANET_RADIUS;
	//lengh of any side on the planet equator
	const bigFloat sideLength = planetCircum / numSides;
	const bigFloat halfSideLength = sideLength / 2;
	//factor to determine number of base points, equal to one quarter of total mesh points, numSides / 4
	int terrainNum = 2;
	//factor to determine LOD mesh simplification
	int LODnum = 1;
	//center point between planet center and the camera
	double_vec_ centerSurfacePoint;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our triangle to OpenGL
	GLuint VertexBufferID;
	GLuint IndexBufferID;
	//variables for terrain size
	int terrainWidth = terrainNum * 2;
	int terrainDepth = 100;
	float maxAltitude = 20;

	//buffers for terrain generation
	GLfloat *vertexBufferData;
	GLuint *indexBufferData;
	//globals to store buffer sizes to ensure data is transfered to GPU  correctly
	int vertexBufferSize = 0, indexBufferSize = 0;
	//GLfloat vertexBufferData[12] = { 0 };
	//GLuint indexBufferData[6] = { 0 };

	int octaves = 8;

public:
	WindowManager * windowManager = nullptr;

	//shader programs
	shared_ptr<Program> prog, heightShader, spaceShader;

	bool mouseDown = false;

	double_vec_ crossDouble(double_vec_ u, double_vec_ v) {
		bigFloat x = u.y * v.z - u.z * v.y;
		bigFloat y = u.z * v.x - u.x * v.z;
		bigFloat z = u.x * v.y - u.y * v.x;
		return double_vec_(x, y, z);
	}

	vec3 downScale(double_vec_ vec) {
		return vec3(vec.x.ToFloat(), vec.y.ToFloat(), vec.z.ToFloat());
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods) {
		double posX = 0, posY = 0, deltaX = 0, deltaY = 0;

		if (action == GLFW_PRESS)
		{
			mouseDown = true;
			glfwGetCursorPos(window, &posX, &posY);
			glfwGetCursorPos(window, &myCam.prevX, &myCam.prevY);
		}

		if (action == GLFW_RELEASE)
		{
			mouseDown = false;
		}
	}

	void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
		//glfwSetInputMode(windowManager->getHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		//calc offset from last cursor position
		if (mouseDown) {
			myCam.deltaX = xpos - myCam.prevX;
			myCam.deltaY = ypos - myCam.prevY;

			myCam.phi += float((myCam.deltaY * 0.001));
			if (myCam.phi > 0.8) {
				myCam.phi = 0.8;
			}

			if (myCam.phi < -0.8) {
				myCam.phi = -0.8;
			}

			myCam.theta += float((myCam.deltaX * 0.001));

			//update current cursor position
			myCam.prevX = xpos;
			myCam.prevY = ypos;

			//cout << xpos << endl;
			//cout << ypos << endl;
		}

		else {
			myCam.deltaX = 0;
			myCam.deltaY = 0;
		}
	}

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
		//close window
		if (key == GLFW_KEY_ESCAPE && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		//forwards backwards movement controls
		if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			myCam.w = myCam.moved = true;
			myCam.s = false;
		}
		else if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			myCam.s = myCam.moved = true;
			myCam.w = false;
		}

		//side to side movement controls
		if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			myCam.a = myCam.moved = true;
			myCam.d = false;
		}
		else if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			myCam.d = myCam.moved = true;
			myCam.a = false;
		}

		//disable movement on key release
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
			myCam.w = myCam.moved = false;
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
			myCam.s = myCam.moved = false;
		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
			myCam.a = myCam.moved = false;
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
			myCam.d = myCam.moved = false;
	}

	void resizeCallback(GLFWwindow *window, int in_width, int in_height) {
		glViewport(0, 0, in_width, in_height);
	}

	//check to see if the max distance the camera can see extends passed the most extreme point on the generated mesh
	//return 1 if the mesh needs to be shrunk, 2 if expanded, 3 if fine as is
	int checkHorizon() {
		//calculate hypotenuse of right triangle formed by camera, planet center, and farthest visible point on horizon
		//the lengths of the sides being the the altitude of the camera(h), the planet radius, and len (unknown)
		float h = distance_vec(myCam.getPos(), planetCenter).ToFloat() - PLANET_RADIUS;
		//the length from the camera to the farthest visible point on the horizon (rendered or not)
		float len = sqrt(h*(2*PLANET_RADIUS + h));

		if (distance_vec(myCam.getPos(), extPointVertN) - sideLength > bigFloat(len))
			return 1;
		
		else if (distance_vec(myCam.getPos(), extPointVertS) - sideLength > bigFloat(len))
			return 1;

		else if (distance_vec(myCam.getPos(), extPointHorzW) - sideLength > bigFloat(len))
			return 1;

		else if (distance_vec(myCam.getPos(), extPointHorzE) - sideLength > bigFloat(len))
			return 1;

		else if (distance_vec(myCam.getPos(), extPointVertN) < bigFloat(len))
			return 2;

		else if (distance_vec(myCam.getPos(), extPointVertS) < bigFloat(len))
			return 2;

		else if (distance_vec(myCam.getPos(), extPointHorzE) < bigFloat(len))
			return 2;

		else if (distance_vec(myCam.getPos(), extPointHorzW) < bigFloat(len))
			return 2;

		else
			return 3;
	}

	//downscale from bigFloat vec3 to float vec3
	vec3 downscale(double_vec_ vec) {
		bigFloat x, y, z;
		x = bigFloat(vec.x);
		y = bigFloat(vec.y);
		z = bigFloat(vec.z);
		return vec3(x.ToFloat(), y.ToFloat(), z.ToFloat());
	}

	//calculate position on planet surface centered between the camera postion and planet center
	double_vec_ calcCenterSurfacePt() {
		//store locals of globals
		const double_vec_ pos = myCam.getPos();
		const double_vec_ pCenter = planetCenter;

		//calculate parametric value: u
		const bigFloat d = distance_vec(pos, pCenter);
		const bigFloat u = bigFloat(PLANET_RADIUS) / d;

		//calculate surface position: surfacePt
		double_vec_ surfacePt;
		const bigFloat ONE = bigFloat(1);	//avoid magic numbers
		surfacePt.x = ((ONE - u) * pCenter.x) + (u * pos.x);
		surfacePt.y = ((ONE - u) * pCenter.y) + (u * pos.y);
		surfacePt.z = ((ONE - u) * pCenter.z) + (u * pos.z);

		return surfacePt;
	}

	//theta: rotation on circle to find desired pt
	//centerPoint: starting point on planet surface and center of circle
	//BCLen: distance between centerPoint and point you're calculating, changes based on whether C is an extreme pt or positional pt (b)
	double_vec_ calcSurfacePoint(bigFloat theta, double_vec_ centerPoint, bigFloat BCLen) {
		const double_vec_ A = planetCenter;
		const double_vec_ B = centerPoint;
		const bigFloat ABLen = PLANET_RADIUS; //c
		const bigFloat ACLen = PLANET_RADIUS; //a
		const bigFloat TWO = bigFloat(2); //because we need to double a lot of values and ttmath is very type sensitive

		//setup useful constants
		const bigFloat aSqrd = BCLen * BCLen;
		const bigFloat bSqrd = ACLen * ACLen;
		const bigFloat cSqrd = ABLen * ABLen;
		
		//find AHlen
		const bigFloat AHLen = (bSqrd - aSqrd + cSqrd) / (TWO * ABLen); //c sub b

		//find BHlen
		const bigFloat BHLen = (aSqrd - bSqrd + cSqrd) / (TWO * ABLen); // c sub a
		
		//find length of CH (alitude)	
		const bigFloat CHLen = Sqrt((BCLen + ACLen + ABLen)*(BCLen + ACLen - ABLen)*(BCLen - ACLen + ABLen)*(-BCLen + ACLen + ABLen)) / (TWO * ABLen);
		
		//unit vecto e1
		double_vec_ e1 = double_vec_(1.f, 0.f, 0.f);
		
		//vector between A and B
		const double_vec_ ABVec = double_vec_(B.x - A.x, B.y - A.y, B.z - A.z);

		//calculate vector AC (vector from planet center to new point on surface, C)
		double_vec_ ABxABxe1 = crossDouble(ABVec, crossDouble(ABVec, e1));
		double_vec_ e1xAB = crossDouble(e1, ABVec);
		const double_vec_ out = (AHLen / ABLen) * ABVec;
		const double_vec_ in1 = Cos(theta)*ABxABxe1 / ABxABxe1.getlen();
		const double_vec_ in2 = Sin(theta)*e1xAB / e1xAB.getlen();
		const double_vec_ in3 = CHLen * double_vec_(in1.x + in2.x, in1.y + in2.y, in1.z + in2.z);
		const double_vec_ ACVec = double_vec_(out.x + in3.x, out.y + in3.y, out.z + in3.z);

		//offset A by ACVec, resulting in point C
		return A + ACVec;
	}

	//probably can be moved to graphics card, or maybe generate then pass as uniform
	////////////////////////////////////////////////////////////////
	//float getAltitude(float color) {
	//	float tempHeight = color / 127.5 - 1.0f;
	//	return tempHeight * maxAltitude;
	//}

	//float generateHeight(float x, float z, int seed) {
	//	float total = 0;
	//	for (int i = 0; i < octaves; i++) {
	//		float divisor = 8.0 / pow(2, i);
	//		total += getInterpolatedNoise(x / divisor, z / divisor, seed) * (maxAltitude / pow(3, i));
	//	}
	//	return total;
	//}

	//float getInterpolatedNoise(float x, float z, int seed) {
	//	int intX = (int)x;
	//	int intZ = (int)z;
	//	float fracX = x - intX;
	//	float fracZ = z - intZ;

	//	float v1 = getSmoothNoise(intX, intZ, seed);
	//	float v2 = getSmoothNoise(intX + 1, intZ, seed);
	//	float v3 = getSmoothNoise(intX, intZ + 1, seed);
	//	float v4 = getSmoothNoise(intX + 1, intZ + 1, seed);
	//	float i1 = interpolate(v1, v2, fracX);
	//	float i2 = interpolate(v3, v4, fracX);
	//	return interpolate(i1, i2, fracZ);
	//}

	//float interpolate(float a, float b, float blend) {
	//	double t = blend * PI;
	//	float f = (float)(1.0 - cos(t)) * 0.5f;
	//	return a * (1 - f) + b * f;
	//}

	//float getSmoothNoise(int x, int z, int seed) {
	//	float corners = (getNoise(x - 1, z - 1, seed) + getNoise(x + 1, z - 1, seed) + getNoise(x - 1, z + 1, seed) + getNoise(x + 1, z + 1, seed)) / 16.0f;
	//	float sides = (getNoise(x - 1, z, seed) + getNoise(x + 1, z, seed) + getNoise(x, z - 1, seed) + getNoise(x, z + 1, seed)) / 8.0f;
	//	float center = getNoise(x, z, seed) / 4.0f;
	//	return corners + sides + center;
	//}

	//float getNoise(int x, int z, int seed) {
	//	srand(x * 49632 + z * 325176 + seed);
	//	return ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
	//}
	/////////////////////////////////////////////////////////////////////////

	//calculate the surface normal of a triangle with vertices a, b, c
	double_vec_ calcSurfaceNormalTriangle(double_vec_ a, double_vec_ b, double_vec_ c) {
		return crossDouble(b - a, c - a);
	}

	//calculate distance between any point the surface of a plane
	bigFloat distanceFromPlane(double_vec_ point, double_vec_ planeNormal) {
		return (planeNormal.x * point.x + planeNormal.y * point.y + planeNormal.z * point.z) 
			  / Sqrt(planeNormal.x*planeNormal.x + planeNormal.y*planeNormal.y + planeNormal.z*planeNormal.z);
	}

	//generate planet mesh (must be done every time the LODnum changes)
	GLfloat * generateMesh(GLfloat * vertices) {
		//constants
		const int meshWidth = terrainNum * 2;
		const int halfMeshWidth = terrainNum;

		//calc extreme points, initially calculated in initGeom()
		extPointVertN = calcSurfacePoint(PI / 2, extPointVertN, sideLength);
		extPointVertS = calcSurfacePoint(3 * PI / 2, extPointVertS, sideLength);

		extPointHorzW = calcSurfacePoint(PI, extPointHorzW, sideLength);
		extPointHorzE = calcSurfacePoint(0, extPointHorzE, sideLength);

		//generate vertex buffer to hand off to OGL
		//glGenBuffers(1, &VertexBufferID);
		//set the current state to focus on our vertex buffer
		//glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);

		//generate an array to store points (vec3s), generated as a LODnum x LODnum square of vec3s (* 3), then mirrored twice (*4)
		vertexBufferSize = terrainNum * terrainNum * 4 * 3; //equivalent to meshWidth * meshWidth * 3
		//double_vec_ * vertices = new double_vec_[bufferSize];
		//delete vertices;
		vertices = new float[vertexBufferSize];

		//calculate new center point
		centerSurfacePoint = calcCenterSurfacePt();
		vec3 pt_down = downscale(centerSurfacePoint);

		//lower righthand index of the generated square
		const int LRindex = ((meshWidth*meshWidth)/2 - (terrainNum + 1)) * 3;
		
		//calculate upper left point from the center point, the lower righthand point of the generated square
		double_vec_ pt_UL = calcSurfacePoint(PI / 4, centerSurfacePoint, Sqrt(bigFloat(2)*halfSideLength*halfSideLength));
		double_vec_ refPt;
		vec3 ptUL_down = downscale(pt_UL); //point at vertices[LRindex]
		vec3 temp;
		int index;

		//generate base first point of the upper left square, at index Lower Right (LRindex)
		vertices[LRindex] = ptUL_down.x;
		vertices[LRindex+1] = ptUL_down.y;
		vertices[LRindex+2] = ptUL_down.z;

		//create upper left of mesh lattice if it is greater than one vertex
		if (LRindex > 0) {
			//create bottom horizantle branch of mesh points, starting with LRindex and working left towards the edge
			for (int i = 3; i < terrainNum * 3; i += 3) {
				index = LRindex - i;
				refPt = double_vec_(vertices[index + 3], vertices[index + 4], vertices[index + 5]);
				temp = downScale(calcSurfacePoint(0, refPt, sideLength));
				vertices[index] = temp.x;
				vertices[index + 1] = temp.y;
				vertices[index + 2] = temp.z;
			}

			//create vertical branches of each vertex of the horizantel lattice branch
			for (int j = 0; j < terrainNum * 3; j += 3) {
				index = LRindex - j;
				refPt = double_vec_(vertices[index], vertices[index + 1], vertices[index + 2]);
				for (int k = meshWidth*3; k < meshWidth*terrainNum*3; k += meshWidth*3) {
					temp = downscale(calcSurfacePoint(PI / 2, double_vec_(vertices[index], vertices[index + 1], vertices[index + 2]), sideLength));
					index -= meshWidth * 3;
					vertices[index] = temp.x;
					vertices[index + 1] = temp.y;
					vertices[index + 2] = temp.z;
				}
			}
		}

		//todo: mirroring wrong, repeating copying of certain vertices

		//mirror points across verticle
		double_vec_ planeNormal = calcSurfaceNormalTriangle(myCam.getPos(), planetCenter, extPointVertN);
		double_vec_ planeNormal_normalized = planeNormal;
		planeNormal_normalized.normalize();
		for (int i = 0; i < halfMeshWidth * meshWidth * 3; i += meshWidth*3) {
			for (int k = i + (meshWidth * 3) - 3; k >= i + halfMeshWidth * 3; k -= 3) {
				for (int j = i; j < i + halfMeshWidth * 3; j += 3) {
					bigFloat dist = distanceFromPlane(double_vec_(vertices[j], vertices[j + 1], vertices[j + 2]), planeNormal);
					//cout << "dist: " << dist.ToFloat() << endl;

					vertices[k] = vertices[j] + -1*(dist.ToFloat() * planeNormal_normalized.x.ToFloat()) * 2;
					vertices[k+1] = vertices[j+1] + -1 * (dist.ToFloat() * planeNormal_normalized.y.ToFloat()) * 2;
					vertices[k+2] = vertices[j+2] + -1 * (dist.ToFloat() * planeNormal_normalized.z.ToFloat()) * 2;
				}
			}
		}

		//mirror points across horizantle
		planeNormal = planeNormal_normalized = calcSurfaceNormalTriangle(myCam.getPos(), planetCenter, extPointHorzW);
		planeNormal_normalized.normalize();
		for (int i = 0; i < halfMeshWidth * meshWidth * 3; i += meshWidth * 3) {
			int k = meshWidth * (meshWidth - 1) * 3 - i;
				for (int j = 0; j < i + meshWidth*3; j+=3) {
					bigFloat dist = distanceFromPlane(double_vec_(vertices[j], vertices[j + 1], vertices[j + 2]), planeNormal);
					//cout << "dist: " << dist.ToFloat() << endl;
					
					vertices[j+k] = vertices[i+j] + -1 * (dist.ToFloat() * planeNormal_normalized.x.ToFloat()) * 2;
					vertices[j+k+1] = vertices[i+j+1] + -1 * (dist.ToFloat() * planeNormal_normalized.y.ToFloat()) * 2;
					vertices[j+k+2] = vertices[i+j+2] + -1 * (dist.ToFloat() * planeNormal_normalized.z.ToFloat()) * 2;
				}
		}

		//actually memcopy the data - only do this once
		//glBufferData(GL_ARRAY_BUFFER, sizeof(VBOvertices), VBOvertices, GL_DYNAMIC_DRAW);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

		//memory cleanup/management
		//delete VBOvertices;

		//for debug
		for (int i = 0; i < vertexBufferSize; i++) {
			cout << "vert buffer pos" << i << ": " << vertices[i] << endl;
		}
		cout << "size of vert buffer: " << vertexBufferSize << endl;

		return vertices;
		//delete vertices;
	}

	//recreate indexbuffer based on LOD and terrain level
	GLuint * generateMeshIndex(GLuint * elements) {
		//glGenBuffers(1, &IndexBufferID);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferID);
		terrainWidth = terrainNum * 2;
		indexBufferSize = pow(terrainWidth - 1, 2) * 6;
		delete elements;
		elements = new GLuint[indexBufferSize];

		//generate index buffer
		int ind = 0, skipIndex = terrainWidth-1, row = 1;
		for (int i = 0; i < indexBufferSize; i += 6, ind++) {
			elements[i + 0] = ind;
			elements[i + 1] = ind + 1;
			elements[i + 2] = ind + terrainWidth;

			elements[i + 3] = ind + terrainWidth;
			elements[i + 4] = ind + terrainWidth + 1;
			elements[i + 5] = ind + 1;

			if (ind % ((int)terrainWidth * row - 2) == 0 && ind != 0) {
				ind++;
				row++;
			}
		}

		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_DYNAMIC_DRAW);
		//glBindVertexArray(0);

		//for debug
		for (int i = 0; i < indexBufferSize; i++) {
			cout << "index buffer pos" << i << ": " << elements[i] << endl;
		}
		cout << "size of index buffer: " << indexBufferSize << endl;

		return elements;
		//delete elements;
	}

	//change input vertices and elements pointer to update different meshes
	void updateMesh(GLfloat * vertices, GLuint * elements) {
		vertices = generateMesh(vertices);
		elements = generateMeshIndex(elements);
	}

	//initialize meshes, VAO, VBO
	void initGeom() {
		//temp code to check if camera is working as expected
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);

		//generate the VAO
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &VertexBufferID);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);

		//oriantation looking from planet center towards camera
		extPointHorzW = calcSurfacePoint(PI, myCam.getPos() - planetCenter, halfSideLength);
		extPointHorzE = calcSurfacePoint(0, myCam.getPos() - planetCenter, halfSideLength);
		extPointVertN = calcSurfacePoint(PI / 2, myCam.getPos() - planetCenter, halfSideLength);
		extPointVertS = calcSurfacePoint(3 * PI / 2, myCam.getPos() - planetCenter, halfSideLength);
		//buildVertexBufferProcedural(vertexBufferData);
		
		vertexBufferData = generateMesh(vertexBufferData);
		/*cout << "in initgeom()"<< endl;
		for (int i = 0; i < 12; i++) {
			cout << "vert buffer pos" << i << ": " << vertexBufferData[i] << endl;
		}
		cout << "size of vert buffer: " << sizeof(vertexBufferData) << endl;
		*/
		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData)*vertexBufferSize, vertexBufferData, GL_DYNAMIC_DRAW);

		//we need to set up the vertex array
		glEnableVertexAttribArray(0);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Create and bind IBO
		glGenBuffers(1, &IndexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferID);

		//buildIndexBuffer(indexBufferData);
		indexBufferData = generateMeshIndex(indexBufferData);
		/*cout << "in initGeom()" << endl;
		for (int i = 0; i < 6; i++) {
			cout << "index buffer pos" << i << ": " << indexBufferData[i] << endl;
		}
		cout << "size of index buffer: " << sizeof(indexBufferData) << endl;
		*/
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexBufferData)*indexBufferSize, indexBufferData, GL_DYNAMIC_DRAW);
		glBindVertexArray(0);
	}

	//initialize shaders, call geom initialization
	void init(const std::string& resourceDirectory) {
		GLSL::checkVersion();
		//set background color.
		glClearColor(.12f, .34f, .56f, 1.0f);
		//enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		//initialize the shader programs
		prog = make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(
			resourceDirectory + "/simple_vert.glsl",
			resourceDirectory + "/simple_frag.glsl");
		if (!prog->init()) {
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("maxAltitude");
		prog->addAttribute("vertPos");

		initGeom();
	}

	//draw everything
	void render() {
		static double count = 0;
		double frametime = get_last_elapsed_time();
		count += frametime;

		//if the mesh is too small, resize it
		//todo, uncomment and test on shit appears
		/*int horizonNum = checkHorizon();
		if (horizonNum == 2) {
			terrainNum++;
			updateMesh(vertexBufferData, indexBufferData);
		}

		else if (horizonNum == 1 && terrainNum > 1) {
			terrainNum--;
			updateMesh(vertexBufferData, indexBufferData);
		}*/

		//TEST code for camera tests
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		//glfwSetCursorPosCallback(windowManager->getHandle(), cursorPosCallback);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* Leave this code to just draw the meshes alone */
		float aspect = width / (float)height;

		// Create the matrix stacks
		auto Projection = make_shared<MatrixStack>();
		auto View = make_shared<MatrixStack>();
		auto Model = make_shared<MatrixStack>();
		// Apply perspective projection.
		Projection->pushMatrix();
		Projection->perspective(45, width, height, 0.01f, 1000.0f);

		//View for fps camera
		mat4 V = myCam.process(frametime);

		Model->pushMatrix();
		//Model->translate(vec3(myCam.getPos().x.ToFloat(), myCam.getPos().y.ToFloat(), myCam.getPos().z.ToFloat()));
		//Model->rotate(-myCam.getRot().x, vec3(1, 0, 0));
		//Model->rotate(-myCam.getRot().y, vec3(0, 1, 0));

		//Draw our scene - two meshes - right now to a texture
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		glUniform1f(prog->getUniform("maxAltitude"), maxAltitude);
		glBindVertexArray(VertexArrayID);

		//draw triangles
		/*for (int i = 0; i < 12; i++) {
			cout << vertexBufferData[i] << endl;
		}
		for (int i = 0; i < 6; i++) {
			cout << indexBufferData[i] << endl;
		}*/
		//use GL_TRIANGLE to draw everything filled in
		glDrawElements(GL_LINE_STRIP, sizeof(indexBufferData)*indexBufferSize, GL_UNSIGNED_INT, nullptr);

		glBindVertexArray(0);

		prog->unbind();

		Model->popMatrix();
		Projection->popMatrix();
	}
};

int main(int argc, char **argv) {
	// Where the resources are loaded from
	std::string resourceDir = "../src/resources";

	Application *application = new Application();

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager *windowManager = new WindowManager();
	windowManager->init(512, 512);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->init(resourceDir);
	//application->initGeom();

	// Loop until the user closes the window.
	while (!glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}


//TODO: learn from this to init planet meshes
//void init_mesh()
//{
//
//	//generate the VAO
//	glGenVertexArrays(1, &VertexArrayID);
//	glBindVertexArray(VertexArrayID);
//
//	//generate vertex buffer to hand off to OGL
//	glGenBuffers(1, &MeshPosID);
//	glBindBuffer(GL_ARRAY_BUFFER, MeshPosID);
//	glm::vec3 *vertices = new glm::vec3[MESHSIZE * MESHSIZE * 6];
//	for (int x = 0; x < MESHSIZE; x++)
//	{
//		for (int z = 0; z < MESHSIZE; z++)
//		{
//			vertices[x * 6 + z * MESHSIZE * 6 + 0] = vec3(0.0, 0.0, 0.0) + vec3(x, 0, z);//LD
//			vertices[x * 6 + z * MESHSIZE * 6 + 1] = vec3(1.0, 0.0, 0.0) + vec3(x, 0, z);//RD
//			vertices[x * 6 + z * MESHSIZE * 6 + 2] = vec3(1.0, 0.0, 1.0) + vec3(x, 0, z);//RU
//			vertices[x * 6 + z * MESHSIZE * 6 + 3] = vec3(0.0, 0.0, 0.0) + vec3(x, 0, z);//LD
//			vertices[x * 6 + z * MESHSIZE * 6 + 4] = vec3(1.0, 0.0, 1.0) + vec3(x, 0, z);//RU
//			vertices[x * 6 + z * MESHSIZE * 6 + 5] = vec3(0.0, 0.0, 1.0) + vec3(x, 0, z);//LU
//
//			/*Debug vertices
//			printf("x = %d : z = %d\n", x, z);
//			printf("{%f, %f, %f}\n", vertices[x * 6 + z * MESHSIZE * 6 + 0].x, vertices[x * 6 + z * MESHSIZE * 6 + 0].y, vertices[x * 6 + z * MESHSIZE * 6 + 0].z);
//			printf("{%f, %f, %f}\n", vertices[x * 6 + z * MESHSIZE * 6 + 1].x, vertices[x * 6 + z * MESHSIZE * 6 + 1].y, vertices[x * 6 + z * MESHSIZE * 6 + 1].z);
//			printf("{%f, %f, %f}\n", vertices[x * 6 + z * MESHSIZE * 6 + 2].x, vertices[x * 6 + z * MESHSIZE * 6 + 2].y, vertices[x * 6 + z * MESHSIZE * 6 + 2].z);
//			printf("{%f, %f, %f}\n", vertices[x * 6 + z * MESHSIZE * 6 + 3].x, vertices[x * 6 + z * MESHSIZE * 6 + 3].y, vertices[x * 6 + z * MESHSIZE * 6 + 3].z);
//			printf("{%f, %f, %f}\n", vertices[x * 6 + z * MESHSIZE * 6 + 4].x, vertices[x * 6 + z * MESHSIZE * 6 + 4].y, vertices[x * 6 + z * MESHSIZE * 6 + 4].z);
//			printf("{%f, %f, %f}\n\n", vertices[x * 6 + z * MESHSIZE * 6 + 5].x, vertices[x * 6 + z * MESHSIZE * 6 + 5].y, vertices[x * 6 + z * MESHSIZE * 6 + 5].z);
//			*/
//		}
//
//	}
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * MESHSIZE * MESHSIZE * 6, vertices, GL_DYNAMIC_DRAW);
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
//	delete[] vertices;
//	//tex coords
//	/*float t = 1. / MESHSIZE;
//	vec2 *tex = new vec2[MESHSIZE * MESHSIZE * 6];
//	for (int x = 0; x<MESHSIZE; x++)
//		for (int y = 0; y < MESHSIZE; y++)
//		{
//			tex[x * 6 + y*MESHSIZE * 6 + 0] = vec2(0.0, 0.0)+ vec2(x, y)*t;	//LD
//			tex[x * 6 + y*MESHSIZE * 6 + 1] = vec2(t, 0.0)+ vec2(x, y)*t;	//RD
//			tex[x * 6 + y*MESHSIZE * 6 + 2] = vec2(t, t)+ vec2(x, y)*t;		//RU
//			tex[x * 6 + y*MESHSIZE * 6 + 3] = vec2(0.0, 0.0) + vec2(x, y)*t;	//LD
//			tex[x * 6 + y*MESHSIZE * 6 + 4] = vec2(t, t) + vec2(x, y)*t;		//RU
//			tex[x * 6 + y*MESHSIZE * 6 + 5] = vec2(0.0, t)+ vec2(x, y)*t;	//LU
//		}
//	glGenBuffers(1, &MeshTexID);
//	//set the current state to focus on our vertex buffer
//	glBindBuffer(GL_ARRAY_BUFFER, MeshTexID);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * MESHSIZE * MESHSIZE * 6, tex, GL_STATIC_DRAW);
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
//	delete[] tex;*/
//	/*glGenBuffers(1, &IndexBufferIDBox);
//	//set the current state to focus on our vertex buffer
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
//	GLuint *elements = new GLuint[MESHSIZE * MESHSIZE * 8];
////	GLuint ele[10000];
//	int ind = 0,i=0;
//	for (i = 0; i<(MESHSIZE * MESHSIZE * 8); i+=8, ind+=6)
//		{
//		elements[i + 0] = ind + 0;
//		elements[i + 1] = ind + 1;
//		elements[i + 2] = ind + 1;
//		elements[i + 3] = ind + 2;
//		elements[i + 4] = ind + 2;
//		elements[i + 5] = ind + 5;
//		elements[i + 6] = ind + 5;
//		elements[i + 7] = ind + 0;
//		}
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*MESHSIZE * MESHSIZE * 8, elements, GL_STATIC_DRAW);
//	delete[] elements;*/
//	glBindVertexArray(0);
//}
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
#define PLANET_RADIUS 10000

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
	bool w, a, s, d, moved = false;
	float theta, phi;

	camera() {
		pos = double_vec_(0, 0, 0);
		rot = vec3(0.0);
		w = a = s = d = false;
		deltaX = deltaY = prevX = prevY = 0;
		theta = phi = 0;
	}

	double_vec_ getPos() {
		return -1.f*pos;
	}

	vec3 getRot() {
		return rot;
	}

	mat4 process(double fTime) {
		float speedFB = 0;
		float speedLR = 0;
		float fwdspeed = 50;

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
	uint DRAW_MODE = GL_FILL;
	const double_vec_ planetCenter = double_vec_(0, 0, -(PLANET_RADIUS + 100));
	//extreme point for LOD refactor testing
	double_vec_ extPointVertN, extPointVertS, extPointHorzE, extPointHorzW;
	//number of subdivions to great a n-gon to create planet sphere
	const int numSides = 100;
	const int maxMeshSize = pow(numSides / 2 - 1, 2);
	//angle between center point of the planet for any two adjacent points on the planets equator
	const bigFloat deltaTheta = 2 * PI / numSides;
	//planet circumference
	const bigFloat planetCircum = 2 * PI * PLANET_RADIUS;
	//lengh of any side on the planet equator
	const bigFloat sideLength = planetCircum / numSides;
	const bigFloat halfSideLength = sideLength / 2;
	//factor to determine number of base points, equal to one quarter of total mesh points, numSides / 4
	int terrainNum = 1;
	//factor to determine LOD mesh simplification
	int LODnum = 1;
	//center point between planet center and the camera
	double_vec_ centerSurfacePoint;
	//resolution constant for tesselation
	int resolution = PLANET_RADIUS / 500;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our triangle to OpenGL
	GLuint VertexBufferID;
	GLuint IndexBufferID;
	//variables for terrain size
	int terrainWidth = terrainNum * 2;
	float maxAltitude = PLANET_RADIUS * 0.001;

	//buffers for terrain generation
	GLfloat *vertexBufferData;
	GLuint *indexBufferData;

	//globals for mesh rotations
	GLfloat rotationTheta = 0.f;
	double_vec_ rotationAxis = double_vec_(0, 0, 0);

	//globals for camera movements deltas
	double_vec_ newCamPos, oldCamPos;

	//random seed global
	int randSeed;

	//globals to store buffer sizes to ensure data is transfered to GPU correctly
	int vertexBufferSize = 0, indexBufferSize = 0;

	//starting center surface point on planet for calculating delta theta of camera rotation
	double_vec_ startingCenterPos;

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

	bigFloat dotDouble(double_vec_ v, double_vec_ u) {
		bigFloat dat = v.x*u.x + v.y*u.y + v.z*u.z;
		return dat;
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
			oldCamPos = myCam.getPos();
			myCam.w = myCam.moved = true;
			myCam.s = false;
		}
		else if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			oldCamPos = myCam.getPos();
			myCam.s = myCam.moved = true;
			myCam.w = false;
		}

		//side to side movement controls
		if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			oldCamPos = myCam.getPos();
			myCam.a = myCam.moved = true;
			myCam.d = false;
		}
		else if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			oldCamPos = myCam.getPos();
			myCam.d = myCam.moved = true;
			myCam.a = false;
		}

		//controls to switch between shaded and mesh rendering
		else if (key == GLFW_KEY_T && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			DRAW_MODE = GL_LINE;
		}

		else if (key == GLFW_KEY_Y && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			DRAW_MODE = GL_FILL;
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
	//return 1 if the mesh needs to be shrunk, 2 if expanded, 0 if fine as is
	int checkHorizon() {
		//calculate hypotenuse of right triangle formed by camera, planet center, and farthest visible point on horizon
		//the lengths of the sides being the the altitude of the camera(h), the planet radius, and len (unknown)
		float h = Abs(distance_vec(myCam.getPos(), planetCenter)).ToFloat() - PLANET_RADIUS;
		//the length from the camera to the farthest visible point on the horizon (rendered or not)
		float len = sqrt(h*(2*PLANET_RADIUS + h));
		//float h2 = Abs(distance_vec(myCam.getPos(), calcCenterSurfacePt())).ToFloat();

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
			return 0;
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
		const bigFloat d = Abs(distance_vec(pos, pCenter));
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

		//calc new extreme points, initially calculated in initGeom()
		extPointVertN = calcSurfacePoint(PI / 2, extPointVertN, sideLength);
		extPointVertS = calcSurfacePoint(3 * PI / 2, extPointVertS, sideLength);
		extPointHorzW = calcSurfacePoint(PI, extPointHorzW, sideLength);
		extPointHorzE = calcSurfacePoint(0, extPointHorzE, sideLength);

		//generate an array to store points (vec3s), generated as a LODnum x LODnum square of vec3s (* 3), then mirrored twice (*4)
		vertexBufferSize = terrainNum * terrainNum * 4 * 3; //equivalent to meshWidth * meshWidth * 3
		vertices = new float[vertexBufferSize];

		//calculate new center point
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
				temp = downScale(calcSurfacePoint(PI / 2, refPt, sideLength));
				vertices[index] = temp.x;
				vertices[index + 1] = temp.y;
				vertices[index + 2] = temp.z;

				extPointVertN = calcSurfacePoint(PI / 2, extPointVertN, sideLength);
				extPointVertS = calcSurfacePoint(3 * PI / 2, extPointVertS, sideLength);
				extPointHorzW = calcSurfacePoint(PI, extPointHorzW, sideLength);
				extPointHorzE = calcSurfacePoint(0, extPointHorzE, sideLength);
			}

			//create vertical branches of each vertex of the horizantel lattice branch
			for (int j = 0; j < terrainNum * 3; j += 3) {
				index = LRindex - j;
				refPt = double_vec_(vertices[index], vertices[index + 1], vertices[index + 2]);
				for (int k = meshWidth*3; k < meshWidth*terrainNum*3; k += meshWidth*3) {
					temp = downscale(calcSurfacePoint(0, double_vec_(vertices[index], vertices[index + 1], vertices[index + 2]), sideLength));
					index -= meshWidth * 3;
					vertices[index] = temp.x;
					vertices[index + 1] = temp.y;
					vertices[index + 2] = temp.z;
				}
			}
		}

		//mirror points across verticle
		double_vec_ planeNormal = calcSurfaceNormalTriangle(myCam.getPos(), planetCenter, extPointHorzW);
		double_vec_ planeNormal_normalized = planeNormal;
		planeNormal_normalized.normalize();
		for (int i = 0; i < halfMeshWidth * meshWidth * 3; i += meshWidth*3) {
			int j = i;
			for (int k = i + (meshWidth * 3) - 3; k >= i + halfMeshWidth * 3; k -= 3) {
				bigFloat dist = distanceFromPlane(double_vec_(vertices[j], vertices[j + 1], vertices[j + 2]), planeNormal);

				vertices[k] = vertices[j] + -1*(dist.ToFloat() * planeNormal_normalized.x.ToFloat()) * 2;
				vertices[k+1] = vertices[j+1] + -1 * (dist.ToFloat() * planeNormal_normalized.y.ToFloat()) * 2;
				vertices[k+2] = vertices[j+2] + -1 * (dist.ToFloat() * planeNormal_normalized.z.ToFloat()) * 2;

				j += 3;
			}
		}

		//mirror points across horizantle
		planeNormal = planeNormal_normalized = calcSurfaceNormalTriangle(myCam.getPos(), planetCenter, extPointVertN);
		planeNormal_normalized.normalize();
		for (int i = 0; i < halfMeshWidth * meshWidth * 3; i += meshWidth * 3) {
			int k = meshWidth * (meshWidth - 1) * 3 - i;
				for (int j = 0; j < meshWidth*3; j+=3) {
					bigFloat dist = distanceFromPlane(double_vec_(vertices[j + i], vertices[j + i + 1], vertices[j + i + 2]), planeNormal);
					
					vertices[j+k] = vertices[i+j] + -1 * (dist.ToFloat() * planeNormal_normalized.x.ToFloat()) * 2;
					vertices[j+k+1] = vertices[i+j+1] + -1 * (dist.ToFloat() * planeNormal_normalized.y.ToFloat()) * 2;
					vertices[j+k+2] = vertices[i+j+2] + -1 * (dist.ToFloat() * planeNormal_normalized.z.ToFloat()) * 2;
				}
		}

		return vertices;
	}

	//recreate indexbuffer based on LOD and terrain level
	GLuint * generateMeshIndex(GLuint * elements) {
		terrainWidth = terrainNum * 2;
		indexBufferSize = pow(terrainWidth - 1, 2) * 6;
		elements = new GLuint[indexBufferSize];

		//generate index buffer
		int ind = 0, row = 1;
		for (int i = 0; i < indexBufferSize; i += 6, ind++) {
			elements[i + 0] = ind;
			elements[i + 1] = ind + 1;
			elements[i + 2] = ind + terrainWidth;

			elements[i + 3] = ind + terrainWidth;
			elements[i + 4] = ind + terrainWidth + 1;
			elements[i + 5] = ind + 1;

			if (terrainNum > 1 && ind % ((int)terrainWidth * row - 2) == 0 && ind != 0) {
				ind++;
				row++;
			}
		}

		return elements;
	}

	//rotate a point x along a curve theta according to a determined axis
	double_vec_ eulerRotate(bigFloat theta, double_vec_ axis, double_vec_ x) {
		double_vec_ xPrime;

		//shift x about the origin
		double_vec_ shiftX = x - planetCenter;

		//calculate rotated x
		xPrime = (x * Cos(theta)) + (axis * (dotDouble(axis, x)*(bigFloat(1) - Cos(theta)))) + crossDouble(x, axis)*Sin(theta);

		//reverse original shift of x for xPrime
		xPrime += planetCenter;

		return xPrime;
	}

	//find an axis from the planet center to the surface, such that it creates a 90deg angle with the vector between
	//the input surface point and planet center, at a position theta on the circular slice the axis lies on
	double_vec_ findRotationAxis(double_vec_ surfacePt, bigFloat theta) {
		double_vec_ aXb = crossDouble(surfacePt, startingCenterPos);
		return aXb / (surfacePt.getlen() * startingCenterPos.getlen() * Sin(theta));
	}

	//change input vertices and elements pointer to update different meshes, resizeMode = 1 for shrink, 2 for expand
	GLuint * updateMesh(GLfloat * vertices, GLuint * elements, int resizeMode) {
		//generate the VAO
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &VertexBufferID);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
		vec3 centerTemp = downScale(centerSurfacePoint);

		centerSurfacePoint = calcCenterSurfacePt();
		centerTemp = downScale(centerSurfacePoint);
		extPointHorzW = calcSurfacePoint(PI, centerSurfacePoint, halfSideLength);
		extPointHorzE = calcSurfacePoint(0, centerSurfacePoint, halfSideLength);
		extPointVertN = calcSurfacePoint(PI / 2, centerSurfacePoint, halfSideLength);
		extPointVertS = calcSurfacePoint(3 * PI / 2, centerSurfacePoint, halfSideLength);

		vertices = generateMesh(vertices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)*vertexBufferSize, vertices, GL_DYNAMIC_DRAW);

		//we need to set up the vertex array
		glEnableVertexAttribArray(0);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Create and bind IBO
		glGenBuffers(1, &IndexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferID);
		elements = generateMeshIndex(elements);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements)*indexBufferSize, elements, GL_DYNAMIC_DRAW);
		glBindVertexArray(0);

		return elements;
	}

	float findTriangleArea(vec3 A, vec3 B, vec3 C) {
		vec3 AB = A - B;
		vec3 AC = A - C;
		float lengthAB = distance(A, B);
		float lengthAC = distance(A, C);
		float cosTheta = dot(AB, AC) / (lengthAB*lengthAC);
		float sinTheta = sqrt(1 - pow(cosTheta, 2));
		
		return lengthAB * lengthAC * sinTheta / 2.f;
	}

	void updateOnMove() {
		//save globals
		double_vec_ tempExtPointHorzE = extPointHorzE;
		double_vec_ tempExtPointHorzW = extPointHorzW;
		double_vec_ tempExtPointVertN = extPointVertN;
		double_vec_ tempExtPointVertS = extPointVertS;
		double_vec_ tempSurfacePt;

		//size of triangle formed by newCamPos, oldCamPos, and the planet center
		float triangleSize = findTriangleArea(downScale(myCam.getPos()), downScale(oldCamPos), downScale(planetCenter));
		
		//if triangleSize is positive, the camera has moved away from the vector it was previously on with the planet center, meaning rotation is needed
		if (triangleSize > 0) {
			rotationTheta = 0;
			rotationAxis = double_vec_(NULL, NULL, NULL);
			tempSurfacePt = calcCenterSurfacePt();
			vec3 tempSurfacePtDown = downscale(tempSurfacePt);
			vec3 centersurfacePtDown = downscale(centerSurfacePoint);
			float dis = distance_vec(tempSurfacePt, startingCenterPos).ToFloat();

			//find angle between tempSurfacePt (point between new cam pos and planet center) and centerSurfacePoint (old surface pt)
			rotationTheta = acos(((2 * PLANET_RADIUS*PLANET_RADIUS) - pow(dis, 2)) / (2 * PLANET_RADIUS * PLANET_RADIUS));

			//find axis of rotation
			rotationAxis = findRotationAxis(tempSurfacePt,rotationTheta);
			vec3 rotAxDown = downscale(rotationAxis);

			//rotate extreme points using euler's theorem
			extPointHorzE = eulerRotate(rotationTheta, rotationAxis, tempExtPointHorzE);
			extPointHorzW = eulerRotate(rotationTheta, rotationAxis, tempExtPointHorzW);
			extPointVertN = eulerRotate(rotationTheta, rotationAxis, tempExtPointVertN);
			extPointVertS = eulerRotate(rotationTheta, rotationAxis, tempExtPointVertS);
			centerSurfacePoint = tempSurfacePt;
		}
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
		centerSurfacePoint = startingCenterPos= calcCenterSurfacePt();
		extPointHorzW = calcSurfacePoint(PI, centerSurfacePoint, halfSideLength);
		extPointHorzE = calcSurfacePoint(0, centerSurfacePoint, halfSideLength);
		extPointVertN = calcSurfacePoint(PI / 2, centerSurfacePoint, halfSideLength);
		extPointVertS = calcSurfacePoint(3 * PI / 2, centerSurfacePoint, halfSideLength);
		
		vertexBufferData = generateMesh(vertexBufferData);
		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData)*vertexBufferSize, vertexBufferData, GL_DYNAMIC_DRAW);

		//we need to set up the vertex array
		glEnableVertexAttribArray(0);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Create and bind IBO
		glGenBuffers(1, &IndexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferID);

		indexBufferData = generateMeshIndex(indexBufferData);
		
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexBufferData)*indexBufferSize, indexBufferData, GL_DYNAMIC_DRAW);
		glBindVertexArray(0);
	}

	//initialize shaders, call geom initialization
	void init(const std::string& resourceDirectory) {
		//cout << randSeed << endl;
		GLSL::checkVersion();
		//set background color.
		glClearColor(.12f, .34f, .56f, 1.0f);
		//enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		GLint mpv = 0;
		glGetIntegerv(GL_MAX_PATCH_VERTICES, &mpv);
		printf("max supported patch vertices %d\n", mpv);

		//initialize the shader programs
		prog = make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(
			resourceDirectory + "/simple_vert.glsl",
			resourceDirectory + "/simple_frag.glsl",
			resourceDirectory + "/tess_Control.glsl",
			resourceDirectory + "/tess_Eval.glsl");
		
		if (!prog->init()) {
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog->addAttribute("position_VS_in");
		prog->addAttribute("normal_VS_in");

		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("maxAltitude");
		prog->addUniform("theta");
		prog->addUniform("axis");
		prog->addUniform("planetCenter");
		prog->addUniform("camPos");
		prog->addUniform("meshSize");
		prog->addUniform("randSeed");
		prog->addUniform("meshMaxSize");
		prog->addUniform("resolution");
			   
		srand(time(NULL));
		randSeed = rand() % 100;

		initGeom();
	}

	//draw everything
	void render() {
		int v = 0;

		//uncomment for constantly changing landscapes!
		/*srand(time(NULL));
		randSeed = rand() % 100;*/

		static double count = 0;
		double frametime = get_last_elapsed_time();
		count += frametime;

		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

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
		Projection->perspective(45, width, height, 0.01f, 1000000);

		//View for fps camera
		mat4 V = myCam.process(frametime);
		if (myCam.moved)
			updateOnMove();

		Model->pushMatrix();
		if (rotationTheta > 0) {
			Model->translate(downscale(planetCenter));
			Model->rotate(rotationTheta, downScale(rotationAxis));
			Model->translate(-downscale(planetCenter));
		}

		//Draw our scene - two meshes - right now to a texture
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		glUniform3fv(prog->getUniform("camPos"), 1, &downScale(myCam.getPos())[0]);
		glUniform1f(prog->getUniform("maxAltitude"), maxAltitude);
		glUniform1f(prog->getUniform("theta"), rotationTheta);
		glUniform3fv(prog->getUniform("axis"), 1, value_ptr(downScale(rotationAxis)));
		glUniform3fv(prog->getUniform("planetCenter"), 1, value_ptr(downScale(planetCenter)));
		glUniform1i(prog->getUniform("meshMaxSize"), maxMeshSize);
		glUniform1i(prog->getUniform("randSeed"), randSeed);
		glUniform1i(prog->getUniform("resolution"), resolution);

		glBindVertexArray(VertexArrayID);

		//set draw mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//use GL_TRIANGLE to draw everything filled in
		glDrawElements(GL_PATCHES, indexBufferSize, GL_UNSIGNED_INT, nullptr);

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
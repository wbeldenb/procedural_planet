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
	//number of subdivions to great a n-gon to create planet sphere
	const int numSides = 100;
	//angle between center point of the planet for any two adjacent points on the planets equator
	const bigFloat deltaTheta = 2 * PI / numSides;
	//planet circumference
	const bigFloat planetCircum = 2 * PI * PLANET_RADIUS;
	//lengh of any side on the planet equator
	const bigFloat sideLength = planetCircum / numSides;
	const bigFloat halfSideLength = sideLength / 2;
	//factor to determine number of base points
	int terrainNum = 1;
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
	int terrainWidth = 100;
	int terrainDepth = 100;
	float maxAltitude = 20;

	//buffers for terrain generation
	GLfloat vertexBufferData[100 * 100 * 3] = { 0 };
	GLuint indexBufferData[99 * 99 * 6] = { 0 };

	int octaves = 8;

public:
	WindowManager * windowManager = nullptr;

	//shader programs
	shared_ptr<Program> prog, heightShader, spaceShader;

	bool mouseDown = false;

	double_vec_ crossDouble(double_vec_ a, double_vec_ b) {
		bigFloat x = a.y * b.z - a.z * b.y;
		bigFloat y = a.z * b.x - a.x * b.z;
		bigFloat z = a.x * b.y - a.y * b.x;
		return double_vec_(x, y, z);
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

	float getAltitude(float color) {
		float tempHeight = color / 127.5 - 1.0f;
		return tempHeight * maxAltitude;
	}

	float generateHeight(float x, float z, int seed) {
		float total = 0;
		for (int i = 0; i < octaves; i++) {
			float divisor = 8.0 / pow(2, i);
			total += getInterpolatedNoise(x / divisor, z / divisor, seed) * (maxAltitude / pow(3, i));
		}
		return total;
	}

	float getInterpolatedNoise(float x, float z, int seed) {
		int intX = (int)x;
		int intZ = (int)z;
		float fracX = x - intX;
		float fracZ = z - intZ;

		float v1 = getSmoothNoise(intX, intZ, seed);
		float v2 = getSmoothNoise(intX + 1, intZ, seed);
		float v3 = getSmoothNoise(intX, intZ + 1, seed);
		float v4 = getSmoothNoise(intX + 1, intZ + 1, seed);
		float i1 = interpolate(v1, v2, fracX);
		float i2 = interpolate(v3, v4, fracX);
		return interpolate(i1, i2, fracZ);
	}

	float interpolate(float a, float b, float blend) {
		double t = blend * PI;
		float f = (float)(1.0 - cos(t)) * 0.5f;
		return a * (1 - f) + b * f;
	}

	float getSmoothNoise(int x, int z, int seed) {
		float corners = (getNoise(x - 1, z - 1, seed) + getNoise(x + 1, z - 1, seed) + getNoise(x - 1, z + 1, seed) + getNoise(x + 1, z + 1, seed)) / 16.0f;
		float sides = (getNoise(x - 1, z, seed) + getNoise(x + 1, z, seed) + getNoise(x, z - 1, seed) + getNoise(x, z + 1, seed)) / 8.0f;
		float center = getNoise(x, z, seed) / 4.0f;
		return corners + sides + center;
	}

	float getNoise(int x, int z, int seed) {
		srand(x * 49632 + z * 325176 + seed);
		return ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
	}

	void buildVertexBufferProcedural(GLfloat vertBuff[]) {
		srand(time(NULL) * 45672134);
		int seed = rand();
		int color;

		/*int index = 0;
		for (int z = 0; z < terrainDepth; z++) {
			for (int x = 0; x < terrainWidth; x++) {
				float height = generateHeight(x, z, seed);

				vertBuff[index] = x - terrainWidth / 2;
				vertBuff[index + 1] = height - 15;
				vertBuff[index + 2] = z - terrainDepth / 2;

				index += 3;
			}
		}*/

		double_vec_ pt = calcCenterSurfacePt();
		vec3 pt_down = downscale(pt);

		double_vec_ pt_UL = calcSurfacePoint(PI / 4, pt, Sqrt(bigFloat(2)*halfSideLength*halfSideLength));
		vec3 ptUL_down = downscale(pt_UL);

		double_vec_ pt_LL = calcSurfacePoint(7 * PI / 4, pt, Sqrt(bigFloat(2)*halfSideLength*halfSideLength));
		vec3 ptLL_down = downscale(pt_LL);

		double_vec_ pt_UR = calcSurfacePoint(3 * PI / 4, pt, Sqrt(bigFloat(2)*halfSideLength*halfSideLength));
		vec3 ptUR_down = downscale(pt_UR);

		double_vec_ pt_LR = calcSurfacePoint(5 * PI / 4, pt, Sqrt(bigFloat(2)*halfSideLength*halfSideLength));
		vec3 ptLR_down = downscale(pt_LR);

		//debug first points
		printf("Center Point {%f, %f, %f}\n", pt_down.x, pt_down.y, pt_down.z);
		printf("Upper Left Point {%f, %f, %f}\n", ptUL_down.x, ptUL_down.y, ptUL_down.z);
		printf("Lower Left Point {%f, %f, %f}\n", ptLL_down.x, ptLL_down.y, ptLL_down.z);
		printf("Upper Right Point {%f, %f, %f}\n", ptUR_down.x, ptUR_down.y, ptUR_down.z);
		printf("Lower Right Point {%f, %f, %f}\n", ptLR_down.x, ptLR_down.y, ptLR_down.z);
		printf("LengthX UL UR: %f\n", distance(ptUL_down, ptUR_down));
		printf("LengthY UL LL: %f\n", distance(ptUL_down, ptLL_down));
		printf("LengthX LL LR: %f\n", distance(ptLL_down, ptLR_down));
		printf("LengthY UR LR: %f\n\n", distance(ptLR_down, ptUR_down));

		vertBuff[0] = ptUL_down.x;
		vertBuff[1] = ptUL_down.y;
		vertBuff[2] = ptUL_down.z;

		vertBuff[3] = ptLL_down.x;
		vertBuff[4] = ptLL_down.y;
		vertBuff[5] = ptLL_down.z;

		vertBuff[6] = ptUR_down.x;
		vertBuff[7] = ptUR_down.y;
		vertBuff[8] = ptUR_down.z;

		vertBuff[9] = ptLR_down.x;
		vertBuff[10] = ptLR_down.y;
		vertBuff[11] = ptLR_down.z;

		cout << "VBO after generation" << endl;
		printf("cam pos {%f, %f, %f}\n", myCam.getPos().x.ToFloat(), myCam.getPos().y.ToFloat(), myCam.getPos().z.ToFloat());
		cout << vertexBufferData[0] << endl;
		cout << vertexBufferData[1] << endl;
		cout << vertexBufferData[2] << endl;
		cout << vertexBufferData[3] << endl;
		cout << vertexBufferData[4] << endl;
		cout << vertexBufferData[5] << endl;
		cout << vertexBufferData[6] << endl;
		cout << vertexBufferData[7] << endl;
		cout << vertexBufferData[8] << endl;
		cout << vertexBufferData[9] << endl;
		cout << vertexBufferData[10] << endl;
		cout << vertexBufferData[11] << endl << endl;
	}

	void buildIndexBuffer(GLuint indexBuff[]) {
		int index = 0;
		int row = 1;
		/*for (int i = 0; i < (terrainWidth - 1) * (terrainDepth - 1) * 6; i += 6) {
			indexBuff[i] = index;
			indexBuff[i + 1] = index + 1;
			indexBuff[i + 2] = index + terrainWidth;

			indexBuff[i + 3] = index + terrainWidth;
			indexBuff[i + 4] = index + terrainWidth + 1;
			indexBuff[i + 5] = index + 1;

			if (index % ((int)terrainWidth * row - 2) == 0 && index != 0) {
				index += 2;
				row++;
			}

			else
				index++;
		}*/

		indexBuff[0] = 0;
		indexBuff[1] = 1;
		indexBuff[2] = 2;
		indexBuff[3] = 2;
		indexBuff[4] = 3;
		indexBuff[5] = 1;

	}

	//generate planet mesh (must be done every time the LODnum changes)
	void generateMesh() {
		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &VertexBufferID);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);

		//generate an array to store points (vec3s), generated as a LODnum x LODnum square of vec3s (* 3), then mirrored twice (*4)
		const int bufferSize = terrainNum * terrainNum * 4;
		double_vec_ * vertices = new double_vec_[bufferSize];

		//calculate new center point
		centerSurfacePoint = calcCenterSurfacePt();
		vec3 pt_down = downscale(centerSurfacePoint);

		//lower righthand point of the generated square
		const int LRindex = terrainNum * terrainNum - 1;
		
		//calculate upper left point from the center point, the lower righthand point of the generated square
		double_vec_ pt_UL = calcSurfacePoint(PI / 4, centerSurfacePoint, Sqrt(bigFloat(2)*halfSideLength*halfSideLength));
		vec3 ptUL_down = downscale(pt_UL);

		//generate base of the square, vertices (LRindex+1-terrainNum) -> LRindex
		vertices[LRindex] = pt_UL;
		for (int i = 1; i < terrainNum; i++) {
			vertices[LRindex - i] = calcSurfacePoint(0, vertices[LRindex - (i - 1)], PLANET_RADIUS);
		}

		//create the rest of the upper left square of base points
		for (int j = 0; j < terrainNum; j++) {
			for (int k = 1; k < terrainNum; k++) {
				vertices[(LRindex - j) - (terrainNum*k)] = calcSurfacePoint(PI/2, vertices[LRindex - j], PLANET_RADIUS);
			}
		}

		//TODO transform base points and inbetween points to {x, y, z} VBO
		const int VBOsize = bufferSize * 3 * LODnum;
		GLfloat * VBOvertices = new GLfloat[VBOsize];

		//TODO mirror points

		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, sizeof(VBOvertices), VBOvertices, GL_DYNAMIC_DRAW);

		//memory cleanup/management
		delete VBOvertices;
		delete vertices;
	}

	//recreate indexbuffer based on LOD and terrain level
	void generateMeshIndex() {
		glGenBuffers(1, &IndexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferID);
		const int bufferSize = terrainNum * terrainNum * LODnum * 6;
		GLuint * elements = new GLuint[bufferSize];

		/*for (int i = 0; i < (terrainWidth - 1) * (terrainDepth - 1) * 6; i += 6) {
			indexBuff[i] = index;
			indexBuff[i + 1] = index + 1;
			indexBuff[i + 2] = index + terrainWidth;

			indexBuff[i + 3] = index + terrainWidth;
			indexBuff[i + 4] = index + terrainWidth + 1;
			indexBuff[i + 5] = index + 1;

			if (index % ((int)terrainWidth * row - 2) == 0 && index != 0) {
				index += 2;
				row++;
			}

			else
				index++;
		}*/

		//generate index buffer
		int ind = 0;
		for (int i = 0; i < bufferSize; i += 6, ind += 6) {
			elements[i + 0] = ind;
			elements[i + 1] = ind + 1;
			elements[i + 2] = ind + terrainNum * 2;

			elements[i + 3] = ind + terrainNum * 2;
			elements[i + 4] = ind + terrainNum * 2 + 1;
			elements[i + 5] = ind + 1;
		}

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_DYNAMIC_DRAW);
		glBindVertexArray(0);
		delete elements;
	}

	void updateMesh() {
		generateMesh();
		updateMesh();
	}

	void updateGeom() {
		/*glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
		buildVertexBufferProcedural(vertexBufferData);

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertexBufferData), &vertexBufferData);*/

		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), NULL, GL_DYNAMIC_DRAW);

		buildVertexBufferProcedural(vertexBufferData);

		//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertexBufferData), &vertexBufferData);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_DYNAMIC_DRAW);

		// get pointer
		//void *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		//buildVertexBufferProcedural(vertexBufferData);

		//// now copy data into memory
		//memcpy(ptr, vertexBufferData, sizeof(vertexBufferData));
		//// make sure to tell OpenGL we're done with the pointer
		//glUnmapBuffer(GL_ARRAY_BUFFER);
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

		buildVertexBufferProcedural(vertexBufferData);
		//generateMesh();

		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_DYNAMIC_DRAW);

		//we need to set up the vertex array
		glEnableVertexAttribArray(0);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Create and bind IBO
		glGenBuffers(1, &IndexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferID);

		buildIndexBuffer(indexBufferData);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexBufferData), indexBufferData, GL_DYNAMIC_DRAW);
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
		if (myCam.moved) {
			updateGeom();
		}

		static double count = 0;
		double frametime = get_last_elapsed_time();
		count += frametime;

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

		//Draw our scene - two meshes - right now to a texture
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		glUniform1f(prog->getUniform("maxAltitude"), maxAltitude);
		glBindVertexArray(VertexArrayID);

		//draw triangles
		glDrawElements(GL_TRIANGLES, sizeof(indexBufferData), GL_UNSIGNED_INT, nullptr);

		if (myCam.moved) {
			cout << "VBO after draw" << endl;
			printf("cam pos {%f, %f, %f}\n", myCam.getPos().x.ToFloat(), myCam.getPos().y.ToFloat(), myCam.getPos().z.ToFloat());
			cout << vertexBufferData[0] << endl;
			cout << vertexBufferData[1] << endl;
			cout << vertexBufferData[2] << endl;
			cout << vertexBufferData[3] << endl;
			cout << vertexBufferData[4] << endl;
			cout << vertexBufferData[5] << endl;
			cout << vertexBufferData[6] << endl;
			cout << vertexBufferData[7] << endl;
			cout << vertexBufferData[8] << endl;
			cout << vertexBufferData[9] << endl;
			cout << vertexBufferData[10] << endl;
			cout << vertexBufferData[11] << endl << endl;
		}

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
	application->initGeom();

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
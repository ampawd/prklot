#define GLEW_STATIC

#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>


#include <iostream>
#include <algorithm>
#include <vector>
#include <list>
#include <fstream>
#include <cassert>
#include <string>
#include <stdexcept>

#include "shader_manager.h"
#include "parking_lot.h"

#define COUT std::cout <<

namespace prklot
{
	GLuint lotShaderProgram, carShaderProgram, cardndShaderProgram;
	GLuint projectionLoc, projectionLocCar, projectionLocCardnd;
	glm::vec2 WINDOW_SIZE;
	glm::vec2 WINDOW_PREV_SIZE;	
	glm::vec2 WINDOW_POSITION(2, 25);
	glm::vec2 mouseDown(0.0, 0.0);
	glm::vec3 shift(0.0, 0.0, 0.0);
	glm::vec3 pickedCarOriginalPos;
	glm::mat4 projection;

	int n, m;
	double percents;
	double allScaleFactorX, allScaleFactorY;
	double scaleX, scaleY, moffsetX, moffsetY;
	bool mouseLeftPressed = false;
	bool navigating = false;
	bool used[100000000] = { 0 };
	unsigned int slotW = 100, slotH = 100;
	unsigned int offsetInSlot = 50;
	unsigned int numberOfCars = 0;
	GLfloat zoomFactor = 1.0f;
	GLfloat windowScaleX = 1.0f;
	GLfloat windowScaleY = 1.0f;

	ShaderManager shaderManager;
	ParkingLot parkingLot;
	ParkingCar* pickedCar = nullptr;
	std::vector<ParkingCar> cars;
	std::vector<glm::vec3> visibleAreaPositions;

	struct isFIlled
	{
		glm::vec3 pos;
		bool operator == (const isFIlled& left) const
		{
			return left.pos.x == pos.x && left.pos.y == pos.y;
		}
	};

	std::list<isFIlled> filledSlots;

	void mouseCallback(GLFWwindow* window, int, int, int);
	void mouseMoveCallback(GLFWwindow* window, double, double);
	void mouseScrollCallback(GLFWwindow* window, double, double);
	void onWindowResizeCallback(GLFWwindow* window, int, int);
	void render();

	void mouseCallback(GLFWwindow* window, int button, int action, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			if (GLFW_PRESS == action)
			{
				mouseLeftPressed = true;
			}
			else if (GLFW_RELEASE == action)
			{
				mouseLeftPressed = false;
			}
		}

		if (mouseLeftPressed)
		{
			double startX, startY;
			glfwGetCursorPos(window, &startX, &startY);
			mouseDown.x = startX;
			mouseDown.y = startY;

			double a = WINDOW_SIZE.x - WINDOW_PREV_SIZE.x;
			double b = WINDOW_SIZE.y - WINDOW_PREV_SIZE.y;

			double mx = startX - WINDOW_SIZE.x / 2;
			double my = -startY + WINDOW_SIZE.y / 2;
			double w1, h1;

			scaleX = (WINDOW_PREV_SIZE.x + a) / WINDOW_PREV_SIZE.x;
			scaleY = (WINDOW_PREV_SIZE.y + b) / WINDOW_PREV_SIZE.y;
			allScaleFactorX = scaleX * zoomFactor;
			allScaleFactorY = scaleY * zoomFactor;

			pickedCar = nullptr;
			for (auto& car : cars)
			{
				w1 = car.getWidth()  * scaleX;
				h1 = car.getHeight() * scaleY;
				car.isDragging = false;

				if (mx >= allScaleFactorX * (car.leftBottom.x + shift.x) && mx <= allScaleFactorX * (car.leftBottom.x + w1 + shift.x) &&
					my >= allScaleFactorY * (car.leftBottom.y + shift.y) && my <= allScaleFactorY * (car.leftBottom.y + h1 + shift.y))
				{
					pickedCar = &car;
					moffsetX = mx / allScaleFactorX - pickedCar->leftBottom.x - shift.x;
					moffsetY = my / allScaleFactorY - pickedCar->leftBottom.y - shift.y;
					pickedCarOriginalPos = pickedCar->leftBottom;

					pickedCarOriginalPos.x = floor(pickedCarOriginalPos.x / slotW) * slotW + (n & 1 ? slotW / 2 : 0);
					pickedCarOriginalPos.y = floor(pickedCarOriginalPos.y / slotH) * slotH + (m & 1 ? slotH / 2 : 0);
					break;
				}
			}

			glfwSetCursorPosCallback(window, mouseMoveCallback);
		}
		else
		{
			if (pickedCar != nullptr)
			{
				pickedCar->leftBottom -= shift;

				isFIlled test;
				test.pos.x = floor( (pickedCar->leftBottom.x) / slotW) * slotW + (n & 1 ? slotW / 2 : 0);
				test.pos.y = floor( (pickedCar->leftBottom.y) / slotH) * slotH + (m & 1 ? slotH / 2 : 0);

				bool filled = false;

				for (auto& filledSlot : filledSlots)
				{
					if (filledSlot == test)
					{
						filled = true;
						pickedCarOriginalPos.x += offsetInSlot / 2;
						pickedCarOriginalPos.y += offsetInSlot / 2;
						pickedCar->setPosition(pickedCarOriginalPos);
						pickedCar->translate(shift);
						break;
					}
				}

				if (!filled)
				{
					filledSlots.push_back(test);
					isFIlled originPos;
					originPos.pos = pickedCarOriginalPos;
					for (auto& isf : filledSlots)
					{
						if (isf == originPos)
						{
							filledSlots.remove(isf);
							break;
						}
					}

					test.pos.x += offsetInSlot / 2;
					test.pos.y += offsetInSlot / 2;
					pickedCar->setPosition(test.pos);
					pickedCar->translate(shift);
				}
			}

			navigating = false;
			glfwSetCursorPosCallback(window, nullptr);
		}
	}

	void mouseMoveCallback(GLFWwindow* window, double currentMouseX, double currentMouseY)
	{
		navigating = true;
		if (pickedCar == nullptr)
		{
			shift.x += currentMouseX - mouseDown.x;
			shift.y += mouseDown.y - currentMouseY;
			parkingLot.translate(shift);

			for (auto& car : cars)
			{
				car.translate(shift);
			}
		}
		else
		{
			pickedCar->setPosition(
				glm::vec3((currentMouseX - WINDOW_SIZE.x / 2) / allScaleFactorX - moffsetX,
				(-currentMouseY + WINDOW_SIZE.y / 2) / allScaleFactorY - moffsetY,
					0.0
				)
			);
		}

		mouseDown.x = currentMouseX;
		mouseDown.y = currentMouseY;
	}

	void mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
	{
		if (yOffset > 0)
		{
			zoomFactor += 0.1;
		}
		if (yOffset < 0)
		{
			zoomFactor -= 0.1;
		}
		if (zoomFactor < 0.1)
		{
			zoomFactor = 0.1;
		}
		if (zoomFactor > 5)
		{
			zoomFactor = 5;
		}

		parkingLot.scaleProportionally(zoomFactor);

		for (auto& car : cars)
		{
			car.scaleProportionally(zoomFactor);
		}
	}

	void onWindowResizeCallback(GLFWwindow* window, int x, int y)
	{
		WINDOW_SIZE.x = x; WINDOW_SIZE.y = y;
		glViewport(0, 0, x, y);
		render();
		glfwSwapBuffers(window);
	}

	void render()
	{
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shaderManager.use(lotShaderProgram);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
		parkingLot.render(lotShaderProgram);


		shaderManager.use(carShaderProgram);
		glUniformMatrix4fv(projectionLocCar, 1, GL_FALSE, glm::value_ptr(projection));
		for (auto& car : cars)
		{
			car.render(carShaderProgram);
		}
	}

	int random(int min, int max)
	{
		static bool first = true;
		if (first)
		{
			srand(time(NULL));
			first = false;
		}
		return min + rand() % (max - min);
	}
};

using namespace prklot;

int main(int argc, char *argv[])
{
	std::cout << "Please specify parking lot sizes (example: 8 8): ";
	std::cin >> n >> m;
	if (n <= 0 || m <= 0)
		std::cerr << "Invalid value for sizes, both values should be greater or equal 1 and integers" << '\n';

	WINDOW_SIZE.x = n * slotW;	WINDOW_SIZE.y = m * slotH;
	WINDOW_PREV_SIZE.x = WINDOW_SIZE.x;	WINDOW_PREV_SIZE.y = WINDOW_SIZE.y;
	std::cout << "Please specify in percentage amount of cars to be filled in empty slots (example: 70): ";
	std::cin >> percents;

	if (!percents || percents < 0)
	{
		std::cerr << "Invalid value for percents" << '\n';
		return EXIT_FAILURE;
	}
	if (percents > 100)
	{
		std::cerr << "Percentage value should not be greater 100" << '\n';
		return EXIT_FAILURE;
	}

	if (!glfwInit())
	{
		std::cerr << "glfw initialization failed" << '\n';
		return EXIT_FAILURE;
	}

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	GLFWwindow* window = glfwCreateWindow((int)WINDOW_SIZE.x, (int)WINDOW_SIZE.y, "ParkingLot Problem", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "glfwOpenWindow failed. Can your hardware handle OpenGL 3.3?" << '\n';
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glfwSetMouseButtonCallback(window, mouseCallback);
	glfwSetScrollCallback(window, mouseScrollCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetWindowPos(window, (int)WINDOW_POSITION.x, (int)WINDOW_POSITION.y);
	glfwSetWindowSizeCallback(window, onWindowResizeCallback);
	glViewport(0, 0, (GLsizei)WINDOW_SIZE.x, (GLsizei)WINDOW_SIZE.y);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "glew Initialization failed" << '\n';
		return EXIT_FAILURE;
	}

	//	fix window dimensions when they bigger than screen width/height
	//GLsizei fitWindowWidth, fitWindowHeight;
	//glfwGetWindowSize(window, &fitWindowWidth, &fitWindowHeight);
	//if (WINDOW_SIZE.x > fitWindowWidth)
	//{
	//	WINDOW_SIZE.x = floor((fitWindowWidth / n)) * n;
	//	WINDOW_PREV_SIZE.x = WINDOW_SIZE.x;
	//}
	//if (WINDOW_SIZE.y > fitWindowHeight)
	//{
	//	WINDOW_SIZE.y = floor((fitWindowHeight / m) * m);
	//	WINDOW_PREV_SIZE.y = WINDOW_SIZE.y;
	//}
	//glViewport(0, 0, (GLsizei)WINDOW_SIZE.x, (GLsizei)WINDOW_SIZE.y);

	Shader vshader_parking_lot(GL_VERTEX_SHADER, "shaders/vshader_parking_lot");
	Shader fshader_parking_lot(GL_FRAGMENT_SHADER, "shaders/fshader_parking_lot");

	Shader vshader_parking_car(GL_VERTEX_SHADER, "shaders/vshader_parking_car");
	Shader fshader_parking_car(GL_FRAGMENT_SHADER, "shaders/fshader_parking_car");

	projection = glm::ortho(-WINDOW_SIZE.x / 2, WINDOW_SIZE.x / 2, -WINDOW_SIZE.y / 2, WINDOW_SIZE.y / 2);

	lotShaderProgram = shaderManager.buildProgram(vshader_parking_lot, fshader_parking_lot);
	carShaderProgram = shaderManager.buildProgram(vshader_parking_car, fshader_parking_car);

	projectionLoc = glGetUniformLocation(lotShaderProgram, "projection");
	projectionLocCar = glGetUniformLocation(carShaderProgram, "projection");

	parkingLot.setVisibleArea(WINDOW_SIZE);
	parkingLot.initPositions(slotW, slotH);
	parkingLot.load();
	visibleAreaPositions = parkingLot.getVisibeAreaPositions();
	numberOfCars = (visibleAreaPositions.size() * percents / 100);	 

	while (cars.size() < numberOfCars)
	{
		int randIndex = random(0, visibleAreaPositions.size());
		if (!used[randIndex])
		{
			used[randIndex] = true;

			isFIlled isf;
			isf.pos = visibleAreaPositions[randIndex];
			filledSlots.push_back(isf);

			glm::vec3 carPos(
				visibleAreaPositions[randIndex].x + offsetInSlot / 2,
				visibleAreaPositions[randIndex].y + offsetInSlot / 2,
				0
			);			

			ParkingCar parkingCar(
				carPos,
				glm::vec3((double)rand() / (double)(RAND_MAX),
						(double)rand() / (double)(RAND_MAX),
						(double)rand() / (double)(RAND_MAX)
				),

				slotW - offsetInSlot + random(-10, 10),
				slotH - offsetInSlot + random(-10, 10),
				randIndex
			);

			parkingCar.create();
			parkingCar.load();
			cars.push_back(parkingCar);
		}
	}


	//	rendering loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		render();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return EXIT_SUCCESS;
}
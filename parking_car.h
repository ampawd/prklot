#ifndef PARKING_CAR
#define PARKING_CAR

#define GLEW_STATIC

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>
#include <iostream>
#include <vector>

namespace prklot
{
#define POSITION_LOCATION 0

	class ParkingCar
	{
	public:
		ParkingCar(const glm::vec3& = glm::vec3(0.0f, 0.0f, 0.0f),
			const glm::vec3& = glm::vec3(0.0f, 0.0f, 0.0f),
			int = 100,
			int = 100,
			int = 0);

		void render(GLuint);
		void scaleProportionally(GLfloat);
		void translate(const glm::vec3& = glm::vec3(0.0, 0.0, 0.0));
		void setPosition(const glm::vec3&);
		unsigned int getWidth() const;
		unsigned int getHeight() const;
		void setWidth(unsigned int);
		void setHeight(unsigned int);
		int index;
		void create();
		void load();
		//updateModelMatrix();
		glm::vec3 carFillColor, leftBottom;
		bool isDragging;

	private:
		glm::mat4 model, translation, scale, rotation;
		int width, height;
		GLuint vao, vbo, ebo;
		std::vector<GLfloat> vertices;
		std::vector<GLuint> indices;
	};

};

#endif
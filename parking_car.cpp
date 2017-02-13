#include "parking_car.h"

using namespace prklot;

ParkingCar::ParkingCar(const glm::vec3& leftBottom, const glm::vec3& carFillColor, int width, int height, int index) :
	leftBottom(leftBottom),
	carFillColor(carFillColor),
	width(width),
	height(height),
	index(index),
	isDragging(false)
{
	translation = glm::translate(glm::mat4(1.0f), leftBottom);
}

void ParkingCar::render(GLuint program)
{
	model = scale * translation;

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniform3f(glGetUniformLocation(program, "carFillColor"), carFillColor.x, carFillColor.y, carFillColor.z);
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void ParkingCar::scaleProportionally(GLfloat zoomFactor)
{
	scale = glm::scale(glm::mat4(1.0f), glm::vec3(zoomFactor, zoomFactor, zoomFactor));
}

void ParkingCar::translate(const glm::vec3& shift)
{
	translation = glm::translate(glm::mat4(1.0f), leftBottom + shift);
}

void ParkingCar::setPosition(const glm::vec3& newLeftBottom)
{
	leftBottom.x = newLeftBottom.x;
	leftBottom.y = newLeftBottom.y;
	leftBottom.z = newLeftBottom.z;
	translate();
}

unsigned int ParkingCar::getWidth() const
{
	return width;
}

unsigned int ParkingCar::getHeight() const
{
	return height;
}

void ParkingCar::setWidth(unsigned int newWidth)
{
	width = newWidth;
}

void ParkingCar::setHeight(unsigned int newHeight)
{
	height = newHeight;
}

void ParkingCar::create()
{
	vertices.clear(); indices.clear();

	vertices.push_back(static_cast<GLfloat>(0));
	vertices.push_back(static_cast<GLfloat>(0));
	vertices.push_back(static_cast<GLfloat>(0));

	vertices.push_back(static_cast<GLfloat>(width));
	vertices.push_back(static_cast<GLfloat>(0));
	vertices.push_back(static_cast<GLfloat>(0));

	vertices.push_back(static_cast<GLfloat>(width));
	vertices.push_back(static_cast<GLfloat>(height));
	vertices.push_back(static_cast<GLfloat>(0));

	vertices.push_back(static_cast<GLfloat>(0));
	vertices.push_back(static_cast<GLfloat>(height));
	vertices.push_back(static_cast<GLfloat>(0));

	indices.push_back(0);	indices.push_back(1);	indices.push_back(2);
	indices.push_back(0);	indices.push_back(2);	indices.push_back(3);
}

void ParkingCar::load()
{
	glDeleteBuffers(1, &vao); glDeleteBuffers(1, &vbo); glDeleteBuffers(1, &ebo);
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &ebo);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices), &indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(POSITION_LOCATION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
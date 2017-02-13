#include "parking_lot.h"

#include <iostream>

using namespace parking_task;

ParkingLot::ParkingLot() : loaded(false)
{}

void ParkingLot::render(GLuint program)
{
	model = scale * translation;
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(vao);
	glDrawArrays(GL_LINES, 0, vertices.size() / 3);
	glBindVertexArray(0);
}

void ParkingLot::scaleProportionally(GLfloat zoomFactor)
{
	if (!loaded) return;

	scale = glm::scale(glm::mat4(1.0f), glm::vec3(zoomFactor, zoomFactor, zoomFactor));
}

void ParkingLot::translate(const glm::vec3& shift)
{
	if (!loaded) return;

	translation = glm::translate(glm::mat4(1.0f), glm::vec3(shift.x, shift.y, shift.z));
}

std::vector<glm::vec3> ParkingLot::getVisibeAreaPositions() const
{
	return visibleArea;
}

void ParkingLot::setVisibleArea(glm::vec2 size)
{
	visibleAreaDimensions.x = size.x; visibleAreaDimensions.y = size.y;
}

void ParkingLot::initPositions(unsigned int slotW, unsigned int slotH)
{
	visibleArea.clear();
	vertices.clear();
	int x, y;
	for (x = -visibleAreaDimensions.x * 2.5; x < visibleAreaDimensions.x * 2.5; x += slotW)
	{
		for (y = -visibleAreaDimensions.y * 2.5; y < visibleAreaDimensions.y * 2.5; y += slotH)
		{
			//	vertical line
			vertices.push_back(static_cast<GLfloat>(x));
			vertices.push_back(static_cast<GLfloat>(visibleAreaDimensions.y * 2.5));
			vertices.push_back(0.0f);
			vertices.push_back(static_cast<GLfloat>(x));
			vertices.push_back(static_cast<GLfloat>(-visibleAreaDimensions.y * 2.5));
			vertices.push_back(0.0f);

			//	horizontal line
			vertices.push_back(static_cast<GLfloat>(visibleAreaDimensions.x * 2.5));
			vertices.push_back(static_cast<GLfloat>(y));
			vertices.push_back(0.0f);
			vertices.push_back(static_cast<GLfloat>(-visibleAreaDimensions.x * 2.5));
			vertices.push_back(static_cast<GLfloat>(y));
			vertices.push_back(0.0f);

			allPositions.push_back(glm::vec3(x, y, 0.0));

			if (x >= -visibleAreaDimensions.x / 2 && x < visibleAreaDimensions.x / 2
				&& y >= -visibleAreaDimensions.y / 2 && y < visibleAreaDimensions.y / 2)
			{
				visibleArea.push_back(glm::vec3(x, y, 0));
			}
		}
	}
}

std::vector< glm::vec3 > ParkingLot::getAllPositions() const
{
	return allPositions;
}

void ParkingLot::load()
{
	if (vertices.size() == 0)
	{
		loaded = false;	return;
	}

	glDeleteBuffers(1, &vao); glDeleteBuffers(1, &vbo);
	//	load vertices data to GPU
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(POSITION_LOCATION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	loaded = true;
}
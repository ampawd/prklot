#ifndef PARKING_LOT
#define PARKING_LOT

#define GLEW_STATIC

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <map>
#include <stdexcept>

#include "parking_car.h"

namespace prklot
{
#define POSITION_LOCATION 0

	class ParkingLot
	{
	public:
		ParkingLot();
		void load();
		void render(GLuint);
		void scaleProportionally(GLfloat);
		void translate(const glm::vec3& diff);
		std::vector<glm::vec3> getVisibeAreaPositions() const;
		void setVisibleArea(glm::vec2);
		void initPositions(unsigned int, unsigned int);
		std::vector< glm::vec3 > getAllPositions() const;

	private:
		glm::mat4 model, translation, scale, rotation;
		glm::vec2 visibleAreaDimensions;
		std::vector< glm::vec3 > allPositions;
		GLuint vbo, vao;
		std::vector<ParkingCar> cars;
		std::vector<GLfloat> vertices;
		std::vector<glm::vec3> visibleArea;
		bool loaded;
	};

};

#endif // !PARKING_LOT
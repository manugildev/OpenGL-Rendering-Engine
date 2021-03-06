#include "DirLight.h"

DirLight::DirLight(Application* app, glm::vec3 direction, glm::vec3 light_color) : Light(app, direction * -50.0f, light_color), direction(direction) {
	this->model = new Model("models/cube.obj");
	glm::mat4 look_at = glm::lookAt(glm::vec3(0.0f), direction, glm::vec3(0.0f, 1.0f, 0.0f));
	this->set_model_mat(this->get_model_mat() * look_at);
}

glm::vec3 DirLight::get_direction() {
	return this->direction;
}

DirLight::~DirLight() {}
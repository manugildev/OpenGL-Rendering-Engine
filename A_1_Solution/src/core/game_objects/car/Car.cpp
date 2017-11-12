#include "Car.h"
#include <glm\ext.hpp>

Car::Car(Application * app, Model* model, glm::vec3 object_color, Texture* texture) : GameObject(app, model, object_color, texture) {
	this->initial_position = this->get_pos();
}

void Car::update(float delta_time) {
	GameObject::update(delta_time);
	this->distance_traveled = glm::length(this->get_pos() - this->initial_position);
	if (this->distance_traveled > city_size - 5 && !animation_running) {
		start_turn_around_animation();
	}
	if (animation_running && std::abs(this->get_rotation()[1] - this->initial_rotation_y) > (180.0f)) {
		stop_turn_around_animation();
	}
}


void Car::set_initial_pos(glm::vec3 pos) {
	this->initial_position = pos;
	this->set_pos(pos);
}

void Car::start_turn_around_animation() {
	animation_running = true;
	initial_rotation_y = get_rotation()[1];
	initial_speed = get_speed();

	if (get_speed()[2] > 0)
		set_pos(glm::vec3(get_pos()[0], get_pos()[1], city_size - 4.999f));
	else if (get_speed()[2] < 0)
		set_pos(glm::vec3(get_pos()[0], get_pos()[1], - city_size + 4.999f));
	else if (get_speed()[0] > 0)
		set_pos(glm::vec3(city_size - 4.999f, get_pos()[1], get_pos()[2]));
	else if (get_speed()[0] < 0)
		set_pos(glm::vec3(-city_size + 4.999f, get_pos()[1], get_pos()[2]));

	previous_random_mutiplier = std::rand() < RAND_MAX / 2 ? 1.0f : -1.0f;
	set_speed(glm::vec3(0.0f));
	set_acceleration(glm::vec3(0.0f));
	set_rotation_acceleration(glm::vec3(0.0f, (std::rand() % 120 + 50) * previous_random_mutiplier, 0.0f));
}

void Car::stop_turn_around_animation() {
	set_rotation(glm::vec3(0, this->initial_rotation_y + (180* previous_random_mutiplier), 0));
	initial_rotation_y = get_rotation()[1];
	set_rotation_speed(glm::vec3(0.0f));
	set_rotation_acceleration(glm::vec3(0.0f));
	set_speed(initial_speed * -1.0f);
	animation_running = false;
}

Car::~Car() {}

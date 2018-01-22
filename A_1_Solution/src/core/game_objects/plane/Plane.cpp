#include "Plane.h"
#include "../../Application.h"
#include <glm\gtc\matrix_transform.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <tween\tween.h>



Plane::Plane(Application * app, Model* model, Camera* camera) : GameObject(app, model) {
	camera = app->get_camera();

	this->attach_propeller(app);
	this->attach_wheels(app);
	this->attach_arrows(app);

	this->transform = glm::angleAxis(glm::radians(-90.0f), forwardVector); // We init the transform quaternion to 0.0f in the forward vector
	this->transform = transform * glm::angleAxis(glm::radians(90.0f), barrel);
}

void Plane::attach_wheels(Application * app) {
	Model* wheels_model = new Model("models/plane/wheels.obj");
	wheels = new GameObject(app, wheels_model);
	wheels->set_pos(glm::vec3(0.0f, -1.02f, 1.7f));
	wheels->set_parent(this);
	wheels->set_scale(glm::vec3(1.0f, 1.1f, 1.1f));
	wheels->set_rotation_speed(glm::vec3(300.0f, 00.0f, 0.0f));
}

void Plane::attach_propeller(Application * app) {
	Model* propeller_model = new Model("models/plane/propeller.obj");
	propeller = new GameObject(app, propeller_model);
	propeller->set_pos(glm::vec3(0.0f, -0.0f, 3.19f));
	propeller->set_parent(this);
	propeller->set_scale(glm::vec3(1.1f, 1.1f, 1.1f));
	propeller->set_rotation_speed(glm::vec3(0.0f, 0.0f, 900.0f));
}

void Plane::attach_arrows(Application * app) {
	Model* green_arrow_model = new Model("models/green_arrow.obj");
	Model* blue_arrow_model = new Model("models/blue_arrow.obj");
	Model* red_arrow_model = new Model("models/red_arrow.obj");

	green_arrow = new GameObject(app, green_arrow_model, glm::vec3(0.0f, 1.0f, 0.0f));
	green_arrow->set_pos(glm::vec3(0.0f, 0.0f, 0.0f));
	green_arrow->set_scale(glm::vec3(3.45f, 3.45f, 3.45f));
	green_arrow->set_rotation(glm::vec3(0, 0, 0));

	blue_arrow = new GameObject(app, green_arrow_model, glm::vec3(0.0f, 0.0f, 1.0f));
	blue_arrow->set_pos(glm::vec3(0.0f, 0.0f, 0.0f));
	blue_arrow->set_parent(green_arrow);
	blue_arrow->set_scale(glm::vec3(0.90f, 0.90f, 0.90f));
	blue_arrow->set_rotation(glm::vec3(90, 0, -90));

	red_arrow = new GameObject(app, green_arrow_model, glm::vec3(1.0f, 0.0f, 0.0f));
	red_arrow->set_pos(glm::vec3(0.0f, 0.0f, 0.0f));
	red_arrow->set_parent(blue_arrow);
	red_arrow->set_scale(glm::vec3(0.90f, 0.90f, 0.90f));
	red_arrow->set_rotation(glm::vec3(-90, 0, -90));
}

void Plane::update(float delta_time) {

	elapsed += delta_time;
	tween::Tween::updateTweens(elapsed);

	if (animation_start) {
		transform = glm::slerp(transform, new_transform, interpolation_value);
		if (interpolation_value >= to) animation_start = false;

	}

	this->set_parent(red_arrow);
	green_arrow->set_pos(glm::vec3(0, 25, 0));
	set_pos(glm::vec3(0, 0, 0));


	/* Update Following the Hierarchy that we have decided*/
	green_arrow->update(delta_time);
	blue_arrow->update(delta_time);
	red_arrow->update(delta_time);

	GameObject::update(delta_time);
	if (with_quaternions) {
		this->update_quaternion_speed(delta_time);
		this->set_quaternion(transform);
		this->update_model_mat();
	}
	if (app->get_camera()->first_person) {
		app->get_camera()->set_parent_model_mat(get_model_mat());
	}


	wheels->update(delta_time);
	propeller->update(delta_time);
}

void Plane::render() {
	GameObject::render();
	wheels->render();
	propeller->render();

	if (show_debug) {
		green_arrow->render();
		blue_arrow->render();
		red_arrow->render();
	}
}

void Plane::set_shader_program(LightingShader * shader_program) {
	GameObject::set_shader_program(shader_program);
	this->wheels->set_shader_program(shader_program);
	this->propeller->set_shader_program(shader_program);
	this->green_arrow->set_shader_program(shader_program);
	this->green_arrow->set_mix_power(1.0f);
	this->blue_arrow->set_shader_program(shader_program);
	this->blue_arrow->set_mix_power(1.0f);
	this->red_arrow->set_shader_program(shader_program);
	this->red_arrow->set_mix_power(1.0f);
}

GameObject* Plane::get_green_arrow() {
	return this->green_arrow;
}
GameObject* Plane::get_blue_arrow() {
	return this->blue_arrow;
}
GameObject* Plane::get_red_arrow() {
	return this->red_arrow;
}

void Plane::start_roll() {
	if (!with_quaternions) return;

	if (animation_start) return;

	interpolation_value = 0;
	tween::Tween::make().to(interpolation_value, to).seconds(1.0);
	axis1 = glm::angleAxis(glm::radians(180.0f), forwardVector);
	barrel = axis1 * barrel;
	normal = axis1 * normal;
	axis1 = glm::normalize(axis1);
	new_transform = glm::normalize(transform);
	new_transform = new_transform * axis1;
	animation_start = true;

}


void Plane::start_yaw() {
	if (!with_quaternions) return;
	if (animation_start) return;

	interpolation_value = 0;
	tween::Tween::make().to(interpolation_value, to).seconds(1.0);
	axis2 = glm::angleAxis(glm::radians(180.0f), barrel);
	axis2 = glm::normalize(axis2);
	new_transform = glm::normalize(transform);
	new_transform = new_transform * axis2;
	animation_start = true;

}

void Plane::update_quaternion_speed(float delta_time) {
	/* Along Y Axis*/
	axis3 = glm::angleAxis(glm::radians(quaternion_speed.y*delta_time), normal);
	axis3 = glm::normalize(axis3);
	transform = glm::normalize(transform);
	transform = transform * axis3;

	/* Along Z Axis*/
	axis1 = glm::angleAxis(glm::radians(quaternion_speed.z*delta_time), forwardVector);
	barrel = axis1 * barrel;
	normal = axis1 * normal;
	axis1 = glm::normalize(axis1);
	transform = glm::normalize(transform);
	transform = transform * axis1;

	/* Along X Axis*/
	axis2 = glm::angleAxis(glm::radians(quaternion_speed.x*delta_time), barrel);
	axis2 = glm::normalize(axis2);
	transform = glm::normalize(transform);
	transform = transform * axis2;


}

void Plane::set_speed_x(float speed) {
	this->quaternion_speed = glm::vec3(speed, quaternion_speed.y, quaternion_speed.z);
}

void Plane::set_speed_y(float speed) {
	this->quaternion_speed = glm::vec3(quaternion_speed.x, speed, quaternion_speed.z);
}

void Plane::set_speed_z(float speed) {
	this->quaternion_speed = glm::vec3(quaternion_speed.x, quaternion_speed.y, speed);
}


Plane::~Plane() {

}

#pragma once
#include "..\GameObject.h"
#include <tween\tween.h>
#include <glm/gtx/matrix_decompose.hpp>

class Application;

class PlaneDemo : public GameObject {
public:
	PlaneDemo(Application* app, Model* model, Camera* camera);
	void attach_wheels(Application * app);
	void attach_propeller(Application * app);
	void update(float delta_time) override;
	void render() override;
	void set_shader_program(LightingShader* shader_program) override;

	bool show_debug = false;

	~PlaneDemo();

private:
	GameObject * propeller;
	GameObject * wheels;
};

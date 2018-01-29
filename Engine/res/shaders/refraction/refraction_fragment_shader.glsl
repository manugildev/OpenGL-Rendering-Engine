#version 450

struct DirLight {
	vec3 direction;
	vec3 light_color;
};

out vec4 frag_color;

in vec3 frag_pos;
in vec3 eye_dir;
in vec3 normal;
in vec2 tex_coords;
in vec3 reflected_vector;

uniform sampler2D texture_0;
uniform samplerCube environment_map;
uniform DirLight dir_light;
uniform vec3 object_color = vec3(1,1,1);

// Refraction and reflective properties
uniform float ambient_brightness = 0.4;
uniform float ior = 1.0f;
uniform float chromatic_offset = 0.05f;
uniform bool fresnel = false;
uniform float refractive_power = 8;
uniform float refractive_factor = 1;
uniform float refractive_strength = 0.9;

void main(){
	float brightness = max(dot(-dir_light.direction, normalize(normal)), 0.0) + ambient_brightness;
	vec4 result = texture(texture_0, tex_coords) * brightness;
	// Without texture
	result = vec4(object_color, 1.0f) * brightness * vec4(dir_light.light_color, 1.0f);
	
	vec4 reflected_color = texture(environment_map, reflected_vector);

	vec3 refraction_r = refract(-eye_dir, normalize(normal), ior + chromatic_offset);
	vec3 refraction_g = refract(-eye_dir, normalize(normal), ior);
	vec3 refraction_b = refract(-eye_dir, normalize(normal), ior - chromatic_offset);

	vec4 refracted_color;
	refracted_color.r = texture(environment_map, refraction_r).r;
	refracted_color.g = texture(environment_map, refraction_g).g;
	refracted_color.b = texture(environment_map, refraction_b).b;
		
	vec3 view_vector = normalize(eye_dir);
	float refractive_factor;
	if(fresnel){
		refractive_factor = dot(view_vector, normalize(normal));	
		refractive_factor = pow(refractive_factor, refractive_power);
	}

	vec4 enviro_color = mix(reflected_color, refracted_color,  refractive_factor);


	frag_color = mix(result, enviro_color, refractive_strength);

}
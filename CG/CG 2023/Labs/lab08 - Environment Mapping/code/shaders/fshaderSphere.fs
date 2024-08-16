#version 330 core

in vec3 fVertex;
in vec3 fNormal;

uniform mat4 model_inv;
uniform samplerCube cube_map;
out vec4 fragColor;

void main(void) { 
       //material color
       //fragColor = vec4(abs(normalize(fVertex)), 1.0);
       vec4 materialColor = 0.4*vec4(abs((fNormal+1)/2), 1.0);
	
       //reflected color
       vec3 incident = normalize(-fVertex);
       vec4 reflected = model_inv*vec4(reflect(incident, fNormal),0.0);
       vec4 reflectedColor = texture(cube_map, vec3(reflected));
	
	
	//TODO
        float mu_1 = 1.0;
        float mu_2 = 1.5;
        //refraction
        vec4 refracted = model_inv * vec4(refract(incident, fNormal, mu_1/mu_2), 0.0);
        vec4 refractedColor = texture(cube_map, vec3(refracted));
	
	// Schlicks approximation
        float R0 = pow((mu_2-mu_1)/(mu_2+mu_1), 2.0);
        float R = R0 + (1.0-R0)*pow(1.0-dot(-incident, fNormal), 5.0);
        vec4 finalColor = R*reflectedColor + (1.0-R)*refractedColor;
	
	//final color
        fragColor = finalColor;
}

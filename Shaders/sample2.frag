//Point light

#version 330 core //Version

uniform sampler2D tex0;

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform float ambientStr;
uniform vec3 ambientColor;

uniform vec3 cameraPos;
uniform float specStr;
uniform float specPhong;
uniform float add_int;

in vec2 texCoord;
in vec3 normCoord;
in vec3 fragPos;

out vec4 fragColor; //Color of the Pixel

void main(){
	//fragColor = vec4(0.7f, 0.4f, 0.7f, 1.0f);

	vec3 normal = normalize(normCoord);
	vec3 lightDir = normalize(lightPos - fragPos);

	float diff = max(
		dot(normal, lightDir), 0.0f
	);

	//vec3 diffuse = diffuseIntensity * diff * lightColor;
	// specCol, diffuse, ambient

	vec3 diffuse = diff * lightColor;

	vec3 ambientCol = ambientStr * ambientColor;

	vec3 viewDir = normalize(cameraPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);

	float spec = pow(max(dot(reflectDir, viewDir), 0.1), specPhong);

	vec3 specColorTest = vec3(1,3,0);

	//vec3 specCol = spec * specStr * vec3(1,1,1);
	vec3 specCol = spec * specStr * lightColor;
	
	// retrieving the distance by calculating the difference vector between the fragment and the light source
	// to be used in the formula later which is intensity = 1 / (distance * distance)
	float distance = length(lightPos - fragPos);
	
	// the formula for instensity of the light
	float intensity = 1.0 / (distance * distance);
	
	//intensity = intensity + add_int;

	// adding the intensity value in lighting calculations (ambientCol, diffuse, specCol)
	specCol *= intensity;  

	//vec3 diffuse = intensity * diff * lightColor;

	fragColor = vec4(specCol + diffuse + ambientCol, 1.0f) * texture(tex0, texCoord);
}
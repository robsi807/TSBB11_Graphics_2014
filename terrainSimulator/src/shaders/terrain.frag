#version 150

// terrain.frag
// Author: Peter Thulin

in vec2 texCoord;
in vec3 exNormal;
in vec3 surf;
in vec3 exPosition;
in vec3 terrainNormal;
in vec3 terrainPosition;

out vec4 outColor;

uniform vec3 lightDirection;
uniform float specularExponent;
uniform float distanceFogConstant;

uniform mat4 world2View;
uniform mat4 mdl2World;

uniform sampler2D grassTex;
uniform sampler2D rockTex1;
uniform sampler2D rockTex2;
uniform sampler2D noiseTex;

// Beta should be > 0.0
float cosInterpolate(float x,float beta){
      float pi = 3.1415927;
      float T = 1.0;
      float res = 0.0;
      if(x <= (1-beta)/(2*T))
      {
      	   res = 1.0;
      } 
      else if (x > (1-beta)/(2*T) && x <= (1+beta)/(2*T))
      {
      	   res = T/2 * (1 + cos(pi*T/beta * (x-(1-beta)/(2*T))));      
	   }

      return res;
}

vec4 calculateColor()
{	
	// Calculate slope
	float wSlope = clamp(1.2-terrainNormal.y,0,1); // Shortcut for dot product with y-axis!
	
	float texScale = 9.0; // Scale up the texture coordinates
	vec4 grass1 = texture(grassTex,texCoord / texScale);
	vec4 rock1 = texture(rockTex1,texCoord / texScale);
	vec4 rock2 = texture(rockTex2,texCoord / texScale);	
	vec4 noise1 = texture(noiseTex,texCoord / 500);	

	float perlNoise = noise1.x;
	float wRock = (perlNoise + 1) * 0.5;

	// Weight the rock textures
	vec4 rock = cosInterpolate(wRock,0.2)*rock1 +
	cosInterpolate(1-wRock,0.2)*rock2; 

	return cosInterpolate(wSlope,0.3)*grass1 + cosInterpolate(1-wSlope,0.3)*rock;
}

// Simply fades to gray
vec4 applyDistanceFog(vec4 rgb){
     // fogColor should ideally be calculate from the skybox
     const vec4 fogColor = vec4(.6,.87,0.99,1.0);
     float dist = length(exPosition);
     float fogAmount = clamp(dist*distanceFogConstant,0.0,1.0);
     float fogWeight = cosInterpolate(fogAmount,0.7);
     return fogWeight*rgb + (1-fogWeight)*fogColor;
}

void main(void)
{

	float shade,diffuseShade;
	vec3 reflectedLightDirection,eyeDirection,lightDir;
	vec3 normalizedNormal = normalize(exNormal);
	lightDir = normalize(mat3(world2View)*lightDirection); 

	float specularStrength = 0.0;
	diffuseShade = max(dot(normalizedNormal, lightDir),0.01);

	if(dot(normalizedNormal,lightDir) > 0.0)
	{
		reflectedLightDirection = reflect(normalize(-lightDir),normalize(exNormal));
		eyeDirection = -normalize(surf);

		specularStrength = dot(reflectedLightDirection, eyeDirection);
		specularStrength = max(specularStrength, 0.01);
		specularStrength = pow(specularStrength, specularExponent);
	}

	shade = (0.7*diffuseShade + 0.3*specularStrength);
	vec4 color = applyDistanceFog(shade*calculateColor());
	outColor = clamp(color, 0,1);
	  
}

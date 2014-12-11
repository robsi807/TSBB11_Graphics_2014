#version 150

in vec2 texCoord;
in vec3 exNormal;
in vec3 surf;

out vec4 outColor;

uniform vec3 lightDirection;
uniform float specularExponent;

uniform int evader;
uniform sampler2D evaderTexture;
uniform sampler2D chaserTexture;

uniform mat4 world2View;
uniform mat4 mdl2World;

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

	shade = 0.7*diffuseShade + 0.3*specularStrength;

	if(evader == 1)
	   outColor = applyDistanceFog(clamp(shade*texture(evaderTexture, texCoord),0,1));
	else
	   outColor = applyDistanceFog(clamp(shade*texture(chaserTexture, texCoord),0,1));
}

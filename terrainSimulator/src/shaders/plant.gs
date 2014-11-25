#version 150

layout(triangles) in;
layout(triangle_strip, max_vertices = 12) out;

in vec3 exNormal[3];
in vec3 exPosition[3];
in vec2 texCoord[3];
out vec2 texCoordG;
out vec3 exNormalG;
out vec3 exPositionG;

uniform mat4 projMatrix;
uniform mat4 mdl2World;
uniform mat4 world2View;
uniform float time;
uniform sampler2D noiseTex;

void createGrassBladeSimple(vec4 position,vec3 normal,vec2 texCord,float height,float angle1,float angle2){
    texCoordG = texCord; 
    exNormalG = normal;
    vec4 newPos = position;
    exPositionG = vec3( world2View * mdl2World * newPos);
    gl_Position = projMatrix * vec4(exPositionG,1.0);
    EmitVertex();
    
    texCoordG = texCord+vec2(0.1,0.1); 
    exNormalG = normal;
    newPos = position;
     exPositionG = vec3( world2View * mdl2World * newPos);
    gl_Position = projMatrix * vec4(exPositionG,1.0);
    gl_Position.x += 0.2;
    EmitVertex();

    texCoordG = texCord; 
    exNormalG = normal;
    newPos = position;
    newPos.y += cos(angle1)*height;
    newPos.x += sin(angle1)*height;
    newPos.z += sin(angle2)*height;
    exPositionG = vec3( world2View * mdl2World * newPos);
    gl_Position = projMatrix * vec4(exPositionG,1.0);
    EmitVertex();
    EndPrimitive();
}

void main(){
    
    mat3 normalMatrix = mat3(world2View * mdl2World);
    vec3 normal0 = normalMatrix * exNormal[0];
    vec4 cameraCoord = projMatrix * vec4(exPosition[0],1.0);
    
    
    if(slope < 0.5 && cameraCoord.z > 0.0 ){

        // Level-of-detail parameters
        float zLod0 = 100;
        float zLod1 = 200;
        float zLod2 = 400;
        float zLod3 = 800;
        float xLod0 = 120;
        float xLod1 = 160;
        float xLod2 = 520;
        float xLod3 = 1024;
        
        // Grass parameters
        float height = 3.5;
        float heightScale = 1;
        float angleScale = 3.14 / 6;
        float timeScale = 0.5;
        float posScale = 0.5;
        float scaleNoise = 3.5;
        
        vec4 pos0,pos1,pos2;
        pos0 = gl_in[0].gl_Position;
        pos1 = gl_in[1].gl_Position;
        pos2 = gl_in[2].gl_Position;
                
        vec2 tex0,tex1,tex2;
        tex0 = texCoord[0];
        tex1 = texCoord[1];
        tex2 = texCoord[2];
        
        if(cameraCoord.z < zLod0 && cameraCoord.x > -xLod0 && cameraCoord.x < xLod0){
        // zLod 0
            vec3 rgbNoise1 = 2*vec3(texture(noiseTex,tex0/scaleNoise))-1;
            vec3 rgbNoise2 = 2*vec3(texture(noiseTex,1.5*(tex1/scaleNoise)))-1;
            vec3 rgbNoise3 = 2*vec3(texture(noiseTex,2*(tex2/scaleNoise)))-1;
            vec3 rgbNoise4 = 2*vec3(texture(noiseTex,0.5*(tex0/scaleNoise)))-1;
            
            vec4 grPos0 = mix(mix(pos0,pos1,0.4+posScale*rgbNoise1.x),pos2,0.5+posScale*rgbNoise1.y);
            vec4 grPos1 = mix(mix(pos0,pos1,0.5+posScale*rgbNoise1.z),pos2,0.5+posScale*rgbNoise2.x);
            vec4 grPos2 = mix(mix(pos0,pos1,0.5+posScale*rgbNoise2.y),pos2,0.5+posScale*rgbNoise2.z);
            vec4 grPos3 = mix(mix(pos0,pos1,0.5+posScale*rgbNoise3.x),pos2,0.5+posScale*rgbNoise3.y);        
            vec4 grPos4 = mix(mix(pos0,pos1,0.5+posScale*rgbNoise3.z),pos2,0.5+posScale*rgbNoise4.x);
            
            createGrassBladeSimple(grPos0,normal0,tex0,height+heightScale*rgbNoise1.x,angleScale*rgbNoise1.x,angleScale*rgbNoise1.y);
            createGrassBladeSimple(grPos1,normal0,tex0,height+heightScale*rgbNoise1.y,angleScale*rgbNoise1.z,angleScale*rgbNoise2.x);
            createGrassBladeSimple(grPos2,normal0,tex0,height+heightScale*rgbNoise1.z,angleScale*rgbNoise2.y,angleScale*rgbNoise2.z);
            createGrassBladeSimple(grPos3,normal0,tex0,height+heightScale*rgbNoise2.x,angleScale*rgbNoise3.x,angleScale*rgbNoise3.y);
            createGrassBladeSimple(grPos4,normal0,tex0,height+heightScale*rgbNoise2.y,angleScale*rgbNoise3.z,angleScale*rgbNoise3.z);
            
        }
 
    }
}

/*
void main()
{
  float grassHeight = 2.0;
  for(int i = 0; i < 1; i++){

    if(viewPosition[i].z < 128){
      gl_Position = gl_in[i].gl_Position;
      texCoordG = vec2(0.0,1.0);
      exNormalG = exNormal[i];
      gl_Position = projMatrix * world2View * mdl2World * gl_Position;
      EmitVertex();


      gl_Position = gl_in[i].gl_Position;
      gl_Position.y += grassHeight;
      texCoordG = vec2(0.0,0.0);
      exNormalG = exNormal[i];
      gl_Position = projMatrix * world2View * mdl2World * gl_Position;
      EmitVertex();

  //    if(i+1 < 3){
        gl_Position = gl_in[i+1].gl_Position;
        texCoordG = vec2(1.0,1.0);
        exNormalG = exNormal[i+1];
        gl_Position = projMatrix * world2View * mdl2World * gl_Position;
        EmitVertex();

        gl_Position = gl_in[i+1].gl_Position;
        gl_Position.y += grassHeight;
        texCoordG = vec2(1.0,0.0);
        exNormalG = exNormal[i+1];
        gl_Position = projMatrix * world2View * mdl2World * gl_Position;
        EmitVertex();

      }else{
        gl_Position = gl_in[0].gl_Position;
        texCoordG = texCoord[0];
        exNormalG = exNormal[0];
        gl_Position = projMatrix * world2View * mdl2World * gl_Position;
        EmitVertex();

        gl_Position = gl_in[0].gl_Position;
        gl_Position.y += grassHeight;
        texCoordG = texCoord[0];
        exNormalG = exNormal[0];
        gl_Position = projMatrix * world2View * mdl2World * gl_Position;
        EmitVertex();

      } 


      EndPrimitive();
    }
  }

}
*/

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

void createGrassBlade(vec4 position,vec3 normal,vec2 texCord,float height){
    texCoordG = texCord; 
    exNormalG = normal;
    vec4 newPos = position;
    exPositionG = vec3( world2View * mdl2World * newPos);
    gl_Position = projMatrix * vec4(exPositionG,1.0);
    EmitVertex();
    
    texCoordG = texCord; 
    exNormalG = normal;
    newPos = position;
     exPositionG = vec3( world2View * mdl2World * newPos);
    gl_Position = projMatrix * vec4(exPositionG,1.0);
    gl_Position.x += 0.2;
    EmitVertex();

    texCoordG = texCord; 
    exNormalG = normal;
    newPos = position;
    newPos.y += height;
    exPositionG = vec3( world2View * mdl2World * newPos);
    gl_Position = projMatrix * vec4(exPositionG,1.0);
    EmitVertex();
    EndPrimitive();
}

void main(){
    mat3 normalMatrix = mat3(world2View * mdl2World);
    float slope = clamp(1.2-exNormal[0].y,0,1);
    vec3 normal0 = normalMatrix * exNormal[0];
    float lod0 = 256;
    float lod1 = 512;
    float lod2 = 1500;
    float height = 1.7;
    vec4 cameraCoord = projMatrix * vec4(exPosition[0],1.0);
    if(slope < 0.5){
        if(cameraCoord.z > 0.0 && cameraCoord.z < lod0){
        // LOD 1
            vec4 pos0,pos1,pos2;
            pos0 = gl_in[0].gl_Position;
            pos1 = gl_in[1].gl_Position;
            pos2 = gl_in[2].gl_Position;
            createGrassBlade(pos0,normal0,texCoord[0],height);
            
            createGrassBlade(mix(pos0,pos1,0.25),normal0,texCoord[0],height);
           // createGrassBlade(mix(pos0,pos2,0.2),exNormal[0],texCoord[0]);
            createGrassBlade(mix(pos1,pos2,0.25),normal0,texCoord[0],height);
            
           //createGrassBlade(mix(pos0,pos1,0.5),exNormal[0],texCoord[0]);
           // createGrassBlade(mix(pos0,pos2,0.4),exNormal[0],texCoord[0]);
           // createGrassBlade(mix(pos1,pos2,0.4),exNormal[0],texCoord[0]);
        }
        else if(cameraCoord.z >= lod0 && cameraCoord.z < lod1){
        // LOD 2
            vec4 pos0,pos1,pos2;
            pos0 = gl_in[0].gl_Position;
            pos1 = gl_in[1].gl_Position;
            pos2 = gl_in[2].gl_Position;
            createGrassBlade(pos0,normal0,texCoord[0],height);
            
            createGrassBlade(mix(pos0,pos1,0.25),normal0,texCoord[0],height);
        }
        else if(cameraCoord.z >= lod1 && cameraCoord.z < lod2){
            vec4 pos0,pos1,pos2;
            pos0 = gl_in[0].gl_Position;
            pos1 = gl_in[1].gl_Position;
            pos2 = gl_in[2].gl_Position;
            createGrassBlade(pos0,normal0,texCoord[0],height);
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

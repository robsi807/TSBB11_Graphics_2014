#version 150

layout(triangles) in;
layout(triangle_strip, max_vertices = 12) out;

in vec3 exNormal[3];
in vec3 exPosition[3];
in vec2 texCoord[3];
in vec3 surf[3];
out vec2 texCoordG;
out vec3 exNormalG;
out vec3 exPositionG;
out vec3 surfG;

uniform mat4 projMatrix;
uniform mat4 mdl2World;
uniform mat4 world2View;
uniform float time;
uniform sampler2D noiseTex;

void createLeaf(vec4 position,vec3 normal,vec3 tangent,vec2 texCord,float height,float width,mat3 normalMatrix){

    texCoordG = texCord; 
    exNormalG = normalMatrix * normal;
    vec4 newPos = position - 0.05*vec4(normal,0.0);
    exPositionG = vec3( world2View * mdl2World * newPos);
    surfG = surf[0];
    gl_Position = projMatrix * vec4(exPositionG,1.0);
    EmitVertex();
    
		texCoordG = texCord; 
    exNormalG = normalMatrix * normal;
    newPos = position + (height/2)*vec4(normal,0.0)+(width/2)*vec4(tangent,0.0);
    exPositionG = vec3( world2View * mdl2World * newPos);
    surfG = surf[0];
    gl_Position = projMatrix * vec4(exPositionG,1.0);
    EmitVertex();
    
    texCoordG = texCord; 
    exNormalG = normalMatrix * normal;
    newPos = position + (height/2)*vec4(normal,0.0) - (width/2)*vec4(tangent,0.0);
    exPositionG = vec3( world2View * mdl2World * newPos);
    surfG = surf[0];
    gl_Position = projMatrix * vec4(exPositionG,1.0);
    EmitVertex();
    
    texCoordG = texCord; 
    exNormalG = normalMatrix * normal;
    newPos = position + height*vec4(normal,0.0);
    exPositionG = vec3( world2View * mdl2World * newPos);
    surfG = surf[0];
    gl_Position = projMatrix * vec4(exPositionG,1.0);
    EmitVertex();

    EndPrimitive();
}

void main(){
    
    mat3 normalMatrix = mat3(world2View * mdl2World);
    vec4 cameraCoord = projMatrix * vec4(exPosition[0],1.0);

    if(cameraCoord.z > 0.0 ){

        // Level-of-detail parameters
        float zLod0 = 200;
        float zLod1 = 400;
        
        float zLod2 = 500;
        float zLod3 = 800;
        
        float xLod0 = 240;
        float xLod1 = 400;
        
        float xLod2 = 520;
        float xLod3 = 1300;
        
        // Grass parameters
        float height = 0.25;
        float width = 0.25;
        float heightScale = 0.5;
        float timeScale = 0.5;
        float posScale = 0.5;
        float scaleNoise = 3.5;
        
        vec4 pos0,pos1,pos2;
        pos0 = gl_in[0].gl_Position;
        pos1 = gl_in[1].gl_Position;
        pos2 = gl_in[2].gl_Position;
        
        vec3 tang0,tang1,tang2;
        tang0 = normalize(vec3(pos0-pos1));
        tang1 = normalize(vec3(pos0-pos2));  
        tang2 = normalize(vec3(pos1-pos2));  
              
        vec2 tex0,tex1,tex2;
        tex0 = texCoord[0];
        tex1 = texCoord[1];
        tex2 = texCoord[2];
        
        vec3 normal0,normal1,normal2;
        normal0 = exNormal[0];
        normal1 = exNormal[1];
        normal2 = exNormal[2];
        
        if(cameraCoord.z < zLod0 && cameraCoord.x > -xLod0 && cameraCoord.x < xLod0){
        // zLod 0
            vec3 rgbNoise1 = 2*vec3(texture(noiseTex,tex0/scaleNoise))-1;
            //vec3 rgbNoise2 = 2*vec3(texture(noiseTex,1.5*(tex1/scaleNoise)))-1;
            //vec3 rgbNoise3 = 2*vec3(texture(noiseTex,2*(tex2/scaleNoise)))-1;
            
            vec4 grPos0 = mix(mix(pos0,pos1,0.5+posScale*rgbNoise1.x),pos2,0.5+posScale*rgbNoise1.y);
            //vec4 grPos1 = mix(mix(pos0,pos1,0.5+posScale*rgbNoise1.z),pos2,0.5+posScale*rgbNoise2.x);
            //vec4 grPos2 = mix(mix(pos0,pos1,0.5+posScale*rgbNoise2.y),pos2,0.5+posScale*rgbNoise2.z);
            
            vec3 grNorm0 = mix(mix(normal0,normal1,0.5+posScale*rgbNoise1.x),normal2,0.5+posScale*rgbNoise1.y);
            //vec3 grNorm1 = mix(mix(normal0,normal1,0.5+posScale*rgbNoise1.z),normal2,0.5+posScale*rgbNoise2.x);
            //vec3 grNorm2 = mix(mix(normal0,normal1,0.5+posScale*rgbNoise1.y),normal2,0.5+posScale*rgbNoise1.z);
            
            vec3 grTang0 = mix(mix(tang0,tang1,0.5+posScale*rgbNoise1.x),tang2,0.5+posScale*rgbNoise1.y);
            //vec3 grTang1 = mix(mix(tang0,tang1,0.5+posScale*rgbNoise1.z),tang2,0.5+posScale*rgbNoise2.x);
            
            createLeaf(grPos0,grNorm0,grTang0,tex0,height+heightScale*rgbNoise1.x,width,normalMatrix); 
        }
        else if(cameraCoord.z <= zLod1 && cameraCoord.x > -xLod1 && cameraCoord.x < xLod1 ){
        // zLod 0
            vec3 rgbNoise1 = vec3(texture(noiseTex,pos0.xy/scaleNoise));
            float temp = (rgbNoise1.x + rgbNoise1.y + rgbNoise1.z ) / 3.0;
           	if(temp > 0.5){
		          rgbNoise1 = 2*rgbNoise1 - 1; 
		          width *= 2.0;
		          vec4 grPos0 = mix(mix(pos0,pos1,0.5+posScale*rgbNoise1.x),pos2,0.5+posScale*rgbNoise1.y);
		          
		          vec3 grNorm0 = mix(mix(normal0,normal1,0.5+posScale*rgbNoise1.x),normal2,0.5+posScale*rgbNoise1.y);
		          
		          vec3 grTang0 = mix(mix(tang0,tang1,0.5+posScale*rgbNoise1.x),tang2,0.5+posScale*rgbNoise1.y);
		          
		          createLeaf(grPos0,grNorm0,grTang0,tex0,height+heightScale*rgbNoise1.x,width,normalMatrix);
            } 
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

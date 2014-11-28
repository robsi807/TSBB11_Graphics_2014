#version 150

layout(triangles) in;
layout(triangle_strip, max_vertices = 15) out;

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

vec4 permute(vec4 x)
{
  return mod(((x*34.0)+1.0)*x, 289.0);
}

vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

vec3 fade(vec3 t) {
  return t*t*t*(t*(t*6.0-15.0)+10.0);
}

// Classic Perlin noise
float cnoise(vec3 P)
{
  vec3 Pi0 = floor(P); // Integer part for indexing
  vec3 Pi1 = Pi0 + vec3(1.0); // Integer part + 1
  Pi0 = mod(Pi0, 289.0);
  Pi1 = mod(Pi1, 289.0);
  vec3 Pf0 = fract(P); // Fractional part for interpolation
  vec3 Pf1 = Pf0 - vec3(1.0); // Fractional part - 1.0
  vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
  vec4 iy = vec4(Pi0.yy, Pi1.yy);
  vec4 iz0 = Pi0.zzzz;
  vec4 iz1 = Pi1.zzzz;

  vec4 ixy = permute(permute(ix) + iy);
  vec4 ixy0 = permute(ixy + iz0);
  vec4 ixy1 = permute(ixy + iz1);

  vec4 gx0 = ixy0 / 7.0;
  vec4 gy0 = fract(floor(gx0) / 7.0) - 0.5;
  gx0 = fract(gx0);
  vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);
  vec4 sz0 = step(gz0, vec4(0.0));
  gx0 -= sz0 * (step(0.0, gx0) - 0.5);
  gy0 -= sz0 * (step(0.0, gy0) - 0.5);

  vec4 gx1 = ixy1 / 7.0;
  vec4 gy1 = fract(floor(gx1) / 7.0) - 0.5;
  gx1 = fract(gx1);
  vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);
  vec4 sz1 = step(gz1, vec4(0.0));
  gx1 -= sz1 * (step(0.0, gx1) - 0.5);
  gy1 -= sz1 * (step(0.0, gy1) - 0.5);

  vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
  vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
  vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
  vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
  vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
  vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
  vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
  vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);

  vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
  g000 *= norm0.x;
  g010 *= norm0.y;
  g100 *= norm0.z;
  g110 *= norm0.w;
  vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
  g001 *= norm1.x;
  g011 *= norm1.y;
  g101 *= norm1.z;
  g111 *= norm1.w;

  float n000 = dot(g000, Pf0);
  float n100 = dot(g100, vec3(Pf1.x, Pf0.yz));
  float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
  float n110 = dot(g110, vec3(Pf1.xy, Pf0.z));
  float n001 = dot(g001, vec3(Pf0.xy, Pf1.z));
  float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
  float n011 = dot(g011, vec3(Pf0.x, Pf1.yz));
  float n111 = dot(g111, Pf1);

  vec3 fade_xyz = fade(Pf0);
  vec4 n_z = mix(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
  vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
  float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x); 
  return 2.2 * n_xyz;
}

void createGrassBlade(vec4 position,vec3 normal,vec2 texCord,float height,float angle1,float angle2){
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

void createGrassBladeSimple(vec4 position,vec3 normal,vec2 texCord,float height,float width){
    float texScale = width;
    texCoordG = texCord; 
    exNormalG = normal;
    vec4 newPos = position;
    exPositionG = vec3( world2View * mdl2World * newPos);
    gl_Position = projMatrix * vec4(exPositionG,1.0);
    EmitVertex();
    
    texCoordG = texCord + vec2(0.0,texScale); 
    exNormalG = normal;
    newPos = position;
    exPositionG = vec3( world2View * mdl2World * newPos);
    gl_Position = projMatrix * vec4(exPositionG,1.0);
    gl_Position.x += width;
    EmitVertex();
    
    texCoordG = texCord  + vec2(texScale,texScale);; 
    exNormalG = normal;
    newPos = position;
    newPos.y += height;
    exPositionG = vec3( world2View * mdl2World * newPos);
    gl_Position = projMatrix * vec4(exPositionG,1.0);
    gl_Position.x += width;
    EmitVertex();

    texCoordG = texCord + vec2(texScale,0.0);;
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
    vec4 cameraCoord = projMatrix * vec4(exPosition[0],1.0);
    
    
    if(slope < 0.5 && cameraCoord.z > 0.0 ){

        // Level-of-detail parameters
        float zLod0 = 100;
        float zLod1 = 250;
        float zLod2 = 400;
        float zLod3 = 800;
        float xLod0 = 120;
        float xLod1 = 300;
        float xLod2 = 520;
        float xLod3 = 1300;
        
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
            
            createGrassBlade(grPos0,normal0,tex0,height+heightScale*rgbNoise1.x,angleScale*rgbNoise1.x,angleScale*rgbNoise1.y);
            createGrassBlade(grPos1,normal0,tex0,height+heightScale*rgbNoise1.y,angleScale*rgbNoise1.z,angleScale*rgbNoise2.x);
            createGrassBlade(grPos2,normal0,tex0,height+heightScale*rgbNoise1.z,angleScale*rgbNoise2.y,angleScale*rgbNoise2.z);
            createGrassBlade(grPos3,normal0,tex0,height+heightScale*rgbNoise2.x,angleScale*rgbNoise3.x,angleScale*rgbNoise3.y);
            createGrassBlade(grPos4,normal0,tex0,height+heightScale*rgbNoise2.y,angleScale*rgbNoise3.z,angleScale*rgbNoise3.z);
            //createGrassBlade(grPos4,normal0,tex0,height+heightScale*rgbNoise2.z,angleScale*rgbNoise4.x,angleScale*rgbNoise4.y);
            
            /*            
            float rnd1 = cnoise(vec3(mix(pos0,pos2,0.25)))+1;
            float rndTime = cnoise(vec3(mix(pos0.xy,pos2.xy,0.25),time/3));
            //float rndTime = cnoise(vec3(tex0,time));
            createGrassBlade(mix(pos0,pos2,0.25 + posScale*rnd1),normal0,tex0,height + heightScale*rnd1,angleScale*(rndTime));//sin(timeScale*time*rnd1+3*rnd1)));
            rnd1 = cnoise(vec3(mix(pos0,pos1,0.25)))+1;
            rndTime = cnoise(vec3(mix(pos0.xy,pos1.xy,0.25),time/3));
            createGrassBlade(mix(pos0,pos1,0.25 + posScale*rnd1),normal0,mix(tex0,tex1,0.25),height + heightScale*rnd1,angleScale*(rndTime));//sin(timeScale*time*rnd1+3*rnd1)));
            rnd1 = cnoise(vec3(mix(pos1,pos2,0.25)))+1;
            rndTime = cnoise(vec3(mix(pos1.xy,pos2.xy,0.25),time/3));
            createGrassBlade(mix(pos1,pos2,0.25 + posScale*rnd1),normal0,mix(tex0,tex1,0.25),height + heightScale*rnd1,angleScale*(rndTime));//sin(timeScale*time*rnd1+3*rnd1)));
            */
            
        }
        else if(cameraCoord.z >= zLod0 && cameraCoord.z < zLod1 && cameraCoord.x > -xLod1 && cameraCoord.x < xLod1){
        // LOD 1
            vec3 rgbNoise1 = 2*vec3(texture(noiseTex,tex0/scaleNoise))-1;
            vec3 rgbNoise2 = 2*vec3(texture(noiseTex,1.5*(tex1/scaleNoise)))-1;
            
            vec4 grPos0 = mix(mix(pos0,pos1,0.4+posScale*rgbNoise1.x),pos2,0.5+posScale*rgbNoise1.y);
            vec4 grPos1 = mix(mix(pos0,pos1,0.5+posScale*rgbNoise1.z),pos2,0.5+posScale*rgbNoise2.x);
            vec4 grPos2 = mix(mix(pos0,pos1,0.5+posScale*rgbNoise2.y),pos2,0.5+posScale*rgbNoise2.z);
            
            createGrassBlade(grPos0,normal0,tex0,height+heightScale*rgbNoise1.x,angleScale*rgbNoise1.x,angleScale*rgbNoise1.y);
            createGrassBlade(grPos1,normal0,tex0,height+heightScale*rgbNoise1.y,angleScale*rgbNoise1.z,angleScale*rgbNoise2.x);
            createGrassBlade(grPos2,normal0,tex0,height+heightScale*rgbNoise1.z,angleScale*rgbNoise2.y,angleScale*rgbNoise2.z);
            /*
            float rnd1 = cnoise(vec3(mix(pos0,pos1,0.25)))+1;
            createGrassBlade(mix(pos0,pos1,0.25 + posScale*rnd1),normal0,mix(tex0,tex1,0.25),height + heightScale*rnd1,angleScale*(rnd1-1));
            rnd1 = cnoise(vec3(mix(pos1,pos2,0.25)))+1;
            createGrassBlade(mix(pos1,pos2,0.25 + posScale*rnd1),normal0,mix(tex0,tex1,0.25),height + heightScale*rnd1,angleScale*(rnd1-1));
            */
        }
        else if(cameraCoord.z>=zLod1 && cameraCoord.z < zLod2 && cameraCoord.x > -xLod2 && cameraCoord.x < xLod2){
        // LOD 2
            vec3 rgbNoise1 = 2*vec3(texture(noiseTex,tex0/scaleNoise))-1;
            vec3 rgbNoise2 = 2*vec3(texture(noiseTex,1.5*(tex1/scaleNoise)))-1;
            
            vec4 grPos0 = mix(mix(pos0,pos1,0.4+posScale*rgbNoise1.x),pos2,0.5+posScale*rgbNoise1.y);
            vec4 grPos1 = mix(mix(pos0,pos1,0.5+posScale*rgbNoise1.z),pos2,0.5+posScale*rgbNoise2.x);
            
            createGrassBlade(grPos0,normal0,tex0,height+heightScale*rgbNoise1.x,angleScale*rgbNoise1.x,angleScale*rgbNoise1.y);
            createGrassBlade(grPos1,normal0,tex0,height+heightScale*rgbNoise1.y,angleScale*rgbNoise1.z,angleScale*rgbNoise2.x);
        
            /*
            float rnd1 = cnoise(vec3(mix(pos0,pos1,0.25)))+1;
            createGrassBlade(mix(pos0,pos1,0.25 + posScale*rnd1),normal0,mix(tex0,tex1,0.25),height + heightScale*rnd1,angleScale*(rnd1-1));
            */
        }
        else if(cameraCoord.z < zLod3 && cameraCoord.x > -xLod3 && cameraCoord.x < xLod3){
        // LOD 3
            // Calculate the resolution...
            float d = cameraCoord.z-zLod2;
            float k = 8.0/1000.0;
            float res = floor(k*d + 2.0);
            float res1 = mod(pos0.x,res);
            float res2 = mod(pos0.z,res);
            float width = res + 0.1;
            if(res1 == 0 && res2 == 0){
                vec3 rgbNoise1 = 2*vec3(texture(noiseTex,tex0/scaleNoise))-1;
                vec4 grPos0 = mix(mix(pos0,pos1,0.4+posScale*rgbNoise1.x),pos2,0.5+posScale*rgbNoise1.y);
                createGrassBladeSimple(grPos0,normal0,tex0,height+heightScale*rgbNoise1.x,width);
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

// randfunctions.frag
// A bunch of examples on rand functions in GLSL
// Not to be used as a shader. 

// -------------------------------------------------------------------------
// Random function found at
// http://byteblacksmith.com/improvements-to-the-canonical-one-liner-glsl-rand-for-opengl-es-2-0/
float rand1(vec2 co)
{
    float a = 12.9898;
    float b = 78.233;
    float c = 43758.5453;
    float dt= dot(co.xy ,vec2(a,b));
    float sn= mod(dt,3.14);
    return fract(sin(sn) * c);
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

/*
// A function for procedural coloring of surfaces
vec3 calculateColor(vec3 normal,vec3 position)
{
	const vec3 snow = vec3(0.9,0.9,1.0);
	const vec3 rock = vec3(0.3,0.15,0.001);
	const vec3 grass = vec3(0.4,0.8,0.2);
	const vec3 sand = vec3(0.9,0.8,0.1);
	
	// Calculate normalized height
	const float heightScale = 0.0025; // TODO: this should be uploaded to the shader
	float normalizedHeight = position.y * heightScale;

	// Calculate slope
	float slope = clamp(1.2-normal.y,0,1); // Shortcut for dot product with y-axis!
	
	// Calculate the height color
	// TODO: Remove if statements and somehow weight
	
	float fnoise = cnoise(position*2);
	vec3 noiseVec = 0.2 * vec3(2*fnoise,fnoise,fnoise);
	
	vec3 heightColor = grass + noiseVec;
	if(normalizedHeight < 0.2)
	{
	    noiseVec = 0.15 * vec3(fnoise,fnoise,fnoise);
		heightColor = sand + noiseVec;	
	}
	if(normalizedHeight > 0.37)
	{
	    noiseVec = 0.06 * vec3(fnoise,fnoise,fnoise);
	    	heightColor = snow;
		slope = 0.1;
	}
	
	//heightColor = vec4(0,normalizedHeight,0,1.0);
	return slope*rock + (1-slope)*heightColor;
}
*/

























// -------------------------------------------------------------------------
// Noise functions found at 
// http://webstaff.itn.liu.se/~stegu/jgt2012/article.pdf
vec3 permute(vec3 x){
     return mod(((x*34.0)+1.0)*x,289.0);
}

vec3 taylorInvSqrt(vec3 r) {
     return 1.79284291400159 - 0.85373472095314 * r;
}

float snoise(vec2 P) {
      const vec2 C=vec2 (0.211324865405187134,//(3.0-sqrt(3.0))/6.0;
      0.366025403784438597);//0.5*(sqrt(3.0)-1.0);
      //First corner
      vec2 i = floor(P+dot(P,C.yy));
      vec2 x0=P-i+dot(i,C.xx);

      //Othercorners
      vec2 i1;
      i1.x=step(x0.y,x0.x); //1.0ifx0.x>x0.y,else0.0
      i1.y=1.0-i1.x;

      //x1=x0-i1+1.0*C.xx; x2=x0-1.0+2.0*C.xx;
      vec4 x12=x0.xyxy+vec4(C.xx,C.xx*2.0-1.0);
      x12.xy-=i1;

      //Permutations
      i = mod(i,289.0);//Avoidtruncationinpolynomialevaluation
      vec3 p=permute(permute(i.y+vec3(0.0,i1.y,1.0))+i.x+vec3(0.0,i1.x,1.0));
      //Circularly symmetricblendingkernel
      vec3 m = max(0.5-vec3(dot(x0,x0),dot(x12.xy,x12.xy),
      dot(x12.zw,x12.zw)),0.0);
      m = m * m ;
      m = m * m ;

      //Gradients from41 points on a line,mapped on to a diamond
      vec3 x = fract(p*(1.0/41.0))*2.0-1.0;
      vec3 gy = abs(x)-0.5;
      vec3 ox=floor(x+0.5);//round(x)isaGLSL1.30feature
      vec3 gx = x - ox;

      //Normalisegradientsimplicitlybyscalingm
      m *= taylorInvSqrt(gx*gx+gy*gy);

      //Compute finalnoisevalueatP
      vec3 g;
      g.x=gx.x*x0.x+gy.x*x0.y;
      g.yz=gx.yz*x12.xz+gy.yz*x12.yw;
      //Scaleoutputtospanrange[-1,1]
      //(scalingfactordeterminedbyexperiments)
      return 130.0*dot(m,g);
}

// -------------------------------------------------------------------------
// From: https://github.com/ashima/webgl-noise/blob/master/src/noise3D.glsl
vec3 mod289(vec3 x) {
return x - floor(x * (1.0 / 289.0)) * 289.0;
}
vec4 mod289(vec4 x) {
return x - floor(x * (1.0 / 289.0)) * 289.0;
}
vec4 permute(vec4 x) {
return mod289(((x*34.0)+1.0)*x);
}
vec4 taylorInvSqrt(vec4 r)
{
return 1.79284291400159 - 0.85373472095314 * r;
}
float snoise3(vec3 v)
{
const vec2 C = vec2(1.0/6.0, 1.0/3.0) ;
const vec4 D = vec4(0.0, 0.5, 1.0, 2.0);
// First corner
vec3 i = floor(v + dot(v, C.yyy) );
vec3 x0 = v - i + dot(i, C.xxx) ;
// Other corners
vec3 g = step(x0.yzx, x0.xyz);
vec3 l = 1.0 - g;
vec3 i1 = min( g.xyz, l.zxy );
vec3 i2 = max( g.xyz, l.zxy );
// x0 = x0 - 0.0 + 0.0 * C.xxx;
// x1 = x0 - i1 + 1.0 * C.xxx;
// x2 = x0 - i2 + 2.0 * C.xxx;
// x3 = x0 - 1.0 + 3.0 * C.xxx;
vec3 x1 = x0 - i1 + C.xxx;
vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
vec3 x3 = x0 - D.yyy; // -1.0+3.0*C.x = -0.5 = -D.y
// Permutations
i = mod289(i);
vec4 p = permute( permute( permute(
i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
+ i.y + vec4(0.0, i1.y, i2.y, 1.0 ))
+ i.x + vec4(0.0, i1.x, i2.x, 1.0 ));
// Gradients: 7x7 points over a square, mapped onto an octahedron.
// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
float n_ = 0.142857142857; // 1.0/7.0
vec3 ns = n_ * D.wyz - D.xzx;
vec4 j = p - 49.0 * floor(p * ns.z * ns.z); // mod(p,7*7)
vec4 x_ = floor(j * ns.z);
vec4 y_ = floor(j - 7.0 * x_ ); // mod(j,N)
vec4 x = x_ *ns.x + ns.yyyy;
vec4 y = y_ *ns.x + ns.yyyy;
vec4 h = 1.0 - abs(x) - abs(y);
vec4 b0 = vec4( x.xy, y.xy );
vec4 b1 = vec4( x.zw, y.zw );
//vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
//vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
vec4 s0 = floor(b0)*2.0 + 1.0;
vec4 s1 = floor(b1)*2.0 + 1.0;
vec4 sh = -step(h, vec4(0.0));
vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;
vec3 p0 = vec3(a0.xy,h.x);
vec3 p1 = vec3(a0.zw,h.y);
vec3 p2 = vec3(a1.xy,h.z);
vec3 p3 = vec3(a1.zw,h.w);
//Normalise gradients
vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
p0 *= norm.x;
p1 *= norm.y;
p2 *= norm.z;
p3 *= norm.w;
// Mix final noise value
vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
m = m * m;
return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1),
dot(p2,x2), dot(p3,x3) ) );
}

// -------------------------------------------------------
// From:
// http://www.itn.liu.se/~stegu/simplexnoise/GLSL-noise-vs-noise.zip

vec4 permute(vec4 x) { return mod(((x*34.0)+1.0)*x, 289.0); }

vec2 fade(vec2 t) {
  return t*t*t*(t*(t*6.0-15.0)+10.0);
}

// Classic Perlin noise
float cnoise(vec2 P)
{
  vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
  vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
  Pi = mod(Pi, 289.0); // To avoid truncation effects in permutation
  vec4 ix = Pi.xzxz;
  vec4 iy = Pi.yyww;
  vec4 fx = Pf.xzxz;
  vec4 fy = Pf.yyww;
  vec4 i = permute(permute(ix) + iy);
  vec4 gx = 2.0 * fract(i * 0.0243902439) - 1.0; // 1/41 = 0.024...
  vec4 gy = abs(gx) - 0.5;
  vec4 tx = floor(gx + 0.5);
  gx = gx - tx;
  vec2 g00 = vec2(gx.x,gy.x);
  vec2 g10 = vec2(gx.y,gy.y);
  vec2 g01 = vec2(gx.z,gy.z);
  vec2 g11 = vec2(gx.w,gy.w);
  vec4 norm = 1.79284291400159 - 0.85373472095314 * 
    vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11));
  g00 *= norm.x;
  g01 *= norm.y;
  g10 *= norm.z;
  g11 *= norm.w;
  float n00 = dot(g00, vec2(fx.x, fy.x));
  float n10 = dot(g10, vec2(fx.y, fy.y));
  float n01 = dot(g01, vec2(fx.z, fy.z));
  float n11 = dot(g11, vec2(fx.w, fy.w));
  vec2 fade_xy = fade(Pf.xy);
  vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
  float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
  return 2.3 * n_xy;
}


// ------------------------------------------------------------------------------------------------
// Possible use in water shader. Creates a small displacement of vertices.
// Found at:
// http://www.geeks3d.com/20100831/shader-library-noise-and-pseudo-random-number-generator-in-glsl/

[Vertex_Shader]
#extension GL_EXT_gpu_shader4: enable

uniform float time;

int LFSR_Rand_Gen(in int n)
{
  // <<, ^ and & require GL_EXT_gpu_shader4.
  n = (n << 13) ^ n; 
  return (n * (n*n*15731+789221) + 1376312589) & 0x7fffffff;
}

float LFSR_Rand_Gen_f( in int n )
{
  return float(LFSR_Rand_Gen(n));
}

float noise3f(in vec3 p)
{
  ivec3 ip = ivec3(floor(p));
  vec3 u = fract(p);
  u = u*u*(3.0-2.0*u);

  int n = ip.x + ip.y*57 + ip.z*113;

  float res = mix(mix(mix(LFSR_Rand_Gen_f(n+(0+57*0+113*0)),
                          LFSR_Rand_Gen_f(n+(1+57*0+113*0)),u.x),
                      mix(LFSR_Rand_Gen_f(n+(0+57*1+113*0)),
                          LFSR_Rand_Gen_f(n+(1+57*1+113*0)),u.x),u.y),
                 mix(mix(LFSR_Rand_Gen_f(n+(0+57*0+113*1)),
                          LFSR_Rand_Gen_f(n+(1+57*0+113*1)),u.x),
                      mix(LFSR_Rand_Gen_f(n+(0+57*1+113*1)),
                          LFSR_Rand_Gen_f(n+(1+57*1+113*1)),u.x),u.y),u.z);

  return 1.0 - res*(1.0/1073741824.0);
}

void main()
{	
  float disp = noise3f(gl_Vertex.xyz);
  vec3 P = gl_Vertex.xyz + (gl_Normal * disp * 2.0 * sin(time * 2.0));
  gl_Position = gl_ModelViewProjectionMatrix * vec4(P, 1.0);
}

[Pixel_Shader]
uniform vec4 color;
void main(void)
{
  gl_FragColor = color;
}



// ------------------------------------------------------------------------------------------------

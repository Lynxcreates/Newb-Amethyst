#ifndef CLOUDS_H
#define CLOUDS_H

#include "noise.h"

// simple clouds 2D noise
float cloudNoise2D(vec2 p, highp float t, float rain) {
  t *= NL_CLOUD1_SPEED;
  p += t;
  p.x += sin(p.y*0.4 + t);

  vec2 p0 = floor(p);
  vec2 u = p-p0;
  u *= u*(3.0-2.0*u);
  vec2 v = 1.0-u;

  // rain transition
  vec2 d = vec2(0.09+0.5*rain,0.089+0.5*rain*rain);

  return v.y*(randt(p0,d)*v.x + randt(p0+vec2(1.0,0.0),d)*u.x) +
         u.y*(randt(p0+vec2(0.0,1.0),d)*v.x + randt(p0+vec2(1.0,1.0),d)*u.x);
}

// simple clouds
vec4 renderCloudsSimple(vec3 pos, highp float t, float rain, vec3 zenithCol, vec3 horizonCol, vec3 fogCol) {
  pos.xz *= NL_CLOUD1_SCALE;

  float cloudAlpha = cloudNoise2D(pos.xz, t, rain);
  float cloudShadow = cloudNoise2D(pos.xz*0.91, t, rain);

  vec4 color = vec4(0.02,0.04,0.05,cloudAlpha);

  color.rgb += fogCol;
  color.rgb *= 1.0 - 0.5*cloudShadow*step(0.0, pos.y);

  color.rgb += zenithCol*0.7;
  color.rgb *= 1.0 - 0.4*rain;

  return color;
}

// rounded clouds

// rounded clouds 3D density map
// Simplex noise implementation
vec3 mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec2 mod289(vec2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec3 permute(vec3 x) { return mod289(((x * 34.0) + 1.0) * x); }
float snoise(vec2 v) {
  const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                      0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                      -0.577350269189626, // -1.0 + 2.0 * C.x
                      0.024390243902439); // 1.0 / 41.0
  vec2 i = floor(v + dot(v, C.yy));
  vec2 x0 = v - i + dot(i, C.xx);
  vec2 i1;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;
  i = mod289(i);
  vec3 p = permute(permute(i.y + vec3(0.0, i1.y, 1.0))
                    + i.x + vec3(0.0, i1.x, 1.0));
  vec3 m = max(0.5 - vec3(dot(x0, x0), dot(x12.xy, x12.xy), dot(x12.zw, x12.zw)), 0.0);
  m = m * m;
  m = m * m;
  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;
  m *= (1.79284291400159 - 0.85373472095314 * (a0 * a0 + h * h));
  vec3 g;
  g.x = a0.x * x0.x + h.x * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}

float cloudDf(vec3 pos, float rain) {
  vec2 p0 = floor(pos.xz);
  vec2 u = smoothstep(0.999 * NL_CLOUD2_SHAPE, 1.0, pos.xz - p0);

  // rain transition
  vec2 t = vec2(0.1001 + 0.2 * rain, 0.1 + 0.2 * rain * rain);

  float n = mix(
    mix(randt(p0, t), randt(p0 + vec2(1.0, 0.0), t), u.x),
    mix(randt(p0 + vec2(0.0, 1.0), t), randt(p0 + vec2(1.0, 1.0), t), u.x),
    u.y
  );

  // round y
  float b = 1.0 - 1.9 * smoothstep(NL_CLOUD2_SHAPE, 2.0 - NL_CLOUD2_SHAPE, 2.0 * abs(pos.y - 0.5));

  // Add noise to make the clouds fluffy
  float noiseFactor = 0.4;  // Adjust this factor to control the amount of fluffiness
  float fluffiness = noiseFactor * (snoise(pos.xz * 3.0 + pos.y * 2.0) - 0.5);

  return smoothstep(0.2, 1.0, (n + fluffiness) * b);
}

vec4 renderClouds(vec3 vDir, vec3 vPos, float rain, float time, vec3 fogCol, vec3 skyCol) {
  float height = 7.0 * mix(NL_CLOUD2_THICKNESS, NL_CLOUD2_RAIN_THICKNESS, rain);

  // scaled ray offset
  vec3 deltaP;
  deltaP.y = 1.0;
  deltaP.xz = (NL_CLOUD2_SCALE * height) * vDir.xz / (0.02 + 0.98 * abs(vDir.y));
  // deltaP.xyz = (NL_CLOUD2_SCALE * height) * vDir.xyz;
  // deltaP.y = abs(deltaP.y);

  // local cloud pos
  vec3 pos;
  pos.y = 0.0;
  pos.xz = NL_CLOUD2_SCALE * (vPos.xz + vec2(1.0, 0.5) * (time * NL_CLOUD2_VELOCIY));
  pos += deltaP;

  deltaP /= -float(NL_CLOUD2_STEPS);

  // alpha, gradient
  vec2 d = vec2(0.0, 1.0);
  for (int i = 1; i <= NL_CLOUD2_STEPS; i++) {
    float m = cloudDf(pos, rain);

    d.x += m;
    d.y = mix(d.y, pos.y, m);

    pos += deltaP;
  }
  d.x *= smoothstep(0.03, 0.1, d.x);
  d.x = d.x / ((float(NL_CLOUD2_STEPS) / NL_CLOUD2_DENSITY) + d.x);

  if (vPos.y > 0.0) { // view from bottom
    d.y = 1.0 - d.y;
  }

  d.y = 1.0 - 0.7 * d.y * d.y;

  vec4 col = vec4(0.6 * skyCol, d.x);
  col.rgb += (vec3(0.03, 0.05, 0.05) + 0.8 * fogCol) * d.y;
  col.rgb *= 1.0 - 0.5 * rain;

  return col;
}

// aurora is rendered on clouds layer
#ifdef NL_AURORA
vec4 renderAurora(vec3 p, float t, float rain, vec3 FOG_COLOR) {
  t *= NL_AURORA_VELOCITY;
  p.xz *= NL_AURORA_SCALE;
  p.xz += 0.05*sin(p.x*4.0 + 20.0*t);

  float d0 = sin(p.x*0.1 + t + sin(p.z*0.2));
  float d1 = sin(p.z*0.1 - t + sin(p.x*0.2));
  float d2 = sin(p.z*0.1 + 1.0*sin(d0 + d1*2.0) + d1*2.0 + d0*1.0);
  d0 *= d0; d1 *= d1; d2 *= d2;
  d2 = d0/(1.0 + d2/NL_AURORA_WIDTH);

  float mask = (1.0-0.8*rain)*max(1.0 - 3.0*max(FOG_COLOR.b, FOG_COLOR.g), 0.0);
  return vec4(NL_AURORA*mix(NL_AURORA_COL1,NL_AURORA_COL2,d1),1.0)*d2*mask;
}
#endif

#endif

#ifndef TERRAIN_H
#define TERRAIN_H

#include "constants.h"
#include "sky.h"
#include "noise.h"

// Function to calculate terrain reflection based on view direction and position
vec4 calculateTerrainReflection(
    vec3 viewDir, vec3 wPos, vec3 zenithCol, vec3 horizonCol, vec3 horizonEdgeCol, vec4 FOG_COLOR, float t, float rainFactor
) {
    vec3 terrainRefl = vec3(0.0);

    // Calculate reflection on terrain
    if (wPos.y > 0.0) {
        vec2 pa = viewDir.xz / viewDir.y;
        vec2 reflPos = wPos.xz - pa * 80.0;

        float fade = clamp(2.0 - 0.004 * length(reflPos), 0.0, 1.0);

#if NL_CLOUD_TYPE == 2
        vec4 clouds = renderClouds(viewDir, reflPos.xyy, rainFactor, t, zenithCol, FOG_COLOR.rgb);
        terrainRefl = mix(terrainRefl, 0.45 * clouds.rgb, clouds.a * fade);
#elif NL_CLOUD_TYPE == 1
        vec4 clouds = renderCloudsSimple(reflPos.xyy, t, rainFactor, zenithCol, horizonCol, horizonEdgeCol);
        terrainRefl = mix(terrainRefl, NL_WATER_CLOUD_REFL * clouds.rgb, clouds.a * fade);
#endif
    }

    return vec4(terrainRefl, 1.0);
}

// Function to calculate fresnel reflection for terrain
float calculateTerrainFresnel(float cosR, float r0) {
    float a = 1.0 - cosR;
    float a2 = a * a;
    return r0 + (1.0 - r0) * a2 * a2 * a;
}

#endif // TERRAIN_H

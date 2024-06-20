$input v_texcoord0, v_posTime

#include <bgfx_shader.sh>
#include <newb/main.sh>

// Sampler for the material texture
SAMPLER2D(s_MatTexture, 0);

// Sampler for the eclipse texture
SAMPLER2D(s_EclipseTexture, 1);

vec3 getRainbowColor(float t) {
    float r = abs(sin(t * 6.2831853 + 0.0)); // 2 * PI for a full cycle
    float g = abs(sin(t * 6.2831853 + 2.094393)); // 2 * PI / 3 phase shift
    float b = abs(sin(t * 6.2831853 + 4.188787)); // 4 * PI / 3 phase shift
    return vec3(r, g, b);
}

void main() {
    vec4 diffuse = texture2D(s_MatTexture, v_texcoord0);

    // Generate rainbow color based on texture coordinates
    vec3 rainbowColor = getRainbowColor(v_texcoord0.x + v_texcoord0.y);

    // end sky gradient
    vec3 color = renderEndSky(getEndHorizonCol(), getEndZenithCol(), normalize(v_posTime.xyz), v_posTime.w);

    // stars with rainbow color
    color += 8.0 * diffuse.rgb * rainbowColor;

    // Sample the eclipse texture
    // Assuming the eclipse is positioned at a specific vertex
    // Here we use the texture coordinates (0.3, 0.7) as an example
    vec2 eclipsePos = vec2(0.3, 0.7); // The position where you want the eclipse
    float eclipseRadius = 0.1; // Radius of the eclipse

    // Calculate distance from the eclipse position
    float dist = distance(v_texcoord0, eclipsePos);

    // Blend the eclipse texture based on distance
    if (dist < eclipseRadius) {
        vec4 eclipseColor = texture2D(s_EclipseTexture, v_texcoord0);
        color = mix(color, eclipseColor.rgb, 1.0 - (dist / eclipseRadius));
    }

    color = colorCorrection(color);

    gl_FragColor = vec4(color, 1.0);
}

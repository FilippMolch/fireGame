#version 420 core

out vec4 FragColor;

in vec3 FragCoord;
in vec2 TexCoord;

uniform float time;
uniform vec2 resolution;
uniform vec3 viewPos;

//---------------------------------------------------------------------------
//1D Perlin noise implementation
//---------------------------------------------------------------------------
#define HASHSCALE 0.1031

float hash(float p)
{
    vec3 p3  = fract(vec3(p) * HASHSCALE);
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

float fade(float t) { return t*t*t*(t*(6.*t-15.)+10.); }

float grad(float hash, float p)
{
    int i = int(1e4*hash);
    return (i & 1) == 0 ? p : -p;
}

float perlinNoise1D(float p)
{
    float pi = floor(p), pf = p - pi, w = fade(pf);
    return mix(grad(hash(pi), pf), grad(hash(pi + 1.0), pf - 1.0), w) * 2.0;
}

float fbm(float pos, int octaves, float persistence)
{
    float total = 0., frequency = 1., amplitude = 1., maxValue = 0.;
    for(int i = 0; i < octaves; ++i)
    {
        total += perlinNoise1D(pos * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 1.;
    }
    return total / maxValue;
}

void main() {

    vec2 uv = (TexCoord * 90.0f - resolution) / resolution.y;

    float aspect = resolution.x / resolution.y;

    uv.y /= 4.0f;

    float noise = .04 * (fbm(uv.x, 2, 0.) + fbm(uv.x + time * 1.0f, 1, 0.1));
    float d = (uv.y + noise + 0.1f) * 1000.0;
    float t = clamp(d, 0., 1.);

    vec4 skyColor = vec4(0.0f);
    vec4 waterColor = vec4(.0, .4, .6, 1.0f);

    waterColor.xyz -= uv.y / 1.9f;
    waterColor.xyz += clamp(exp((-uv.y - 0.4f) * 8.0f), 0.0f, 1.0f);

    FragColor = vec4(mix(skyColor, waterColor, t));

    float gradI = clamp(exp((-uv.y - 0.4f) * 8.0f), 0.0f, 1.0f);

}
#version 430 core
in vec2 TexCoords;
out vec4 FragColor;

// Textures
uniform sampler2D image;    
uniform sampler2D normal;   
uniform sampler2D waterNormal;
uniform sampler2D moistureMap;
uniform sampler2D forestNormal;

uniform float aspectRatio = 1.0;

uniform vec3  sunPos             = vec3(1.0, 1.0, 1.0);
uniform vec3  ambientColor       = vec3(1.0, 0.9, 0.9);
uniform float ambientStrength    = 0.1;
uniform float diffuseAmbient     = 0.5;
uniform bool  debug              = false;

// Terrain
uniform int   numSteps          = 64;
uniform float stepSize          = 0.001;
uniform float shadowLength      = 0.2; 
uniform float shadowStr         = 0.7; 
uniform float waterShadowStr    = 0.2; 

uniform float terrainSpecStr = 0.1;
uniform float terrainSpecSpred = 16.0;
uniform float terrainNormalStr = 0.5;

// Forest
uniform float forestBumpStr = 0.5;
uniform float forestNormStr = 0.5;

// Water
uniform float waterLevel       = 0.33;
uniform vec3  waterColor       = vec3(0.38, 0.65, 0.66);
uniform vec3  deepWaterColor   = vec3(0.05, 0.18, 0.40);

uniform float waterNormalStr   = 0.5;
uniform float waterSpecStr     = 0.5;
uniform float waterSpecSpread  = 64.0;

uniform float time             = 0.0;

const vec3 viewPos = vec3(0.0, 0.0, 2.0);

float getHeight(vec2 uv)
{
    vec4 h = texture(image, uv);
    return dot(h, vec4(1.0, 1.0/255.0, 1.0/65025.0, 1.0/16581375.0));
}

float getMoisture(vec2 uv)
{
    return texture(moistureMap, uv).r;
}

// =========================== Terrain ============================================
bool inShadow(vec2 pos, out float shadowDist)
{
    float fragHeight = getHeight(pos);
    vec3  fragPos    = vec3(pos, fragHeight);
    vec3  rayDir     = normalize(sunPos);

    if (rayDir.z <= 0.0) { shadowDist = 0.0; return true; }

    vec3 p = fragPos + rayDir * stepSize;
    for (int i = 1; i <= numSteps; i++)
    {
        if (p.x < 0.0 || p.x > 1.0 || p.y < 0.0 || p.y > 1.0) break;
        if (p.z >= 1.0) break;
        float terrainH = getHeight(p.xy);
        if (terrainH > p.z)
        {
            shadowDist = length(p.xy - pos);
            return true;
        }
        p += rayDir * max(stepSize, (p.z - terrainH) * 0.04);
    }
    shadowDist = 0.0;
    return false;
}

vec3 terrainColor(float h, float moisture)
{
    vec3 sand     = vec3(0.65, 0.50, 0.33);
    //vec3 grass    = vec3(0.18, 0.25, 0.19);
    vec3 grass    = vec3(0.40, 0.46, 0.26);
    vec3 hills    = vec3(0.28, 0.34, 0.14);
    vec3 mountains = vec3(0.20, 0.14, 0.10);
    vec3 snow     = vec3(0.92, 0.90, 0.89);

    vec3 c = sand;
    c = mix(c, grass,    smoothstep(0.40, 0.55, h));
    c = mix(c, hills,    smoothstep(0.55, 0.68, h));
    c = mix(c, mountains,smoothstep(0.65, 0.70, h));
    c = mix(c, snow,     smoothstep(0.70, 0.90, h));
    return c;
}

vec3 getTerrainNormal()
{
    vec2 uv = TexCoords;
    vec3 _normal = normalize(texture(normal, uv).rgb * 2.0 - 1.0);

    return mix(vec3(0.0, 0.0, 1.0), _normal, terrainNormalStr);
}

float diffuse(vec3 fragPos, vec3 normal)
{
    vec3 lightDir = normalize(sunPos - fragPos);
    return max(dot(normal, lightDir), 0.0);
}

vec3 specular(vec3 fragPos, vec3 normal, float strength = 0.2, float disperse = 32)
{
    vec3 viewDir    = normalize(viewPos - fragPos);
    vec3 lightDir   = normalize(sunPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), disperse);
    return strength * spec * vec3(1.0);
}

float terrainAO(vec2 uv, float h)
{
    float ao = 0.0;
    const int AO_SAMPLES = 8;
    const float AO_RADIUS = 0.015;   // world-space sample radius
    const float AO_STRENGTH = 2.0;   // intensity multiplier

    // Sample in a spiral / uniform ring pattern
    for (int i = 0; i < AO_SAMPLES; i++)
    {
        float angle = float(i) * (3.14159 * 2.0 / float(AO_SAMPLES));
        vec2 offset = vec2(cos(angle), sin(angle)) * AO_RADIUS;

        float neighborH = getHeight(uv + offset);

        // How much higher is the neighbor? That blocks ambient light.
        float heightDiff = neighborH - h;
        ao += clamp(heightDiff / AO_RADIUS, 0.0, 1.0);
    }

    ao /= float(AO_SAMPLES);
    return clamp(1.0 - ao * AO_STRENGTH, 0.0, 1.0);
}

// =========================== Forest ===========================================
vec3 getForestNormal()
{
    vec2 uv = TexCoords;
    uv.x *= aspectRatio;
    vec3 normal = normalize(texture(forestNormal, uv * 8.0).rgb * 2.0 - 1.0);

    return mix(vec3(0.0, 0.0, 1.0), normal, forestNormStr);
}

vec3 forestColor(vec2 uv, float moisture)
{
    // Base forest tones — dark muted greens fitting your palette
    vec3 dark    = vec3(0.10, 0.18, 0.09);
    vec3 mid     = vec3(0.16, 0.26, 0.12);
    vec3 light   = vec3(0.24, 0.34, 0.14);  // matches your hills color
    vec3 olive   = vec3(0.28, 0.30, 0.10);
    vec3 muted   = vec3(0.14, 0.22, 0.16);

    // Two noise scales — large patches of same-species trees + fine per-tree variation
    float n1 = texture(waterNormal, uv * 2.5).r;               // large clumps
    float n2 = texture(waterNormal, uv * 8.0 + vec2(0.4)).r;   // mid variation
    float n3 = texture(forestNormal, uv * 20.0).r;             // fine per-tree

    // Large scale picks which "species" dominates the patch
    vec3 _patch = mix(dark, olive, smoothstep(0.3, 0.7, n1));
    _patch      = mix(_patch, light, smoothstep(0.6, 0.9, n1));

    // Mid scale blends between neighboring tones
    vec3 c = mix(_patch, mid,   smoothstep(0.4, 0.6, n2));
    c      = mix(c,     muted, smoothstep(0.65, 0.8, n2));

    // Fine scale adds subtle per-tree brightness pop
    c += (n3 - 0.5) * 0.06;

    // Moisture darkens and deepens the greens
    c = mix(c, dark * 0.85, moisture * 0.4);

    return clamp(c, 0.0, 1.0);
}

// =========================== Water ============================================
vec2 animatedUv(vec2 uv)
{
    return uv * 2.0 + vec2(time * 0.015, time * 0.01);
}

vec2 animatedUvOpposite(vec2 uv)
{
    return uv * 2.0 - vec2(time * 0.01, time * 0.02);
}

vec2 rotateUv(vec2 uv, float angle)
{
    float s = sin(angle); float c = cos(angle);
    return mat2(c, -s, s, c) * (uv - 0.5) + 0.5;
}

vec3 getWaterNormal()
{
    vec2 uv = TexCoords;
    uv.x *= aspectRatio;

    vec3 n1 = texture(waterNormal, animatedUv(uv)).rgb * 2.0 - 1.0;
    vec3 n2 = texture(waterNormal, rotateUv(animatedUvOpposite(uv), 3.14)).rgb * 2.0 - 1.0;
    vec3 finalN = normalize(vec3(n1.xy + n2.xy, n1.z + n2.z));
    return mix(vec3(0.0, 0.0, 1.0), finalN, waterNormalStr);
}

float waterDiffuse(vec3 fragPos)
{
    vec3 lightDir = normalize(sunPos - fragPos);
    vec3 n = getWaterNormal();
    return max(dot(n, lightDir), 0.0);
}

vec3 waterSpecular(vec3 fragPos)
{
    vec3 viewDir    = normalize(viewPos - fragPos);
    vec3 lightDir   = normalize(sunPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, getWaterNormal());

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), waterSpecSpread);
    return waterSpecStr * spec * vec3(1.0);
}

// =========================== Main ============================================
// Add clouds
void main()
{
    if (debug)
    {
        //FragColor = vec4(vec3(getMoisture(TexCoords)), 1.0);
        FragColor = vec4(vec3(getHeight(TexCoords)), 1.0);
        return;
    }

    float h           = getHeight(TexCoords);
    float waterDepth  = waterLevel - h; 
    float isWater     = step(0.0, waterDepth);

    float shadowDist      = 0.0;
    float shadowIntensity = 0.0;

    vec2 shadowUV = TexCoords;
    if (isWater > 0.5)
    {
        vec2 refractUv  = TexCoords * 0.5;
        refractUv.x    *= aspectRatio;
        vec2 scroll1    = refractUv + vec2(time * 0.003, time * 0.002);
        vec2 scroll2    = refractUv - vec2(time * 0.002, time * 0.003);
        vec2 ripple1    = (texture(waterNormal, scroll1).rg * 2.0 - 1.0);
        vec2 ripple2    = (texture(waterNormal, scroll2).rg * 2.0 - 1.0);

        vec2 distortion = (ripple1 + ripple2) * 0.5 * 0.006 * clamp(waterDepth * 6.0, 0.0, 1.0);
        shadowUV        = TexCoords + distortion;
    }

    float waterBlend = clamp(1.0 - pow(1.0 - clamp(waterDepth / waterLevel, 0.0, 1.0), 6.0), 0.0, 1.0);
    if (inShadow(shadowUV, shadowDist))
    {
        float falloff   = 1.0 - smoothstep(0.0, shadowLength, shadowDist);
        shadowIntensity = mix(shadowStr, waterShadowStr, isWater * waterBlend) * falloff;
    }

    shadowIntensity *= h;

    // ===== Forest ============================================================
    // Make moisture map also an influence
    // Make forest have bigger variation in color (since different trrees will have diff. color)
    float aboveWater  = smoothstep(0.37, 0.4, h);
    float belowRock   = 1.0 - smoothstep(0.5, 0.55, h);
    float forestBlend = aboveWater * belowRock;

    float moisture    = getMoisture(TexCoords);

    vec3 forest      = vec3(0.15,  0.30,  0.13); 
    vec3 wetForest   = vec3(0.0,  1.0,  0.0);

    // ===== Terrain ==========================================================
    vec3 terrainAlbedo = mix(terrainColor(h, moisture), forestColor(TexCoords, moisture), forestBlend);

    vec3  fragPos = vec3(TexCoords, h);

    vec3 terrainN = getTerrainNormal();
    vec3 forestN  = getForestNormal();
    vec3 normal   = mix(terrainN, forestN, forestBlend);
    normal.xy     += terrainN.xy * forestBumpStr * forestBlend;
    normal        = normalize(normal);

    //shadowIntensity       *= mix(1.0, shadowStr/2, forestBlend);

    float diff            = diffuse(fragPos, normal) * (1.0 - shadowIntensity);
    vec3 spec             = specular(fragPos, normal, terrainSpecStr, terrainSpecSpred) * (1.0 - shadowIntensity);
    float ao              = terrainAO(TexCoords, h);

    terrainAlbedo *= (ambientColor * ambientStrength) * ao + diff + spec;
    // ===== Water =============================================================
    vec3 wCol = mix(waterColor, deepWaterColor, waterBlend);

    float foamMask  = texture(image, TexCoords).r;
    float threshold = (waterLevel - 0.015) + 0.01 * sin(time * 1.0 + TexCoords.x * 10.0);
    float foamEdge  = smoothstep(threshold - 0.02, threshold + 0.02, foamMask);
    float fizz      = texture(waterNormal, animatedUv(TexCoords * 4.0)).r;
    foamEdge       *= mix(0.7, 1.0, fizz);
    wCol            = mix(wCol, vec3(1.0), foamEdge);

    vec3 wFragPos   = vec3(TexCoords, 0.0);
    vec3 wNormal    = getWaterNormal();
    float wDiff     = diffuse(wFragPos, wNormal) * (1.0 - shadowIntensity);
    vec3 wSpec      = specular(wFragPos, wNormal, waterSpecStr, waterSpecSpread) * (1.0 - shadowIntensity);
    float waterAO   = mix(1.0, ao, 0.4);   // weaker effect under water
    wCol *= (ambientColor * ambientStrength) * waterAO + wDiff + wSpec;

    //wCol *= (ambientColor * ambientStrength) + wDiff + wSpec;

    // ===== Final Color ===========================================================
    vec3 finalColor = mix(terrainAlbedo, wCol, waterBlend * isWater);
    FragColor = vec4(finalColor, 1.0);
}
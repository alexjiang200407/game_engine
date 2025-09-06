cbuffer LightCBuf
{
    float3 viewLightPos;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};

cbuffer ObjectCBuf
{
    bool normalMapEnabled;
    bool specularEnabled;
    bool diffuseEnabled;
    bool hasAlpha;
    float specularPowerConst;
    float3 specularColor;
    float specularMapWeight;
    float3 diffuseColorConst;
};

#include "shader_ops.hlsl"
#include "light_vector.hlsl"


Texture2D tex;
Texture2D spec;
Texture2D nmap;

SamplerState splr;


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    if (normalMapEnabled)
    {
        viewNormal = MapNormal(viewTan, viewBitan, viewNormal, tc, nmap, splr);
    }
    float4 dtex = tex.Sample(splr, tc);
    
#ifdef MASK
    clip(dtex.a < 0.1f ? -1 : 1);
    
    // flip normal when backface
    if (dot(viewNormal, viewFragPos) >= 0.0f)
    {
        viewNormal = -viewNormal;
    }
#endif
    
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);


    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);

    float specularPower = specularPowerConst;
    float3 specularReflectionColor;
    if (specularEnabled)
    {
        const float4 specularSample = spec.Sample(splr, tc);
        specularReflectionColor = specularSample.rrr * specularMapWeight;
        if (hasAlpha)
        {
            specularPower = pow(2.0f, specularSample.a * 13.0f);
        }
        else
        {
            specularPower = specularPowerConst;
        }
    }
    else
    {
        specularReflectionColor = specularColor;
    }
    const float3 specularReflected = Speculate(
        specularReflectionColor, 1.0f, viewNormal,
        lv.vToL, viewFragPos, att, specularPower
    );
    
    float3 materialColor;
    if (diffuseEnabled)
    {
        materialColor = dtex.rgb;
    }
    else
    {
        materialColor.rgb = diffuseColorConst;
    }

	// final color
    return float4(saturate((diffuse + ambient) * materialColor + specularReflected), dtex.a);
}
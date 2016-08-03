#ifndef INSTANCES_COUNT
#error Missing INSTANCES_COUNT Define
#define INSTANCES_COUNT 0
#endif

#ifndef ENTITIES_COUNT
#error Missing ENTITIES_COUNT Define
#define ENTITIES_COUNT 0
#endif

cbuffer ColorsAndUVTransformsConstantBuffer : register(b0)
{
    float4 colorScaleAndIndex[INSTANCES_COUNT];
    float4 uvTranslationAndScales[ENTITIES_COUNT];
}

struct VShaderOut
{
    float4 positionH : SV_POSITION;
    float2 textCoord : TEXTCOORD;
    uint instanceID : SV_InstanceID;
};

Texture2D<float3> colorTexture : register(t0);
SamplerState colorTextureSampler : register(s0);

float4 main(VShaderOut pin) : SV_TARGET
{
    uint uvScaleAndTranslationIndex = uint(colorScaleAndIndex[pin.instanceID].w);
    float2 textCoord = pin.textCoord * uvTranslationAndScales[uvScaleAndTranslationIndex].zw + uvTranslationAndScales[uvScaleAndTranslationIndex].xy;
    
    float3 textureColor = colorTexture.Sample(colorTextureSampler, textCoord);
    float3 pixelColor = textureColor * colorScaleAndIndex[pin.instanceID].xyz;

    return float4(pixelColor, 1.0f);
}
#ifndef INSTANCES_COUNT
#error Missing INSTANCES_COUNT Define
#define INSTANCES_COUNT 0
#endif

cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 viewProjection;
}

cbuffer TransformsConstantBuffer : register(b1)
{
    float4 translationAndScales[INSTANCES_COUNT];
}

struct Vertex
{
    float3 position : POSITION;
    float2 textCoord : TEXTCOORD;
};

struct VShaderOut
{
    float4 positionH : SV_POSITION;
    float2 textCoord : TEXTCOORD;
    uint instanceID : SV_InstanceID;
};

VShaderOut main(Vertex vin, uint instanceID : SV_InstanceID)
{
    VShaderOut vout;

    float4 worldPos = float4(vin.position.xy * translationAndScales[instanceID].zw + translationAndScales[instanceID].xy, 0.0f, 1.0f);
    vout.positionH = mul(worldPos, viewProjection);

    vout.textCoord = vin.textCoord;

    vout.instanceID = instanceID;

    return vout;
}
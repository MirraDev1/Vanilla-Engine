cbuffer MaterialBuffer : register(b1)
{
    float4 diffuseColor;
};

struct Pixel_in
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
};

float4 PSMain(Pixel_in input) : SV_TARGET
{
    float3 gradient = float3(0.15f + input.texCoord.x * 0.85f,
                             0.15f + (1.0f - input.texCoord.y) * 0.85f,
                             0.85f);
    float lighting = 0.35f + 0.65f * saturate(dot(normalize(input.normal), float3(0.0f, 0.0f, -1.0f)));
    return float4(gradient * diffuseColor.rgb * lighting, diffuseColor.a);
}

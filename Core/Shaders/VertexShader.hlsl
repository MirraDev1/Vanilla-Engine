cbuffer TransformBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix normalMatrix;
};

struct vertex_in
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
};

struct vertex_out
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
};

vertex_out VSMain(vertex_in input)
{
    vertex_out output;
    output.position = mul(float4(input.position, 1.0), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    output.normal = mul(float4(input.normal, 0.0), normalMatrix).xyz;
    output.texCoord = input.texCoord;
    return output;
}

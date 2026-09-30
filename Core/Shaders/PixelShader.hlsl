Texture2D vltexture : register(t0);
sampler samplerState : register(s0);

struct Pixel_in
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

float4 PSMain(Pixel_in input) : SV_TARGET
{
    float4 color = vltexture.Sample(samplerState, input.TexCoord.xy);
    return color.a > 0.0f ? color : float4(0.20f, 0.65f, 0.95f, 1.0f);
}

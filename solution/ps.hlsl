
struct VERTEX
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D image : register(t0);
SamplerState samler : register(s0);

float4 main(VERTEX input) : SV_TARGET
{
   
    return image.Sample(samler, input.uv);
}
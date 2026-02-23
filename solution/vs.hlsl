
struct VERTEX
{
    float2 pos : POSITION;
    float2 uv : UVCOORD;
};

struct VERTEX_OUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

VERTEX_OUT main(VERTEX input)
{
    VERTEX_OUT output;
    output.pos = float4(input.pos.x, input.pos.y, 0.0f, 1.0f);
    output.uv = input.uv;
	return output;
}
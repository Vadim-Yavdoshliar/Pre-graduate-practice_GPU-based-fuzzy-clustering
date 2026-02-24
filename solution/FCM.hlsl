

struct VERTEX
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D image : register(t0);
SamplerState samler : register(s0);

struct sumBlock
{
uint
    nr,
    ng,
    nb, 
    na,
    d;
};



RWStructuredBuffer<sumBlock> newCenters : register(u0);
RWTexture3D<uint> assignmentMatrix : register(u1);
StructuredBuffer<float4> centers : register(t1);

cbuffer clusteringData : register(b0)
{
    uint clustersNum;
    float m;
    float width;
    float height;
};


float4 main(VERTEX input) : SV_TARGET
{
    
    float4 pixel = image.Sample(samler, input.uv);
    uint3 assignmentPos;
    assignmentPos.x = (uint)(width * input.uv.x);
    assignmentPos.y = (uint)(height * input.uv.y);
    
    // update centroids
    for (int i = 0; i < clustersNum; ++i)
    {
        assignmentPos.z = i;
        float modifiedAssignment = pow(assignmentMatrix[assignmentPos], m);
        float4 modifiedPixel = mul(modifiedAssignment, pixel);
        InterlockedAdd(newCenters[i].nr, modifiedPixel.r);
        InterlockedAdd(newCenters[i].ng, modifiedPixel.g);
        InterlockedAdd(newCenters[i].nb, modifiedPixel.b);
        InterlockedAdd(newCenters[i].na, modifiedPixel.a);
        InterlockedAdd(newCenters[i].d, modifiedAssignment);
        
    }
    
    // update assignment matrix
    float powerValue = 2.0f / (m - 1.0f);
    for (int i = 0; i < clustersNum; ++i)
    {
        float i_res = 0.0f;
        float nominator = length(pixel - centers[i]);
        for (int k = 0; k < clustersNum; ++k)
        {
            float denominator = length(pixel - centers[k]);
            if (denominator > 0.0f)
            {
                i_res +=
                pow
                (
                    nominator /
                    length(pixel - centers[k]),
                    powerValue
                );
            }
            
        }
        
        assignmentPos.z = i;
        assignmentMatrix[assignmentPos] = pow(i_res, -1.0f);;

    }
    
    return image.Sample(samler, input.uv);
}
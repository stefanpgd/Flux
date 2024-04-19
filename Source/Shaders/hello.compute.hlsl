//#define BLOCK_SIZE 8

struct ComputeShaderInput
{
    uint3 GroupID : SV_GroupID;                     // 3D index of the thread group in the dispatch.
    uint3 GroupThreadID : SV_GroupThreadID;         // 3D index of local thread ID in a thread group.
    uint3 DispatchThreadID : SV_DispatchThreadID;   // 3D index of global thread ID in the dispatch.
    uint GroupIndex : SV_GroupIndex;                // Flattened local index of the thread within a thread group.
};

RWTexture2D<float2> testTexture : register(u0);

[numthreads(1, 1, 1)]
void main(ComputeShaderInput IN)
{
    float strengthX = IN.DispatchThreadID.x / 1024.0f;
    float strengthY = IN.DispatchThreadID.y / 1024.0f;
    
    testTexture[IN.DispatchThreadID.xy] = float2(strengthX, strengthY);
}
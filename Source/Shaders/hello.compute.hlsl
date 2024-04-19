//#define BLOCK_SIZE 8

struct ComputeShaderInput
{
    uint3 GroupID : SV_GroupID;                     // 3D index of the thread group in the dispatch.
    uint3 GroupThreadID : SV_GroupThreadID;         // 3D index of local thread ID in a thread group.
    uint3 DispatchThreadID : SV_DispatchThreadID;   // 3D index of global thread ID in the dispatch.
    uint GroupIndex : SV_GroupIndex;                // Flattened local index of the thread within a thread group.
};

struct TestData
{
    float R;
};

RWTexture2D<float4> testTexture : register(u0);
RWStructuredBuffer<TestData> test : register(u1);

[numthreads(1, 1, 1)]
void main(ComputeShaderInput IN)
{
    float strength = test[IN.DispatchThreadID.x].R;
    
    testTexture[IN.DispatchThreadID.xy] = float4(strength, 0.0f, 0.0f, 1.0f);
}
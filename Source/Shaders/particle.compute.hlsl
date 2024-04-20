struct ComputeShaderInput
{
    uint3 GroupID : SV_GroupID; // 3D index of the thread group in the dispatch.
    uint3 GroupThreadID : SV_GroupThreadID; // 3D index of local thread ID in a thread group.
    uint3 DispatchThreadID : SV_DispatchThreadID; // 3D index of global thread ID in the dispatch.
    uint GroupIndex : SV_GroupIndex; // Flattened local index of the thread within a thread group.
};

struct Particle
{
    float2 position;
};

RWTexture2D<float4> testTexture : register(u0);
RWStructuredBuffer<Particle> particles : register(u1);

[numthreads(16, 1, 1)]
void main(ComputeShaderInput IN)
{
    Particle p = particles[IN.DispatchThreadID.x];
    
    p.position.x += 1.0f;
    if(p.position.x > 1023.0f)
    {
        p.position.x = 0.0f;
    }
    
    particles[IN.DispatchThreadID.x].position.x = p.position.x;
    
    testTexture[int2(p.position)] = float4(1.0, 1.0, 1.0, 1.0f);
}
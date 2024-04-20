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
    float2 velocity;
};

RWTexture2D<float4> testTexture : register(u0);
RWStructuredBuffer<Particle> particles : register(u1);

float2 CheckBounds(float2 position)
{
    int width;
    int height;
    
    // Grab texture bounds //
    testTexture.GetDimensions(width, height);

    if(position.x > width)
    {
        position.x = 0.0;
    }
    else if(position.x < 0.0)
    {
        position.x = width - 0.1;
    }
    
    if(position.y > height)
    {
        position.y = 0.0;
    }
    else if(position.y < 0.0)
    {
        position.y = height - 0.1;
    }
    
    return position;
}

[numthreads(16, 1, 1)]
void main(ComputeShaderInput IN)
{
    Particle p = particles[IN.DispatchThreadID.x];
    
    p.position += p.velocity;
    p.position = CheckBounds(p.position);
    
    particles[IN.DispatchThreadID.x].position = p.position;
    
    testTexture[int2(p.position)] = float4(1.0, 1.0, 1.0, 1.0f);
}
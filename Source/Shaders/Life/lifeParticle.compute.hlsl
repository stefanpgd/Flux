struct ComputeShaderInput
{
    uint3 GroupID : SV_GroupID; // 3D index of the thread group in the dispatch.
    uint3 GroupThreadID : SV_GroupThreadID; // 3D index of local thread ID in a thread group.
    uint3 DispatchThreadID : SV_DispatchThreadID; // 3D index of global thread ID in the dispatch.
    uint GroupIndex : SV_GroupIndex; // Flattened local index of the thread within a thread group.
};

struct SimulationSettings
{
    uint particleCount;
};
ConstantBuffer<SimulationSettings> settings : register(b0);

struct Particle
{
    float2 position;
    float2 velocity;
    int color;
};

RWTexture2D<float4> backBuffer : register(u0); // acts as 'TrailMap'
RWStructuredBuffer<Particle> particles : register(u1);

static float3 cellColors[2] =
{
    float3(1.0, 0.0, 0.0),
    float3(0.0, 1.0, 0.0)
};

float2 CheckBounds(float2 position)
{
    int width;
    int height;
    
    // Grab texture bounds //
    backBuffer.GetDimensions(width, height);

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

[numthreads(64, 1, 1)]
void main(ComputeShaderInput IN)
{
    uint ID = IN.DispatchThreadID.x;
    if(ID >= settings.particleCount)
    {
        return;
    }
    
    Particle particle = particles[ID];
    
    for(uint i = 0; i < settings.particleCount; i++)
    {
        if(ID == i)
        {
            continue;
        }
        
        Particle p2 = particles[i];
    }
    
    backBuffer[uint2(particle.position)] = float4(cellColors[particle.color], 1.0);
}
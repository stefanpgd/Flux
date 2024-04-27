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
    float deltaTime;
};
ConstantBuffer<SimulationSettings> settings : register(b0);

struct Agent
{
    float2 position;
    float angle;
};

RWTexture2D<float4> backBuffer : register(u0); // acts as 'TrailMap'
RWStructuredBuffer<Agent> agents : register(u1);

static float PI = 3.14159265;

float Random(uint seed)
{
    // Hash function from H. Schechter & R. Bridson, goo.gl/RXiKaH
    seed ^= 2747636419u;
    seed *= 2654435769u;
    seed ^= seed >> 16;
    seed *= 2654435769u;
    seed ^= seed >> 16;
    seed *= 2654435769u;
    
    return float(seed) / 4294967295.0; // 2^32-1
}

[numthreads(64, 1, 1)]
void main(ComputeShaderInput IN)
{
    uint ID = IN.DispatchThreadID.x;
    if(ID >= settings.particleCount)
    {
        return;
    }
    
    int width;
    int height;
    
    // TODO: probably makes this a part of the constant Data settings
    // Grab texture bounds //
    backBuffer.GetDimensions(width, height);
    
    Agent agent = agents[ID];
    float2 direction = float2(cos(agent.angle), sin(agent.angle));
    
    const float speed = 50.0f * settings.deltaTime;
    agent.position += direction * speed;
    
    if(agent.position.x > width || agent.position.x < 0.0 ||
        agent.position.y > height || agent.position.y < 0.0)
    {
        agent.position.x = clamp(agent.position.x, 0.1f, width - 0.1f);
        agent.position.y = clamp(agent.position.y, 0.1f, height - 0.1f);
        
        // Set a new direction ( angle )
        uint seed = agent.position.y * width + agent.position.x;
        agent.angle = Random(seed) * PI * 2.0;
    }
    
    agents[ID] = agent;
    backBuffer[uint2(agent.position)] = float4(1.0, 1.0, 1.0, 1.0);
}
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
    float mass;
};

struct SimulationSettings
{
    uint particleCount;
    float deltaTime;
    float G;
    float maxVelocity;
	float simulationSpeed;
};
ConstantBuffer<SimulationSettings> settings : register(b0);

RWTexture2D<float4> backBuffer : register(u0);
RWStructuredBuffer<Particle> particles : register(u1);

float2 CheckBounds(float2 position)
{
    int width;
    int height;
    
    // Grab texture bounds //
    backBuffer.GetDimensions(width, height);

    if (position.x > width)
    {
        position.x = 0.0;
    }
    else if (position.x < 0.0)
    {
        position.x = width - 0.1;
    }
    
    if (position.y > height)
    {
        position.y = 0.0;
    }
    else if (position.y < 0.0)
    {
        position.y = height - 0.1;
    }
    
    return position;
}

[numthreads(64, 1, 1)]
void main(ComputeShaderInput IN)
{
    if (IN.DispatchThreadID.x >= settings.particleCount)
    {
        return;
    }
    
    const float G = settings.G;
    const float maxVelocity = settings.maxVelocity;
    float2 acceleration = float2(0.0, 0.0);
    
    uint particleIndex = IN.DispatchThreadID.x;
    Particle p = particles[particleIndex];
    
    for (uint i = 0; i < settings.particleCount; i++)
    {
        if (particleIndex == i)
        {
            continue;
        }
        
        Particle p2 = particles[i];
        
        float2 dir = p2.position - p.position;
        float distance = length(dir);
        float forceStrength = (G * p.mass * p2.mass) / (distance * distance);
        
        if (isnan(forceStrength))
        {
            continue;
        }
        
        float2 force = normalize(dir) * forceStrength;
        acceleration += force * (1.0 / p.mass);
    }
    
    p.velocity += acceleration * (settings.deltaTime * settings.simulationSpeed);
    
    float vLength = length(p.velocity);
    
    if (length(p.velocity) >= maxVelocity)
    {
        p.velocity = normalize(p.velocity) * maxVelocity;
    }
    
    p.position += p.velocity * (settings.deltaTime * settings.simulationSpeed);
    p.position = CheckBounds(p.position);
    
    particles[IN.DispatchThreadID.x] = p;
    
    const float coreStengthColor = 0.175f;
    const float sideStrengthColor = 0.015;
    const float outerStrengthColor = 0.005;
    
    float4 coreColor = float4(coreStengthColor, coreStengthColor, coreStengthColor, 1.0);
    float4 sideColor = float4(sideStrengthColor, sideStrengthColor, sideStrengthColor, 1.0);
    
    backBuffer[int2(p.position)] += coreStengthColor;
    backBuffer[int2(p.position) + int2(1, 0)] += sideColor;
    backBuffer[int2(p.position) + int2(-1, 0)] += sideColor;
    backBuffer[int2(p.position) + int2(0, 1)] += sideColor;
    backBuffer[int2(p.position) + int2(0, -1)] += sideColor;

    backBuffer[int2(p.position) + int2(1, 1)] += sideColor;
    backBuffer[int2(p.position) + int2(-1, -1)] += sideColor;
    backBuffer[int2(p.position) + int2(-1, 1)] += sideColor;
    backBuffer[int2(p.position) + int2(1, -1)] += sideColor;

    backBuffer[int2(p.position) + int2(2, 0)] += outerStrengthColor;
    backBuffer[int2(p.position) + int2(-2, 0)] += outerStrengthColor;
    backBuffer[int2(p.position) + int2(0, 2)] += outerStrengthColor;
    backBuffer[int2(p.position) + int2(0, -2)] += outerStrengthColor;
}
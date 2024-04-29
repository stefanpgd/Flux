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

float AttractionForce(float distance, float attractionFactor)
{
    float result = 0.0f;
    const float beta = 0.3f;
    
    if(distance < beta)
    {
        result = (distance / beta) - 1.0f; // 0 to -1.0
    }
    else if(distance < 1.0f)
    {
        float y = 1.0 - (abs(distance * 2.0 - 1.0 - beta)) / (1.0 - beta);
        result = y * attractionFactor;
    }
    
    return result;
}

[numthreads(64, 1, 1)]
void main(ComputeShaderInput IN)
{
    uint ID = IN.DispatchThreadID.x;
    if(ID >= settings.particleCount)
    {
        return;
    }
    
    const float maxDistance = 15.0f;
    float AM[2][2];
    AM[0][0] = 0.8;    // Red's attraction to Red
    AM[0][1] = -0.6;    // Red's attraction to Green
    AM[1][0] = 0.9;    // Green's attraction to Red
    AM[1][1] = 0.05;    // Green's attraction to Green
    
    Particle p = particles[ID];
    
    float2 acceleration = float2(0.0f, 0.0f);
    for(uint i = 0; i < settings.particleCount; i++)
    {
        if(ID == i)
        {
            continue;
        }
        
        Particle p2 = particles[i];
        
        float2 BA = p2.position - p.position;
        float distance = length(BA);
        
        if(distance > maxDistance)
        {
            continue;
        }
        
        float attraction = AM[p.color][p2.color];
        float normalizedDistance = distance / maxDistance;
        
        if(isnan(normalizedDistance))
        {
            continue;
        }
        
        float force = AttractionForce(normalizedDistance, attraction) * maxDistance;
        acceleration += normalize(BA) * force;
    }
    
    // TODO: Add simulation speed again
    const float friction = 2.5f;
    p.velocity = lerp(p.velocity, p.velocity * 0.5, friction * settings.deltaTime);
    
    p.velocity += acceleration * settings.deltaTime;
    p.position += p.velocity * settings.deltaTime;
    
    p.position = CheckBounds(p.position);
    
    particles[ID] = p;
    
    backBuffer[uint2(p.position)] = float4(cellColors[p.color], 1.0);
}
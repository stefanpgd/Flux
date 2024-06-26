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
    
    float agentSpeed;
    float agentTurnSpeed;
    
    float agentSensorAngle;
    float agentSensorDistance;
    int agentSensorSize;
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

// Senses the trailmap with a given size & location //
float SenseTrail(Agent agent, float angleOffset)
{
    // TODO: Again make width & height part of the settings...
    int width;
    int height;
    backBuffer.GetDimensions(width, height);
    
    float angle = agent.angle + angleOffset;
    float2 sensorDir = float2(cos(angle), sin(angle));
    int2 sensorCenter = agent.position + sensorDir * settings.agentSensorDistance;
    
    float summedTrail = 0.0f;
    for(int x = -settings.agentSensorSize; x <= settings.agentSensorSize; x++)
    {
        for(int y = -settings.agentSensorSize; y <= settings.agentSensorSize; y++)
        {
            int2 coord = sensorCenter + int2(x, y);

            // TODO: Test the difference between 'stay in bounds' and 'move to other side'
            if(coord.x < 0 || coord.x > int(width) || coord.y < 0 || coord.y > int(height))
            {
                continue;
            }
            
            float3 sample = backBuffer[coord].rgb;
            summedTrail += sample.r + sample.b + sample.g; // since you can change color here
        }
    }

    return summedTrail;
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
    
    float speed = settings.agentSpeed * settings.deltaTime;
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
    
    agents[ID].position = agent.position;
    
    // Steering 'weights' based on the intensity of the 'sensed' trailmap
    float forward = SenseTrail(agent, 0.0f);
    float left = SenseTrail(agent, settings.agentSensorAngle);
    float right = SenseTrail(agent, -settings.agentSensorAngle);
    
    float rand = Random(agent.position.y * width + agent.position.x);
    
    // If forward is the strongest
    if(forward > left && forward > right)
    {
        agents[ID].angle = agent.angle;
    }
    else if(forward < left && forward < right) // if no forward, then make random decisions
    {
        agents[ID].angle += (rand - 0.5) * 2.0 * settings.agentTurnSpeed * settings.deltaTime;
    }
    else if(right > left) // Right is strongest
    {
        agents[ID].angle -= rand * settings.agentTurnSpeed * settings.deltaTime;
    }
    else if(left > right) // Left is strongest
    {
        agents[ID].angle += rand * settings.agentTurnSpeed * settings.deltaTime;
    }
    
    backBuffer[uint2(agent.position)] = float4(1.0, 1.0, 1.0, 1.0);
}
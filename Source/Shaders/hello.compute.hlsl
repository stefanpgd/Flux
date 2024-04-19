//#define BLOCK_SIZE 8

struct ComputeShaderInput
{
    uint3 GroupID : SV_GroupID;                     // 3D index of the thread group in the dispatch.
    uint3 GroupThreadID : SV_GroupThreadID;         // 3D index of local thread ID in a thread group.
    uint3 DispatchThreadID : SV_DispatchThreadID;   // 3D index of global thread ID in the dispatch.
    uint GroupIndex : SV_GroupIndex;                // Flattened local index of the thread within a thread group.
};

struct Particle
{
    float2 test;
};

RWTexture2D<float4> testTexture : register(u0);
RWStructuredBuffer<Particle> particles : register(u1);

[numthreads(16, 1, 1)]
void main(ComputeShaderInput IN)
{
    // boundaries x: 0, 10
    // boundaries y: 0, 5
    Particle p = particles[IN.DispatchThreadID.x];
    
    p.test.x += 1.0f;
    if(p.test.x > 1023.0f)
    {
        p.test.x = 0.0f;
    }
    
    particles[IN.DispatchThreadID.x].test.x = p.test.x;
    
   // const float worldBoundaryX = 10.0;
   // const float worldBoundaryY = 10.0;
   // const int screenWidth = 1024;
   // const int screenHeight = 1024;
   // 
   // float unitPixelSizeX = float(screenWidth) / worldBoundaryX;
   // float unitPixelSizeY = float(screenHeight) / worldBoundaryX;
   // 
   // int pixelX = clamp(int(p.position.x * unitPixelSizeX), 0, screenWidth - 1);
   // int pixelY = clamp(int(p.position.y * unitPixelSizeY), 0, screenHeight - 1);
   // 
   // uint2 location = uint2(pixelX, pixelY);
    
    uint2 testLocation = uint2(p.test.x, p.test.y);
    testTexture[testLocation] = float4(1.0, 1.0, 1.0, 1.0f);
}
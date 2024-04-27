struct ComputeShaderInput
{
    uint3 GroupID : SV_GroupID; // 3D index of the thread group in the dispatch.
    uint3 GroupThreadID : SV_GroupThreadID; // 3D index of local thread ID in a thread group.
    uint3 DispatchThreadID : SV_DispatchThreadID; // 3D index of global thread ID in the dispatch.
    uint GroupIndex : SV_GroupIndex; // Flattened local index of the thread within a thread group.
};

struct Settings
{
    float deltaTime;
    float trailDissolveSpeed;
    float trailDiffuseSpeed;
};
ConstantBuffer<Settings> settings : register(b0);

RWTexture2D<float4> targetTexture : register(u0);

[numthreads(1, 1, 1)]
void main(ComputeShaderInput IN)
{
    uint2 ID = IN.DispatchThreadID.xy;
    uint width;
    uint height;
    targetTexture.GetDimensions(width, height);
    
    if(ID.x > width || ID.y > height)
    {
        return;
    }
    
    // TODO: Maybe make texture a FloatRGB texture instead of UNORM
    float4 trail = targetTexture[ID.xy];
    
    float4 summedTrail = float4(0.0, 0.0, 0.0, 0.0);
    for(int x = -1; x <= 1; x++)
    {
        for(int y = -1; y <= 1; y++)
        {
            int coordX = ID.x + x;
            int coordY = ID.y + y;
            
            if(coordX < 0 || coordX > int(width) || coordY < 0 || coordY > int(height))
            {
                continue;
            }
            
            summedTrail += targetTexture[uint2(coordX, coordY)];
        }
    }
    
    // Samped 3x3 grid, so average it out
    summedTrail /= 9.0;
    
    // TODO: Really need to switch to Float maps for trail, since multiplication gets cutoff due to it
    // being below a 0-255 range....
    float4 diffuse = lerp(trail, summedTrail, settings.trailDiffuseSpeed * settings.deltaTime);
    float4 dissolve = diffuse - settings.trailDissolveSpeed * settings.deltaTime;
    float4 processedTrail = max(0, dissolve);
    
    // TODO: Since its sampling nearby stuff, shouldn't I use a different texture to write to?
    targetTexture[ID.xy] = float4(processedTrail.rgb, 1.0f);
}
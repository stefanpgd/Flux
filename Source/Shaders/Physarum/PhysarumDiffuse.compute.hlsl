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
    float4 processed = trail - (settings.trailDissolveSpeed * settings.deltaTime);
    processed = max(0, processed);
    
    targetTexture[ID.xy] = float4(processed.rgb, 1.0f);
}
struct ComputeShaderInput
{
    uint3 GroupID : SV_GroupID; // 3D index of the thread group in the dispatch.
    uint3 GroupThreadID : SV_GroupThreadID; // 3D index of local thread ID in a thread group.
    uint3 DispatchThreadID : SV_DispatchThreadID; // 3D index of global thread ID in the dispatch.
    uint GroupIndex : SV_GroupIndex; // Flattened local index of the thread within a thread group.
};

RWTexture2D<float4> targetTexture : register(u0);

struct Settings
{
    float trailStrength;
    float trailCutoffOpacity;
};
ConstantBuffer<Settings> settings : register(b0);

[numthreads(4, 4, 1)]
void main(ComputeShaderInput IN)
{
    float density = targetTexture[IN.DispatchThreadID.xy].r * settings.trailStrength;
    float opacity = settings.trailCutoffOpacity;
    
    targetTexture[IN.DispatchThreadID.xy] = float4(density, opacity, 0.0f, 1.0f);
}
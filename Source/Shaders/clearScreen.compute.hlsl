RWTexture2D<float4> targetTexture : register(u0);

[numthreads(1, 1, 1)]
void main( uint3 dID : SV_DispatchThreadID )
{
    targetTexture[uint2(dID.xy)] = float4(0.0f, 0.0f, 0.0f, 1.0f);
}
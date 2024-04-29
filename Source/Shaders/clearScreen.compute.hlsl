RWTexture2D<float4> targetTexture : register(u0);

[numthreads(8, 8, 1)]
void main( uint3 dID : SV_DispatchThreadID )
{
    int width;
    int height;
    
    targetTexture.GetDimensions(width, height);
    
    if(dID.x > uint(width) || dID.y > uint(height))
    {
        return;
    }
    
    targetTexture[uint2(dID.xy)] = float4(0.0f, 0.0f, 0.0f, 1.0f);
}
struct PixelIN
{
    float2 TexCoord : TexCoord;
};

Texture2D screenTexture : register(t0);
SamplerState LinearSampler : register(s0);

float4 main(PixelIN IN) : SV_TARGET
{
    int width;
    int height;
    screenTexture.GetDimensions(width, height);
    
    float xOffset = 1.0f / float(width);
    float yOffset = 1.0f / float(height);
    
    float3 sum = float3(0.0f, 0.0f, 0.0f);
    for(int x = -1; x <= 1; x++)
    {
        for(int y = -1; y <= 1; y++)
        {
            float coordX = IN.TexCoord.x + x * xOffset;
            float coordY = IN.TexCoord.y + y * yOffset;
            
            sum += screenTexture.Sample(LinearSampler, float2(coordX, coordY)).rgb;
        }
    }
    sum /= 6.0;
    sum = clamp(sum, float3(0.0f, 0.0f, 0.0f), float3(1.0f, 1.0f, 1.0f));
    
    float3 color = screenTexture.Sample(LinearSampler, IN.TexCoord).rgb;
    return float4(sum, 1.0f);
}
struct PixelIN
{
    float2 TexCoord : TexCoord;
    float4 screenSpace : SV_Position;
};

Texture2D screenTexture : register(t0);
SamplerState LinearSampler : register(s0);

uint Hash(uint s)
{
    s ^= 2747636419u;
    s *= 2654435769u;
    s ^= s >> 16;
    s *= 2654435769u;
    s ^= s >> 16;
    s *= 2654435769u;
    return s;
}

float Random(uint seed)
{
    return float(Hash(seed)) / 4294967295.0; // 2^32-1
}

float4 main(PixelIN IN) : SV_TARGET
{
    float4 color = screenTexture.Sample(LinearSampler, IN.TexCoord);
    
    float c = Random(IN.screenSpace.y * 1024 + IN.screenSpace.x);
    return float4(c, c, c, 1.0f);
}
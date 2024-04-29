struct PixelIN
{
    float2 TexCoord : TexCoord;
};

Texture2D screenTexture : register(t0);
SamplerState LinearSampler : register(s0);

float4 main(PixelIN IN) : SV_TARGET
{
    float3 color = screenTexture.Sample(LinearSampler, IN.TexCoord).rgb;
    return float4(color, 1.0f);
}
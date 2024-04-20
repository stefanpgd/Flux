struct PixelIN
{
    float2 TexCoord : TexCoord;
};

Texture2D screenTexture : register(t0);
SamplerState LinearSampler : register(s0);

float4 main(PixelIN IN) : SV_TARGET
{
    float sample = screenTexture.Sample(LinearSampler, IN.TexCoord).r;
    return float4(sample, sample, sample, 1.0f);
}
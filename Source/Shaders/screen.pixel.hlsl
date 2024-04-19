struct PixelIN
{
    float2 TexCoord : TexCoord;
};

Texture2D screenTexture : register(t0);
SamplerState LinearSampler : register(s0);

float4 main(PixelIN IN) : SV_TARGET
{
    //float2 sample = screenTexture.Sample(LinearSampler, IN.TexCoord).rg;
    return float4(IN.TexCoord, 0.0f, 1.0f);
}
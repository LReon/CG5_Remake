#include "Test.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float32_t2 uv = input.texCoord;
    float32_t4 textureColor = gTexture.Sample(gSampler, uv);
    
    //float32_t2 uv = input.texCoord;
    // https://learn.microsoft.com/ja-jp/windows/win32/direct3dhlsl/dx-hlsl-per-component-math
    // 位置セット( x y z w ) か　カラーセット( r g b a )でアクセスできる
    output.color = float32_t4(uv.x, uv.y, 0.0f, 1.0f); // Red color
    return output;
}
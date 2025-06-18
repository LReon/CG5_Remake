#include "Test.hlsli"

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float32_t2 uv = input.texCoord;
    // https://learn.microsoft.com/ja-jp/windows/win32/direct3dhlsl/dx-hlsl-per-component-math
    // �ʒu�Z�b�g( x y z w ) ���@�J���[�Z�b�g( r g b a )�ŃA�N�Z�X�ł���
    output.color = float32_t4(uv.x, uv.y, 0.0f, 1.0f); // Red color
    return output;
}
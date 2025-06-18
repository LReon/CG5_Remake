#include "Test.hlsli"

struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t2 texCoord : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = input.position;
    output.texCoord = input.texCoord;
    return output;
}

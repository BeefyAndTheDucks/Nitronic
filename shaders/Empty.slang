struct Varyings
{
};

struct VertexInput
{
    [[vk::location(0)]] float3 position : POSITION;
    [[vk::location(1)]] float3 normal   : NORMAL;
    [[vk::location(2)]] float2 texCoord : TEXCOORD;
};

[shader("vertex")]
Varyings vertexMain(VertexInput input)
{
    Varyings output;
    return output;
}

[shader("fragment")]
float4 fragmentMain(Varyings input) : SV_Target
{
    return float4(1.0);
}

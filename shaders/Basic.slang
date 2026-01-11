struct Varyings
{
    float4 pos : SV_Position;
    float3 color : COLOR;
};

struct VertexInput
{
    [[vk::location(0)]] float3 position : POSITION;
    [[vk::location(1)]] float2 texCoord : TEXCOORD;
};

static const float3 colors[4] = {
    float3(0.122, 0.227, 0.373),
    float3(0.906, 0.435, 0.318),
    float3(0.165, 0.616, 0.561),
    float3(0.957, 0.945, 0.871)
};

[shader("vertex")]
Varyings vertexMain(
    VertexInput input,
    uint vertexIndex : SV_VertexID)
{
    Varyings output;

    output.pos = float4(input.position, 1.0);
    output.color = colors[vertexIndex % 4];

    return output;
}

[shader("fragment")]
float4 fragmentMain(Varyings input) : SV_Target
{
    return float4(input.color, 1.0);
}
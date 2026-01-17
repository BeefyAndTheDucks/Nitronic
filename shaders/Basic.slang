[[vk::binding(0)]]
cbuffer CB : register(b0)
{
    float4x4 model;
    float4x4 view;
    float4x4 projection;
};

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
Varyings vertexMain(VertexInput input)
{
    Varyings output;

    float4x4 mvp = mul(projection, mul(view, model));
    output.pos = mul(mvp, float4(input.position, 1.0));

    // lets use texcoord to index into our color array
    int flattened = (int)input.texCoord.x + (int)input.texCoord.y * 2;
    output.color = colors[flattened];

    return output;
}

[shader("fragment")]
float4 fragmentMain(Varyings input) : SV_Target
{
    return float4(input.color, 1.0);
}

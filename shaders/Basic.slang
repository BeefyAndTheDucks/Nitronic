cbuffer ModelConstants : register(b0)
{
  float4x4 viewProj;
  float4x4 model;
  float4x4 normalMatrix;
};

struct FragmentInput
{
    float4 pos : SV_Position;
    float3 normal : NORMAL;
    float3 worldPos : TEXCOORD1;
};

struct VertexInput
{
    [[vk::location(0)]] float3 position : POSITION;
    [[vk::location(1)]] float3 normal   : NORMAL;
    [[vk::location(2)]] float2 texCoord : TEXCOORD0;
};

[shader("vertex")]
FragmentInput vertexMain(VertexInput input)
{
    FragmentInput output;
    float4x4 mvp = mul(viewProj, model);
    output.pos = mul(mvp, float4(input.position, 1.0));
    output.worldPos = mul(model, float4(input.position, 1.0)).xyz;
    output.normal = mul((float3x3)normalMatrix, input.normal);
    return output;
}

[shader("fragment")]
float4 fragmentMain(FragmentInput input) : SV_Target
{
    float3 N = normalize(input.normal);
    float3 L = normalize(float3(5,5,5) - input.worldPos);

    float diffuse = max(dot(N, L), 0.0);
    float ambient = 0.2;

    float3 color = (ambient + diffuse) * float3(1.0, 1.0, 1.0);
    return float4(color, 1.0);
}

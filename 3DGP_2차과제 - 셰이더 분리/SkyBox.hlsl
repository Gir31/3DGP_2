/////////////////////////////////////////////////////////////// 
// include 
///////////////////////////////////////////////////////////////
#include "Common.hlsl"

/////////////////////////////////////////////////////////////// 
// struct
///////////////////////////////////////////////////////////////
struct VS_SKYBOX_CUBEMAP_INPUT
{
    float3 position : POSITION;
};

struct VS_SKYBOX_CUBEMAP_OUTPUT
{
    float3 positionL : POSITION;
    float4 position : SV_POSITION;
};

/////////////////////////////////////////////////////////////// 
// VS - PS
///////////////////////////////////////////////////////////////
VS_SKYBOX_CUBEMAP_OUTPUT VSSkyBox(VS_SKYBOX_CUBEMAP_INPUT input)
{
    VS_SKYBOX_CUBEMAP_OUTPUT output;

    // 스카이박스는 '거대한 큐브'라서 positionL = local direction 그대로 사용
    output.positionL = input.position;

    // View에서 위치 성분 삭제 → 카메라 이동 무시됨
    float4x4 view = gmtxView;
    view._41 = 0;
    view._42 = 0;
    view._43 = 0;

    // 큐브의 정점은 월드 변환 사용 X (고정)
    float4 posW = float4(input.position, 1.0f);

    // 카메라의 회전만 적용
    output.position = mul(mul(posW, view), gmtxProjection);

    return output;
}

float4 PSSkyBox(VS_SKYBOX_CUBEMAP_OUTPUT input) : SV_TARGET
{
    float4 cColor = gtxtSkyCubeTexture.Sample(gssClamp, input.positionL);

    return (cColor);
}
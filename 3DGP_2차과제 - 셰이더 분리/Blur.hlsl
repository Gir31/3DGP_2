/////////////////////////////////////////////////////////////// 
// 
///////////////////////////////////////////////////////////////
cbuffer cbBlurInfo : register(b0)
{
    float g_fBlurStrength; // 블러 강도 (플레이어 속도에 비례)
    float3 g_vPadding;
};

Texture2D<float4> g_Input : register(t0);

RWTexture2D<float4> g_Output : register(u0);

/////////////////////////////////////////////////////////////// 
// struct
///////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////// 
// VS - GS - PS
///////////////////////////////////////////////////////////////
[numthreads(32, 32, 1)]
void CSMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    // 텍스처 크기 가져오기
    uint width, height;
    g_Input.GetDimensions(width, height);

    // 쓰레드 ID가 텍스처 범위를 벗어나면 종료
    if (dispatchThreadID.x >= width || dispatchThreadID.y >= height)
        return;

    float2 uv = float2(dispatchThreadID.x, dispatchThreadID.y) / float2(width, height);
    float2 center = float2(0.5f, 0.5f); // 화면 중심

    // 중심에서 현재 픽셀까지의 벡터
    float2 dir = center - uv;
    float dist = length(dir);

    // 거리에 따라 블러 강도 조절 (중심은 선명하게, 외곽은 흐리게)
    dir = (dist > 0.0f) ? (dir / dist) : float2(0, 0);
    
    float4 color = g_Input[dispatchThreadID.xy];
    int nSamples = 10;
    
    float fStrength = g_fBlurStrength * dist; // 외곽일수록 더 강하게

    for (int i = 1; i < nSamples; ++i)
    {
        float2 sampleUV = uv + dir * (fStrength * (float) i / (float) nSamples);
        uint2 samplePos = (uint2) (sampleUV * float2(width, height));

        if (samplePos.x < width && samplePos.y < height)
        {
            color += g_Input[samplePos];
        }
    }

    color /= (float) nSamples; 

    g_Output[dispatchThreadID.xy] = color;
    
    //g_Output[dispatchThreadID.xy] = float4(0.0f, 1.0f, 0.0f, 1.0f);
}
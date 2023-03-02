//=============================================================================
// Constant Values
//=============================================================================
static const float F_PI = 3.141596535f;

//=============================================================================
// Vertex Shader Output Struct
//=============================================================================
struct VSOutput
{
	float4 Position : SV_POSITION;	// 位置座標
	float2 TexCoord : TEXCOORD;		// テクスチャ座標
	float3 Normal	: NORMAL;		// 法線ベクトル
	float3 WorldPos : WORLD_POS;	// ワールド空間での位置座標
};

//=============================================================================
// Pixel Shader Output Struct (Single Render Target) 
//=============================================================================
struct PSOutput
{
	float4 Color : SV_TARGET0; //出力カラー
};


//=============================================================================
// Light Buffuer
//=============================================================================
cbuffer LightBuffer : register(b1){
	float3 LightPosition  : packoffset(c0);
	float3 LightColor	 : packoffset(c1);
	float3 CameraPosition : packoffset(c2);
}

//=============================================================================
// Material Buffer
//=============================================================================
cbuffer MaterialBuffer : register(b2){
	float3 BaseColor : packoffset(c0);
	float  Alpha     : packoffset(c0.w);
	float3 Metallic  : packoffset(c1);
    float Shininess : packoffset(c1.w);
}

//=============================================================================
// Texture and Samplers
//=============================================================================
SamplerState WrapSmp : register(s0);
Texture2D ColorMap : register(t0);

//-----------------------------------------------------------------------------
// Main Entry Point
//-----------------------------------------------------------------------------
PSOutput main(VSOutput input) : SV_TARGET
{
    PSOutput output = (PSOutput) 0;

    float3 N = normalize(input.Normal);
    float3 L = normalize(LightPosition - input.WorldPos);
    float3 V = normalize(CameraPosition - input.WorldPos);
    float3 R = normalize(-V + 2.0f * dot(N, V) * N);

    float NL = saturate(dot(N, L));
    float LR = saturate(dot(L, R));

    float4 color = ColorMap.Sample(WrapSmp, input.TexCoord);
    float3 diffuse = BaseColor * (1.0f - Metallic) * (1.0 / F_PI);
    float3 specular = BaseColor * Metallic * ((Shininess + 2.0) / (2.0 * F_PI)) * pow(LR, Shininess);

    output.Color = float4(LightColor * color.rgb * (diffuse + specular) * NL, color.a * Alpha);
    return output;
}
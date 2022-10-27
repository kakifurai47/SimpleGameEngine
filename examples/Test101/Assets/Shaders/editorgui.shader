#if 0
Shader {
	Properties {
	}

	Pass {
		// Queue	"Transparent"
		Cull		Off

		DepthTest	Always
//		DepthWrite	false

		BlendRGB 	Add One OneMinusSrcAlpha
		BlendAlpha	Add One OneMinusSrcAlpha
		
		VsFunc		vs_main
		PsFunc		ps_main
	}
}
#endif

//cbuffer vertexBuffer : register(b0)
//{
//    float4x4 ProjectionMatrix;
//};

struct VS_INPUT
{
    float2 pos : POSITION;
    float4 col : COLOR0;
    float2 uv  : TEXCOORD0;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
    float2 uv  : TEXCOORD0;
};

float4x4 ProjectionMatrix;

Texture2D     texture0;
SamplerState  sampler0;


PS_INPUT vs_main(VS_INPUT input)
{
    PS_INPUT output;
    output.pos = mul( ProjectionMatrix, float4(input.pos.xy, 0.f, 1.f));
    output.col = input.col;
    output.uv  = input.uv;
    return output;
}


float4 ps_main(PS_INPUT input) : SV_Target
{
    float4 out_col = input.col * texture0.Sample(sampler0, input.uv).r;
    return out_col;
}
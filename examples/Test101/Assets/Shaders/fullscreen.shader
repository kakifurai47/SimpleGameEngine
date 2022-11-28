#if 0
Shader {
	Properties {
	}
	
	Pass {
//		Queue	"Transparent"
		Cull		Off
//		BlendRGB 	Add One OneMinusSrcAlpha
//		BlendAlpha	Add One OneMinusSrcAlpha
		DepthTest	Always
		DepthWrite	false

		VsFunc		vs_main
		PsFunc		ps_main
	}
}
#endif

//struct VertexIn {
//	float4 position	 : POSITION;
//	float2 uv		 : TEXCOORD0;
//};

struct PixelIn {
    float4 position : SV_POSITION;    
	float2 uv 		: TEXCOORD0;
};

float4x4	SGE_MVP;

Texture2D mainTex;
SamplerState mainTex_Sampler;

PixelIn vs_main(uint vi : SV_VertexID) {
    PixelIn o;
	float2 uv   = float2((vi << 1) & 2, vi & 2);
    o.uv        = uv;
    o.position  = float4(uv.x * 2 - 1, -uv.y * 2 + 1, 0, 1);
	
    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
//	return float4(1,0,0,1);
	float3 texCol = mainTex.Sample(mainTex_Sampler, i.uv);
	return float4(texCol, 1.0f);
}

#if 0
Shader {
	Properties {
		Float	test  = 0.5
		Vec4f	test2 = {0,0,0,1}
		
		[DisplayName="Color Test"]
		Color4f	color = {1,1,1,1}
	}
	
	Pass {
		//Queue	"Transparent"
		Cull		Off
		BlendRGB 	Add One OneMinusSrcAlpha
		BlendAlpha	Add One OneMinusSrcAlpha
		DepthTest	LessEqual
		DepthWrite	false

		VsFunc		vs_main
		PsFunc		ps_main
	}
}
#endif

struct VertexIn {
	float4 position	 : POSITION;
//	float4 color	 : COLOR;
};

struct PixelIn {
    float4 position : SV_POSITION;
//  float4 color	: COLOR;
	float2 uv 		: TEXCOORD0;
};

float4x4	SGE_MVP;

float  x;
float4 p;
float4 v;

float  patchSize;
float3 patchPos;
float  patchLevel;
float  terrainSize;
float  terrainHeight;

Texture2D    terrainHeightMap;
SamplerState terrainHeightMap_Sampler;

PixelIn vs_main(VertexIn i) {
    PixelIn o;

	float4 p = float4(i.position.xyz * (patchSize * 0.97f), 1.f);
	
	p += float4(patchPos, 0.f);
	p =  float4(p.xyz * terrainSize, 1.f);

	float2 uv = i.position.xz * patchSize + patchPos.xz;
	float   y = terrainHeight * terrainHeightMap.SampleLevel(terrainHeightMap_Sampler, uv, 0).r;

	p = float4(p.x, y, p.z, 1.f);


	o.position = mul(SGE_MVP, p);

//  o.position = mul(SGE_MVP, i.position);
//  o.color    = i.color;
//	o.uv	   = i.uv;
    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
	return float4(1, patchLevel / 7.f, 0, 1);


//	float3 texCol = mainTex.Sample(mainTex_Sampler, i.uv);
//	return float4(texCol, i.color.a);
    //return i.color * x;// * b;// * c;
}

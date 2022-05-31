Shader {
	Properties {
		Float	test  = 0.5
		Vec4f	test2 = {0,0,0,1}
		
		[DisplayName="Color Test"]
		Color4f	color = {1,1,1,1}
	}
	
	Pass {
		// Queue	"Transparent"
		// Cull		None

		// BlendRGB 	Add One OneMinusSrcAlpha
		// BlendAlpha	Add One OneMinusSrcAlpha

		// DepthTest	Always
		// DepthWrite	false
		
		VsFunc		vs_main
		PsFunc		ps_main
	}
}

struct VertexIn {
	float4 position : POSITION;
	float4 color : COLOR;
};

struct PixelIn {
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

float4x4	SGE_MVP;

float  x;
double b;
float4 p;
float4 v;

cbuffer VSConstants {
	float4x4 WVPMatrix1;
	float4x4 WVPMatrix2;
}

cbuffer VSConstants2 {
	float4x4 WVPMatrix3;
	float4x4 WVPMatrix4;
}


PixelIn vs_main(VertexIn i) {
    PixelIn o;	
    o.position = i.position;
	o.position = p;
    o.color    = i.color;
	o.color.x  = x;
	o.color.y  = v.y;
	o.color.z  = WVPMatrix1._m00;
	o.color.w  = WVPMatrix3._m02;
    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
    return i.color * x;// * b;// * c;
}

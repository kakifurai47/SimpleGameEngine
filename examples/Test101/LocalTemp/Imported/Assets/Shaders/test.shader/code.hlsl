//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//


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
float4 p;
float4 v;

PixelIn vs_main(VertexIn i) {
    PixelIn o;	
    o.position = i.position;
    o.color    = i.color;
    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
    return i.color * x;// * b;// * c;
}

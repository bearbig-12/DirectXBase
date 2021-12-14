cbuffer MatrixBuffer
{
	matrix worldMatrix;

};
struct VOut
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

VOut main(float4 pos : POSITION, float2 tex : TEXCOORD0)
{

	VOut output;

	output.position = mul(pos, worldMatrix);
	output.tex = tex;

	return output;
}
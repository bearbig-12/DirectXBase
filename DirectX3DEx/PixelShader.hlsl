Texture2D shaderTexture;

SamplerState Sampler
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct PIn
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};


float4 main(PIn input) : SV_TARGET
{
	float4 result = shaderTexture.Sample(Sampler, input.tex);

	
	//float aver = (result[0] + result[1] + result[2]) / 3;
	//if (aver > 0.6)
	//{
	//	result[0] = result[1] = result[2] = 255;
	//}
	//else
	//{
	//	result[0] = result[1] = result[2] = 0;
	//}

	//result[0] = aver;
	//result[1] = aver;
	//result[2] = aver;

	return result;
}
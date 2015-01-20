Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer cbData
{
	float4x4 World;
	float4x4 View;
	float4x4 Projection;

	float4 gDiffuseMtrl;	//Diffusal
	float4 gDiffuseLight;


	float4 gAmbientMtrl;	//Ambient Material
	float4 gAmbientLight;	//Ambient Colour

	float4 gSpecularMtrl;	//Specular
	float4 gSpecularLight;
	float gSpecularPower;

	float3 gEyePosW;
	float3 gLightVecW;

};

struct VS_IN
{
	float4 PosL   : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex : TEXCOORD0;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float3 PosW : POSITION;
	float3 Norm : NORMAL;
	float2 Tex : TEXCOORD0;
};


VS_OUT VS(VS_IN vIn)
{
	VS_OUT vout = (VS_OUT)0; // Transform to world space space. 

	vout.Pos = mul(vIn.PosL, World);
	vout.PosW = vout.Pos.xyz;

	vout.Pos = mul(vout.Pos, View);
	vout.Pos = mul(vout.Pos, Projection);

	float3 normalW = mul(float4(vIn.NormalL, 0.0f), World).xyz;
		vout.Norm = normalize(normalW);

	vout.Tex = vIn.Tex;

	return vout;


}


float4 PS(VS_OUT pIn) : SV_Target
{
	pIn.Norm = normalize(pIn.Norm);
	float3 lightVec = normalize(gLightVecW);
		float3 toEye = normalize(gEyePosW - pIn.PosW.xyz);

		// Compute Colour
		// Compute the reflection vector.
		float3 r = reflect(-lightVec, pIn.Norm);
		// Determine how much (if any) specular light makes it into the eye.
		float t = pow(max(dot(r, toEye), 0.0f), gSpecularPower);

	// Determine the diffuse light intensity that strikes the vertex.
	float s = max(dot(lightVec, pIn.Norm), 0.0f);

	if (s <= 0.0f)
	{
		t = 0.0f;
	}

	// Compute the ambient, diffuse, and specular terms separately.
	float3 spec = t*(gSpecularMtrl*gSpecularLight).rgb;
		float3 diffuse = s*(gDiffuseMtrl*gDiffuseLight).rgb;
		float3 ambient = (gAmbientMtrl * gAmbientLight).rgb;

		// Sum all the terms together and copy over the diffuse alpha.
		float4 Col;
	float4 textureColour = txDiffuse.Sample(samLinear, pIn.Tex);
	Col.rgb = (textureColour.rgb * (ambient + diffuse)) + spec;
	Col.a = gDiffuseMtrl.a;

	return Col;
}

technique11 Render
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));
	}
}

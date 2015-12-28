#include "EzD3DX9HardwareSkinning.h"

/*

struct appdata  {
	float3 Position: POSITION;
	float4 Weight:BLENDWEIGHT;
	short Indices:BLENDINDICES;
	float3 Normal: NORMAL;
	float2 TexCoord:TEXCOORD0;    
};

struct vpconn  {
	float4 HPosition: POSITION;
	float2 TexCoord: TEXCOORD0;
	float4 Color: COLOR0;
};


vpconn main(appdata IN,
			uniform float4x4 WorldViewProj: register (c0),
			uniform float3 LightVec: register (c4),
			uniform float4 Diffuse: register (c5),
			uniform float4 Ambient: register (c6),
			uniform float3x4 Bones[MAXBONES]: register (c7)
			)
{
	vpconn OUT;

	float4 tempPos;
	tempPos.xyz = IN.Position.xyz;
	tempPos.w = 1.0;

	float3 pos= 0;
	float3 normal= 0;

	float4 i;

	i = IN.Indices;

	pos  = mul(Bones[i.x], tempPos) * IN.Weight.x;
	pos += mul(Bones[i.y], tempPos) * IN.Weight.y;
	pos += mul(Bones[i.z], tempPos) * IN.Weight.z;
	pos += mul(Bones[i.w], tempPos) * IN.Weight.w;

	float3x3 m;

	m[0].xyz = Bones[i.x][0].xyz;
	m[1].xyz = Bones[i.x][1].xyz;
	m[2].xyz = Bones[i.x][2].xyz;

	normal += mul(m, IN.Normal)* IN.Weight.x;


	m[0].xyz = Bones[i.y][0].xyz;
	m[1].xyz = Bones[i.y][1].xyz;
	m[2].xyz = Bones[i.y][2].xyz;

	normal += normalize(mul(m, IN.Normal)* IN.Weight.y);		

	normal = normalize(normal);

	OUT.Color.xyzw = (max(dot(normal, LightVec),0).xxxx+Ambient)*Diffuse;

	OUT.TexCoord.xy = IN.TexCoord.xy;

	float4 tempPos2;

	tempPos2.xyz = pos.xyz;
	tempPos2.w = 1.0;

	OUT.HPosition = mul(WorldViewProj, tempPos2);

	return OUT;
}
*/

/*
; Constants specified by the app
;       c0-3 = ViewProj
;       c4   = World-space light direction
;       c5   = constants - 1.0f, 0.0f, 1.0f, 1020.01f
;       c10+ = bone matrices (with bone-to-skin-to-world transform)
;
; Vertex components (as specified in the vertex DECL)
;       v0  = pVertex[i].position
;       v1  = pVertex[i].blendweights
;       v2  = pVertex[i].blendindices
;       v3  = pVertex[i].normal
;       v7  = pVertex[i].texturecoords
;
*/


char vertexShaderStr[] =
"	vs_1_1\n"\
"	def c4, 3, 1, 0, 0\n"\
"	dcl_position v0\n"\
"	dcl_blendweight v1\n"\
"	dcl_blendindices v2\n"\
"	dcl_texcoord v3\n"\
"	frc r4.xy, v2.zwzw\n"\
"	mov r0.zw, r4.xyxy\n"\
"	frc r0.xy, v2\n"\
"	add r0, -r0, v2\n"\
"	mul r0, r0, c4.x\n"\
"	mad r1, v0.xyzx, c4.yyyz, c4.zzzy\n"\
"	mov a0.x, r0.y\n"\
"	dp4 r2.x, c7[a0.x], r1\n"\
"	dp4 r2.y, c8[a0.x], r1\n"\
"	dp4 r2.z, c9[a0.x], r1\n"\
"	mul r2.xyz, r2, v1.y\n"\
"	mov a0.x, r0.x\n"\
"	dp4 r3.x, c7[a0.x], r1\n"\
"	dp4 r3.y, c8[a0.x], r1\n"\
"	dp4 r3.z, c9[a0.x], r1\n"\
"	mad r2.xyz, r3, v1.x, r2\n"\
"	mov a0.x, r0.z\n"\
"	dp4 r3.x, c7[a0.x], r1\n"\
"	dp4 r3.y, c8[a0.x], r1\n"\
"	dp4 r3.z, c9[a0.x], r1\n"\
"	mad r0.xyz, r3, v1.z, r2\n"\
"	mov a0.x, r0.w\n"\
"	dp4 r2.x, c7[a0.x], r1\n"\
"	dp4 r2.y, c8[a0.x], r1\n"\
"	dp4 r2.z, c9[a0.x], r1\n"\
"	mad r0.xyz, r2, v1.w, r0\n"\
"	mov r0.w, c4.y\n"\
"	dp4 oPos.x, c0, r0\n"\
"	dp4 oPos.y, c1, r0\n"\
"	dp4 oPos.z, c2, r0\n"\
"	dp4 oPos.w, c3, r0\n"\
"	mov oT0.xy, v3\n";

//char vertexShaderStr[] =
//"	vs_1_1\n"\
//"	def c5, 1.0f, 0.0f, 1.0f, 3\n"\
//"	dcl_position        v0\n"\
//"	dcl_blendweight     v1\n"\
//"	dcl_blendindices    v2\n"\
//"	dcl_normal          v3\n"\
//"	dcl_texcoord        v7\n"\
//"	mov     r0, v1\n"\
//"	mul     r1, v2, c5.w\n"\
//"	mov     a0.x, r1.x\n"\
//"	mul     r2, r0.x, c[a0.x + 0 + 10]\n"\
//"	mul     r3, r0.x, c[a0.x + 1 + 10]\n"\
//"	mul     r4, r0.x, c[a0.x + 2 + 10]\n"\
//"	mov     a0.x, r1.y\n"\
//"	mad     r2, r0.y, c[a0.x + 0 + 10], r2\n"\
//"	mad     r3, r0.y, c[a0.x + 1 + 10], r3\n"\
//"	mad     r4, r0.y, c[a0.x + 2 + 10], r4\n"\
//"	mov     a0.x, r1.z\n"\
//"	mad     r2, r0.z, c[a0.x + 0 + 10], r2\n"\
//"	mad     r3, r0.z, c[a0.x + 1 + 10], r3\n"\
//"	mad     r4, r0.z, c[a0.x + 2 + 10], r4\n"\
//"	mov     a0.x, r1.w\n"\
//"	mad     r2, r0.w, c[a0.x+0+10], r2\n"\
//"	mad     r3, r0.w, c[a0.x+1+10], r3\n"\
//"	mad     r4, r0.w, c[a0.x+2+10], r4\n"\
//"	dp4     r5.x, v0, r2\n"\
//"	dp4     r5.y, v0, r3\n"\
//"	dp4     r5.z, v0, r4\n"\
//"	mov     r5.w, c5.x\n"\
//"	m4x4    oPos, r5, c0\n"\
//"	mov     oT0.xy, v7.xy\n";


//char vertexShaderStr[] =
//"	vs_1_1\n"\
//"	def c5, 1.0f, 0.0f, 1.0f, 3\n"\
//"	dcl_position        v0\n"\
//"	dcl_blendweight     v1\n"\
//"	dcl_blendindices    v2\n"\
//"	dcl_normal          v3\n"\
//"	dcl_texcoord        v7\n"\
//"	mov     r0, v1\n"\
//"	mul     r1, v2, c5.w\n"\
//"	mov     a0.x, r1.x\n"\
//"	m4x3    r5, v0, c[a0.x + 10]\n"\
//"	m3x3    r6, v3, c[a0.x + 10]\n"\
//"	mul     r5.xyz, r5.xyz, r0.xxx\n"\
//"	mul     r6.xyz, r6.xyz, r0.xxx\n"\
//"	mov     a0.x, r1.y\n"\
//"	m4x3    r2, v0, c[a0.x + 10]\n"\
//"	m3x3    r3, v3, c[a0.x + 10]\n"\
//"	mad     r5.xyz, r2.xyz, r0.yyy, r5.xyz\n"\
//"	mad     r6.xyz, r3.xyz, r0.yyy, r6.xyz\n"\
//"	mov     a0.x, r1.z\n"\
//"	m4x3    r2, v0, c[a0.x + 10]\n"\
//"	m3x3    r3, v3, c[a0.x + 10]\n"\
//"	mad     r5.xyz, r2.xyz, r0.zzz, r5.xyz\n"\
//"	mad     r6.xyz, r3.xyz, r0.zzz, r6.xyz\n"\
//"	mov     a0.x, r1.w\n"\
//"	m4x3    r2, v0, c[a0.x + 10]\n"\
//"	m3x3    r3, v3, c[a0.x + 10]\n"\
//"	mad     r5.xyz, r2, r0.wwww, r5\n"\
//"	mad     r6.xyz, r3, r0.wwww, r6\n"\
//"	mov     r5.w, c5.x\n"\
//"	m4x4    oPos, r5, c0\n"\
//"	dp3     r11.x, r6.xyz, r6.xyz\n"\
//"	rsq     r11.xyz, r11.x\n"\
//"	mul     r6.xyz, r6.xyz, r11.xyz\n"\
//"	mov     oT0.xy, v7.xy\n";



//"	mul r0, v2, c4.x\n"\

//char vertexShaderStr[] =
//"	vs_1_1\n"\
//"	def c4, 3, 1, 0, 0\n"\
//"	dcl_position v0\n"\
//"	dcl_blendweight v1\n"\
//"	dcl_blendindices v2\n"\
//"	dcl_texcoord v3\n"\
//"   mul r0, v2, c4.x\n"
//"	mad r1, v0.xyzx, c4.yyyz, c4.zzzy\n"\
//"	mov a0.x, r0.x\n"\
//"	dp4 r2.x, c7[a0.x], r1\n"\
//"	dp4 r2.y, c8[a0.x], r1\n"\
//"	dp4 r2.z, c9[a0.x], r1\n"\
//"	mul r2.xyz, r2, v1.y\n"\
//"	mov a0.x, r0.y\n"\
//"	dp4 r3.x, c7[a0.x], r1\n"\
//"	dp4 r3.y, c8[a0.x], r1\n"\
//"	dp4 r3.z, c9[a0.x], r1\n"\
//"	mad r2.xyz, r3, v1.x, r2\n"\
//"	mov a0.x, r0.z\n"\
//"	dp4 r3.x, c7[a0.x], r1\n"\
//"	dp4 r3.y, c8[a0.x], r1\n"\
//"	dp4 r3.z, c9[a0.x], r1\n"\
//"	mad r0.xyz, r3, v1.z, r2\n"\
//"	mov a0.x, r0.w\n"\
//"	dp4 r2.x, c7[a0.x], r1\n"\
//"	dp4 r2.y, c8[a0.x], r1\n"\
//"	dp4 r2.z, c9[a0.x], r1\n"\
//"	mad r0.xyz, r2, v1.w, r0\n"\
//"	mov r0.w, c4.y\n"\
//"	dp4 oPos.x, c0, r0\n"\
//"	dp4 oPos.y, c1, r0\n"\
//"	dp4 oPos.z, c2, r0\n"\
//"	dp4 oPos.w, c3, r0\n"\
//"	mov oT0.xy, v3\n";





EzD3DX9HardwareSkinning::EzD3DX9HardwareSkinning(IDirect3DDevice9* pDev)
: EzD3DX9GPUVertexProgram(pDev)
{
	m_Source = vertexShaderStr;
	loadImpl();
}

EzD3DX9HardwareSkinning::~EzD3DX9HardwareSkinning(void)
{
}

EzGpuProgramParameters* EzD3DX9HardwareSkinning::createParameters(void)
{
	return EzNew EzGpuProgramParameters;
}


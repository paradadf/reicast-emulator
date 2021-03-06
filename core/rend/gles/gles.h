#pragma once
#include "rend/rend.h"
#include <glsm/glsm.h>
#include <glsm/glsmsym.h>
#include "glcache.h"

#define VERTEX_POS_ARRAY      0
#define VERTEX_COL_BASE_ARRAY 1
#define VERTEX_COL_OFFS_ARRAY 2
#define VERTEX_UV_ARRAY       3
// OIT only
#define VERTEX_COL_BASE1_ARRAY 4
#define VERTEX_COL_OFFS1_ARRAY 5
#define VERTEX_UV1_ARRAY 6

#ifndef GL_UNSIGNED_INT_8_8_8_8
#define GL_UNSIGNED_INT_8_8_8_8 0x8035
#endif

#define glCheck()

//vertex types
extern u32 gcflip;
extern float scale_x, scale_y;

void DrawStrips(void);

struct PipelineShader
{
	GLuint program;
   GLuint scale,depth_scale;
   GLuint extra_depth_scale;
   GLuint pp_ClipTest,cp_AlphaTestValue;
 	GLuint sp_FOG_COL_RAM,sp_FOG_COL_VERT,sp_FOG_DENSITY;
   GLuint trilinear_alpha;
   GLuint fog_clamp_min, fog_clamp_max;
   //
   u32 cp_AlphaTest; s32 pp_ClipTestMode;
 	u32 pp_Texture, pp_UseAlpha, pp_IgnoreTexA, pp_ShadInstr, pp_Offset, pp_FogCtrl;
   bool pp_Gouraud, pp_BumpMap;
   bool fog_clamping;
};



struct gl_ctx
{
	struct
	{
		GLuint program;

		GLuint scale,depth_scale;
      GLuint extra_depth_scale;
		GLuint sp_ShaderColor;

	} modvol_shader;

	PipelineShader program_table[12288];

	struct
	{
		GLuint geometry,modvols,idxs,idxs2;
	} vbo;

   const char *gl_version;
   const char *glsl_version_header;
   int gl_major;
   bool is_gles;
   GLuint fog_image_format;
   GLenum index_type;
   bool stencil_present;

   size_t get_index_size() { return index_type == GL_UNSIGNED_INT ? sizeof(u32) : sizeof(u16); }
};

GLuint gl_GetTexture(TSP tsp,TCW tcw);
struct text_info {
	u16* pdata;
	u32 width;
	u32 height;
	u32 textype; // 0 565, 1 1555, 2 4444
};

extern gl_ctx gl;
extern GLuint fbTextureId;
extern float fb_scale_x, fb_scale_y;

struct modvol_shader_type
{
   GLuint program;
   GLuint scale;
   GLuint sp_ShaderColor;
};
enum ModifierVolumeMode { Xor, Or, Inclusion, Exclusion, ModeCount };

bool ProcessFrame(TA_context* ctx);
void UpdateFogTexture(u8 *fog_table, GLenum texture_slot, GLint fog_image_format);
text_info raw_GetTexture(TSP tsp, TCW tcw);
void CollectCleanup();
void DoCleanup();
void SortPParams(int first, int count);
void SetCull(u32 CullMode);
s32 SetTileClip(u32 val, GLint uniform);
void SetMVS_Mode(ModifierVolumeMode mv_mode, ISP_Modvol ispc);

void killtex();
void BindRTT(u32 addy, u32 fbw, u32 fbh, u32 channels, u32 fmt);
void ReadRTTBuffer();
void RenderFramebuffer();
void DrawFramebuffer(float w, float h);

int GetProgramID(u32 cp_AlphaTest, u32 pp_ClipTestMode,
							u32 pp_Texture, u32 pp_UseAlpha, u32 pp_IgnoreTexA, u32 pp_ShadInstr, u32 pp_Offset,
							u32 pp_FogCtrl, bool pp_Gouraud, bool pp_BumpMap, bool fog_clamping);
void vertex_buffer_unmap(void);

GLuint gl_CompileShader(const char* shader, GLuint type);
GLuint gl_CompileAndLink(const char* VertexShader, const char* FragmentShader);
bool CompilePipelineShader(PipelineShader* s);
void co_dc_yield(void);
void vertex_buffer_unmap();

extern GLuint vmuTextureId[4];
void UpdateVmuTexture(int vmu_screen_number);

extern struct ShaderUniforms_t
{
	float PT_ALPHA;
	float scale_coefs[4];
	float depth_coefs[4];
   float extra_depth_scale;
	float fog_den_float;
	float ps_FOG_COL_RAM[3];
	float ps_FOG_COL_VERT[3];
	float trilinear_alpha;
   float fog_clamp_min[4];
	float fog_clamp_max[4];

	void Set(PipelineShader* s)
	{
		if (s->cp_AlphaTestValue!=-1)
			glUniform1f(s->cp_AlphaTestValue,PT_ALPHA);

		if (s->scale!=-1)
			glUniform4fv( s->scale, 1, scale_coefs);

		if (s->depth_scale!=-1)
			glUniform4fv( s->depth_scale, 1, depth_coefs);

      if (s->extra_depth_scale != -1)
			glUniform1f(s->extra_depth_scale, extra_depth_scale);

		if (s->sp_FOG_DENSITY!=-1)
			glUniform1f( s->sp_FOG_DENSITY,fog_den_float);

		if (s->sp_FOG_COL_RAM!=-1)
			glUniform3fv( s->sp_FOG_COL_RAM, 1, ps_FOG_COL_RAM);

		if (s->sp_FOG_COL_VERT!=-1)
			glUniform3fv( s->sp_FOG_COL_VERT, 1, ps_FOG_COL_VERT);

		if (s->trilinear_alpha != -1)
			glUniform1f(s->trilinear_alpha, trilinear_alpha);

      if (s->fog_clamp_min != -1)
			glUniform4fv(s->fog_clamp_min, 1, fog_clamp_min);
		if (s->fog_clamp_max != -1)
			glUniform4fv(s->fog_clamp_max, 1, fog_clamp_max);
	}

} ShaderUniforms;

struct IndexTrig
{
	u32 id[3];
	u16 pid;
	f32 z;
};

struct SortTrigDrawParam
{
	PolyParam* ppid;
	u32 first;
	u32 count;
};

// Render to texture
struct FBT
{
	u32 TexAddr;
	GLuint depthb,stencilb;
	GLuint tex;
	GLuint fbo;
};
extern FBT fb_rtt;

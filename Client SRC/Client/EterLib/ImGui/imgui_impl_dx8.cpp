#include "StdAfx.h"

#include <d3d8.h>
#include "imgui.h"
#include "imgui_impl_dx8.h"
#include <d3d8.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
static LPDIRECT3DDEVICE8        g_pd3dDevice = NULL;
static LPDIRECT3DVERTEXBUFFER8  g_pVB = NULL;
static LPDIRECT3DINDEXBUFFER8   g_pIB = NULL;
static LPDIRECT3DVERTEXBUFFER8  g_maskVB = NULL;
static LPDIRECT3DINDEXBUFFER8   g_maskIB = NULL;
static LPDIRECT3DTEXTURE8       g_FontTexture = NULL;
static int                      g_VertexBufferSize = 5000, g_IndexBufferSize = 10000;
static IDirect3DSurface8* g_DepthBuffer = nullptr;
IDirect3DSurface8* realDepthStencilBuffer = nullptr;
IDirect3DSurface8* realRenderTarget = nullptr;

struct CUSTOMVERTEX
{
	float    pos[3];
	D3DCOLOR col;
	float    uv[2];
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

static void ImGui_ImplDX8_SetupRenderState(ImDrawData* draw_data)
{
	D3DVIEWPORT8 vp;
	vp.X = vp.Y = 0;
	vp.Width = (DWORD)draw_data->DisplaySize.x;
	vp.Height = (DWORD)draw_data->DisplaySize.y;
	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;
	g_pd3dDevice->SetViewport(&vp);
	g_pd3dDevice->GetDepthStencilSurface(&realDepthStencilBuffer);
	g_pd3dDevice->GetRenderTarget(&realRenderTarget);
	g_pd3dDevice->SetPixelShader(NULL);
	g_pd3dDevice->SetVertexShader(D3DFVF_CUSTOMVERTEX);
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, false);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, false);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, false);
	g_pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);//new
	g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, false);//new
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	{
		float L = draw_data->DisplayPos.x + 0.5f;
		float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x + 0.5f;
		float T = draw_data->DisplayPos.y + 0.5f;
		float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y + 0.5f; \
			D3DMATRIX mat_identity = { { { 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f } } };
		D3DMATRIX mat_projection =
		{ { {
			2.0f / (R - L),   0.0f,         0.0f,  0.0f,
			0.0f,         2.0f / (T - B),   0.0f,  0.0f,
			0.0f,         0.0f,         0.5f,  0.0f,
			(L + R) / (L - R),  (T + B) / (B - T),  0.5f,  1.0f
		} } };
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &mat_identity);
		g_pd3dDevice->SetTransform(D3DTS_VIEW, &mat_identity);
		g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &mat_projection);
	}
}

static void build_mask_vbuffer(const RECT* rect) {
	CUSTOMVERTEX* vtx_dst;
	g_maskVB->Lock(0, (UINT)(6 * sizeof(CUSTOMVERTEX)), (BYTE**)&vtx_dst, 0);
	vtx_dst[0].pos[0] = (float)rect->left;
	vtx_dst[0].pos[1] = (float)rect->bottom;
	vtx_dst[0].pos[2] = 0;
	vtx_dst[1].pos[0] = (float)rect->left;
	vtx_dst[1].pos[1] = (float)rect->top;
	vtx_dst[1].pos[2] = 0;
	vtx_dst[2].pos[0] = (float)rect->right;
	vtx_dst[2].pos[1] = (float)rect->top;
	vtx_dst[2].pos[2] = 0;
	vtx_dst[3].pos[0] = (float)rect->left;
	vtx_dst[3].pos[1] = (float)rect->bottom;
	vtx_dst[3].pos[2] = 0;
	vtx_dst[4].pos[0] = (float)rect->right;
	vtx_dst[4].pos[1] = (float)rect->top;
	vtx_dst[4].pos[2] = 0;
	vtx_dst[5].pos[0] = (float)rect->right;
	vtx_dst[5].pos[1] = (float)rect->bottom;
	vtx_dst[5].pos[2] = 0;
	vtx_dst[0].col = 0xFFFFFFFF;
	vtx_dst[1].col = 0xFFFFFFFF;
	vtx_dst[2].col = 0xFFFFFFFF;
	vtx_dst[3].col = 0xFFFFFFFF;
	vtx_dst[4].col = 0xFFFFFFFF;
	vtx_dst[5].col = 0xFFFFFFFF;
	vtx_dst[0].uv[0] = 0;
	vtx_dst[0].uv[1] = 0;
	vtx_dst[1].uv[0] = 0;
	vtx_dst[1].uv[1] = 0;
	vtx_dst[2].uv[0] = 0;
	vtx_dst[2].uv[1] = 0;
	vtx_dst[3].uv[0] = 0;
	vtx_dst[3].uv[1] = 0;
	vtx_dst[4].uv[0] = 0;
	vtx_dst[4].uv[1] = 0;
	vtx_dst[5].uv[0] = 0;
	vtx_dst[5].uv[1] = 0;
	g_maskVB->Unlock();
}
void ImGui_ImplDX8_RenderDrawData(ImDrawData* draw_data)
{
	try
	{
		if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
			return;
		if (!g_pVB || g_VertexBufferSize < draw_data->TotalVtxCount)
		{
			if (g_pVB) { g_pVB->Release(); g_pVB = NULL; }
			g_VertexBufferSize = draw_data->TotalVtxCount + 5000;
			if (g_pd3dDevice->CreateVertexBuffer(g_VertexBufferSize * sizeof(CUSTOMVERTEX), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB) < 0)
				return;
		}
		if (!g_pIB || g_IndexBufferSize < draw_data->TotalIdxCount)
		{
			if (g_pIB) { g_pIB->Release(); g_pIB = NULL; }
			g_IndexBufferSize = draw_data->TotalIdxCount + 10000;
			if (g_pd3dDevice->CreateIndexBuffer(g_IndexBufferSize * sizeof(ImDrawIdx), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, sizeof(ImDrawIdx) == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32, D3DPOOL_DEFAULT, &g_pIB) < 0)
				return;
		}
		if (!g_maskVB && !g_maskIB) {
			if (g_pd3dDevice->CreateVertexBuffer(6 * sizeof(CUSTOMVERTEX), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_maskVB) < 0) return;
			if (g_pd3dDevice->CreateIndexBuffer(6, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, sizeof(ImDrawIdx) == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32, D3DPOOL_DEFAULT, &g_maskIB) < 0) return;
			ImDrawIdx* idx_dst;
			g_maskIB->Lock(0, 6 * sizeof(ImDrawIdx), (BYTE**)&idx_dst, D3DLOCK_DISCARD);
			idx_dst[0] = 0;
			idx_dst[1] = 1;
			idx_dst[2] = 2;
			idx_dst[3] = 0;
			idx_dst[4] = 2;
			idx_dst[5] = 3;
			g_maskIB->Unlock();
		}
		DWORD stateBlockToken = 0;
		if (g_pd3dDevice->CreateStateBlock(D3DSBT_ALL, &stateBlockToken) < 0) return;
		D3DMATRIX last_world, last_view, last_projection;//new
		g_pd3dDevice->GetTransform(D3DTS_WORLD, &last_world);//new
		g_pd3dDevice->GetTransform(D3DTS_VIEW, &last_view);//new
		g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &last_projection);//new
		CUSTOMVERTEX* vtx_dst;
		ImDrawIdx* idx_dst;
		if (g_pVB->Lock(0, (UINT)(draw_data->TotalVtxCount * sizeof(CUSTOMVERTEX)), (BYTE**)&vtx_dst, D3DLOCK_DISCARD) < 0)
			return;
		if (g_pIB->Lock(0, (UINT)(draw_data->TotalIdxCount * sizeof(ImDrawIdx)), (BYTE**)&idx_dst, D3DLOCK_DISCARD) < 0)
			return;
		for (int n = 0; n < draw_data->CmdListsCount; n++)
		{
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			const ImDrawVert* vtx_src = cmd_list->VtxBuffer.Data;
			for (int i = 0; i < cmd_list->VtxBuffer.Size; i++)
			{
				vtx_dst->pos[0] = vtx_src->pos.x;
				vtx_dst->pos[1] = vtx_src->pos.y;
				vtx_dst->pos[2] = 0.0f;
				vtx_dst->col = (vtx_src->col & 0xFF00FF00) | ((vtx_src->col & 0xFF0000) >> 16) | ((vtx_src->col & 0xFF) << 16);
				vtx_dst->uv[0] = vtx_src->uv.x;
				vtx_dst->uv[1] = vtx_src->uv.y;
				vtx_dst++;
				vtx_src++;
			}
			memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
			idx_dst += cmd_list->IdxBuffer.Size;
		}
		g_pVB->Unlock();
		g_pIB->Unlock();
		g_pd3dDevice->SetStreamSource(0, g_pVB, sizeof(CUSTOMVERTEX));
		g_pd3dDevice->SetIndices(g_pIB, 0);
		g_pd3dDevice->SetVertexShader(D3DFVF_CUSTOMVERTEX);//new
		ImGui_ImplDX8_SetupRenderState(draw_data);
		int global_vtx_offset = 0;
		int global_idx_offset = 0;
		ImVec2 clip_off = draw_data->DisplayPos;

		for (int n = 0; n < draw_data->CmdListsCount; n++)
		{
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
			{
				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
				if (pcmd->UserCallback != NULL)
				{
					if (pcmd->UserCallback == ImDrawCallback_ResetRenderState) {
					}
					else {
					}
				}
				else
				{
					const RECT r = { (LONG)(pcmd->ClipRect.x - clip_off.x), (LONG)(pcmd->ClipRect.y - clip_off.y), (LONG)(pcmd->ClipRect.z - clip_off.x), (LONG)(pcmd->ClipRect.w - clip_off.y) };
					const LPDIRECT3DTEXTURE8 texture = (LPDIRECT3DTEXTURE8)pcmd->TextureId;
					g_pd3dDevice->SetTexture(0, texture);
					build_mask_vbuffer(&r);
					g_pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);
					g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, true);
					g_pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, true);
					g_pd3dDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0xFF);
					g_pd3dDevice->SetRenderState(D3DRS_STENCILMASK, 0xFF);
					g_pd3dDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
					g_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
					g_pd3dDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);
					g_pd3dDevice->SetRenderState(D3DRS_STENCILREF, 0xFF);
					g_pd3dDevice->Clear(0, NULL, D3DCLEAR_STENCIL, 0, 1.0f, 0);
					g_pd3dDevice->SetStreamSource(0, g_maskVB, sizeof(CUSTOMVERTEX));
					g_pd3dDevice->SetIndices(g_maskIB, 0);
					g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 4, 0, 2);//new			
					g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
					g_pd3dDevice->SetStreamSource(0, g_pVB, sizeof(CUSTOMVERTEX));
					IDirect3DIndexBuffer8* oldind = NULL;
					UINT oldvtxoff = 0;
					g_pd3dDevice->GetIndices(&oldind, &oldvtxoff);
					g_pd3dDevice->SetIndices(g_pIB, global_vtx_offset);
					g_pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xF);
					g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, false);
					g_pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, true);
					g_pd3dDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0);
					g_pd3dDevice->SetRenderState(D3DRS_STENCILMASK, 0xFF);
					g_pd3dDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
					g_pd3dDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
					g_pd3dDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
					g_pd3dDevice->SetRenderState(D3DRS_STENCILREF, 0xFF);
					g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, (UINT)cmd_list->VtxBuffer.Size, pcmd->IdxOffset + global_idx_offset, pcmd->ElemCount / 3);

					g_pd3dDevice->SetIndices(oldind, oldvtxoff);
					if (oldind) { oldind->Release(); }
				}
			}
			global_idx_offset += cmd_list->IdxBuffer.Size;
			global_vtx_offset += cmd_list->VtxBuffer.Size;
		}
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &last_world);//new
		g_pd3dDevice->SetTransform(D3DTS_VIEW, &last_view);//new
		g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &last_projection);//new
		g_pd3dDevice->ApplyStateBlock(stateBlockToken);
		g_pd3dDevice->DeleteStateBlock(stateBlockToken);
		if (realDepthStencilBuffer) {
			realDepthStencilBuffer->Release();
			realDepthStencilBuffer = NULL;
		}
		if (realRenderTarget) {
			realRenderTarget->Release();
			realRenderTarget = NULL;
		}
	}
	catch (...)
	{

	}
}

bool ImGui_ImplDX8_Init(IDirect3DDevice8* device)
{
	ImGuiIO& io = ImGui::GetIO();
	io.BackendRendererName = "imgui_impl_dx8";
	io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

	g_pd3dDevice = device;
	g_pd3dDevice->AddRef();
	return true;
}

void ImGui_ImplDX8_Shutdown()
{
	ImGui_ImplDX8_InvalidateDeviceObjects();
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

bool ImGui_ImplDX8_CreateFontsTexture()
{
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int width, height, BYTEs_per_pixel;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &BYTEs_per_pixel);
	if (g_FontTexture) g_FontTexture->Release();
	g_FontTexture = NULL;
	if (g_pd3dDevice->CreateTexture(width, height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_FontTexture) < 0)
		return false;
	D3DLOCKED_RECT tex_locked_rect;
	if (g_FontTexture->LockRect(0, &tex_locked_rect, NULL, 0) != D3D_OK)
		return false;
	for (int y = 0; y < height; y++)
		memcpy((unsigned char*)tex_locked_rect.pBits + tex_locked_rect.Pitch * y, pixels + (width * BYTEs_per_pixel) * y, (width * BYTEs_per_pixel));
	g_FontTexture->UnlockRect(0);
	io.Fonts->TexID = (ImTextureID)g_FontTexture;

	return true;
}

bool ImGui_ImplD3D8_CreateDepthStencilBuffer() {
	if (g_pd3dDevice == nullptr) {
		return false;
	}
	if (g_DepthBuffer == nullptr) {
		IDirect3DSurface8* realDepth;
		D3DSURFACE_DESC sfcDesc;

		g_pd3dDevice->GetDepthStencilSurface(&realDepth);
		if (realDepth->GetDesc(&sfcDesc)) {
			return false;
		}
		if (g_pd3dDevice->CreateDepthStencilSurface(sfcDesc.Width, sfcDesc.Height, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, &g_DepthBuffer)) {
			return false;
		}
		realDepth->Release();
	}

	return true;
}


bool ImGui_ImplDX8_CreateDeviceObjects()
{
	if (!g_pd3dDevice)
		return false;
	if (!ImGui_ImplDX8_CreateFontsTexture())
		return false;
	if (!ImGui_ImplD3D8_CreateDepthStencilBuffer()) {
		return false;
	}
	return true;
}

void ImGui_ImplDX8_InvalidateDeviceObjects()
{
	if (!g_pd3dDevice)
		return;
	if (g_pVB)
	{
		g_pVB->Release();
		g_pVB = NULL;
	}
	if (g_pIB)
	{
		g_pIB->Release();
		g_pIB = NULL;
	}
	if (g_maskVB) {
		g_maskVB->Release();
		g_maskVB = nullptr;
	}
	if (g_maskIB) {
		g_maskIB->Release();
		g_maskIB = nullptr;
	}
	if (g_DepthBuffer) {
		g_DepthBuffer->Release();
		g_DepthBuffer = nullptr;
	}
	if (g_FontTexture) { g_FontTexture->Release(); g_FontTexture = NULL; ImGui::GetIO().Fonts->TexID = NULL; }
}

void ImGui_ImplDX8_NewFrame()
{
	if (!g_FontTexture || !g_DepthBuffer)
		ImGui_ImplDX8_CreateDeviceObjects();
}
#pragma once

#include <Windows.h>
#include <WinUser.h>
#include <Dwmapi.h> 
#pragma comment(lib, "dwmapi.lib")
#include <stdlib.h>
#include <vector>
#include <chrono>
#include <cwchar>
#include <thread>
#include <string>
#include "XorString.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include "globals.h" // For Vector3, Matrix3x4, Vector2, etc.

#define GREEN ImColor(0, 255, 0)
#define RED ImColor(255, 0, 0)
#define BLUE ImColor(0, 0, 255)
#define ORANGE ImColor(255, 165, 0)
#define WHITE ImColor(255, 255, 255)

typedef struct visuals
{
	bool box = false;
	bool line = false;
	//bool xp_level = false;
	bool distance = true;
	bool healthbar = true;
	bool shieldbar = true;
	bool name = true;
}visuals;

struct GColor {
	float r, g, b;
};

class Overlay
{
public:
	void Start();
	DWORD CreateOverlay();
	void Clear();
	int getWidth();
	int getHeight();
	void RenderInfo();
	void RenderMenu();
	void RenderEsp();
	//void RenderSpectator();
	void ClickThrough(bool v);
	void DrawLine(ImVec2 a, ImVec2 b, ImColor color, float width);
	void DrawBox(ImColor color, float x, float y, float w, float h);
	void Text(ImVec2 pos, ImColor color, const char* text_begin, const char* text_end, float wrap_width, const ImVec4* cpu_fine_clip_rect);
	void RectFilled(float x0, float y0, float x1, float y1, ImColor color, float rounding, int rounding_corners_flags);
	void ProgressBar(float x, float y, float w, float h, int value, int v_max);
	void String(ImVec2 pos, ImColor color, const char* text);
	//seer
	void DrawSeerLikeHealth(float x, float y, int shield, int max_shield, int armorType, int health);

	// World to Screen function
	bool WorldToScreen(const Vector3& world_pos, const Matrix3x4& view_matrix, Vector2& screen_pos_out, int screen_width, int screen_height);

private:
	bool running;
	HWND overlayHWND;
};
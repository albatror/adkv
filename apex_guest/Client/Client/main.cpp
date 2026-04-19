#include "main.h"
#include "config.h"
#include <random>
#include <Windows.h>
#include <SetupAPI.h>
#include <devguid.h>
#include <cfgmgr32.h>
#include <numeric>
#include <algorithm>
#include <cctype>
//#include <chrono>
//test
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "cfgmgr32.lib")
//test contraste texte
#include ".\imgui\imgui.h"

typedef struct player
{
	float dist = 0;
	int entity_team = 0;
	float boxMiddle = 0;
	float h_y = 0;
	float width = 0;
	float height = 0;
	float b_x = 0;
	float b_y = 0;
	bool knocked = false;
	bool visible = false;
	int health = 0;
	int shield = 0;
	int maxshield = 0;
	int armortype = 0;
	int xp_level = 0;
	int platform = 0;
	char name[33] = { 0 };
	char weapon[33] = { 0 };
	float bones[17][2] = { 0 };
}player;

typedef struct spectator {
	bool is_spec = false;
	char name[33] = { 0 };
}spectator;

uint32_t check = 0xABCD;

int aim_key = VK_LBUTTON;
int aim_key2 = VK_RBUTTON;

bool triggerbot = false;
int triggerbot_key = VK_LSHIFT;
bool triggerbot_aiming = false;
float triggerbot_fov = 10.0f;

bool superglide = false;
bool bhop = false;
bool walljump = false;

bool use_nvidia = false;
bool active = true;
bool ready = false;
extern visuals v;
int aim = 0; //read
bool esp = false; //read
bool item_glow = false;
bool player_glow = false;
bool aim_no_recoil = true;
bool lock_target = false;
bool aiming = false; //read
uint64_t g_Base = 0; //write
float max_dist = 120.0f * 40.0f;
float aim_dist = 120.0f * 40.0f;

float ads_fov = 3.0f;
float ads_smooth = 15.0f;
float hip_fov = 8.0f;
float hip_smooth = 25.0f;

bool aassist = false;
float aassist_dist = 50.0f * 40.0f;
bool aassist_aiming = false;

int bone = 2;
// Declare constants for key detection
int SuperKey = VK_SPACE;  // VK_SPACE is the spacebar keycode

bool firing_range = false;
bool shooting = false; //read

bool dump = false;
bool update_offsets = false;

//bool bone_auto = true;
//int shoot_key = VK_LBUTTON;

//1v1
bool onevone = false;

char real_edid[16] = { 0 };
char fake_edid[16] = { 0 };
bool monitor_spoof = false;

//items
//bool medbackpack = true;

///test contraste texte
ImU32 GetContrastColor(ImU32 backgroundColor) {
	// Calculer le contraste entre la couleur de fond et le blanc
	int r = (backgroundColor >> 16) & 0xFF;
	int g = (backgroundColor >> 8) & 0xFF;
	int b = backgroundColor & 0xFF;
	float luminance = (0.299f * r + 0.587f * g + 0.114f * b) / 255.0f;

	return luminance > 0.5f ? IM_COL32(0, 0, 0, 255) : IM_COL32(255, 255, 255, 255);
}

// Obtenir dynamiquement la couleur de fond
ImU32 GetImU32Color(const ImVec4& color) {
	return IM_COL32(
		static_cast<int>(color.x * 255.0f),
		static_cast<int>(color.y * 255.0f),
		static_cast<int>(color.z * 255.0f),
		static_cast<int>(color.w * 255.0f)
	);
}

std::string GetPlatformName(int platform)
{
	switch (platform)
	{
	case 514:
	case 1794:
		return "PC";
	case 0:
	case 2570:
		return "X1";
	case 257:
	case 2056:
		return "PS4";
	default:
		return "PC";
	}
}
///end test

static bool showing = false;
static bool k_del = 0;

int spectators = 0; //write
int allied_spectators = 0; //write

int screen_width = 2560;
int screen_height = 1440;

//Player Glow Color and Brightness
unsigned char insidevalue = 6;
unsigned char outlinesize = 32;
float glowr = 100.0f; //Red Value
float glowg = 0.0f; //Green Value
float glowb = 0.0f; //Blue Value
float glowcolor[3] = { 000.0f, 000.0f, 000.0f };
//more glow stuff
//glow visable
float glowrviz = 0.0f;
float glowgviz = 100.0f;
float glowbviz = 0.0f;
float glowcolorviz[3] = { 000.0f, 000.0f, 000.0f };
//knocked
float glowrknocked = 100.0f;
float glowgknocked = 100.0f;
float glowbknocked = 100.0f;
float glowcolorknocked[3] = { 000.0f, 000.0f, 000.0f };

bool valid = false; //write
bool next = false; //read write

int index = 0;

uint64_t add[64];

bool k_f1 = 0;
bool k_f2 = 0;
bool k_f3 = 0;

bool k_f5 = 0;
bool k_f6 = 0;
bool k_f7 = 0;
bool k_f8 = 0;
bool k_f9 = 0;
bool k_f10 = 0;
bool k_f11 = 0;
//bool k_lshift = 0;

bool IsKeyDown(int vk)
{
	return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

player players[100];

bool ModifyEdid(std::vector<BYTE>& edid, char* out_real, char* out_fake) {
	if (edid.size() < 128) return false;

	bool modified = false;
	std::random_device rd;
	std::mt19937 gen(rd());

	// 1. Spoof Manufacturer ID (offsets 0x08-0x09)
	// Characters are 5 bits each, 'A'=1, ..., 'Z'=26.
	uint16_t m_id = 0;
	m_id |= ((gen() % 26) + 1) << 10;
	m_id |= ((gen() % 26) + 1) << 5;
	m_id |= ((gen() % 26) + 1);
	edid[8] = (m_id >> 8) & 0xFF;
	edid[9] = m_id & 0xFF;

	// 2. Spoof Product Code (offsets 0x0A-0x0B)
	edid[10] = gen() % 256;
	edid[11] = gen() % 256;

	// 3. Spoof Serial Number (numeric, offsets 0x0C-0x0F)
	edid[12] = gen() % 256;
	edid[13] = gen() % 256;
	edid[14] = gen() % 256;
	edid[15] = gen() % 256;

	// 4. Spoof Week/Year of manufacture (offsets 0x10, 0x11)
	edid[16] = (gen() % 54); // Week 0-53
	edid[17] = (gen() % 35) + 10; // Year 2000 + (10 to 45)

	modified = true;

	for (size_t offset = 54; offset <= 108; offset += 18) {
		if (edid[offset] == 0x00 && edid[offset + 1] == 0x00 &&
			edid[offset + 2] == 0x00) {

			if (edid[offset + 3] == 0xFF) { // Serial Number descriptor string
				BYTE* serial_bytes = &edid[offset + 5];
				char current[14] = { 0 };
				memcpy(current, serial_bytes, 13);

				std::string current_s(current);
				size_t end = current_s.find('\n');
				if (end != std::string::npos) current_s.resize(end);
				current_s.erase(current_s.find_last_not_of(' ') + 1);

				if (current_s.empty()) continue;

				if (out_real && out_real[0] == 0) {
					strncpy(out_real, current_s.c_str(), 15);
				}

				std::string newSerial = current_s;
				for (size_t i = 0; i < newSerial.size(); ++i) {
					if (std::isdigit(static_cast<unsigned char>(newSerial[i]))) {
						newSerial[i] = '0' + (gen() % 10);
					}
					else if (std::isalpha(static_cast<unsigned char>(newSerial[i]))) {
						newSerial[i] = (std::isupper(static_cast<unsigned char>(newSerial[i])) ? 'A' : 'a') + (gen() % 26);
					}
				}

				memset(serial_bytes, 0x20, 13);
				memcpy(serial_bytes, newSerial.c_str(), (std::min)((size_t)13, newSerial.size()));
				if (newSerial.size() < 13) serial_bytes[newSerial.size()] = 0x0A;

				if (out_fake && out_fake[0] == 0) {
					strncpy(out_fake, newSerial.c_str(), 15);
				}
				modified = true;
			}
			else if (edid[offset + 3] == 0xFC || edid[offset + 3] == 0xFE) { // Monitor Name or Unspecified Text descriptor string
				BYTE* name_bytes = &edid[offset + 5];
				for (int i = 0; i < 13; i++) {
					if (name_bytes[i] == 0x0A) break;
					if (std::isalnum(name_bytes[i])) {
						if (std::isdigit(name_bytes[i])) name_bytes[i] = '0' + (gen() % 10);
						else name_bytes[i] = (std::isupper(name_bytes[i]) ? 'A' : 'a') + (gen() % 26);
					}
				}
				modified = true;
			}
		}
	}

	if (modified) {
		BYTE checksum = 0;
		for (size_t j = 0; j < 127; ++j) checksum += edid[j];
		edid[127] = static_cast<BYTE>(256 - checksum);
	}

	return modified;
}

bool RestartDevice(HDEVINFO hDevInfo, SP_DEVINFO_DATA* pDevInfoData) {
	SP_PROPCHANGE_PARAMS pcp;
	pcp.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
	pcp.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
	pcp.StateChange = DICS_PROPCHANGE;
	pcp.Scope = DICS_FLAG_GLOBAL;
	pcp.HwProfile = 0;

	if (!SetupDiSetClassInstallParams(hDevInfo, pDevInfoData, &pcp.ClassInstallHeader, sizeof(pcp))) {
		return false;
	}

	if (!SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, hDevInfo, pDevInfoData)) {
		return false;
	}

	return true;
}

bool IsUserAdmin() {
	BOOL bRet = FALSE;
	HANDLE hToken = NULL;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		TOKEN_ELEVATION elevation;
		DWORD dwSize;
		if (GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize)) {
			bRet = elevation.TokenIsElevated;
		}
	}
	if (hToken) CloseHandle(hToken);
	return bRet;
}

void spoofmonitor() {
	if (!IsUserAdmin()) {
		printf("Error: Client is NOT running as Admin. Registry spoofing will likely fail.\n");
	}

	HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_MONITOR, NULL, NULL, DIGCF_PRESENT);
	if (hDevInfo == INVALID_HANDLE_VALUE) return;

	SP_DEVINFO_DATA devInfoData;
	devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData); ++i) {
		char szInstanceId[MAX_DEVICE_ID_LEN];
		if (SetupDiGetDeviceInstanceIdA(hDevInfo, &devInfoData, szInstanceId, MAX_DEVICE_ID_LEN, NULL)) {
			printf("Processing monitor %d: %s\n", i, szInstanceId);
		}

		HKEY hKey = SetupDiOpenDevRegKey(hDevInfo, &devInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_ALL_ACCESS);
		if (hKey == INVALID_HANDLE_VALUE) {
			DWORD err = GetLastError();
			printf("Failed to open registry key for monitor %d (Error: %d). Trying fallback...\n", i, err);

			// Fallback: Manually open the key in Enum\DISPLAY
			char szRegPath[MAX_PATH];
			sprintf(szRegPath, "SYSTEM\\CurrentControlSet\\Enum\\%s\\Device Parameters", szInstanceId);
			if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, szRegPath, 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS) {
				printf("Fallback failed for monitor %d. Skipping.\n", i);
				continue;
			}
		}

		BYTE edidData[1024] = { 0 };
		DWORD edidSize = sizeof(edidData);
		if (RegQueryValueExA(hKey, "EDID", NULL, NULL, edidData, &edidSize) == ERROR_SUCCESS) {
			std::vector<BYTE> edid(edidData, edidData + edidSize);
			if (ModifyEdid(edid, real_edid, fake_edid)) {
				// 1. Update the original EDID value
				if (RegSetValueExA(hKey, "EDID", 0, REG_BINARY, edid.data(), (DWORD)edid.size()) == ERROR_SUCCESS) {
					printf("Successfully patched EDID in registry.\n");
				}

				// 2. Apply EDID_OVERRIDE
				HKEY hOverrideKey;
				if (RegCreateKeyExA(hKey, "EDID_OVERRIDE", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hOverrideKey, NULL) == ERROR_SUCCESS) {
					if (RegSetValueExA(hOverrideKey, "0", 0, REG_BINARY, edid.data(), (DWORD)edid.size()) == ERROR_SUCCESS) {
						printf("Successfully applied EDID_OVERRIDE.\n");
						monitor_spoof = true;
					}
					RegCloseKey(hOverrideKey);
				}

				// 3. Spoof additional values if they exist
				RegDeleteValueA(hKey, "HardwareID");
				RegDeleteValueA(hKey, "ManufacturerID");

				// 4. Force override of Model and Serial if cached elsewhere
				RegSetValueExA(hKey, "SerialNumber", 0, REG_SZ, (const BYTE*)fake_edid, (DWORD)strlen(fake_edid) + 1);
			}
		}
		RegCloseKey(hKey);

		// Restart device to force Windows to re-read the EDID
		if (RestartDevice(hDevInfo, &devInfoData)) {
			printf("Monitor %d restarted successfully.\n", i);
		} else {
			printf("Failed to restart monitor %d (Error: %d)\n", i, GetLastError());
		}
	}
	SetupDiDestroyDeviceInfoList(hDevInfo);

	printf("Real Serial: %s\n", real_edid[0] ? real_edid : "Unknown");
	printf("Spoofed Serial: %s\n", fake_edid[0] ? fake_edid : "None");
}

void randomBone()
{
	int boneArray[3] = { 0, 1, 2 };

	// Seed the random number generator with the current time
	srand((unsigned int)time(NULL));

	int randVal = rand() % 3;
	bone = boneArray[randVal];
	Sleep(500);
}

spectator spectator_list[100];

void Overlay::RenderSpectator() {
	if (!v.spectator_notifier) return;
	ImGui::SetNextWindowPos(ImVec2(630, 10));
	ImGui::SetNextWindowSize(ImVec2(190, 205));
	ImGui::SetNextWindowBgAlpha(0.6f);
	ImGui::Begin(XorStr("##spectator_list"), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

	ImGui::Indent(12.0f);
	//DrawLine(ImVec2(491, 2), ImVec2(679, 2), RED, 2);
	ImGui::TextColored(RED, "%d", spectators);
	ImGui::SameLine();
	ImGui::Text("-");
	ImGui::SameLine();
	ImGui::TextColored(GREEN, "%d", allied_spectators);

	int text_index = 0;
	for (int i = 0; i < 100; i++) {
		if (spectator_list[i].is_spec) {
			ImGui::TextColored(WHITE, "%d", text_index + 1);
			ImGui::SameLine();
			ImGui::TextColored(ORANGE, " %s", spectator_list[i].name);
			text_index++;
		}
	}

	ImGui::End();
}
// Define a function for smooth interpolation using a cubic easing function
float smoothStep(float edge0, float edge1, float x) {
	// Scale, bias and saturate x to 0..1 range
	x = (x - edge0) / (edge1 - edge0);
	x = x < 0 ? 0 : (x > 1 ? 1 : x);
	// Evaluate polynomial
	return x * x * (3 - 2 * x);
}


void Overlay::RenderEsp()
{
	next = false;
	if (g_Base != 0 && esp)
	{
		memset(players, 0, sizeof(players));
		while (!next && esp)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		if (next && valid)
		{

			// Add a log file for debugging
			//std::ofstream logFile("client_debug.log", std::ios::app);

			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(ImVec2((float)getWidth(), (float)getHeight()));
			ImGui::Begin(XorStr("##esp"), (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus);

			int best_player_idx = -1;
			float best_score = 999999.0f;
			float cx = (float)getWidth() / 2.0f;
			float cy = (float)getHeight() / 2.0f;

			for (int i = 0; i < 100; i++)
			{
				if (players[i].health > 0)
				{
					std::string distance = std::to_string(players[i].dist / 39.62);
					distance = distance.substr(0, distance.find('.')) + "m(" + std::to_string(players[i].entity_team) + ")";
					if (v.box)
					{
						if (players[i].visible)
						{
							if (players[i].dist < 9000.0f) //before 9000.0f
								DrawBox(GREEN, players[i].boxMiddle, players[i].h_y, players[i].width, players[i].height); //BOX
							else
								DrawBox(WHITE, players[i].boxMiddle, players[i].h_y, players[i].width, players[i].height); //BOX
						}
						else
						{
							DrawBox(RED, players[i].boxMiddle, players[i].h_y, players[i].width, players[i].height); //red if player not visible
						}
					}

					float dx = players[i].b_x - cx;
					float dy = (players[i].b_y + players[i].h_y) / 2.0f - cy;
					float d = sqrt(dx * dx + dy * dy);
					float score = d + (players[i].dist / 20.0f); // Weighted score (screen distance + world distance)
					if (score < best_score) {
						best_score = score;
						best_player_idx = i;
					}

					//if(v.line)
					//	DrawLine(ImVec2((float)(getWidth() / 2), (float)getHeight()), ImVec2(players[i].b_x, players[i].b_y), BLUE, 1); //LINE FROM MIDDLE SCREEN

					if (v.distance)
					{
						std::string distance = std::to_string(players[i].dist / 39.62);
						distance = distance.substr(0, distance.find('.')) + "m (" + std::to_string(players[i].entity_team) + ")";
						//distance += " |" + std::to_string(players[i].xp_level) + "|";

						// Display the distance just below the name
						if (players[i].knocked)
							String(ImVec2(players[i].boxMiddle, (players[i].b_y + 25)), RED, distance.c_str());  // DISTANCE
						else
							String(ImVec2(players[i].boxMiddle, (players[i].b_y + 25)), GREEN, distance.c_str());  // DISTANCE
					}

					// SEER STYLE health bar logic
					if (v.healthbar)
					{
						if (players[i].dist < 9000.0f)
						{
							// Divide the values by 2 to reduce the size of the health bar
							float barX = players[i].b_x - (players[i].width / 4.0f) + 5;  // Halve the width
							float barY = players[i].b_y - (players[i].height / 2.0f) - 10;  // Halve the height

							DrawSeerLikeHealth(barX, barY, players[i].shield, players[i].maxshield, players[i].armortype, players[i].health);  // Health bar
						}
					}

					// Display the name just below the distance
					if (v.name)
					{
						if (players[i].knocked)
							String(ImVec2(players[i].boxMiddle, (players[i].b_y + 15)), RED, players[i].name);  // NAME in RED if knocked
						else
							String(ImVec2(players[i].boxMiddle, (players[i].b_y + 15)), GREEN, players[i].name);  // NAME in GREEN if not knocked
					}

					if (v.platform)
					{
						std::string platformName = GetPlatformName(players[i].platform);
						if (players[i].knocked)
							String(ImVec2(players[i].boxMiddle, (players[i].b_y + 35)), RED, platformName.c_str());
						else
							String(ImVec2(players[i].boxMiddle, (players[i].b_y + 35)), GREEN, platformName.c_str());
					}

					if (v.weapon)
					{
						if (players[i].knocked)
							String(ImVec2(players[i].boxMiddle, (players[i].b_y + 45)), RED, players[i].weapon);
						else
							String(ImVec2(players[i].boxMiddle, (players[i].b_y + 45)), GREEN, players[i].weapon);
					}

					if (v.skeleton)
					{
						ImColor skelColor = players[i].visible ? GREEN : RED;

						// 0:Head, 1:Neck, 2:UpperChest, 3:LowerChest, 4:Pelvis, 5:L_Shoulder, 6:L_Elbow, 7:L_Hand, 8:R_Shoulder, 9:R_Elbow, 10:R_Hand, 11:L_Hip, 12:L_Knee, 13:L_Foot, 14:R_Hip, 15:R_Knee, 16:R_Foot
						// Skeleton connections
						int skeletonLines[][2] = {
							{1, 2}, {2, 3}, {3, 4}, // Spine
							{2, 5}, {5, 6}, {6, 7}, // Left arm
							{2, 8}, {8, 9}, {9, 10}, // Right arm
							{4, 11}, {11, 12}, {12, 13}, // Left leg
							{4, 14}, {14, 15}, {15, 16}, // Right leg
						};

						bool valid_skeleton = true;
						for (int b = 0; b < 17; b++)
						{
							// If any bone is at (0,0) or invalid, we skip the whole skeleton to avoid "starburst" artifacts
							if (players[i].bones[b][0] <= 1.0f || players[i].bones[b][1] <= 1.0f)
							{
								valid_skeleton = false;
								break;
							}
						}

						if (valid_skeleton)
						{
							for (auto& line : skeletonLines)
							{
								DrawLine(ImVec2(players[i].bones[line[0]][0], players[i].bones[line[0]][1]),
									ImVec2(players[i].bones[line[1]][0], players[i].bones[line[1]][1]),
									skelColor, v.skeleton_thickness);
							}

							// Draw head circle
							float headRadius = players[i].height / 12.0f;
							ImGui::GetWindowDrawList()->AddCircle(ImVec2(players[i].bones[0][0], players[i].bones[0][1]), headRadius, skelColor, 0, v.skeleton_thickness);
							// Connection Neck -> Head
							DrawLine(ImVec2(players[i].bones[1][0], players[i].bones[1][1]),
								ImVec2(players[i].bones[0][0], players[i].bones[0][1]),
								skelColor, v.skeleton_thickness);
						}
					}

				}
			}

			static int locked_player_idx = -1;
			if (!aiming) locked_player_idx = -1;

			int active_idx = -1;
			if (locked_player_idx != -1 && players[locked_player_idx].health > 0) {
				active_idx = locked_player_idx;
			}
			else {
				active_idx = best_player_idx;
				if (aiming) locked_player_idx = best_player_idx;
			}

			if (active_idx != -1) {
				if (v.target_indicator) {
					float dx = players[active_idx].b_x - cx;
					float dy = (players[active_idx].b_y + players[active_idx].h_y) / 2.0f - cy;
					float d = sqrt(dx * dx + dy * dy);

					if (d < v.target_indicator_fov) { // Condition: near crosshair (inside FOV)
						ImColor color = players[active_idx].visible ? GREEN : RED;
						ImGui::GetWindowDrawList()->AddCircle(ImVec2(players[active_idx].b_x, (players[active_idx].b_y + players[active_idx].h_y) / 2.0f), 5.0f, color, 12, 2.0f);
					}
				}
			}

			ImGui::End();
		}
	}
}

int main(int argc, char** argv)
{
	add[0] = (uintptr_t)&check;
	add[1] = (uintptr_t)&aim;
	add[2] = (uintptr_t)&esp;
	add[3] = (uintptr_t)&aiming;
	add[4] = (uintptr_t)&g_Base;
	add[5] = (uintptr_t)&next;
	add[6] = (uintptr_t)&players[0];
	add[7] = (uintptr_t)&valid;
	add[8] = (uintptr_t)&max_dist;
	add[9] = (uintptr_t)&item_glow;
	add[10] = (uintptr_t)&aim_no_recoil;
	add[11] = (uintptr_t)&bone;
	add[12] = (uintptr_t)&spectators;
	add[13] = (uintptr_t)&allied_spectators;
	add[14] = (uintptr_t)&glowr;
	add[15] = (uintptr_t)&glowg;
	add[16] = (uintptr_t)&glowb;
	add[17] = (uintptr_t)&glowrviz;
	add[18] = (uintptr_t)&glowgviz;
	add[19] = (uintptr_t)&glowbviz;
	add[20] = (uintptr_t)&glowrknocked;
	add[21] = (uintptr_t)&glowgknocked;
	add[22] = (uintptr_t)&glowbknocked;
	add[23] = (uintptr_t)&firing_range;
	add[24] = (uintptr_t)&shooting;
	add[25] = (uintptr_t)&onevone;
	add[26] = (uintptr_t)&spectator_list;
	add[27] = (uintptr_t)&dump;
	add[28] = (uintptr_t)&update_offsets;
	add[29] = (uintptr_t)&v.skeleton;
	add[30] = (uintptr_t)&screen_width;
	add[31] = (uintptr_t)&screen_height;
	add[32] = (uintptr_t)&triggerbot;
	add[33] = (uintptr_t)&triggerbot_key;
	add[34] = (uintptr_t)&triggerbot_aiming;
	add[35] = (uintptr_t)&superglide;
	add[36] = (uintptr_t)&bhop;
	add[37] = (uintptr_t)&walljump;
	add[38] = (uintptr_t)&SuperKey;
	add[39] = (uintptr_t)&triggerbot_fov;
	add[40] = (uintptr_t)&lock_target;
	add[41] = (uintptr_t)&player_glow;
	add[42] = (uintptr_t)&aim_dist;
	add[43] = (uintptr_t)&insidevalue;
	add[44] = (uintptr_t)&outlinesize;
	add[45] = (uintptr_t)&ads_fov;
	add[46] = (uintptr_t)&ads_smooth;
	add[47] = (uintptr_t)&hip_fov;
	add[48] = (uintptr_t)&hip_smooth;
	add[49] = (uintptr_t)&v.skeleton_thickness;
	add[50] = (uintptr_t)&aassist;
	add[51] = (uintptr_t)&aassist_dist;
	add[52] = (uintptr_t)&aassist_aiming;
	add[53] = (uintptr_t)&real_edid[0];
	add[54] = (uintptr_t)&fake_edid[0];
	add[55] = (uintptr_t)&monitor_spoof;

	printf(XorStr("add offset: 0x%I64x\n"), (uint64_t)&add[0] - (uint64_t)GetModuleHandle(NULL));

	Overlay ov1 = Overlay();
	ov1.Start();

	printf(XorStr("Waiting for host process...\n"));
	// Step 3: Establish connection between client and server
	while (check == 0xABCD)
	{
		if (IsKeyDown(VK_F4))
		{
			active = false;
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	if (active)
	{
		// Step 4: Execute EDID spoof automatically (Host signal: 0xBCDE)
		if (check == 0xBCDE)
		{
			printf(XorStr("Executing EDID Spoof...\n"));
			spoofmonitor();
			// Step 5: Inform host that spoof is complete
			check = 0xCDEF;
		}

		// Step 6 & 7: Wait for host to detect Apex Legends and signal 0
		printf(XorStr("Waiting for Apex Legends...\n"));
		while (check == 0xCDEF)
		{
			if (IsKeyDown(VK_F4))
			{
				active = false;
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}

	if (active)
	{
		ready = true;
		printf(XorStr("Ready\n"));
	}
		
	while (active)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		screen_width = ov1.getWidth();
		screen_height = ov1.getHeight();
		if (IsKeyDown(VK_F4))
		{
			active = false;
		}

		//Load at start for saved settings to take effect.
		for (static bool once = true; once; once = false) {
			LoadConfig("Settings.txt");
		}

		if (IsKeyDown(VK_F1) && k_f1 == 0)
		{
			k_f1 = 1;
			esp = !esp;
			// aim = 2;
			player_glow = !player_glow;
			k_f6 = 1;
			item_glow = !item_glow;
		}
		else if (!IsKeyDown(VK_F1) && k_f1 == 1)
		{
			k_f1 = 0;
			k_f6 = 0;
		}

		if (IsKeyDown(VK_F5) && k_f5 == 0)
		{
			k_f5 = 1;
			esp = !esp;
		}
		else if (!IsKeyDown(VK_F5) && k_f5 == 1)
		{
			k_f5 = 0;
		}

		if (IsKeyDown(VK_F6) && k_f6 == 0)
		{
			k_f6 = 1;
			//switch (aim)
			//{
			//case 0:
			//	aim = 1;
			//	break;
			//case 1:
			aim = 2;
			//	break;
			//case 2:
			//	aim = 0;
			//	break;
			//default:
			//	break;
			//}
		}
		else if (!IsKeyDown(VK_F6) && k_f6 == 1)
		{
			k_f6 = 0;
		}

		if (IsKeyDown(VK_F7) && k_f7 == 0)
		{
			k_f7 = 1;
			player_glow = !player_glow;
		}
		else if (!IsKeyDown(VK_F7) && k_f7 == 1)
		{
			k_f7 = 0;
		}

		if (IsKeyDown(VK_F8) && k_f8 == 0)
		{
			k_f8 = 1;
			item_glow = !item_glow;
		}
		else if (!IsKeyDown(VK_F8) && k_f8 == 1)
		{
			k_f8 = 0;
		}

		if (IsKeyDown(VK_F9) && k_f9 == 0)
		{
			k_f9 = 1;
			dump = true;
		}
		else if (!IsKeyDown(VK_F9) && k_f9 == 1)
		{
			k_f9 = 0;
		}

		if (IsKeyDown(VK_F10) && k_f10 == 0)
		{
			k_f10 = 1;
			update_offsets = true;
		}
		else if (!IsKeyDown(VK_F10) && k_f10 == 1)
		{
			k_f10 = 0;
		}

		if (IsKeyDown(VK_LEFT))
		{
			if (max_dist > 100.0f * 40.0f)
				max_dist -= 50.0f * 40.0f;
			std::this_thread::sleep_for(std::chrono::milliseconds(130));
		}

		if (IsKeyDown(VK_RIGHT))
		{
			if (max_dist < 800.0f * 40.0f)
				max_dist += 50.0f * 40.0f;
			std::this_thread::sleep_for(std::chrono::milliseconds(130));
		}

		if (IsKeyDown(VK_DELETE) && k_del == 0)
		{
			k_del = 1;
			showing = !showing;
		}
		else if (!IsKeyDown(VK_DELETE) && k_del == 1)
		{
			k_del = 0;
		}

		// Check if the user presses and holds SPACEBAR
		if (IsKeyDown(VK_SPACE))
		{
			SuperKey = true;
		}
		else
		{
			SuperKey = false;
		}

		////////////////////////////////////NORMAL AIM & BUTTON///////////////////////////////////////
		aiming = IsKeyDown(aim_key);
		aassist_aiming = aassist && IsKeyDown(aim_key2);
		triggerbot_aiming = triggerbot && IsKeyDown(VK_LSHIFT);

		// Shooting can be triggered by either left or right mouse button
		shooting = IsKeyDown(VK_LBUTTON) || IsKeyDown(VK_RBUTTON);

	}
	ready = false;
	ov1.Clear();
	if(!use_nvidia)
		system(XorStr("taskkill /F /T /IM Overlay.exe")); //custom overlay process name
	return 0;
}

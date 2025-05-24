#include "main.h"
#include <random>
#include <Windows.h>
//#include <chrono>
//test
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
//test contraste texte
#include "C:\Users\YOUR_USER\YOUR_FOLDER\OKVM\adkv-master\apex_guest\Client\Client\imgui\imgui.h"

// Definitions for global skeleton ESP data variables (declared in globals.h)
SkeletonESPDataPacket g_latest_skeleton_data;
bool g_new_skeleton_data_received = false;

// ProcessSkeletonESPPacket function is removed as data will be written directly by server.

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
	char name[33] = { 0 };
}player;

typedef struct spectator {
	bool is_spec = false;
	char name[33] = { 0 };
}spectator;

uint32_t check = 0xABCD;

int aim_key = VK_LBUTTON;
int aim_key2 = VK_RBUTTON;
bool use_nvidia = false;
bool active = true;
bool ready = false;
extern visuals v; // This is from overlay.h, v.box, v.name etc.
bool v_skeleton_esp = false; // Definition for the new global toggle

int aim = 0; //read
bool esp = false; //read
//bool item_glow = false;
bool player_glow = false;
bool aim_no_recoil = true;
bool aiming = false; //read
uint64_t g_Base = 0; //write
float max_dist = 120.0f * 40.0f;
//float esp_distance = 300.0f * 40.0f;
float smooth = 140.00f;
float max_fov = 3.80f;
int bone = 2;
// Declare constants for key detection
int SuperKey = VK_SPACE;  // VK_SPACE is the spacebar keycode
//int SuperKey = false;

//float esp_distance = 300.0f; // Units in meters

float DDS = 70.0f * 40.0f; //need test 25 before for closets targets but seem to be wrong

// Define the minimum and maximum values for max_fov, cfsize, and smooth
float min_max_fov = 4.00f;
float max_max_fov = 25.00f;
float min_cfsize = min_max_fov;
float max_cfsize = max_max_fov;
float min_smooth = 90.00f;
float max_smooth = 120.00f;

bool firing_range = false;
bool shooting = false; //read

//bool bone_auto = true;
//int shoot_key = VK_LBUTTON;

//1v1
bool onevone = false;

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
///end test

static bool showing = false;
static bool k_del = 0;
float cfsize = max_fov;

int spectators = 0; //write
int allied_spectators = 0; //write

//Player Glow Color and Brightness
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

uint64_t add[32]; // Changed size from 30 to 32

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

//void Overlay::RenderSpectator() {
	//memset(spectator_list, 0, sizeof(spectator_list));
    //ImGui::SetNextWindowPos(ImVec2(490, 0));
    //ImGui::SetNextWindowSize(ImVec2(190, 130));
    //ImGui::Begin(XorStr("##spectator_list"), (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
	//ImGui::TextColored(RED, "%d", spectators);
	//ImGui::SameLine();
	//ImGui::Text("-");
	//ImGui::SameLine();
	//ImGui::TextColored(GREEN, "%d", allied_spectators);

    //int text_index = 0;
    //for (const auto& spectator : spectator_list) {
    //    if (spectator.is_spec) {
	//		ImGui::TextColored(WHITE, "%d", text_index + 1);
    //        ImGui::SameLine(25);
	//		ImGui::TextColored(ORANGE, "%s", spectator.name);
    //        text_index++;
			//TEST// Convert char array to std::wstring
			//std::wstring wideStr;
			//for (int i = 0; spectator.name[i] != '\0'; ++i)
			//	wideStr += static_cast<wchar_t>(spectator.name[i]);

			 //Pass the wide string to ImGui::TextColored
			//ImGui::TextColored(ORANGE, "%ls", wideStr.c_str());

			//text_index++;
			
     //   }
    //}

    //ImGui::End();
//}
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

					// Assuming players[i].dist represents the distance of the player

					// Define the maximum distance you want to consider (previously referred to as max_distance)
					//float max_dist = 80.0f * 40.0f; // Adjust this value as per your requirement

					// Optimize calculations by caching division
					float distRatio = players[i].dist / max_dist;
					float distanceFactor = 1.0f - distRatio;
					float easedDistanceFactor = smoothStep(0.0f, 1.0f, distanceFactor);

					// Pre-calculate the multipliers
					float fovDiff = max_max_fov - min_max_fov;
					float smoothDiff = max_smooth - min_smooth;

					// Combine calculations
					if (players[i].dist < DDS) {
						float lerpFactor = easedDistanceFactor;
						max_fov = min_max_fov + (fovDiff * lerpFactor);
						cfsize = max_fov;
						smooth = min_smooth + (smoothDiff * lerpFactor);
						aim_key = aim_key2 = true;
					}
					else 
					{
						max_fov = 3.80f;
						cfsize = max_fov;
						smooth = 140.00f;
						aim_key = true;
						aim_key2 = false;
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
	//add[9] = (uintptr_t)&item_glow;
	add[9] = (uintptr_t)&player_glow;
	add[10] = (uintptr_t)&aim_no_recoil;
	add[11] = (uintptr_t)&smooth;
	add[12] = (uintptr_t)&max_fov;
	add[13] = (uintptr_t)&bone;
	add[14] = (uintptr_t)&spectators;
	add[15] = (uintptr_t)&allied_spectators;

	//glow notvisable
	add[16] = (uintptr_t)&glowr;
	add[17] = (uintptr_t)&glowg;
	add[18] = (uintptr_t)&glowb;
	//glow visable
	add[19] = (uintptr_t)&glowrviz;
	add[20] = (uintptr_t)&glowgviz;
	add[21] = (uintptr_t)&glowbviz;
	//knocked
	add[22] = (uintptr_t)&glowrknocked;
	add[23] = (uintptr_t)&glowgknocked;
	add[24] = (uintptr_t)&glowbknocked;
	add[25] = (uintptr_t)&firing_range;
	add[26] = (uintptr_t)&shooting;
	//items
	//add[28] = (uintptr_t)&medbackpack; 
	add[27] = (uintptr_t)&onevone;
	add[28] = (uintptr_t)&spectator_list; // Restoring original use, or decide if still needed
	add[29] = (uintptr_t)&SuperKey;       // Restoring original use
	// Assigning new shared memory addresses to add[30] and add[31]
	// This requires the array size to be at least 32.
	add[30] = (uintptr_t)&g_latest_skeleton_data;
	add[31] = (uintptr_t)&g_new_skeleton_data_received;

	printf(XorStr("add offset: 0x%I64x\n"), (uint64_t)&add[0] - (uint64_t)GetModuleHandle(NULL));

	Overlay ov1 = Overlay();
	ov1.Start();
	printf(XorStr("Waiting for host process...\n"));
	while (check == 0xABCD)
	{
        // Data for g_latest_skeleton_data and g_new_skeleton_data_received
        // is now expected to be written directly by the server into this process's memory
        // using addresses previously shared via the add[] array (now indices 30 and 31).
        // No explicit client-side packet reception logic is needed here.

		if (IsKeyDown(VK_F4))
		{
			active = false;
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	if (active)
	{
		ready = true;
		printf(XorStr("Ready\n"));
	}
		
	while (active)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		if (IsKeyDown(VK_F4))
		{
			active = false;
		}

		//Load at start for saved settings to take effect. Need to save once to make the file. 

		for (static bool once = true; once; once = false) {
			std::ifstream config("Settings.txt");
			if (config.is_open())
			{
				//config >> std::boolalpha >> firing_range;
				config >> aim;
				config >> std::boolalpha >> esp;
				//config >> std::boolalpha >> item_glow;
				config >> std::boolalpha >> player_glow;
				//config >> std::boolalpha >> aim_no_recoil;
				config >> max_dist;
				//config >> smooth;
				//config >> max_fov;
				//config >> bone;
				config >> v.healthbar;
				config >> v.shieldbar;
				config >> v.distance;
				//config >> v.player_level;
				config >> v.line;
				//config >> cfsize;
				//config >> DDS;
				//config >> aiming;
				config >> glowr;
				config >> glowg;
				config >> glowb;
				config >> glowcolor[0];
				config >> glowcolor[1];
				config >> glowcolor[2];
				//glow visable
				config >> glowrviz;
				config >> glowgviz;
				config >> glowbviz;
				config >> glowcolorviz[0];
				config >> glowcolorviz[1];
				config >> glowcolorviz[2];
				//glow knocked
				config >> glowrknocked;
				config >> glowgknocked;
				config >> glowbknocked;
				config >> glowcolorknocked[0];
				config >> glowcolorknocked[1];
				config >> glowcolorknocked[2];
				config >> firing_range;
				//config >> onevone;
				config >> min_max_fov;
				config >> max_max_fov;
				config >> min_smooth;
				config >> max_smooth;
				config >> std::boolalpha >> v_skeleton_esp; // Load Skeleton ESP toggle
				config.close();
			}
		}

		memset(spectator_list, 0, sizeof(spectator_list));

		if (IsKeyDown(VK_F1) && k_f1 == 0)
		{
			k_f1 = 1;
			esp = !esp;
			// aim = 2;
			player_glow = !player_glow;
			k_f6 = 1;
			//item_glow = !item_glow;
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
			//item_glow = !item_glow;
		}
		else if (!IsKeyDown(VK_F8) && k_f8 == 1)
		{
			k_f8 = 0;
		}

		if (IsKeyDown(VK_F9) && k_f9 == 0)
		{
			k_f9 = 1;
			onevone = !onevone;
		}
		else if (!IsKeyDown(VK_F9) && k_f9 == 1)
		{
			k_f9 = 0;
		}

		if (IsKeyDown(VK_F10) && k_f10 == 0)
		{
			k_f10 = 1;
			firing_range = !firing_range;
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
		if (IsKeyDown(aim_key) || IsKeyDown(aim_key2))
		{
			aiming = true;
			//randomBone();//RANDOMIZE BONE WHEN SHOOT
		}
		else
		{
			aiming = false;
		}
		
	}
	ready = false;
	ov1.Clear();
	if(!use_nvidia)
		system(XorStr("taskkill /F /T /IM Overlay.exe")); //custom overlay process name
	return 0;
}

// New function to render skeletons
// This function would ideally be called from the ImGui rendering loop in Overlay::CreateOverlay
// For now, defined here. Needs access to an Overlay instance for W2S and screen dimensions.
void RenderSkeletons(Overlay& overlay, int screen_width, int screen_height) {
    if (!v_skeleton_esp) { // Added toggle check
        return;
    }
    if (!g_new_skeleton_data_received || g_latest_skeleton_data.num_skeletons == 0) {
        return;
    }

    // Static storage for screen positions and on-screen status for each skeleton's bones
    // Using std::map to map hitbox_id to screen coordinates and visibility status
    static std::map<int, Vector2> skeleton_screen_pos_maps[MAX_SKELETONS_IN_PACKET];
    static std::map<int, bool> skeleton_on_screen_maps[MAX_SKELETONS_IN_PACKET];

    // Process world positions to screen positions
    for (int skeleton_idx = 0; skeleton_idx < g_latest_skeleton_data.num_skeletons; ++skeleton_idx) {
        skeleton_screen_pos_maps[skeleton_idx].clear();
        skeleton_on_screen_maps[skeleton_idx].clear();

        const PlayerSkeleton& player_skel = g_latest_skeleton_data.skeletons[skeleton_idx];
        if (!player_skel.valid) continue;

        for (int bone_array_idx = 0; bone_array_idx < player_skel.num_bones; ++bone_array_idx) {
            const SkeletonBone& bone = player_skel.bones[bone_array_idx];
            Vector2 screen_pos;
            // Calling WorldToScreen from the passed Overlay object
            bool on_screen = overlay.WorldToScreen(bone.position, g_latest_skeleton_data.view_matrix, screen_pos, screen_width, screen_height);
            skeleton_screen_pos_maps[skeleton_idx][bone.id] = screen_pos; 
            skeleton_on_screen_maps[skeleton_idx][bone.id] = on_screen;
        }
    }
    
    const std::vector<std::pair<int, int>> bone_connections = {
        {0, 1},   // Head to Neck
        {1, 2},   // Neck to Chest (Hitbox ID 2)
        {2, 3},   // Chest to Stomach (Hitbox ID 3)
        {3, 4},   // Stomach to Pelvis (Hitbox ID 4)
        {2, 6},   // Chest to Left Shoulder (ID 6)
        {6, 7},   // Left Shoulder (6) to Left Elbow (7)
        {7, 8},   // Left Elbow (7) to Left Wrist (8)
        {2, 9},   // Chest to Right Shoulder (ID 9)
        {9, 10},  // Right Shoulder (9) to Right Elbow (ID 10)
        {10, 11}, // Right Elbow (10) to Right Wrist (ID 11)
        {4, 12},  // Pelvis (ID 4) to Left Thigh (ID 12)
        {12, 13}, // Left Thigh (12) to Left Knee (ID 13)
        {13, 14}, // Left Knee (13) to Left Ankle (ID 14)
        {4, 16},  // Pelvis (ID 4) to Right Thigh (ID 16)
        {16, 17}, // Right Thigh (16) to Right Knee (ID 17)
        {17, 18}  // Right Knee (17) to Right Ankle (ID 18)
    };

    // This drawing should happen within an ImGui window context.
    // Assuming this function is called from within an ImGui::Begin/End pair,
    // or it sets up its own full-screen transparent window.
    // For now, using ImGui::GetBackgroundDrawList() which might draw on the main overlay.
    ImDrawList* draw_list = ImGui::GetBackgroundDrawList(); 

    for (int skeleton_idx = 0; skeleton_idx < g_latest_skeleton_data.num_skeletons; ++skeleton_idx) {
        const PlayerSkeleton& player_skel = g_latest_skeleton_data.skeletons[skeleton_idx];
        if (!player_skel.valid || skeleton_screen_pos_maps[skeleton_idx].empty()) continue;

        for (const auto& conn : bone_connections) {
            auto it1_pos_data = skeleton_screen_pos_maps[skeleton_idx].find(conn.first);
            auto it1_on_screen_data = skeleton_on_screen_maps[skeleton_idx].find(conn.first);
            auto it2_pos_data = skeleton_screen_pos_maps[skeleton_idx].find(conn.second);
            auto it2_on_screen_data = skeleton_on_screen_maps[skeleton_idx].find(conn.second);

            if (it1_pos_data != skeleton_screen_pos_maps[skeleton_idx].end() &&
                it2_pos_data != skeleton_screen_pos_maps[skeleton_idx].end() &&
                it1_on_screen_data != skeleton_on_screen_maps[skeleton_idx].end() && it1_on_screen_data->second &&
                it2_on_screen_data != skeleton_on_screen_maps[skeleton_idx].end() && it2_on_screen_data->second) {
                
                draw_list->AddLine(
                    ImVec2(it1_pos_data->second.x, it1_pos_data->second.y),
                    ImVec2(it2_pos_data->second.x, it2_pos_data->second.y),
                    WHITE, // Defined in overlay.h, consider moving or passing ImColor
                    1.5f
                );
            }
        }
    }
    g_new_skeleton_data_received = false; 
}

// TODO: Call RenderSkeletons(ov1, ov1.getWidth(), ov1.getHeight()); 
// from Overlay::CreateOverlay() main loop, after ImGui::NewFrame() and other ESP rendering.
// For example, after the RenderEsp() call or as part of it if direct editing was possible.

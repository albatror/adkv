#include <array>
#include "prediction.h"
#include <cassert>
#include <ostream>
#include <iostream>


extern Memory apex_mem;

extern bool firing_range;
float smooth = 12.0f;
extern bool aim_no_recoil;
int bone = 2;
//
bool bone_auto = true;
extern float max_dist;
//
extern float glowr;
extern float glowg;
extern float glowb;

extern int glowtype;
extern int glowtype2;
extern int glowtype3;

// https://github.com/Gerosity/zap-client/blob/master/Core/Player.hpp#L161
bool Entity::Observing(uint64_t localptr)
{
	uint64_t SpectatorList;
	apex_mem.Read<uint64_t>(apex_mem.get_proc_baseaddr() + OFFSET_OBSERVER_LIST, SpectatorList);

	int PlayerData;
	apex_mem.Read<int>(ptr + 0x38, PlayerData);

	int SpecIndex;
	apex_mem.Read<int>(SpectatorList + PlayerData * 8 + OFFSET_OBSERVER_ARRAY, SpecIndex);

	uint64_t SpectatorAddr;
	apex_mem.Read<uint64_t>(apex_mem.get_proc_baseaddr() + OFFSET_ENTITYLIST + ((SpecIndex & 0xFFFF) << 5), SpectatorAddr);

    	if (SpectatorAddr == localptr) 
	{
        return true;
    }
 
    return false;
}

//https://github.com/CasualX/apexbot/blob/master/src/state.cpp#L104
void get_class_name(uint64_t entity_ptr, char* out_str)
{
	uint64_t client_networkable_vtable;
	apex_mem.Read<uint64_t>(entity_ptr + 8 * 3, client_networkable_vtable);

	uint64_t get_client_class;
	apex_mem.Read<uint64_t>(client_networkable_vtable + 8 * 3, get_client_class);

	uint32_t disp;
	apex_mem.Read<uint32_t>(get_client_class + 3, disp);
	const uint64_t client_class_ptr = get_client_class + disp + 7;

	ClientClass client_class;
	apex_mem.Read<ClientClass>(client_class_ptr, client_class);

	apex_mem.ReadArray<char>(client_class.pNetworkName, out_str, 32);
}

//////////////////////////////////
void InState::update(uint32_t address)
{
	extern uint64_t g_Base;
	apex_mem.Read(g_Base + address, button);
	state = (button.state & 1) != 0;
}

void InState::post(uint32_t address)
{
	extern uint64_t g_Base;
	// If active get the most recent state of the button
	if (force && apex_mem.Read(g_Base + address, button))
	{
		// Get the desired state of the button
		int state;
		if (press && !release) {
			state = 5;
		}
		else if (!press && release) {
			state = 4;
		}
		else {
			state = button.down[0] == 0 && button.down[1] == 0 ? 4 : 5;
		}
		// Gently tell the game to that nobody will be harmed if they just do as told
		if (button.state != state) {
			apex_mem.Write(g_Base + address + 8, state);
		}
	}
}

//////////////////////////////////

//////////////////////////////////
uint32_t button_state[4];

bool isPressed(uint32_t button_code)
{
	return (button_state[static_cast<uint32_t>(button_code) >> 5] & (1 << (static_cast<uint32_t>(button_code) & 0x1f))) != 0;
}
//////////////////////////////////

int Entity::getTeamId()
{
	return *(int*)(buffer + OFFSET_TEAM);
}

int Entity::getHealth()
{
	return *(int*)(buffer + OFFSET_HEALTH);
}

int Entity::getShield()
{
	return *(int*)(buffer + OFFSET_SHIELD);
}

int Entity::getMaxShield()
{
	return *(int*)(buffer + OFFSET_MAXSHIELD);
}

int Entity::getArmortype()
{
	return *(int*)(buffer + OFFSET_ARMORTYPE);
}

Vector Entity::getAbsVelocity()
{
	return *(Vector*)(buffer + OFFSET_ABS_VELOCITY);
}

Vector Entity::getPosition()
{
	return *(Vector*)(buffer + OFFSET_ORIGIN);
}

bool Entity::isPlayer()
{
	return *(uint64_t*)(buffer + OFFSET_NAME) == 125780153691248;
}

bool Entity::isDummy()
{
	char class_name[33] = {};
	get_class_name(ptr, class_name);

	return strncmp(class_name, "CAI_BaseNPC", 11) == 0;
}

bool Entity::isKnocked()
{
	return *(int*)(buffer + OFFSET_BLEED_OUT_STATE) > 0;
}

bool Entity::isAlive()
{
	return *(int*)(buffer + OFFSET_LIFE_STATE) == 0;
}

/////////////////////////////////
bool Entity::isOnGround() 
{
	uint32_t flags;
	apex_mem.Read(ptr + OFFSET_FLAGS, flags);

	return (flags & 0x1) != 0;
}

bool Entity::isInSkydive() 
{
	return *(int*)(buffer + OFFSET_SKYDIVE_STATE) > 0;
}
/////////////////////////////////

float Entity::lastVisTime()
{
  return *(float*)(buffer + OFFSET_VISIBLE_TIME);
}

///////////////////////////////
float Entity::lastCrossHairTime()
{
	return *(float*)(buffer + OFFSET_CROSSHAIR_LAST);
}
///////////////////////////////

Vector Entity::getBonePosition(int id)
{
	Vector position = getPosition();
	uintptr_t boneArray = *(uintptr_t*)(buffer + OFFSET_BONES);
	Vector bone = Vector();
	uint32_t boneloc = (id * 0x30);
	Bone bo = {};
	apex_mem.Read<Bone>(boneArray + boneloc, bo);
	bone.x = bo.x + position.x;
	bone.y = bo.y + position.y;
	bone.z = bo.z + position.z;
	return bone;
}

//https://www.unknowncheats.me/forum/apex-legends/496984-getting-hitbox-positions-cstudiohdr-externally.html
//https://www.unknowncheats.me/forum/3499185-post1334.html
//https://www.unknowncheats.me/forum/3562047-post11000.html
Vector Entity::getBonePositionByHitbox(int id)
{
	Vector origin = getPosition();
 
    //BoneByHitBox
	uint64_t Model = *(uint64_t*)(buffer + OFFSET_STUDIOHDR);
    
	//get studio hdr
	uint64_t StudioHdr;
	apex_mem.Read<uint64_t>(Model + 0x8, StudioHdr);
 
    //get hitbox array
	uint16_t HitboxCache;
	apex_mem.Read<uint16_t>(StudioHdr + 0x34, HitboxCache);
	uint64_t HitBoxsArray = StudioHdr + ((uint16_t)(HitboxCache & 0xFFFE) << (4 * (HitboxCache & 1)));
 
	uint16_t IndexCache;
 	apex_mem.Read<uint16_t>(HitBoxsArray + 0x4, IndexCache);
	int HitboxIndex = ((uint16_t)(IndexCache & 0xFFFE) << (4 * (IndexCache & 1)));
 
	uint16_t Bone;
	apex_mem.Read<uint16_t>(HitBoxsArray + HitboxIndex + (id * 0x20), Bone);

	if(Bone < 0 || Bone > 255)
		return Vector();
 
    //hitpos
	uint64_t BoneArray = *(uint64_t*)(buffer + OFFSET_BONES);
 
	matrix3x4_t Matrix = {};
	apex_mem.Read<matrix3x4_t>(BoneArray + Bone * sizeof(matrix3x4_t), Matrix);
 
	return Vector(Matrix.m_flMatVal[0][3] + origin.x, Matrix.m_flMatVal[1][3] + origin.y, Matrix.m_flMatVal[2][3] + origin.z);
}

QAngle Entity::GetSwayAngles()
{
	return *(QAngle*)(buffer + OFFSET_BREATH_ANGLES);
}

QAngle Entity::GetViewAngles()
{
	return *(QAngle*)(buffer + OFFSET_VIEWANGLES);
}

Vector Entity::GetViewAnglesV()
{
	return *(Vector*)(buffer + OFFSET_VIEWANGLES);
}

float Entity::GetYaw()
{
	float yaw = 0;
	apex_mem.Read<float>(ptr + OFFSET_YAW, yaw);

	if (yaw < 0)
    	yaw += 360;
	yaw += 90;
	if (yaw > 360)
    	yaw -= 360;
		
	return yaw;
}

bool Entity::isGlowing()
{
	return *(int*)(buffer + OFFSET_GLOW_ENABLE) == 7;
}

bool Entity::isZooming()
{
	return *(int*)(buffer + OFFSET_ZOOMING) == 1;
}

/////////////////////////
/////////////////////////

/////////////////////////
    extern uint64_t g_Base;
    extern int settingIndex;
    extern int contextId;
    extern std::array<float, 3> highlightParameter;
    //custom glow colo RGB
    unsigned char outsidevalue = 125;
    extern unsigned char insidevalue;
    extern unsigned char insidevalueItem;
    extern unsigned char outlinesize;
     
    void Entity::enableGlow()
    {
    	
    		//static const int contextId = 0;
    		//int settingIndex = 50;
    		std::array<unsigned char, 4> highlightFunctionBits = {
    			insidevalue,   // InsideFunction
    			outsidevalue, // OutlineFunction: HIGHLIGHT_OUTLINE_OBJECTIVE
    			outlinesize,  // OutlineRadius: size * 255 / 8
    			64   // (EntityVisible << 6) | State & 0x3F | (AfterPostProcess << 7)
    		};
     
    		apex_mem.Write<int>(ptr + OFFSET_GLOW_ENABLE, contextId);
    		long highlightSettingsPtr;
    		apex_mem.Read<long>(g_Base + HIGHLIGHT_SETTINGS, highlightSettingsPtr);
    		apex_mem.Write<int>(ptr + OFFSET_GLOW_THROUGH_WALLS, 2);
    		apex_mem.Write<typeof(highlightFunctionBits)>(highlightSettingsPtr + HIGHLIGHT_TYPE_SIZE * contextId + 0x0, highlightFunctionBits);
    		apex_mem.Write<typeof(highlightParameter)>(highlightSettingsPtr + HIGHLIGHT_TYPE_SIZE * contextId + 0x4, highlightParameter);
  		//apex_mem.Write<float>(ptr + 0x264, 8.0E+4);

 		//apex_mem.Write(g_Base + OFFSET_GLOW_FIX, 1);
    		apex_mem.Write<float>(ptr + GLOW_DISTANCE, 88888);
     
    	
     
    }
///////////////////////////

void Entity::disableGlow()
{
	//apex_mem.Write<int>(ptr + OFFSET_GLOW_T1, 0);
	//apex_mem.Write<int>(ptr + OFFSET_GLOW_T2, 0);
	//apex_mem.Write<int>(ptr + OFFSET_GLOW_ENABLE, 2);
	//apex_mem.Write<int>(ptr + OFFSET_GLOW_THROUGH_WALLS, 5);
}

void Entity::SetViewAngles(SVector angles)
{
	apex_mem.Write<SVector>(ptr + OFFSET_VIEWANGLES, angles);
}

void Entity::SetViewAngles(QAngle& angles)
{
	SetViewAngles(SVector(angles));
}

Vector Entity::GetCamPos()
{
	return *(Vector*)(buffer + OFFSET_CAMERAPOS);
}

QAngle Entity::GetRecoil()
{
	return *(QAngle*)(buffer + OFFSET_AIMPUNCH);
}

void Entity::get_name(uint64_t g_Base, uint64_t index, char* name)
{
     int name_index;
    apex_mem.Read<int>(ptr + 0x38, name_index);
    uint64_t name_ptr = 0;
    apex_mem.Read<uint64_t>(g_Base + OFFSET_NAME_LIST + ((name_index - 1) * 24), name_ptr);
	apex_mem.ReadArray<char>(name_ptr, name, 32);
}

////test////
    int Entity::read_xp_level() {
     
     
      int xp = 0;
      apex_mem.Read<int>(ptr + OFFSET_m_xp, xp);
     
      static int levels[] = {2750,   6650,   11400,  17000,  23350,  30450,  38300,
                             46450,  55050,  64100,  73600,  83550,  93950,  104800,
                             116100, 127850, 140050, 152400, 164900, 177550, 190350,
                             203300, 216400, 229650, 243050, 256600, 270300, 284150,
                             298150, 312300, 326600, 341050, 355650, 370400, 385300,
                             400350, 415550, 430900, 446400, 462050, 477850, 493800,
                             509900, 526150, 542550, 559100, 575800, 592650, 609650,
                             626800, 644100, 661550, 679150, 696900, 714800};
     
      if (xp < 0)
        return 0;
      if (xp < 100)
        return 1;
     
      int level = 56;
      int arraySize = sizeof(levels) / sizeof(levels[0]);
     
      for (int i = 0; i < arraySize; i++) {
        if (xp < levels[i]) {
          return i + 1;
        }
      }
      // Debugging statement to log the XP being read
      //std::cout << "XP read: " << xp << std::endl;
      return level + ((xp - levels[arraySize - 1] + 1) / 18000) + 1;
    }

//Deathboxes
//bool Item::isBox()
//{
//	char class_name[33] = {};
//	get_class_name(ptr, class_name);
//
//	return strncmp(class_name, "CDeathBoxProp", 13) == 0;
//}
//Traps
//bool Item::isTrap()
//{
//	char class_name[33] = {};
//	get_class_name(ptr, class_name);
//
//	return strncmp(class_name, "caustic_trap", 13) == 0;
//}

//bool Item::isItem()
//{
//	char class_name[33] = {};
//	get_class_name(ptr, class_name);
//
//	return strncmp(class_name, "CPropSurvival", 13) == 0;
//}

//bool Item::isGlowing()
//{
//	return *(int*)(buffer + OFFSET_ITEM_GLOW) == 1363184265;
//}

//Vector Item::getPosition()
//{
//	return *(Vector*)(buffer + OFFSET_ORIGIN);
//}

float CalculateFov(Entity& from, Entity& target)
{
	QAngle ViewAngles = from.GetViewAngles();
	Vector LocalCamera = from.GetCamPos();
	Vector EntityPosition = target.getPosition();
	QAngle Angle = Math::CalcAngle(LocalCamera, EntityPosition);
	return Math::GetFov(ViewAngles, Angle);
}

QAngle CalculateBestBoneAim(Entity& from, uintptr_t t, float max_fov)
{
	Entity target = getEntity(t);
	if(firing_range)
	{
		if (!target.isAlive())
		{
			return QAngle(0, 0, 0);
		}
	}
	else
	{
		if (!target.isAlive() || target.isKnocked())
		{
			return QAngle(0, 0, 0);
		}
	}
	
	Vector LocalCamera = from.GetCamPos();
	//
	float distanceToTarget;
	//
	Vector TargetBonePosition = target.getBonePositionByHitbox(bone);
	QAngle CalculatedAngles = QAngle(0, 0, 0);
	
	WeaponXEntity curweap = WeaponXEntity();
	curweap.update(from.ptr);
	float BulletSpeed = curweap.get_projectile_speed();
	float BulletGrav = curweap.get_projectile_gravity();
	float zoom_fov = curweap.get_zoom_fov();

	if (zoom_fov != 0.0f && zoom_fov != 1.0f)
	{
		max_fov *= zoom_fov/90.0f;
	}

  // Find best bone
  if (bone_auto) {
    float NearestBoneDistance = max_dist;
    for (int i = 0; i < 4; i++) {
      Vector currentBonePosition = target.getBonePositionByHitbox(i);
      float DistanceFromCrosshair =
          (currentBonePosition - LocalCamera).Length();
      if (DistanceFromCrosshair < NearestBoneDistance) {
        TargetBonePosition = currentBonePosition;
        distanceToTarget = DistanceFromCrosshair;
        NearestBoneDistance = DistanceFromCrosshair;
      }
    }
  } else {
    TargetBonePosition = target.getBonePositionByHitbox(bone);
    distanceToTarget = (TargetBonePosition - LocalCamera).Length();
  }
	/*
	//simple aim prediction
	if (BulletSpeed > 1.f)
	{
		Vector LocalBonePosition = from.getBonePosition(bone);
		float VerticalTime = TargetBonePosition.DistTo(LocalBonePosition) / BulletSpeed;
		TargetBonePosition.z += (BulletGrav * 0.5f) * (VerticalTime * VerticalTime);

		float HorizontalTime = TargetBonePosition.DistTo(LocalBonePosition) / BulletSpeed;
		TargetBonePosition += (target.getAbsVelocity() * HorizontalTime);
	}
	*/
	
	//more accurate prediction
	if (BulletSpeed > 1.f)
	{
		PredictCtx Ctx;
		Ctx.StartPos = LocalCamera;
		Ctx.TargetPos = TargetBonePosition; 
		Ctx.BulletSpeed = BulletSpeed - (BulletSpeed*0.08);
		Ctx.BulletGravity = BulletGrav + (BulletGrav*0.05);
		Ctx.TargetVel = target.getAbsVelocity();

		if (BulletPredict(Ctx))
			CalculatedAngles = QAngle{Ctx.AimAngles.x, Ctx.AimAngles.y, 0.f};
    }

	if (CalculatedAngles == QAngle(0, 0, 0))
    	CalculatedAngles = Math::CalcAngle(LocalCamera, TargetBonePosition);
	QAngle ViewAngles = from.GetViewAngles();
	QAngle SwayAngles = from.GetSwayAngles();
	//remove sway and recoil
	if(aim_no_recoil)
		CalculatedAngles-=SwayAngles-ViewAngles;
	Math::NormalizeAngles(CalculatedAngles);
	QAngle Delta = CalculatedAngles - ViewAngles;
	double fov = Math::GetFov(SwayAngles, CalculatedAngles);
	if (fov > max_fov)
	{
		return QAngle(0, 0, 0);
	}

	Math::NormalizeAngles(Delta);

	QAngle SmoothedAngles = ViewAngles + Delta/smooth;
	return SmoothedAngles;
}

Entity getEntity(uintptr_t ptr)
{
	Entity entity = Entity();
	entity.ptr = ptr;
	apex_mem.ReadArray<uint8_t>(ptr, entity.buffer, sizeof(entity.buffer));
	//return entity;
    	if (entity.isPlayer()) { // Use the isPlayer() function to initialize is_player
        entity.is_player = true;
    	//entity.player_xp_level = entity.read_xp_level();
	//entity.player_level();
    }
    return entity;}

//Item getItem(uintptr_t ptr)
//{
//	Item entity = Item();
//	entity.ptr = ptr;
//	apex_mem.ReadArray<uint8_t>(ptr, entity.buffer, sizeof(entity.buffer));
//	return entity;
//}

bool WorldToScreen(Vector from, float* m_vMatrix, int targetWidth, int targetHeight, Vector& to)
{
	float w = m_vMatrix[12] * from.x + m_vMatrix[13] * from.y + m_vMatrix[14] * from.z + m_vMatrix[15];

	if (w < 0.01f) return false;

	to.x = m_vMatrix[0] * from.x + m_vMatrix[1] * from.y + m_vMatrix[2] * from.z + m_vMatrix[3];
	to.y = m_vMatrix[4] * from.x + m_vMatrix[5] * from.y + m_vMatrix[6] * from.z + m_vMatrix[7];

	float invw = 1.0f / w;
	to.x *= invw;
	to.y *= invw;

	float x = targetWidth / 2;
	float y = targetHeight / 2;

	x += 0.5 * to.x * targetWidth + 0.5;
	y -= 0.5 * to.y * targetHeight + 0.5;

	to.x = x;
	to.y = y;
	to.z = 0;
	return true;
}

void WeaponXEntity::update(uint64_t LocalPlayer)
{
    extern uint64_t g_Base;
	uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;
	uint64_t wephandle = 0;
    apex_mem.Read<uint64_t>(LocalPlayer + OFFSET_WEAPON, wephandle);
	
	wephandle &= 0xffff;

	uint64_t wep_entity = 0;
    apex_mem.Read<uint64_t>(entitylist + (wephandle << 5), wep_entity);

	projectile_speed = 0;
    apex_mem.Read<float>(wep_entity + OFFSET_BULLET_SPEED, projectile_speed);
	projectile_scale = 0;
    apex_mem.Read<float>(wep_entity + OFFSET_BULLET_SCALE, projectile_scale);
	zoom_fov = 0;
    apex_mem.Read<float>(wep_entity + OFFSET_ZOOM_FOV, zoom_fov);
	ammo = 0;
    apex_mem.Read<int>(wep_entity + OFFSET_AMMO, ammo);
}

//////////////////////
//////////////////////

float WeaponXEntity::get_projectile_speed()
{
	return projectile_speed;
}

float WeaponXEntity::get_projectile_gravity()
{
	return 750.0f * projectile_scale;
}

float WeaponXEntity::get_zoom_fov()
{
	return zoom_fov;
}

int WeaponXEntity::get_ammo()
{
	return ammo;
}

//const char *WeaponXEntity::get_name_str() { return name_str; }

// New Entity methods implementations
Vector3 Entity::getAbsOrigin()
{
    Vector3 origin;
    if (!apex_mem.Read<Vector3>(this->ptr + OFFSET_ORIGIN, origin)) {
        // Consider logging an error or handling it more robustly
        return Vector3(); // Return default (zero) vector on failure
    }
    return origin;
}

uint64_t Entity::getStudioHdr_ptr()
{
    uint64_t studio_hdr_ptr = 0;
    if (!apex_mem.Read<uint64_t>(this->ptr + OFFSET_STUDIOHDR, studio_hdr_ptr)) {
        // Consider logging an error
        return 0; // Return 0 on failure
    }
    return studio_hdr_ptr;
}

Matrix3x4 Entity::getBoneMatrix(int bone_index)
{
    if (bone_index < 0 || bone_index >= MAX_BONES) { // Basic bounds check
        // Consider logging an error
        return Matrix3x4();
    }

    uint64_t force_bone_ptr = 0;
    // OFFSET_BONES is defined as 0x0d88 + 0x48.
    // The subtask specifies reading m_nForceBone (0x0d88) first, then adding 0x48.
    if (!apex_mem.Read<uint64_t>(this->ptr + 0x0d88, force_bone_ptr)) {
        // Consider logging an error
        return Matrix3x4();
    }

    if (force_bone_ptr == 0) {
        // Consider logging an error
        return Matrix3x4();
    }

    uint64_t bone_matrix_address = force_bone_ptr + 0x48 + (bone_index * sizeof(Matrix3x4));
    Matrix3x4 matrix;
    if (!apex_mem.Read<Matrix3x4>(bone_matrix_address, matrix)) {
        // Consider logging an error
        return Matrix3x4(); // Return default (zero) matrix on failure
    }
    return matrix;
}

// Define a maximum number of hitboxes to scan to prevent infinite loops in malformed data
// const int MAX_HITBOXES_TO_SCAN = 30; // No longer needed for getBoneIndexByHitbox with direct indexing.

int Entity::getBoneIndexByHitbox(int hitbox_id)
{
    uint64_t studio_hdr_ptr = this->getStudioHdr_ptr();
    if (studio_hdr_ptr == 0) {
        // std::cerr << "getBoneIndexByHitbox: studio_hdr_ptr is null" << std::endl;
        return -1;
    }

    // Read hitbox_set_offset from studio_hdr_ptr + OFFSET_STUDIOHDR_HITBOX_SET_ARRAY (0xB4)
    int hitbox_set_offset = 0;
    if (!apex_mem.Read<int>(studio_hdr_ptr + OFFSET_STUDIOHDR_HITBOX_SET_ARRAY, hitbox_set_offset)) {
        // std::cerr << "getBoneIndexByHitbox: Failed to read hitbox_set_offset" << std::endl;
        return -1;
    }
    if (hitbox_set_offset == 0) { // An offset of 0 might be invalid if it points to studio_hdr_ptr itself.
        // std::cerr << "getBoneIndexByHitbox: hitbox_set_offset is 0" << std::endl;
        return -1;
    }

    uint64_t hitbox_set_addr = studio_hdr_ptr + hitbox_set_offset;

    // Read hitbox_array_offset_in_set from hitbox_set_addr + OFFSET_HITBOX_SET_HITBOX_ARRAY_OFFSET (0x8)
    int hitbox_array_offset_in_set = 0;
    if (!apex_mem.Read<int>(hitbox_set_addr + OFFSET_HITBOX_SET_HITBOX_ARRAY_OFFSET, hitbox_array_offset_in_set)) {
        // std::cerr << "getBoneIndexByHitbox: Failed to read hitbox_array_offset_in_set" << std::endl;
        return -1;
    }
     if (hitbox_array_offset_in_set == 0) {
        // std::cerr << "getBoneIndexByHitbox: hitbox_array_offset_in_set is 0" << std::endl;
        return -1;
    }


    uint64_t hitbox_array_base = hitbox_set_addr + hitbox_array_offset_in_set;

    // Calculate address of the specific hitbox definition using hitbox_id as an index
    // It's crucial that hitbox_id is a valid index (e.g., 0 for head, 1 for neck, etc.)
    // and does not exceed array bounds. A bounds check for hitbox_id might be needed here
    // if HITBOX_STRIDE or number of hitboxes isn't fixed or known.
    // For now, assuming hitbox_id is a safe index.
    uint64_t hitbox_definition_address = hitbox_array_base + (static_cast<uint64_t>(hitbox_id) * HITBOX_STRIDE); // HITBOX_STRIDE is 0x2C

    int bone_index = -1;
    // Read the bone index (int) from hitbox_definition_address + 0x00 (assuming bone index is at the start)
    if (!apex_mem.Read<int>(hitbox_definition_address + 0x00, bone_index)) {
        // std::cerr << "getBoneIndexByHitbox: Failed to read bone_index from hitbox_definition_address" << std::endl;
        return -1;
    }

    return bone_index;
}

Vector3 Entity::getHitboxWorldPosition(int hitbox_id)
{
    int bone_idx = this->getBoneIndexByHitbox(hitbox_id);
    if (bone_idx == -1) {
        // std::cerr << "getHitboxWorldPosition: Failed to get bone_idx for hitbox_id " << hitbox_id << std::endl;
        return Vector3(); // Return default (zero) vector if bone_idx not found
    }

    Matrix3x4 bone_matrix = this->getBoneMatrix(bone_idx);
    // Basic check for zeroed matrix (translation part)
    if (bone_matrix.m[0][3] == 0.f && bone_matrix.m[1][3] == 0.f && bone_matrix.m[2][3] == 0.f &&
        bone_matrix.m[0][0] == 0.f && bone_matrix.m[1][1] == 0.f && bone_matrix.m[2][2] == 0.f) { // Check a few rotation elements too
        // This could mean an error in getBoneMatrix or the bone is truly at origin.
        // The prompt implies returning Vector3() if it's zeroed due to read failure.
        // std::cerr << "getHitboxWorldPosition: bone_matrix is zeroed for bone_idx " << bone_idx << std::endl;
        return Vector3();
    }

    // Extract the bone's translation part from the matrix (relative to entity origin)
    Vector3 bone_offset_in_entity;
    bone_offset_in_entity.x = bone_matrix.m[0][3];
    bone_offset_in_entity.y = bone_matrix.m[1][3];
    bone_offset_in_entity.z = bone_matrix.m[2][3];

    Vector3 entity_origin = this->getAbsOrigin();
    // Basic check for zeroed entity_origin
    if (entity_origin.x == 0.f && entity_origin.y == 0.f && entity_origin.z == 0.f) {
        // This could be a valid origin or a read failure.
        // Per prompt, if it's zeroed, assume failure for this context.
        // std::cerr << "getHitboxWorldPosition: entity_origin is zeroed" << std::endl;
        // However, an entity could genuinely be at (0,0,0). This check might be too aggressive.
        // For now, let's assume if getAbsOrigin() succeeded and returned (0,0,0), it's valid.
        // The previous version of getAbsOrigin() already returns Vector3() on read failure.
        // So, if we get here and it's (0,0,0), it might be a valid position.
        // The subtask says "Add basic checks for zeroed origin or matrix if reads might fail".
        // Let's refine this: if getAbsOrigin itself failed, it would return default Vector3.
        // If it succeeded and returned (0,0,0), we should trust it.
        // The check for bone_matrix is more about it being uninitialized from a failed read in getBoneMatrix.
    }
    
    // The final world position of the hitbox (interpreted as the bone's origin in world space)
    // is entity_origin + bone_offset_in_entity (if bone_offset_in_entity is already in world space relative to entity origin)
    // Or, if bone_matrix is the full world transform of the bone, then bone_offset_in_entity is already the world position.
    // Given how bone matrices usually work (transforming from model space to world space, or relative to entity),
    // bone_matrix.m[?][3] are the world coordinates of the bone's origin *if the entity itself is at (0,0,0)*.
    // Or, they are the bone's coordinates relative to the entity's origin.
    // The common way: BoneToWorldTransform = EntityToWorldTransform * BoneToEntityTransform
    // If bone_matrix is BoneToWorld, then (m[0][3], m[1][3], m[2][3]) is already world.
    // If bone_matrix is BoneToEntity, then we need to transform it by EntityToWorld.
    // The `OFFSET_BONES` gives bone_to_world matrices. So m[?][3] is already world coords.
    // The instruction "Return entity_origin + bone_offset_in_world" is confusing if bone_offset_in_world IS ALREADY WORLD.
    // Let's re-read `getBonePosition` from the same file for clues:
    // bo.x + position.x; where position is entity origin. This suggests the matrix stores bone pos relative to entity.
    // And existing `getBonePositionByHitbox` also does `Matrix.m_flMatVal[0][3] + origin.x`.
    // This means the matrix from OFFSET_BONES is relative to entity origin.
    
    // So, bone_offset_in_entity is correct.
    Vector3 final_world_position;
    final_world_position.x = entity_origin.x + bone_offset_in_entity.x;
    final_world_position.y = entity_origin.y + bone_offset_in_entity.y;
    final_world_position.z = entity_origin.z + bone_offset_in_entity.z;
    
    return final_world_position;
}

PlayerSkeleton Entity::getPlayerSkeleton() {
    PlayerSkeleton skeleton; // num_bones = 0, valid = false by constructor
    
    // Predefined hitbox IDs to fetch
    const int hitbox_ids[] = {
        0, 1, 2, 3, 4,       // Common head and neck parts
        6, 7, 8, 9, 10, 11,  // Torso and arm parts
        12, 13, 14,          // Leg parts
        16, 17, 18           // Feet and other extremity parts (example IDs)
    };
    const int num_hitbox_ids_to_check = sizeof(hitbox_ids) / sizeof(hitbox_ids[0]);

    for (int i = 0; i < num_hitbox_ids_to_check; ++i) {
        if (skeleton.num_bones >= MAX_BONES) {
            break; // Skeleton array is full
        }

        int current_hitbox_id = hitbox_ids[i];
        Vector3 bone_pos = this->getHitboxWorldPosition(current_hitbox_id);

        // Check if the position is not zero (error condition from getHitboxWorldPosition)
        if (!(bone_pos.x == 0.f && bone_pos.y == 0.f && bone_pos.z == 0.f)) {
            skeleton.bones[skeleton.num_bones].id = current_hitbox_id;
            skeleton.bones[skeleton.num_bones].position = bone_pos;
            skeleton.num_bones++;
        }
    }

    if (skeleton.num_bones > 0) {
        skeleton.valid = true;
    }

    return skeleton;
}

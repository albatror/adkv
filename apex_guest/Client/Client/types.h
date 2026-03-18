// types.h
#ifndef TYPES_H
#define TYPES_H

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
	float bones[15][2] = { 0 };
}player;

typedef struct spectator {
	bool is_spec = false;
	char name[33] = { 0 };
}spectator;

typedef struct item_esp {
	float x = 0;
	float y = 0;
	float dist = 0;
	int category = 0;
	char name[33] = { 0 };
}item_esp;

#endif // TYPES_H
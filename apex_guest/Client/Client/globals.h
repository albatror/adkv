#ifndef GLOBALS_H
#define GLOBALS_H

#include "../shared_structs.h" // Path relative to Client/Client/ where globals.h is

extern bool k_del;

// Global variables for skeleton ESP data
extern SkeletonESPDataPacket g_latest_skeleton_data;
extern bool g_new_skeleton_data_received;

// ESP feature toggles
extern bool v_skeleton_esp;

#endif // GLOBALS_H
#pragma once

#include "Game.h"
#include <vector>

void StuffBotLoop();
Vector GetBoxDimensionsForBone(int boneIndex);
std::vector<Vector> CalculateBoxCorners(Vector bonePos, Vector dimensions);

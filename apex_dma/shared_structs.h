#pragma once

#include <cmath> // For sqrt, etc. in Vector3 operations later if needed

#define MAX_BONES 30

struct Vector2 {
    float x, y;

    // Default constructor
    Vector2() : x(0.f), y(0.f) {}

    // Parameterized constructor
    Vector2(float x_val, float y_val) : x(x_val), y(y_val) {}
};

struct Vector3 {
    float x, y, z;

    // Default constructor
    Vector3() : x(0.f), y(0.f), z(0.f) {}

    // Parameterized constructor
    Vector3(float x_val, float y_val, float z_val) : x(x_val), y(y_val), z(z_val) {}

    // Addition operator
    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    // Subtraction operator
    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    // Multiplication by scalar
    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    // Division by scalar
    Vector3 operator/(float scalar) const {
        // Consider adding a check for division by zero if necessary
        return Vector3(x / scalar, y / scalar, z / scalar);
    }

    // Dot product
    float dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    // Cross product
    Vector3 cross(const Vector3& other) const {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    // Magnitude (length)
    float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    // Normalize
    Vector3 normalized() const {
        float l = length();
        if (l > 0) { // Avoid division by zero
            return Vector3(x / l, y / l, z / l);
        }
        return Vector3(); // Return zero vector if length is zero
    }
};

struct Matrix3x4 {
    float m[3][4];

    // Default constructor (initialize to identity or zero matrix if preferred)
    Matrix3x4() {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 4; ++j) {
                m[i][j] = 0.f;
            }
        }
    }

    // Access to translational components
    float get_translation_x() const { return m[0][3]; }
    float get_translation_y() const { return m[1][3]; }
    float get_translation_z() const { return m[2][3]; }

    void set_translation_x(float val) { m[0][3] = val; }
    void set_translation_y(float val) { m[1][3] = val; }
    void set_translation_z(float val) { m[2][3] = val; }
    
    Vector3 get_translation() const {
        return Vector3(m[0][3], m[1][3], m[2][3]);
    }
};

struct SkeletonBone {
    int id; // Bone ID
    Vector3 position; // Bone position in model space or world space
    // Matrix3x4 transform; // Or perhaps a full transform matrix

    SkeletonBone() : id(-1), position() {}
    SkeletonBone(int bone_id, Vector3 pos) : id(bone_id), position(pos) {}
};

struct PlayerSkeleton {
    SkeletonBone bones[MAX_BONES];
    int num_bones; // Number of currently valid bones in the array
    bool valid;    // Indicates if this skeleton data is considered valid

    PlayerSkeleton() : num_bones(0), valid(false) {
        // Initialize bones if necessary, e.g., set all IDs to -1
        // The SkeletonBone constructor already sets id to -1 and position to (0,0,0)
        // So, no explicit loop needed here unless a different default is required.
    }
};

// Maximum number of skeletons to include in a single ESP data packet
#define MAX_SKELETONS_IN_PACKET 10

// Struct for sending skeleton ESP data to a client
struct SkeletonESPDataPacket {
    Matrix3x4 view_matrix;      // Game's current view matrix (first 3x4 part)
    int num_skeletons;          // Number of skeletons actually populated in the array below
    PlayerSkeleton skeletons[MAX_SKELETONS_IN_PACKET]; // Array of player skeletons

    SkeletonESPDataPacket() : num_skeletons(0) {
        // view_matrix will be default constructed (all zeros)
        // skeletons array elements will be default constructed
    }
};

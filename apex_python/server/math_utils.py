import math
import glm

class Vector:
    def __init__(self, x=0.0, y=0.0, z=0.0):
        self.x = float(x)
        self.y = float(y)
        self.z = float(z)

    def __add__(self, other):
        return Vector(self.x + other.x, self.y + other.y, self.z + other.z)

    def __sub__(self, other):
        return Vector(self.x - other.x, self.y - other.y, self.z - other.z)

    def __mul__(self, scalar):
        return Vector(self.x * scalar, self.y * scalar, self.z * scalar)

    def __truediv__(self, scalar):
        return Vector(self.x / scalar, self.y / scalar, self.z / scalar)

    def length(self):
        return math.sqrt(self.x**2 + self.y**2 + self.z**2)

    def dist_to(self, other):
        return (self - other).length()

    def __repr__(self):
        return f"Vector({self.x}, {self.y}, {self.z})"

class QAngle:
    def __init__(self, x=0.0, y=0.0, z=0.0):
        self.x = float(x)
        self.y = float(y)
        self.z = float(z)

    def __sub__(self, other):
        return QAngle(self.x - other.x, self.y - other.y, self.z - other.z)

    def __add__(self, other):
        return QAngle(self.x + other.x, self.y + other.y, self.z + other.z)

    def __mul__(self, scalar):
        return QAngle(self.x * scalar, self.y * scalar, self.z * scalar)

    def __truediv__(self, scalar):
        return QAngle(self.x / scalar, self.y / scalar, self.z / scalar)

    def normalize(self):
        while self.x > 89.0: self.x -= 180.0
        while self.x < -89.0: self.x += 180.0
        while self.y > 180.0: self.y -= 360.0
        while self.y < -180.0: self.y += 360.0

    def __repr__(self):
        return f"QAngle({self.x}, {self.y}, {self.z})"

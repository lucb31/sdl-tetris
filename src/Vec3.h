//
// Created by lucas on 25.05.25.
//

#ifndef VEC3_H
#define VEC3_H
#include <cmath>

namespace Math {
    class Vec3 {
    public:
        float e[3];

        Vec3() : e{0.0f, 0.0f} {
        }

        Vec3(float e0, float e1, float e2) : e{e0, e1, e2} {
        }

        float x() const { return e[0]; }
        float y() const { return e[1]; }
        float z() const { return e[2]; }

        Vec3 Norm() const {
            const float length = Length();
            // Avoid division by zero
            if (length == 0) return {};
            return {e[0] / length, e[1] / length, e[2] / length};
        }

        float Length() const {
            const float lsq = LengthSquared();
            // Rounding errors: If very small, return 0
            if (lsq <= 1e9) return 0.0f;
            return std::sqrt(lsq);
        }

        float LengthSquared() const {
            return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
        }

        Vec3 &operator +=(const Vec3 &rhs) {
            e[0] += rhs.e[0];
            e[1] += rhs.e[1];
            e[2] += rhs.e[2];
            return *this;
        }

        Vec3 &operator *=(float d) {
            e[0] *= d;
            e[1] *= d;
            e[2] *= d;
            return *this;
        }
    };

    inline Vec3 operator *(const Vec3 &rhs, const Vec3 &lhs) {
        // TODO: Revisit. Shouldnt this be dot product?
        return Vec3(rhs.e[0] * lhs.e[0], rhs.e[1] * lhs.e[1], rhs.e[2] * lhs.e[2]);
    }

    inline Vec3 operator *(const Vec3 &rhs, float x) {
        return Vec3(rhs.e[0] * x, rhs.e[1] * x, rhs.e[2] * x);
    }

    inline Vec3 operator +(const Vec3 &rhs, const Vec3 &lhs) {
        return Vec3(rhs.e[0] + lhs.e[0], rhs.e[1] + lhs.e[1], rhs.e[2] + lhs.e[2]);
    }
}

#endif //VEC3_H

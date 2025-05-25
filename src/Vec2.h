//
// Created by lucas on 23.05.25.
//

#ifndef VEC2_H
#define VEC2_H
#include <cmath>

namespace Math {

class Vec2 {
public:
    float e[2];
    Vec2() : e{0.0f, 0.0f} {}
    Vec2(float e0, float e1) : e{e0, e1} {}

    float x() const { return e[0]; }
    float y() const { return e[1]; }
    float z() const { return e[2]; }

    Vec2 Norm() const {
        const float length = Length();
        // Avoid division by zero
        if (length == 0) return {};
        return {e[0] / length, e[1] / length};
    }

    float Length() const {
        if (e[0] == 0.0f && e[1] == 0.0f) return 0.0f;
        return std::sqrt(e[0] * e[0] + e[1] * e[1]);
    }

    Vec2& operator +=(const Vec2& rhs) {
        e[0] += rhs.e[0];
        e[1] += rhs.e[1];
        return *this;
    }

    Vec2& operator *=(float d) {
        e[0] *= d;
        e[1] *= d;
        return *this;
    }
};

   inline Vec2 operator *(const Vec2& rhs, const Vec2& lhs) {
       return Vec2(rhs.e[0]*lhs.e[0], rhs.e[1]*lhs.e[1]);
   }
    inline Vec2 operator *(const Vec2& rhs, float x) {
       return Vec2(rhs.e[0]*x, rhs.e[1]*x);
   }
    inline Vec2 operator +(const Vec2& rhs, const Vec2& lhs) {
       return Vec2(rhs.e[0]+lhs.e[0], rhs.e[1]+lhs.e[1]);
   }
} // Math

#endif //VEC2_H

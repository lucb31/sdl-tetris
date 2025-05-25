//
// Created by lucas on 25.05.25.
//

#ifndef MAT3_H
#define MAT3_H
#include <cmath>

#include "Vec3.h"

namespace Math {
    class Vec3;
    /**
     * Example in Column-major order
     * Mat
     * {
     *    1, 4, 7,
     *    2, 5, 8,
     *    3, 6, 9
     * }
     */
    class Mat3 {
    public:
        float e[9];

        Mat3() : e{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f} {
        }

        Mat3(
            float e00, float e01, float e02,
            float e10, float e11, float e12,
            float e20, float e21, float e22
        ) : e{e00, e01, e02, e10, e11, e12, e20, e21, e22} {
        }

    };

    inline Mat3 operator *(const Mat3& lhs, const Mat3& rhs) {
        Mat3 result;
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 3; j++) {
                const int lhs_idx = (i % 3) + j * 3;
                const int rhs_idx = std::floor(i / 3) * 3 + j;
                result.e[i] += lhs.e[lhs_idx] * rhs.e[rhs_idx];
            }
        }
        return result;
    }

    inline Vec3 operator *(const Mat3& lhs, const Vec3& rhs) {
        Vec3 result;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                const int lhs_idx = i % 3 + j * 3;
                result.e[i] += lhs.e[lhs_idx] * rhs.e[j];
            }
        }
        return result;
    }

} // Math

#endif //MAT3_H

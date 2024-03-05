#ifndef ENGINE_GRAPHICS_DATATYPES_H_
#define ENGINE_GRAPHICS_DATATYPES_H_

enum class TextureType {
    //NOTE: Hard-coded values in GLSL
    DIFFUSE=0,
    AMBIENT=1,
    SPECULAR=2,
    NORMAL=3
};

enum class TextureFilter {
    NEAREST=0,
    LINEAR=1,
    MIPMAP=2
};

enum class TextureWrap {
    REPEAT=0,
    MIRRORED_REPEAT=1,
    CLAMP_TO_EDGE=2,
    CLAMP_TO_BORDER=3
};

enum class TextureFormat {
    RGBA=0,
    RGB=1,
    DEPTH_COMPONENT=2,
    RED=3
};

enum class TextureTargetType {
    TEXTURE_2D=0,
    TEXTURE_1D_ARRAY=1
};

enum class TextureDataType {
    UNSIGNED_BYTE=0,
    FLOAT=1
};


struct Color {
    Color(double r, double g, double b, double a) {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    Color() : Color(1.0, 1.0, 1.0, 1.0) {

    }

    double r;
    double g;
    double b;
    double a;
};

struct Vector3 {
    Vector3(double x, double y, double z);
    Vector3();
    Vector3(const Vector3& v);

    double length() const;

    Vector3 normalize() const;
    Vector3 operator+(const Vector3& v) const;
    Vector3 operator-(const Vector3& v) const;
    Vector3 operator/(const Vector3& v) const;
    Vector3 operator*(const Vector3& v) const;
    Vector3 operator/(double v) const;
    Vector3 operator*(double v) const;
    void operator=(const Vector3& v);

    double x;
    double y;
    double z;
};

#endif /*ENGINE_GRAPHICS_DATATYPES_H_*/

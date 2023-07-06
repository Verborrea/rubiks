// ==================================== //
//                                      //
//      GLM-like matrix library by:     //
//                                      //
//  * Alan Monroy Bernedo               //
//  * Renzo Estefanero Chavez           //
//  * Sharon Valdivia Begazo            //
//                                      //
// ==================================== //

#ifndef MATRIX_H
#define MATRIX_H

#include <cmath> // for M_PI = 3.14.. and sin(), cos()
#include <iostream>

// ================================= VECTORES =================================
struct vec3
{
    float data[3];
    vec3();
    vec3(float, float, float);
    float &operator[](int i) {return data[i];}
    float operator[](int i) const {return data[i];}
    vec3 operator+(const vec3& vec);
    vec3 operator+=(const vec3& vec);
    vec3 operator-(const vec3& vec);
    vec3 operator-=(const vec3& vec);
    vec3 operator*(const float& scalar);
    vec3 operator*=(const float& scalar);
    bool operator==(const vec3& vec);
};

vec3::vec3()
{
    data[0] = data[1] = data[2] = 0;
}

vec3::vec3(float x, float y, float z)
{
    data[0] = x;
    data[1] = y;
    data[2] = z;
}

vec3 vec3::operator+(const vec3& vec) {
    vec3 res;
    res[0] = (*this)[0] + vec[0];
    res[1] = (*this)[1] + vec[1];
    res[2] = (*this)[2] + vec[2];
    return res;
}

vec3 vec3::operator+=(const vec3& vec) {
    (*this)[0] += vec[0];
    (*this)[1] += vec[1];
    (*this)[2] += vec[2];
    return *this;
}

vec3 vec3::operator-(const vec3& vec) {
    vec3 res;
    res[0] = (*this)[0] - vec[0];
    res[1] = (*this)[1] - vec[1];
    res[2] = (*this)[2] - vec[2];
    return res;
}

vec3 vec3::operator-=(const vec3& vec) {
    (*this)[0] -= vec[0];
    (*this)[1] -= vec[1];
    (*this)[2] -= vec[2];
    return *this;
}

vec3 vec3::operator*(const float& scalar) {
    vec3 res;
    res[0] = (*this)[0] * scalar;
    res[1] = (*this)[1] * scalar;
    res[2] = (*this)[2] * scalar;
    return res;
}

vec3 vec3::operator*=(const float& scalar) {
    (*this)[0] *= scalar;
    (*this)[1] *= scalar;
    (*this)[2] *= scalar;
    return *this;
}

bool vec3::operator==(const vec3& vec) {
    if ((*this)[0] != vec[0]) return false;
    if ((*this)[1] != vec[1]) return false;
    if ((*this)[2] != vec[2]) return false;
    return true;
}

struct vec4
{
    float data[4];
    vec4();
    vec4(vec3, float);
    vec4(float, float, float, float);
    float &operator[](int i) {return data[i];}
    float operator[](int i) const {return data[i];}
};

vec4::vec4()
{
    data[0] = data[1] = data[2] = data[3] = 0;
}

vec4::vec4(float x, float y, float z, float w)
{
    data[0] = x;
    data[1] = y;
    data[2] = z;
    data[3] = w;
}

vec4::vec4(vec3 vec, float w)
{
    data[0] = vec[0];
    data[1] = vec[1];
    data[2] = vec[2];
    data[3] = w;
}

// ================================= MATRICES =================================
class mat4
{
private:
    float** matrix;
    void set0();
    void setN(float n);
public:
    mat4();
    ~mat4();

    mat4(float identidad);
    mat4(const mat4 &mat);
    mat4 &operator=(const mat4 &mat);

    mat4 operator*(const mat4& mat2);
    vec4 operator*(const vec4& vec2);

    float* &operator[](int i) { return matrix[i]; }
    float* operator[](int i) const { return matrix[i]; }
    float* value_ptr() { return &(matrix[0][0]); }
    float* value_ptr() const{ return &(matrix[0][0]); }
};

mat4::mat4()
{
    matrix = new float*[4];
    matrix[0] = new float[16];
    for (int i = 1; i < 4; ++i)
        matrix[i] = matrix[0] + i * 4;
    set0();
}

mat4::mat4(const mat4 &mat)
{
    matrix = new float*[4];
    matrix[0] = new float[16];
    for (int i = 1; i < 4; ++i)
        matrix[i] = matrix[0] + i * 4;
    
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            matrix[i][j] = mat[i][j];
}

mat4::mat4(float identidad)
{
    matrix = new float*[4];
    matrix[0] = new float[16];
    for (int i = 1; i < 4; ++i)
        matrix[i] = matrix[0] + i * 4;
    setN(identidad);
}

mat4& mat4::operator=(const mat4 &mat)
{
    matrix = new float*[4];
    matrix[0] = new float[16];
    for (int i = 1; i < 4; ++i)
        matrix[i] = matrix[0] + i * 4;
    
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            matrix[i][j] = mat[i][j];
    return *this;
}

mat4::~mat4()
{
    delete [] matrix[0];
    delete [] matrix;
}

void mat4::set0()
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            matrix[i][j] = 0;
}

void mat4::setN(float n)
{
    set0();
    for (int i = 0; i < 4; i++)
            matrix[i][i] = n;
}

mat4 mat4::operator*(const mat4& mat2){
    mat4 mat;
    for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++)
				mat[i][j] += matrix[i][k] * mat2[k][j];
    return mat;
}

vec4 mat4::operator*(const vec4& vec2){
    vec4 vec;
    for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			vec[i] += matrix[i][j] * vec2[j];
    return vec;
}

// ================================ FUNCIONES =================================
float radians(float deg)
{
    return deg / 180.0f * M_PI;
}

vec3 normalize(vec3 vec)
{
    float norma = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
    if (norma == 0.0f) return vec3(0,0,0);
    return vec3(vec[0]/norma, vec[1]/norma, vec[2]/norma);
}

vec3 cross(vec3 a, vec3 b)
{
    return vec3(
        a[1] * b[2] - a[2] * b[1],
        a[2] * b[0] - a[0] * b[2],
        a[0] * b[1] - a[1] * b[0]
    );
}

mat4 lookAt(vec3 pos, vec3 C, vec3 up)
{
    vec3 w = normalize(pos - C);
    vec3 u = normalize(cross(normalize(up), w));
    vec3 v = cross(w, u);

    mat4 translation = mat4(1.0f);
    translation[0][3] = -pos[0];
    translation[1][3] = -pos[1];
    translation[2][3] = -pos[2];

    mat4 rotation = mat4(1.0f);
    rotation[0][0] = u[0];
    rotation[0][1] = u[1];
    rotation[0][2] = u[2];
    rotation[1][0] = v[0];
    rotation[1][1] = v[1];
    rotation[1][2] = v[2];
    rotation[2][0] = w[0];
    rotation[2][1] = w[1];
    rotation[2][2] = w[2]; 

    return rotation * translation;
}

mat4 translate(mat4 mat, vec3 vec)
{
    mat4 tra = mat4(1.0f);
    tra[0][3] = vec[0];
    tra[1][3] = vec[1];
    tra[2][3] = vec[2];
    return mat * tra;
}

mat4 i_translate(mat4 mat, vec3 vec)
{
    mat4 tra = mat4(1.0f);
    tra[0][3] = -vec[0];
    tra[1][3] = -vec[1];
    tra[2][3] = -vec[2];
    return mat * tra;
}

mat4 scale(mat4 mat, vec3 vec)
{
    mat4 sca = mat4(1.0f);
    sca[0][0] = vec[0];
    sca[1][1] = vec[1];
    sca[2][2] = vec[2];
    return mat * sca;
}

mat4 rotate(mat4 mat, float a, vec3 vec)
{
    mat4 rot = mat4(1.0f);

    vec = normalize(vec);

    rot[0][0] = cos(a) + vec[0]*vec[0]*(1.0f-cos(a));
    rot[1][1] = cos(a) + vec[1]*vec[1]*(1.0f-cos(a));
    rot[2][2] = cos(a) + vec[2]*vec[2]*(1.0f-cos(a));

    rot[0][1] = vec[0]*vec[1]*(1.0f-cos(a)) - vec[2]*sin(a);
    rot[1][0] = vec[0]*vec[1]*(1.0f-cos(a)) + vec[2]*sin(a);
    
    rot[0][2] = vec[0]*vec[2]*(1.0f-cos(a)) + vec[1]*sin(a);
    rot[2][0] = vec[0]*vec[2]*(1.0f-cos(a)) - vec[1]*sin(a);

    rot[1][2] = vec[2]*vec[1]*(1.0f-cos(a)) - vec[0]*sin(a);
    rot[2][1] = vec[2]*vec[1]*(1.0f-cos(a)) + vec[0]*sin(a);

    return mat * rot;
}

mat4 perspective(float fov, float aspect, float n, float f)
{
    mat4 per;

    float d = cos(fov/2.0f) / sin(fov/2.0f);  // cot(x) = cos(x)/sin(x)

    per[0][0] = d/aspect;
    per[1][1] = d;
    per[3][2] = -1;
    per[3][3] = 0;
    per[2][2] = -(f+n)/(f-n);
    per[2][3] = -(2.0f*f*n)/(f-n);
    
    return per;
}

#endif  // MATRIX_H
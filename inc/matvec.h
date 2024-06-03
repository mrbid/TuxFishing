#ifndef MATVEC_H
#define MATVEC_H

/*
    James William Fletcher (github.com/mrbid)
        September 2021 - June 2024

    Portable floating-point Vec4 lib.

    (Don't rely on this, copy and paste functions from this or write your own
    this is just a support lib, relying on it too much will make you dumb.)

    This is free and unencumbered software released into the public domain.

    Anyone is free to copy, modify, publish, use, compile, sell, or
    distribute this software, either in source code form or as a compiled
    binary, for any purpose, commercial or non-commercial, and by any
    means.

    In jurisdictions that recognize copyright laws, the author or authors
    of this software dedicate any and all copyright interest in the
    software to the public domain. We make this dedication for the benefit
    of the public at large and to the detriment of our heirs and
    successors. We intend this dedication to be an overt act of
    relinquishment in perpetuity of all present and future rights to this
    software under copyright law.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
    OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.

    For more information, please refer to <https://unlicense.org>
*/

#include <math.h>   // sqrtf logf fabsf cosf sinf
#include <string.h> // memset memcpy

#define PI 3.141592741f         // PI
#define x2PI 6.283185482f       // PI * 2
#define d2PI 1.570796371f       // PI / 2
#define DEGREE 57.29578018f     // 1 Radian as Degrees
#define RADIAN 0.01745329238f   // PI / 180 (1 Degree as Radians)
#define RAD2DEG DEGREE
#define DEG2RAD RADIAN
#define FLOAT_MAX 9223372036854775807.f

typedef struct{float x,y,z,w;} vec;

static inline float randf();  // uniform [0 to 1]
static inline float randfc(); // uniform [-1 to 1]
float randfn(); // box-muller normal [bi-directional]

static inline float fRandFloat(const float min, const float max);
static inline int   fRand(const float min, const float max);

int vec_ftoi(float f); // float to integer quantise

// normalising the result is optional / at the callers responsibility
void vRuv(vec* v);   // Random Unit Vector
void vRuvN(vec* v);  // Normal Random Unit Vector
void vRuvBT(vec* v); // Brian Tung Random Unit Vector (on surface of unit sphere)
void vRuvTA(vec* v); // T.Davison Trial & Error (inside unit sphere)
void vRuvTD(vec* v); // T.Davison Random Unit Vector Sphere

void  vCross(vec* r, const vec v1, const vec v2);
float vDot(const vec v1, const vec v2);
float vSum(const vec v);
float vSumAbs(const vec v);
void  vReflect(vec* r, const vec v, const vec n);

int  vEqualTol(const vec a, const vec b, const float tol);
int  vEqualInt(const vec a, const vec b);
void vMin(vec* r, const vec v1, const vec v2);
void vMax(vec* r, const vec v1, const vec v2);

void  vNorm(vec* v);
float vDist(const vec v1, const vec v2);
float vDistSq(const vec a, const vec b);
float vDistMh(const vec a, const vec b); // manhattan
float vDistLa(const vec a, const vec b); // longest axis
float vMod(const vec v); // modulus
float vMag(const vec v); // magnitude
void  vInv(vec* v); // invert
void  vCopy(vec* r, const vec v);
void  vDir(vec* r, const vec v1, const vec v2); // direction vector from v1 to v2

void vRotX(vec* v, const float radians);
void vRotY(vec* v, const float radians);
void vRotZ(vec* v, const float radians);

void vAdd(vec* r, const vec v1, const vec v2);
void vSub(vec* r, const vec v1, const vec v2);
void vDiv(vec* r, const vec numerator, const vec denominator);
void vMul(vec* r, const vec v1, const vec v2);

void vAddS(vec* r, const vec v1, const float v2);
void vSubS(vec* r, const vec v1, const float v2);
void vDivS(vec* r, const vec v1, const float v2);
void vMulS(vec* r, const vec v1, const float v2);

//

int srandfq = 74235;
static inline void srandf(const int seed){srandfq = seed;}
static inline float randf()
{
    // https://www.musicdsp.org/en/latest/Other/273-fast-float-random-numbers.html (moc.liamg@seir.kinimod)
    srandfq *= 16807;
    return (float)(srandfq & 0x7FFFFFFF) * 4.6566129e-010f;
}
static inline float randfc()
{
    // https://www.musicdsp.org/en/latest/Other/273-fast-float-random-numbers.html (moc.liamg@seir.kinimod)
    srandfq *= 16807;
    return ((float)(srandfq)) * 4.6566129e-010f;
}
static inline float fRandFloat(const float min, const float max)
{
    return min + randf() * (max-min); 
}
static inline int fRand(const float min, const float max)
{
    return (int)((min + randf() * (max-min))+0.5f); 
}

float randfn()
{
    float u = randfc();
    float v = randfc();
    float r = u * u + v * v;
    while(r == 0.f || r > 1.f)
    {
        u = randfc();
        v = randfc();
        r = u * u + v * v;
    }
    return u * sqrtf(-2.f * logf(r) / r);
}

void vRuv(vec* v)
{
    v->x = randfc();
    v->y = randfc();
    v->z = randfc();
}

void vRuvN(vec* v)
{
    v->x = randfn();
    v->y = randfn();
    v->z = randfn();
}

void vRuvBT(vec* v)
{
    // https://math.stackexchange.com/a/1586185
    // or should I have called this vRuvLR()
    // https://mathworld.wolfram.com/SpherePointPicking.html
    const float y = acosf(randfc()) - d2PI;
    const float p = x2PI * randf();
    v->x = cosf(y) * cosf(p);
    v->y = cosf(y) * sinf(p);
    v->z = sinf(y);
}

void vRuvTA(vec* v)
{
    // T.P.Davison@tees.ac.uk
    while(1)
    {
        v->x = randfc();
        v->y = randfc();
        v->z = randfc();
        const float len = vMag(*v);
        if(len <= 1.0f){return;}
    }
}

void vRuvTD(vec* v)
{
    // T.P.Davison@tees.ac.uk
    v->x = sinf((randf() * x2PI) - PI);
    v->y = cosf((randf() * x2PI) - PI);
    v->z = randfc();
}

void vCross(vec* r, const vec v1, const vec v2)
{
    r->x = (v1.y * v2.z) - (v2.y * v1.z);
    r->y = -((v1.x * v2.z) - (v2.x * v1.z));
    r->z = (v1.x * v2.y) - (v2.x * v1.y);
}

float vDot(const vec v1, const vec v2)
{
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

float vSum(const vec v)
{
    return v.x + v.y + v.z;
}

float vSumAbs(const vec v)
{
    return fabs(v.x) + fabs(v.y) + fabs(v.z);
}

void vInv(vec* v)
{
    v->x = -v->x;
    v->y = -v->y;
    v->z = -v->z;
}

void vNorm(vec* v)
{
    const float len = 1.f/sqrtf(v->x*v->x + v->y*v->y + v->z*v->z);
    v->x *= len;
    v->y *= len;
    v->z *= len;
}

float vDist(const vec v1, const vec v2)
{
    const float xm = (v1.x - v2.x);
    const float ym = (v1.y - v2.y);
    const float zm = (v1.z - v2.z);
    return sqrtf(xm*xm + ym*ym + zm*zm);
}

float vDistSq(const vec a, const vec b)
{
    const float xm = (a.x - b.x);
    const float ym = (a.y - b.y);
    const float zm = (a.z - b.z);
    return xm*xm + ym*ym + zm*zm;
}

float vDistMh(const vec a, const vec b)
{
    return (a.x - b.x) + (a.y - b.y) + (a.z - b.z);
}

float vDistLa(const vec v1, const vec v2)
{
    const float xm = fabsf(v1.x - v2.x);
    const float ym = fabsf(v1.y - v2.y);
    const float zm = fabsf(v1.z - v2.z);

    float dist = xm;
    if(ym > dist)
        dist = ym;
    if(zm > dist)
        dist = zm;

    return dist;
}

void vReflect(vec* r, const vec v, const vec n)
{
    const float angle = vDot(v, n);
    r->x = v.x - (2.f * n.x) * angle;
    r->y = v.y - (2.f * n.y) * angle;
    r->z = v.z - (2.f * n.z) * angle;
}

int vEqualTol(const vec a, const vec b, const float tol)
{
    return  a.x >= b.x - tol && a.x <= b.x + tol &&
            a.y >= b.y - tol && a.y <= b.y + tol &&
            a.z >= b.z - tol && a.z <= b.z + tol;
}

void vMin(vec* r, const vec v1, const vec v2)
{
    if(v1.x < v2.x && v1.y < v2.y && v1.z < v2.z)
    {
        r->x = v1.x;
        r->y = v1.y;
        r->z = v1.z;
    }

    r->x = v2.x;
    r->y = v2.y;
    r->z = v2.z;
}

void vMax(vec* r, const vec v1, const vec v2)
{
    if(v1.x > v2.x && v1.y > v2.y && v1.z > v2.z)
    {
        r->x = v1.x;
        r->y = v1.y;
        r->z = v1.z;
    }

    r->x = v2.x;
    r->y = v2.y;
    r->z = v2.z;
}

int vec_ftoi(float f)
{
    if(f < 0.f)
        f -= 0.5f;
    else
        f += 0.5f;
    return (int)f;
}

int vEqualInt(const vec a, const vec b)
{
    return vec_ftoi(a.x) == vec_ftoi(b.x) && vec_ftoi(a.y) == vec_ftoi(b.y) && vec_ftoi(a.z) == vec_ftoi(b.z);
}

float vMod(const vec v)
{
    return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}

float vMag(const vec v)
{
    return v.x*v.x + v.y*v.y + v.z*v.z;
}

void vCopy(vec* r, const vec v)
{
    memcpy(r, &v, sizeof(vec));
}

void vDir(vec* r, const vec v1, const vec v2)
{
    vSub(r, v2, v1);
    vNorm(r);
}

void vRotX(vec* v, const float radians)
{
    v->y = v->y * cosf(radians) + v->z * sinf(radians);
    v->z = v->y * sinf(radians) - v->z * cosf(radians);
}

void vRotY(vec* v, const float radians)
{
    v->x = v->z * sinf(radians) - v->x * cosf(radians);
    v->z = v->z * cosf(radians) + v->x * sinf(radians);
}

void vRotZ(vec* v, const float radians)
{
    v->x = v->x * cosf(radians) + v->y * sinf(radians);
    v->y = v->x * sinf(radians) - v->y * cosf(radians);
}

void vAdd(vec* r, const vec v1, const vec v2)
{
    r->x = v1.x + v2.x;
    r->y = v1.y + v2.y;
    r->z = v1.z + v2.z;
}

void vSub(vec* r, const vec v1, const vec v2)
{
    r->x = v1.x - v2.x;
    r->y = v1.y - v2.y;
    r->z = v1.z - v2.z;
}

void vDiv(vec* r, const vec numerator, const vec denominator)
{
    r->x = numerator.x / denominator.x;
    r->y = numerator.y / denominator.y;
    r->z = numerator.z / denominator.z;
}

void vMul(vec* r, const vec v1, const vec v2)
{
    r->x = v1.x * v2.x;
    r->y = v1.y * v2.y;
    r->z = v1.z * v2.z;
}

void vAddS(vec* r, const vec v1, const float v2)
{
    r->x = v1.x + v2;
    r->y = v1.y + v2;
    r->z = v1.z + v2;
}

void vSubS(vec* r, const vec v1, const float v2)
{
    r->x = v1.x - v2;
    r->y = v1.y - v2;
    r->z = v1.z - v2;
}

void vDivS(vec* r, const vec v1, const float v2)
{
    r->x = v1.x / v2;
    r->y = v1.y / v2;
    r->z = v1.z / v2;
}

void vMulS(vec* r, const vec v1, const float v2)
{
    r->x = v1.x * v2;
    r->y = v1.y * v2;
    r->z = v1.z * v2;
}

/*
    Updated: 2022
    Credit: Aaftab Munshi, Dan Ginsburg, Dave Shreiner, James William Fletcher, Intel, Gabriel Cramer, Test_User
*/

typedef struct {float m[4][4];} mat;

void mIdent(mat *m);
void mCopy(mat *r, const mat *v);
void mMul(mat *r, const mat *a, const mat *b);
void mMulP(vec *r, const mat *a, const float x, const float y, const float z);
void mMulV(vec *r, const mat *a, const vec v);
void mScale(mat *r, const float x, const float y, const float z);
void mScale1(mat *r, const float s);
void mTranslate(mat *r, const float x, const float y, const float z);
void mRotate(mat *r, const float radians, float x, float y, float z); // rotate axis (rotate X to move Y up and down)
void mRotX(mat *r, const float radians); // rotate on axis
void mRotY(mat *r, const float radians); // rotate on axis
void mRotZ(mat *r, const float radians); // rotate on axis
void mAngleAxisRotate(mat *r, const mat view, const float xrot, const float yrot, const float zrot); // gimbal free rotations
void mFrustum(mat *r, const float left, const float right, const float bottom, const float top, const float nearZ, const float farZ);
void mPerspective(mat *r, const float fovy, const float aspect, const float nearZ, const float farZ);
void mOrtho(mat *r, const float left, const float right, const float bottom, const float top, const float nearZ, const float farZ);
void mLookAt(mat *r, const vec origin, const vec unit_dir);
void mInvert(float *dst, const float *mat);
void mTranspose(mat *r, const mat *m);
void mSetViewDir(mat *r, const vec dir_norm);
void mGetViewDir(vec *r, const mat matrix);
void mGetViewX(vec *r, const mat matrix);
void mGetViewY(vec *r, const mat matrix);
void mGetViewZ(vec *r, const mat matrix);
void mSetDir(mat *r, const vec dir_norm);
void mGetDirX(vec *r, const mat matrix);
void mGetDirY(vec *r, const mat matrix);
void mGetDirZ(vec *r, const mat matrix);
void mGetPos(vec *r, const mat matrix);
void mSetPos(mat *r, const vec pos);
void mDump(const mat matrix);

//

void mIdent(mat *m)
{
    memset(m, 0x0, sizeof(mat));
    m->m[0][0] = 1.0f;
    m->m[1][1] = 1.0f;
    m->m[2][2] = 1.0f;
    m->m[3][3] = 1.0f;
}

void mCopy(mat *r, const mat *v)
{
    memcpy(r, v, sizeof(mat));
}

void mMul(mat *r, const mat *a, const mat *b)
{
    mat tmp;
    for(int i = 0; i < 4; i++)
    {
        tmp.m[i][0] =	(a->m[i][0] * b->m[0][0]) +
                        (a->m[i][1] * b->m[1][0]) +
                        (a->m[i][2] * b->m[2][0]) +
                        (a->m[i][3] * b->m[3][0]) ;

        tmp.m[i][1] =	(a->m[i][0] * b->m[0][1]) + 
                        (a->m[i][1] * b->m[1][1]) +
                        (a->m[i][2] * b->m[2][1]) +
                        (a->m[i][3] * b->m[3][1]) ;

        tmp.m[i][2] =	(a->m[i][0] * b->m[0][2]) + 
                        (a->m[i][1] * b->m[1][2]) +
                        (a->m[i][2] * b->m[2][2]) +
                        (a->m[i][3] * b->m[3][2]) ;

        tmp.m[i][3] =	(a->m[i][0] * b->m[0][3]) + 
                        (a->m[i][1] * b->m[1][3]) +
                        (a->m[i][2] * b->m[2][3]) +
                        (a->m[i][3] * b->m[3][3]) ;
    }
    memcpy(r, &tmp, sizeof(mat));
}

void mMulP(vec *r, const mat *a, const float x, const float y, const float z)
{
    r->x =  (a->m[0][0] * x) +
            (a->m[0][1] * x) +
            (a->m[0][2] * x) +
            (a->m[0][3] * x) ;

    r->y =  (a->m[1][0] * y) +
            (a->m[1][1] * y) +
            (a->m[1][2] * y) +
            (a->m[1][3] * y) ;

    r->z =  (a->m[2][0] * z) +
            (a->m[2][1] * z) +
            (a->m[2][2] * z) +
            (a->m[2][3] * z) ;
}

void mMulV(vec *r, const mat *a, const vec v)
{
    r->x =  (a->m[0][0] * v.x) +
            (a->m[0][1] * v.x) +
            (a->m[0][2] * v.x) +
            (a->m[0][3] * v.x) ;

    r->y =  (a->m[1][0] * v.y) +
            (a->m[1][1] * v.y) +
            (a->m[1][2] * v.y) +
            (a->m[1][3] * v.y) ;

    r->z =  (a->m[2][0] * v.z) +
            (a->m[2][1] * v.z) +
            (a->m[2][2] * v.z) +
            (a->m[2][3] * v.z) ;

    r->w =  (a->m[3][0] * v.w) +
            (a->m[3][1] * v.w) +
            (a->m[3][2] * v.w) +
            (a->m[3][3] * v.w) ;
}

void mScale(mat *r, const float x, const float y, const float z)
{
    r->m[0][0] *= x;
    r->m[0][1] *= x;
    r->m[0][2] *= x;
    r->m[0][3] *= x;

    r->m[1][0] *= y;
    r->m[1][1] *= y;
    r->m[1][2] *= y;
    r->m[1][3] *= y;

    r->m[2][0] *= z;
    r->m[2][1] *= z;
    r->m[2][2] *= z;
    r->m[2][3] *= z;
}
void mScale1(mat *r, const float s){mScale(r, s, s, s);}

void mTranslate(mat *r, const float x, const float y, const float z)
{
    r->m[3][0] += (r->m[0][0] * x + r->m[1][0] * y + r->m[2][0] * z);
    r->m[3][1] += (r->m[0][1] * x + r->m[1][1] * y + r->m[2][1] * z);
    r->m[3][2] += (r->m[0][2] * x + r->m[1][2] * y + r->m[2][2] * z);
    r->m[3][3] += (r->m[0][3] * x + r->m[1][3] * y + r->m[2][3] * z);
}

void mRotate(mat *r, const float radians, float x, float y, float z)
{
    // MIT: Dan Ginsburg, Budirijanto Purnomo, Dave Shreiner, Aaftab Munshi
    const float mag = 1.f/sqrtf(x * x + y * y + z * z);
    const float sinAngle = sinf(radians);
    const float cosAngle = cosf(radians);
    if(mag > 0.0f)
    {
        x *= mag;
        y *= mag;
        z *= mag;

        const float xx = x * x;
        const float yy = y * y;
        const float zz = z * z;
        const float xy = x * y;
        const float yz = y * z;
        const float zx = z * x;
        const float xs = x * sinAngle;
        const float ys = y * sinAngle;
        const float zs = z * sinAngle;
        const float oneMinusCos = 1.0f - cosAngle;

        mat rotMat;
        rotMat.m[0][0] = (oneMinusCos * xx) + cosAngle;
        rotMat.m[0][1] = (oneMinusCos * xy) - zs;
        rotMat.m[0][2] = (oneMinusCos * zx) + ys;
        rotMat.m[0][3] = 0.0F; 

        rotMat.m[1][0] = (oneMinusCos * xy) + zs;
        rotMat.m[1][1] = (oneMinusCos * yy) + cosAngle;
        rotMat.m[1][2] = (oneMinusCos * yz) - xs;
        rotMat.m[1][3] = 0.0F;

        rotMat.m[2][0] = (oneMinusCos * zx) - ys;
        rotMat.m[2][1] = (oneMinusCos * yz) + xs;
        rotMat.m[2][2] = (oneMinusCos * zz) + cosAngle;
        rotMat.m[2][3] = 0.0F; 

        rotMat.m[3][0] = 0.0F;
        rotMat.m[3][1] = 0.0F;
        rotMat.m[3][2] = 0.0F;
        rotMat.m[3][3] = 1.0F;

        mMul(r, &rotMat, r);
    }
}

void mRotX(mat *r, const float radians)
{
    const float s = sinf(radians);
    const float c = cosf(radians);
    const mat t = { c, 0.f, s, 0.f,
                    0.f, 1.f, 0.f, 0.f,
                    -s, 0.f, c, 0.f,
                    0.f, 0.f, 0.f, 1.f };
    mMul(r, &t, r);
}

void mRotY(mat *r, const float radians)
{
    const float s = sinf(radians);
    const float c = cosf(radians);
    const mat t = { 1.f, 0.f, 0.f, 0.f,
                    0.f, c, -s, 0.f,
                    0.f, s, c, 0.f,
                    0.f, 0.f, 0.f, 1.f };
    mMul(r, &t, r);
}

void mRotZ(mat *r, const float radians)
{
    const float s = sinf(radians);
    const float c = cosf(radians);
    const mat t = { c, -s, 0.f, 0.f,
                    s, c, 0.f, 0.f,
                    0.f, 0.f, 1.f, 0.f,
                    0.f, 0.f, 0.f, 1.f };
    mMul(r, &t, r);
}

void mAngleAxisRotate(mat *r, const mat view, const float xrot, const float yrot, const float zrot)
{
    // Test_User angle-axis rotation
    // https://en.wikipedia.org/wiki/Axis%E2%80%93angle_representation
    // https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
    // this is incrementing the rotation of the provided view matrix by the xrot,yrot,zrot
    vec tmp0, tmp1;

    vec vecview[3] = {
        {view.m[0][0], view.m[1][0], view.m[2][0]}, // r
        {view.m[0][1], view.m[1][1], view.m[2][1]}, // u
        {view.m[0][2], view.m[1][2], view.m[2][2]}  // f
    };

    // left/right
    vMulS(&tmp0, vecview[0], cosf(xrot));

    vCross(&tmp1, vecview[1], vecview[0]);
    vMulS(&tmp1, tmp1, sinf(xrot));

    vMulS(&vecview[0], vecview[1], vDot(vecview[1], vecview[0]) * (1.f - cosf(xrot)));

    vAdd(&vecview[0], vecview[0], tmp0);
    vAdd(&vecview[0], vecview[0], tmp1);

    // up/down
    vMulS(&tmp0, vecview[1], cosf(yrot));

    vCross(&tmp1, vecview[0], vecview[1]);
    vMulS(&tmp1, tmp1, sinf(yrot));

    vMulS(&vecview[1], vecview[0], vDot(vecview[0], vecview[1]) * (1.f - cosf(yrot)));

    vAdd(&vecview[1], vecview[1], tmp0);
    vAdd(&vecview[1], vecview[1], tmp1);

    vCross(&vecview[2], vecview[0], vecview[1]);
    vCross(&vecview[1], vecview[2], vecview[0]);

    // roll
    vMulS(&tmp0, vecview[0], cosf(zrot));

    vCross(&tmp1, vecview[2], vecview[0]);
    vMulS(&tmp1, tmp1, sinf(zrot));

    vMulS(&vecview[0], vecview[2], vDot(vecview[2], vecview[0]) * (1.f - cosf(zrot)));

    vAdd(&vecview[0], vecview[0], tmp0);
    vAdd(&vecview[0], vecview[0], tmp1);

    vCross(&vecview[1], vecview[2], vecview[0]);

    vNorm(&vecview[0]);
    vNorm(&vecview[1]);
    vNorm(&vecview[2]);

    *r = (mat){
        vecview[0].x, vecview[1].x, vecview[2].x, 0.f,
        vecview[0].y, vecview[1].y, vecview[2].y, 0.f,
        vecview[0].z, vecview[1].z, vecview[2].z, 0.f,
        0.f, 0.f, 0.f, 1.f
    };
}

void mFrustum(mat *r, const float left, const float right, const float bottom, const float top, const float nearZ, const float farZ)
{
    // MIT: Dan Ginsburg, Budirijanto Purnomo, Dave Shreiner, Aaftab Munshi
    const float dX = right - left;
    const float dY = top - bottom;
    const float dZ = farZ - nearZ;
    const float rdX = 1.f/dX;
    const float rdY = 1.f/dY;
    const float rdZ = 1.f/dZ;
    mat frust;

    if(nearZ <= 0.0f || farZ <= 0.0f || dX <= 0.0f || dY <= 0.0f || dZ <= 0.0f)
        return;

    frust.m[0][0] = 2.0f * nearZ * rdX;
    frust.m[0][1] = frust.m[0][2] = frust.m[0][3] = 0.0f;

    frust.m[1][1] = 2.0f * nearZ * rdY;
    frust.m[1][0] = frust.m[1][2] = frust.m[1][3] = 0.0f;

    frust.m[2][0] = (right + left) * rdX;
    frust.m[2][1] = (top + bottom) * rdY;
    frust.m[2][2] = -(nearZ + farZ) * rdZ;
    frust.m[2][3] = -1.0f;

    frust.m[3][2] = -2.0f * nearZ * farZ * rdZ;
    frust.m[3][0] = frust.m[3][1] = frust.m[3][3] = 0.0f;

    mMul(r, &frust, r);
}

void mPerspective(mat *r, const float fovy, const float aspect, const float nearZ, const float farZ)
{
    float frustumW, frustumH;
    frustumH = tanf(fovy * 0.002777778078f * PI ) * nearZ; // 0x3b360b62
    frustumW = frustumH * aspect;
    mFrustum(r, -frustumW, frustumW, -frustumH, frustumH, nearZ, farZ);
}

void mOrtho(mat *r, const float left, const float right, const float bottom, const float top, const float nearZ, const float farZ)
{
    const float dX = right - left;
    const float dY = top - bottom;
    const float dZ = farZ - nearZ;
    const float rdX = 1.f/dX;
    const float rdY = 1.f/dY;
    const float rdZ = 1.f/dZ;
    mat ortho;

    if(dX == 0.0f || dY == 0.0f || dZ == 0.0f)
        return;

    mIdent(&ortho);
    ortho.m[0][0] = 2.0f * rdX;
    ortho.m[3][0] = -(right + left) * rdX;
    ortho.m[1][1] = 2.0f * rdY;
    ortho.m[3][1] = -(top + bottom) * rdY;
    ortho.m[2][2] = -2.0f * rdZ;
    ortho.m[3][2] = -(nearZ + farZ) * rdZ;

    mMul(r, &ortho, r);
}

void mLookAt(mat *r, const vec origin, const vec unit_dir)
{
    static const vec up = (vec){0.f, 0.f, 1.f};

    vec dirn;
    dirn.x = unit_dir.x;
    dirn.y = unit_dir.y;
    dirn.z = unit_dir.z;

    vec c;
    vCross(&c, up, dirn);
    vNorm(&c);

    vec rup;
    vCross(&rup, dirn, c);

    r->m[0][0] = c.x;
    r->m[0][1] = c.y;
    r->m[0][2] = c.z;

    r->m[1][0] = rup.x;
    r->m[1][1] = rup.y;
    r->m[1][2] = rup.z;

    r->m[2][0] = dirn.x;
    r->m[2][1] = dirn.y;
    r->m[2][2] = dirn.z;

    r->m[3][0] = origin.x;
    r->m[3][1] = origin.y;
    r->m[3][2] = origin.z;
}

void mInvert(float *dst, const float *mat)
{
    // original source: ftp://download.intel.com/design/PentiumIII/sml/24504301.pdf
    // mirrored: https://github.com/esAux/esAux-Menger/raw/main/SIMD%20Matrix%20Inverse.pdf

    // Cramer Invert
    float tmp[12]; /* temp array for pairs */
    float src[16]; /* array of transpose source matrix */
    float det; /* determinant */

    /* transpose matrix */
    for(int i = 0; i < 4; i++)
    {
        src[i] = mat[i*4];
        src[i + 4] = mat[i*4 + 1];
        src[i + 8] = mat[i*4 + 2];
        src[i + 12] = mat[i*4 + 3];
    }

    /* calculate pairs for first 8 elements (cofactors) */
    tmp[0] = src[10] * src[15];
    tmp[1] = src[11] * src[14];
    tmp[2] = src[9] * src[15];
    tmp[3] = src[11] * src[13];
    tmp[4] = src[9] * src[14];
    tmp[5] = src[10] * src[13];
    tmp[6] = src[8] * src[15];
    tmp[7] = src[11] * src[12];
    tmp[8] = src[8] * src[14];
    tmp[9] = src[10] * src[12];
    tmp[10] = src[8] * src[13];
    tmp[11] = src[9] * src[12];

    /* calculate first 8 elements (cofactors) */
    dst[0] = tmp[0]*src[5] + tmp[3]*src[6] + tmp[4]*src[7];
    dst[0] -= tmp[1]*src[5] + tmp[2]*src[6] + tmp[5]*src[7];
    dst[1] = tmp[1]*src[4] + tmp[6]*src[6] + tmp[9]*src[7];
    dst[1] -= tmp[0]*src[4] + tmp[7]*src[6] + tmp[8]*src[7];
    dst[2] = tmp[2]*src[4] + tmp[7]*src[5] + tmp[10]*src[7];
    dst[2] -= tmp[3]*src[4] + tmp[6]*src[5] + tmp[11]*src[7];
    dst[3] = tmp[5]*src[4] + tmp[8]*src[5] + tmp[11]*src[6];
    dst[3] -= tmp[4]*src[4] + tmp[9]*src[5] + tmp[10]*src[6];
    dst[4] = tmp[1]*src[1] + tmp[2]*src[2] + tmp[5]*src[3];
    dst[4] -= tmp[0]*src[1] + tmp[3]*src[2] + tmp[4]*src[3];
    dst[5] = tmp[0]*src[0] + tmp[7]*src[2] + tmp[8]*src[3];
    dst[5] -= tmp[1]*src[0] + tmp[6]*src[2] + tmp[9]*src[3];
    dst[6] = tmp[3]*src[0] + tmp[6]*src[1] + tmp[11]*src[3];
    dst[6] -= tmp[2]*src[0] + tmp[7]*src[1] + tmp[10]*src[3];
    dst[7] = tmp[4]*src[0] + tmp[9]*src[1] + tmp[10]*src[2];
    dst[7] -= tmp[5]*src[0] + tmp[8]*src[1] + tmp[11]*src[2];

    /* calculate pairs for second 8 elements (cofactors) */
    tmp[0] = src[2]*src[7];
    tmp[1] = src[3]*src[6];
    tmp[2] = src[1]*src[7];
    tmp[3] = src[3]*src[5];
    tmp[4] = src[1]*src[6];
    tmp[5] = src[2]*src[5];
    tmp[6] = src[0]*src[7];
    tmp[7] = src[3]*src[4];
    tmp[8] = src[0]*src[6];
    tmp[9] = src[2]*src[4];
    tmp[10] = src[0]*src[5];
    tmp[11] = src[1]*src[4];

    /* calculate second 8 elements (cofactors) */
    dst[8] = tmp[0]*src[13] + tmp[3]*src[14] + tmp[4]*src[15];
    dst[8] -= tmp[1]*src[13] + tmp[2]*src[14] + tmp[5]*src[15];
    dst[9] = tmp[1]*src[12] + tmp[6]*src[14] + tmp[9]*src[15];
    dst[9] -= tmp[0]*src[12] + tmp[7]*src[14] + tmp[8]*src[15];
    dst[10] = tmp[2]*src[12] + tmp[7]*src[13] + tmp[10]*src[15];
    dst[10]-= tmp[3]*src[12] + tmp[6]*src[13] + tmp[11]*src[15];
    dst[11] = tmp[5]*src[12] + tmp[8]*src[13] + tmp[11]*src[14];
    dst[11]-= tmp[4]*src[12] + tmp[9]*src[13] + tmp[10]*src[14];
    dst[12] = tmp[2]*src[10] + tmp[5]*src[11] + tmp[1]*src[9];
    dst[12]-= tmp[4]*src[11] + tmp[0]*src[9] + tmp[3]*src[10];
    dst[13] = tmp[8]*src[11] + tmp[0]*src[8] + tmp[7]*src[10];
    dst[13]-= tmp[6]*src[10] + tmp[9]*src[11] + tmp[1]*src[8];
    dst[14] = tmp[6]*src[9] + tmp[11]*src[11] + tmp[3]*src[8];
    dst[14]-= tmp[10]*src[11] + tmp[2]*src[8] + tmp[7]*src[9];
    dst[15] = tmp[10]*src[10] + tmp[4]*src[8] + tmp[9]*src[9];
    dst[15]-= tmp[8]*src[9] + tmp[11]*src[10] + tmp[5]*src[8];

    /* calculate determinant */
    det = src[0]*dst[0]+src[1]*dst[1]+src[2]*dst[2]+src[3]*dst[3];

    /* calculate matrix inverse */
    det = 1.0f/det;
    for(int j = 0; j < 16; j++){dst[j] *= det;}
}

void mTranspose(mat *r, const mat *m)
{
    r->m[0][0] = m->m[0][0];
    r->m[1][0] = m->m[0][1];
    r->m[2][0] = m->m[0][2];
    r->m[3][0] = m->m[0][3];

    r->m[0][1] = m->m[1][0];
    r->m[1][1] = m->m[1][1];
    r->m[2][1] = m->m[1][2];
    r->m[3][1] = m->m[1][3];

    r->m[0][2] = m->m[2][0];
    r->m[1][2] = m->m[2][1];
    r->m[2][2] = m->m[2][2];
    r->m[3][2] = m->m[2][3];

    r->m[0][3] = m->m[3][0];
    r->m[1][3] = m->m[3][1];
    r->m[2][3] = m->m[3][2];
    r->m[3][3] = m->m[3][3];
}

//

void mSetViewDir(mat *r, const vec dir_norm)
{
    static const vec up_norm = (vec){0.f, 0.f, 1.f};

    vec c;
    vCross(&c, up_norm, dir_norm);
    vNorm(&c);

    vec rup;
    vCross(&rup, dir_norm, c);

    r->m[0][0] = -c.x;
    r->m[1][0] = -c.y;
    r->m[2][0] = -c.z;

    r->m[0][1] = -rup.x;
    r->m[1][1] = -rup.y;
    r->m[2][1] = -rup.z;

    r->m[0][2] = -dir_norm.x;
    r->m[1][2] = -dir_norm.y;
    r->m[2][2] = -dir_norm.z;
}

void mGetViewDir(vec *r, const mat matrix)
{
    mGetViewZ(r, matrix);
}

void mGetViewX(vec *r, const mat matrix)
{
    r->x = -matrix.m[0][0];
    r->y = -matrix.m[1][0];
    r->z = -matrix.m[2][0];
}

void mGetViewY(vec *r, const mat matrix)
{
    r->x = -matrix.m[0][1];
    r->y = -matrix.m[1][1];
    r->z = -matrix.m[2][1];
}

void mGetViewZ(vec *r, const mat matrix)
{
    r->x = -matrix.m[0][2];
    r->y = -matrix.m[1][2];
    r->z = -matrix.m[2][2];
}

//

void mSetDir(mat *r, const vec dir_norm)
{
    static const vec up_norm = (vec){0.f, 0.f, 1.f};

    vec c;
    vCross(&c, up_norm, dir_norm);
    vNorm(&c);

    vec rup;
    vCross(&rup, dir_norm, c);

    r->m[0][0] = c.x;
    r->m[0][1] = c.y;
    r->m[0][2] = c.z;

    r->m[2][0] = rup.x;
    r->m[2][1] = rup.y;
    r->m[2][2] = rup.z;

    r->m[1][0] = -dir_norm.x;
    r->m[1][1] = -dir_norm.y;
    r->m[1][2] = -dir_norm.z;
}

void mGetDirX(vec *r, const mat matrix)
{
    r->x = matrix.m[0][0];
    r->y = matrix.m[0][1];
    r->z = matrix.m[0][2];
}

void mGetDirY(vec *r, const mat matrix)
{
    r->x = matrix.m[1][0];
    r->y = matrix.m[1][1];
    r->z = matrix.m[1][2];
}

void mGetDirZ(vec *r, const mat matrix)
{
    r->x = matrix.m[2][0];
    r->y = matrix.m[2][1];
    r->z = matrix.m[2][2];
}

void mGetPos(vec *r, const mat matrix)
{
    r->x = matrix.m[3][0];
    r->y = matrix.m[3][1];
    r->z = matrix.m[3][2];
}

void mSetPos(mat *r, const vec pos)
{
    r->m[3][0] = pos.x;
    r->m[3][1] = pos.y;
    r->m[3][2] = pos.z;
}

void mDump(const mat matrix)
{
    printf("%+.2f %+.2f %+.2f %+.2f\n", matrix.m[0][0], matrix.m[0][1], matrix.m[0][2], matrix.m[0][3]);
    printf("%+.2f %+.2f %+.2f %+.2f\n", matrix.m[1][0], matrix.m[1][1], matrix.m[1][2], matrix.m[1][3]);
    printf("%+.2f %+.2f %+.2f %+.2f\n", matrix.m[2][0], matrix.m[2][1], matrix.m[2][2], matrix.m[2][3]);
    printf("%+.2f %+.2f %+.2f %+.2f\n", matrix.m[3][0], matrix.m[3][1], matrix.m[3][2], matrix.m[3][3]);
    printf("---\n");
}

#endif

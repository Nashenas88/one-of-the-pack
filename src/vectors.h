#ifndef _VECTORS_H_
#define _VECTORS_H_

//
// originally implemented by Justin Legakis
//

#include <iostream>
#include <cstdio>
#include <cmath>
#include <cassert>

// ====================================================================
// ====================================================================

class Vec2f 
{
public:

  // CONSTRUCTORS & DESTRUCTOR
  Vec2f() { data[0] = data[1] = 0; }
  Vec2f(const Vec2f &V) 
  {
    data[0] = V.data[0];
    data[1] = V.data[1]; 
  }
  Vec2f(float d0, float d1)
  {
    data[0] = d0;
    data[1] = d1; 
  }
  Vec2f(const Vec2f &V1, const Vec2f &V2) 
  {
    data[0] = V1.data[0] - V2.data[0];
    data[1] = V1.data[1] - V2.data[1]; 
  }
  ~Vec2f() { }

  // ACCESSORS
  void Get(float &d0, float &d1) const 
  {
    d0 = data[0];
    d1 = data[1]; 
  }
  float operator[](int i) const 
  { 
    assert (i >= 0 && i < 2); 
    return data[i]; 
  }
  float x() const { return data[0]; }
  float y() const { return data[1]; }
  
  float r() const { return data[0]; }
  float g() const { return data[1]; }
  float Length() const 
  {
    return (float)sqrt( data[0] * data[0] + data[1] * data[1] ); 
  }

  // MODIFIERS
  void Set(float d0, float d1) 
  {
    data[0] = d0;
    data[1] = d1; 
  }
  
  // OVERLOADED OPERATORS
  Vec2f& operator=(const Vec2f &V) 
  {
    data[0] = V.data[0];
    data[1] = V.data[1];
    return *this; 
  }
  int operator==(const Vec2f &V) const { return ((data[0] == V.data[0]) && (data[1] == V.data[1])); }
  
  int operator!=(const Vec2f &V) { return ((data[0] != V.data[0]) || (data[1] != V.data[1])); }
  
  Vec2f& operator+=(const Vec2f &V) 
  {
    data[0] += V.data[0];
    data[1] += V.data[1];
    return *this; 
  }
  Vec2f& operator-=(const Vec2f &V) 
  {
    data[0] -= V.data[0];
    data[1] -= V.data[1];
    return *this; 
  }
  Vec2f& operator*=(float f) 
  {
    data[0] *= f;
    data[1] *= f;
    return *this; 
  }
  Vec2f& operator/=(float f) 
  {
    data[0] /= f;
    data[1] /= f;
    return *this; 
  }

  // STATIC OPERATIONS
  static void Add(Vec2f &a, const Vec2f &b, const Vec2f &c ) 
  {
    a.data[0] = b.data[0] + c.data[0];
    a.data[1] = b.data[1] + c.data[1]; 
  }

  // INPUT / OUTPUT
  friend std::ostream& operator<< (std::ostream &ostr, const Vec2f &v) 
  {
    ostr << v.data[0] << " " << v.data[1] << std::endl;
    return ostr; 
  }

private:
  // REPRESENTATION
  float		data[2];
};
// ====================================================================
// ====================================================================

class Vec3f 
{

public:

  // CONSTRUCTORS & DESTRUCTOR
  Vec3f() { data[0] = data[1] = data[2] = 0; }
  Vec3f(const Vec3f &V) 
  {
    data[0] = V.data[0];
    data[1] = V.data[1];
    data[2] = V.data[2]; 
  }
  Vec3f(float d0, float d1, float d2) 
  {
    data[0] = d0;
    data[1] = d1;
    data[2] = d2;
  }
  Vec3f(const Vec3f &V1, const Vec3f &V2) 
  {
    data[0] = V1.data[0] - V2.data[0];
    data[1] = V1.data[1] - V2.data[1];
    data[2] = V1.data[2] - V2.data[2]; 
  }
  ~Vec3f() { }

  // ACCESSORS
  void Get(float &d0, float &d1, float &d2) const 
  {
    d0 = data[0];
    d1 = data[1];
    d2 = data[2]; 
  }
  float operator[](int i) const 
  {  
    assert (i >= 0 && i < 3); 
    return data[i]; 
  }
  
  float x() const { return data[0]; }
  float y() const { return data[1]; }
  float z() const { return data[2]; }
  
  float r() const { return data[0]; }
  float g() const { return data[1]; }
  float b() const { return data[2]; }
  
  float Length() const { return (float)sqrt( data[0] * data[0] + data[1] * data[1] + data[2] * data[2] ); }

  // MODIFIERS
  void Set(float d0, float d1, float d2) 
  {
    data[0] = d0;
    data[1] = d1;
    data[2] = d2; 
  }

  // OVERLOADED OPERATORS
  Vec3f& operator=(const Vec3f &V) 
  {
    data[0] = V.data[0];
    data[1] = V.data[1];
    data[2] = V.data[2];
    return *this; 
  }
  int operator==(const Vec3f &V) { return ((data[0] == V.data[0]) && (data[1] == V.data[1]) && (data[2] == V.data[2])); }
  
  int operator!=(const Vec3f &V) { return ((data[0] != V.data[0]) || (data[1] != V.data[1]) || (data[2] != V.data[2])); }
  
  // STATIC OPERATIONS
  static void Add(Vec3f &a, const Vec3f &b, const Vec3f &c ) 
  {
    a.data[0] = b.data[0] + c.data[0];
    a.data[1] = b.data[1] + c.data[1];
    a.data[2] = b.data[2] + c.data[2]; 
  }

  static void Min(Vec3f &a, const Vec3f &b, const Vec3f &c ) 
  {
    a.data[0] = (b.data[0] < c.data[0]) ? b.data[0] : c.data[0];
    a.data[1] = (b.data[1] < c.data[1]) ? b.data[1] : c.data[1];
    a.data[2] = (b.data[2] < c.data[2]) ? b.data[2] : c.data[2]; 
  }
  static void Max(Vec3f &a, const Vec3f &b, const Vec3f &c ) 
  {
    a.data[0] = (b.data[0] > c.data[0]) ? b.data[0] : c.data[0];
    a.data[1] = (b.data[1] > c.data[1]) ? b.data[1] : c.data[1];
    a.data[2] = (b.data[2] > c.data[2]) ? b.data[2] : c.data[2]; 
  }
  
  // INPUT / OUTPUT
  friend std::ostream& operator<< (std::ostream &ostr, const Vec3f &v) 
  {
    ostr << v.data[0] << " " << v.data[1] << " " << v.data[2] << std::endl; 
    return ostr; 
  }
  
private:

  // REPRESENTATION
  float		data[3];
  
};

// ====================================================================
// ====================================================================

class Vec4f {
  
public:

  // CONSTRUCTORS & DESTRUCTOR
  Vec4f() { data[0] = data[1] = data[2] = data[3] = 0; }
  Vec4f(const Vec4f &V) 
  {
    data[0] = V.data[0];
    data[1] = V.data[1];
    data[2] = V.data[2];
    data[3] = V.data[3]; 
  }
  Vec4f(float d0, float d1, float d2, float d3) 
  {
    data[0] = d0;
    data[1] = d1;
    data[2] = d2;
    data[3] = d3; 
  }
  Vec4f(const Vec3f &V, float w) 
  {
    data[0] = V.x();
    data[1] = V.y();
    data[2] = V.z();
    data[3] = w; 
  }
  Vec4f(const Vec4f &V1, const Vec4f &V2) 
  {
    data[0] = V1.data[0] - V2.data[0];
    data[1] = V1.data[1] - V2.data[1];
    data[2] = V1.data[2] - V2.data[2];
    data[3] = V1.data[3] - V2.data[3]; 
  }
  ~Vec4f() { }

  // ACCESSORS
  void Get(float &d0, float &d1, float &d2, float &d3) const 
  {
    d0 = data[0];
    d1 = data[1];
    d2 = data[2];
    d3 = data[3]; 
  }
  float operator[](int i) const 
  { 
    assert (i >= 0 && i < 4); 
    return data[i]; 
  }
  
  float x() const { return data[0]; }
  float y() const { return data[1]; }
  float z() const { return data[2]; }
  float w() const { return data[3]; }
  
  float r() const { return data[0]; }
  float g() const { return data[1]; }
  float b() const { return data[2]; }
  float a() const { return data[3]; }
  
  float Length() const {  return (float)sqrt( data[0] * data[0] + data[1] * data[1] + data[2] * data[2] + data[3] * data[3] ); }

  // MODIFIERS
  void Set(float d0, float d1, float d2, float d3) 
  {
    data[0] = d0;
    data[1] = d1;
    data[2] = d2;
    data[3] = d3; 
  }

  // OVERLOADED OPERATORS
  Vec4f& operator=(const Vec4f &V) 
  {
    data[0] = V.data[0];
    data[1] = V.data[1];
    data[2] = V.data[2];
    data[3] = V.data[3];
    return *this; 
  }
  int operator==(const Vec4f &V) const {  return ((data[0] == V.data[0]) && (data[1] == V.data[1]) && (data[2] == V.data[2]) &&(data[3] == V.data[3])); }
  
  int operator!=(const Vec4f &V) const {  return ((data[0] != V.data[0]) || (data[1] != V.data[1]) || (data[2] != V.data[2]) || (data[3] != V.data[3])); }
  
  // STATIC OPERATIONS
  static void Add(Vec4f &a, const Vec4f &b, const Vec4f &c ) 
  {
    a.data[0] = b.data[0] + c.data[0];
    a.data[1] = b.data[1] + c.data[1];
    a.data[2] = b.data[2] + c.data[2];
    a.data[3] = b.data[3] + c.data[3]; 
  }

  // INPUT / OUTPUT
  friend std::ostream& operator<< (std::ostream &ostr, const Vec4f &v) 
  {
    ostr << v.data[0] << " " << v.data[1] << " " << v.data[2] << " " << v.data[3] << std::endl; 
    return ostr; 
  }

private:
  // REPRESENTATION
  float		data[4];
};
#endif


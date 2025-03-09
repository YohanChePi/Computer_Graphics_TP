#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <glm/gtc/type_precision.hpp> //i32vec3
#include <vector>
#include <string>

using namespace glm;
using namespace std;

class Mesh {
 public:
  Mesh(){}
  Mesh(const char* filename);
  ~Mesh();

  i32vec3 get_face(unsigned int i);
  vec3 get_vertex(unsigned int i);
  vec3 get_normal(unsigned int i);
  vec3 get_color(unsigned int i);

  // length 
  unsigned int  nb_vertices;
  unsigned int  nb_faces;
  
  // data
  vector< vec3 > vertices;
  vector< vec3 > normals;
  vector< vec3 > colors;
  vector< unsigned int > faces;
  
  vector< vec3 > computeBB() const ;
  void normalize();
  void colorize(float amplitude=0.5f,float frequency=10.0f,float persistence=0.5f,int nboctaves=4);
  
  // info
  vec3 center;
  float radius;

 private:
  vec3 hash33(vec3 p);
  float gnoise(vec3 x);
  float fnoise(vec3 p,float amplitude,float frequency,float persistence,int nboctaves);
};

#endif // MESH_H

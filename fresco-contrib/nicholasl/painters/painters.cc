#include <Fresco/Types.hh>
#include <strstream>
#include <iostream>

#include <algorithm>
#include <iterator>
#include <functional>

Fresco::Mesh cube();
Fresco::Mesh test();
void dumpMesh(Fresco::Mesh &mesh);

// this is a "strict weak ordering" aka less than.
class TriangleCompare : public binary_function<Fresco::Triangle&, Fresco::Triangle&, bool> {
public:
  explicit TriangleCompare::TriangleCompare(Fresco::Vertices &ref_to_verseq)
    : vertices(ref_to_verseq) {}
  bool operator()(const Fresco::Triangle &one, const Fresco::Triangle &two) const
  {
    // returns true if one is on the wrong side of two, from Flights of Fantasy

    // test 1: do x extents overlap?
    Fresco::Coord x_one_min, x_one_max;
    x_one_min = vertices[one.a].x;
    x_one_max = vertices[one.a].x;
    if (vertices[one.b].x < x_one_min) { x_one_min = vertices[one.b].x; }
    if (vertices[one.b].x > x_one_max) { x_one_max = vertices[one.b].x; }
    if (vertices[one.c].x < x_one_min) { x_one_min = vertices[one.c].x; }
    if (vertices[one.c].x > x_one_max) { x_one_max = vertices[one.c].x; }

    Fresco::Coord x_two_min, x_two_max;
    x_two_min = vertices[two.a].x;
    x_two_max = vertices[two.a].x;
    if (vertices[two.b].x < x_two_min) { x_two_min = vertices[two.b].x; }
    if (vertices[two.b].x > x_two_max) { x_two_max = vertices[two.b].x; }
    if (vertices[two.c].x < x_two_min) { x_two_min = vertices[two.c].x; }
    if (vertices[two.c].x > x_two_max) { x_two_max = vertices[two.c].x; }

    if ((x_two_min > x_one_max) || (x_two_max < x_one_min)) { return false; }

    // test 2: do y extents overlap?
    Fresco::Coord y_one_min, y_one_max;
    y_one_min = vertices[one.a].y;
    y_one_max = vertices[one.a].y;
    if (vertices[one.b].y < y_one_min) { y_one_min = vertices[one.b].y; }
    if (vertices[one.b].y > y_one_max) { y_one_max = vertices[one.b].y; }
    if (vertices[one.c].y < y_one_min) { y_one_min = vertices[one.c].y; }
    if (vertices[one.c].y > y_one_max) { y_one_max = vertices[one.c].y; }

    Fresco::Coord y_two_min, y_two_max;
    y_two_min = vertices[two.a].y;
    y_two_max = vertices[two.a].y;
    if (vertices[two.b].y < y_two_min) { y_two_min = vertices[two.b].y; }
    if (vertices[two.b].y > y_two_max) { y_two_max = vertices[two.b].y; }
    if (vertices[two.c].y < y_two_min) { y_two_min = vertices[two.c].y; }
    if (vertices[two.c].y > y_two_max) { y_two_max = vertices[two.c].y; }

    if ((y_two_min > y_one_max) || (y_two_max < y_one_min)) { return false; }

    // The FoF tests 3 and 4 rely on planar coefficients for the triangles.
    // The catch is that a backfacing triangle will have a negated normal,
    // and will require a reverse comparison function. Here, we implement a
    // backface detection algorithm and reverse the sign of the plane if
    // necessary.

    bool one_backfacing = 
      (vertices[one.c].x*((vertices[one.a].z*vertices[one.b].y)-(vertices[one.a].y*vertices[one.b].z)) +
       vertices[one.c].y*((vertices[one.a].x*vertices[one.b].z)-(vertices[one.a].z*vertices[one.b].x)) +
       vertices[one.c].z*((vertices[one.a].y*vertices[one.b].x)-(vertices[one.a].x*vertices[one.b].y)))<0;

    bool two_backfacing =
      (vertices[two.c].x*((vertices[two.a].z*vertices[two.b].y)-(vertices[two.a].y*vertices[two.b].z)) +
       vertices[two.c].y*((vertices[two.a].x*vertices[two.b].z)-(vertices[two.a].z*vertices[two.b].x)) +
       vertices[two.c].z*((vertices[two.a].y*vertices[two.b].x)-(vertices[two.a].x*vertices[two.b].y)))<0;

    // test 3: is two entirely on the far (correct) side of one?
    Fresco::Coord two_A, two_B, two_C, two_D; // these are planar coefficients

    two_A = (vertices[two.a].y)*(vertices[two.b].z - vertices[two.c].z)
          + (vertices[two.b].y)*(vertices[two.c].z - vertices[two.a].z)
          + (vertices[two.c].y)*(vertices[two.a].z - vertices[two.b].z);
    two_B = (vertices[two.a].z)*(vertices[two.b].x - vertices[two.c].x)
          + (vertices[two.b].z)*(vertices[two.c].x - vertices[two.a].x)
          + (vertices[two.c].z)*(vertices[two.a].x - vertices[two.b].x);
    two_C = (vertices[two.a].x)*(vertices[two.b].y - vertices[two.c].y)
          + (vertices[two.b].x)*(vertices[two.c].y - vertices[two.a].y)
          + (vertices[two.c].x)*(vertices[two.a].y - vertices[two.b].y);
    two_D = (-vertices[two.a].x)*(vertices[two.b].y*vertices[two.c].z -
				  vertices[two.c].y*vertices[two.b].z)
          + (-vertices[two.b].x)*(vertices[two.c].y*vertices[two.a].z -
				  vertices[two.a].y*vertices[two.c].z)
          + (-vertices[two.c].x)*(vertices[two.a].y*vertices[two.b].z -
				  vertices[two.b].y*vertices[two.a].z);

    if (two_backfacing)
      two_A*=-1; two_B*=-1; two_C*=-1; two_D*=-1;

    if (two_A*vertices[one.a].x + two_B*vertices[one.a].y +
	two_C*vertices[one.a].z + two_D < 0) { return false; }
    if (two_A*vertices[one.b].x + two_B*vertices[one.b].y +
	two_C*vertices[one.b].z + two_D < 0) { return false; }
    if (two_A*vertices[one.c].x + two_B*vertices[one.c].y +
	two_C*vertices[one.c].z + two_D < 0) { return false; }

    // test 4: is one entirely on the nearer (correct) side of two?
    Fresco::Coord one_A, one_B, one_C, one_D;
    one_A = (vertices[one.a].y)*(vertices[one.b].z - vertices[one.c].z)
          + (vertices[one.b].y)*(vertices[one.c].z - vertices[one.a].z)
          + (vertices[one.c].y)*(vertices[one.a].z - vertices[one.b].z);
    one_B = (vertices[one.a].z)*(vertices[one.b].x - vertices[one.c].x)
          + (vertices[one.b].z)*(vertices[one.c].x - vertices[one.a].x)
          + (vertices[one.c].z)*(vertices[one.a].x - vertices[one.b].x);
    one_C = (vertices[one.a].x)*(vertices[one.b].y - vertices[one.c].y)
          + (vertices[one.b].x)*(vertices[one.c].y - vertices[one.a].y)
          + (vertices[one.c].x)*(vertices[one.a].y - vertices[one.b].y);
    one_D = (-vertices[one.a].x)*(vertices[one.b].y*vertices[one.c].z -
                                  vertices[one.c].y*vertices[one.b].z)
          + (-vertices[one.b].x)*(vertices[one.c].y*vertices[one.a].z -
                                  vertices[one.a].y*vertices[one.c].z)
          + (-vertices[one.c].x)*(vertices[one.a].y*vertices[one.b].z -
                                  vertices[one.b].y*vertices[one.a].z);

    if (one_backfacing)
      one_A*=-1; one_B*=-1; one_C*=-1; one_D*=-1;

    if (one_A*vertices[two.a].x + one_B*vertices[two.a].y +
	one_C*vertices[two.a].z + one_D > 0) { return false; }
    if (one_A*vertices[two.b].x + one_B*vertices[two.b].y +
	one_C*vertices[two.b].z + one_D > 0) { return false; }
    if (one_A*vertices[two.c].x + one_B*vertices[two.c].y +
	one_C*vertices[two.c].z + one_D > 0) { return false; }

    // skip test 5: do they actually occlude? Who cares, flip them anyways.

    // Irreflexivity: f(x, x) must be FALSE
    Fresco::Coord one_coef, two_coef;
    one_coef = 1./(one_A + one_B + one_C + one_D);
    two_coef = 1./(two_A + two_B + two_C + two_D);
    if (one_A*one_coef == two_A*two_coef && one_B*one_coef == two_B*two_coef
	&& one_C*one_coef == two_C*two_coef) {
      //std::cout << "irreflexivity case caught!" << std::endl;
      return false;
    }

    return true;
  }
private:
  Fresco::Vertices vertices;
};

class TriangleIterator : public random_access_iterator<Fresco::Triangle, ptrdiff_t>{
public:
  TriangleIterator::TriangleIterator(Fresco::TriangleSeq &ref_to_seq, const ptrdiff_t i)
    : seq(&ref_to_seq), index(i) {}
  TriangleIterator operator++(int) { // post-increment
    TriangleIterator old(*this);
    ++index;
    return old;
  }
  TriangleIterator &operator++() { ++index; return *this; }
  TriangleIterator operator--(int) { // post-decrement
    TriangleIterator old(*this);
    --index;
    return old;
  }
  TriangleIterator &operator--() { --index; return *this; }
  Fresco::Triangle &operator[] (ptrdiff_t i) const {
    return (*seq)[i+index];
  }
  Fresco::Triangle &operator*() const { return (*this)[0]; }

  bool operator<(const TriangleIterator&that) const {
    return (this->index < that.index);
  }
  bool operator==(const TriangleIterator&that) const {
    return (this->index == that.index);
  }
  TriangleIterator operator+(const ptrdiff_t n) const {
    TriangleIterator sum(*seq, index+n);
    return sum;
  }
  ptrdiff_t operator-(const TriangleIterator& that) const {
    return this->index - that.index;
  }
  TriangleIterator operator-(const ptrdiff_t n) const {
    TriangleIterator difference(*seq, index-n);
    return difference;
  }
private:
  ptrdiff_t index;
  Fresco::TriangleSeq *seq;
};

/* complex irreflexivity test */
bool cirr()
{
  Fresco::Mesh mesh;
  mesh.nodes.length(3);

  mesh.nodes[0].x = 0; mesh.nodes[0].y = 0; mesh.nodes[0].z = 0;
  mesh.nodes[1].x = 1; mesh.nodes[1].y = 0; mesh.nodes[1].z = 0;
  mesh.nodes[2].x = 1; mesh.nodes[2].y = 1; mesh.nodes[2].z = 0;

  mesh.triangles.length(2);
  mesh.triangles[0].a = 0; mesh.triangles[0].b = 1; mesh.triangles[0].c = 2;
  mesh.triangles[1].a = 0; mesh.triangles[1].b = 2; mesh.triangles[1].c = 1;

  TriangleCompare comp(mesh.nodes);

  return (comp(mesh.triangles[0], mesh.triangles[1]) == false);
}

int main(int argc, char *argv[])
{
  // must initalize ORB before using CORBA::sequence!
  CORBA::ORB_var orb;
  orb = CORBA::ORB_init(argc, argv);

  if (!cirr()) {
    std::cout << "complex irreflexivity test failed." << std::endl;
  }

  //Fresco::Mesh mesh = cube();
  Fresco::Mesh mesh = test();

  std::cout << "before sorting:" << std::endl;
  dumpMesh(mesh);
  TriangleIterator first(mesh.triangles, 0);
  TriangleIterator last(mesh.triangles, mesh.triangles.length());
  Fresco::Vertices v = mesh.nodes; // transform the vertices here
  TriangleCompare comp(v);
  sort(first, last, comp);
  std::cout << std::endl << "after:" << std::endl;
  dumpMesh(mesh);

  orb->shutdown(true);

  return 0;
}

Fresco::Mesh test()
{
  Fresco::Mesh mesh;
  mesh.nodes.length(6);

  mesh.nodes[0].x = 0; mesh.nodes[0].y = 0; mesh.nodes[0].z = 0;
  mesh.nodes[1].x = 1; mesh.nodes[1].y = 0; mesh.nodes[1].z = 0;
  mesh.nodes[2].x = 1; mesh.nodes[2].y = 1; mesh.nodes[2].z = 0;

  mesh.nodes[3].x = 0; mesh.nodes[3].y = 0; mesh.nodes[3].z = 1;
  mesh.nodes[4].x = 1; mesh.nodes[4].y = 0; mesh.nodes[4].z = 1;
  mesh.nodes[5].x = 1; mesh.nodes[5].y = 1; mesh.nodes[5].z = 1;

  mesh.triangles.length(2);
  mesh.triangles[0].a = 3; mesh.triangles[0].b = 5; mesh.triangles[0].c = 4;
  mesh.triangles[1].a = 0; mesh.triangles[1].b = 1; mesh.triangles[1].c = 2;

  return mesh;
}

Fresco::Mesh cube()
{
  /* PrimitiveKitImpl::cube() */
  Fresco::Mesh mesh;
  mesh.nodes.length(8);
  mesh.nodes[0].x = -500; mesh.nodes[0].y = -500; mesh.nodes[0].z = -500;
  mesh.nodes[1].x = -500; mesh.nodes[1].y = -500; mesh.nodes[1].z =  500;
  mesh.nodes[2].x = -500; mesh.nodes[2].y =  500; mesh.nodes[2].z =  500;

  mesh.nodes[3].x = -500; mesh.nodes[3].y =  500; mesh.nodes[3].z = -500;
  mesh.nodes[4].x =  500; mesh.nodes[4].y =  500; mesh.nodes[4].z =  500;
  mesh.nodes[5].x =  500; mesh.nodes[5].y = -500; mesh.nodes[5].z =  500;

  mesh.nodes[6].x =  500; mesh.nodes[6].y = -500; mesh.nodes[6].z = -500;
  mesh.nodes[7].x =  500; mesh.nodes[7].y =  500; mesh.nodes[7].z = -500;

  mesh.triangles.length(12);
  /* x=0 side */
  mesh.triangles[0].a = 0; mesh.triangles[0].b = 2; mesh.triangles[0].c = 3;
  mesh.triangles[1].a = 0; mesh.triangles[1].b = 1; mesh.triangles[1].c = 2;
  /* y=0 side */
  mesh.triangles[2].a = 0; mesh.triangles[2].b = 5; mesh.triangles[2].c = 1;
  mesh.triangles[3].a = 0; mesh.triangles[3].b = 6; mesh.triangles[3].c = 5;
  /* z=0 side */
  mesh.triangles[4].a = 0; mesh.triangles[4].b = 3; mesh.triangles[4].c = 7;
  mesh.triangles[5].a = 0; mesh.triangles[5].b = 7; mesh.triangles[5].c = 6;
  /* x=1 side */
  mesh.triangles[6].a = 4; mesh.triangles[6].b = 5; mesh.triangles[6].c = 6;
  mesh.triangles[7].a = 4; mesh.triangles[7].b = 6; mesh.triangles[7].c = 7;
  /* y=1 side */
  mesh.triangles[8].a = 4; mesh.triangles[8].b = 7; mesh.triangles[8].c = 3;
  mesh.triangles[9].a = 4; mesh.triangles[9].b = 3; mesh.triangles[9].c = 2;
  /* z=1 side */
  mesh.triangles[10].a = 4; mesh.triangles[10].b = 1; mesh.triangles[10].c = 5;
  mesh.triangles[11].a = 4; mesh.triangles[11].b = 2; mesh.triangles[11].c = 1;

  mesh.normals.length(12);
  mesh.normals[0].x = -1.; mesh.normals[0].y =  0.; mesh.normals[0].z =  0.;
  mesh.normals[1].x = -1.; mesh.normals[1].y =  0.; mesh.normals[1].z =  0.;
  mesh.normals[2].x =  0.; mesh.normals[2].y = -1.; mesh.normals[2].z =  0.;
  mesh.normals[3].x =  0.; mesh.normals[3].y = -1.; mesh.normals[3].z =  0.;
  mesh.normals[4].x =  0.; mesh.normals[4].y =  0.; mesh.normals[4].z = -1.;
  mesh.normals[5].x =  0.; mesh.normals[5].y =  0.; mesh.normals[5].z = -1.;
  mesh.normals[6].x =  1.; mesh.normals[6].y =  0.; mesh.normals[6].z =  0.;
  mesh.normals[7].x =  1.; mesh.normals[7].y =  0.; mesh.normals[7].z =  0.;
  mesh.normals[8].x =  0.; mesh.normals[8].y =  1.; mesh.normals[8].z =  0.;
  mesh.normals[9].x =  0.; mesh.normals[9].y =  1.; mesh.normals[9].z =  0.;
  mesh.normals[10].x =  0.; mesh.normals[10].y =  0.; mesh.normals[10].z =  1.;
  mesh.normals[11].x =  0.; mesh.normals[11].y =  0.; mesh.normals[11].z =  1.;

  return mesh;
}

class dumpTriangle : public unary_function<Fresco::Triangle&, void> {
public:
  void operator()(const Fresco::Triangle &t) const {
    std::cout << "(" << t.a << "," << t.b << "," << t.c << ")" << std::endl;
  }
};

void dumpMesh(Fresco::Mesh &mesh)
{
  TriangleIterator i(mesh.triangles, 0);
  TriangleIterator end(mesh.triangles, mesh.triangles.length()); 
  dumpTriangle dt;
  for_each(i, end, dt);
}


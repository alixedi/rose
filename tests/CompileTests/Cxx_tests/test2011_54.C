
namespace Geometry
   {
     class PolygonalMeshTypeInfo {};

     template <typename MeshTypeInfo> class MeshBase
        {
          MeshTypeInfo x;
        };

   }

template<typename MeshType>
class X2
   {
     public:
         MeshType x;
         typedef int Scalar;
   };

// X2<  Geometry::MeshBase < Geometry::PolygonalMeshTypeInfo > >::Scalar *sp;
X2<  Geometry::MeshBase < Geometry::PolygonalMeshTypeInfo > >::Scalar sp_0;

X2<  Geometry::MeshBase < int > >::Scalar sp_1;


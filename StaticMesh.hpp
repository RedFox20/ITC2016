#pragma once
#include "Shader.hpp"
#include <memory> // unique_ptr

namespace itc
{
	using namespace std;
	////////////////////////////////////////////////////////////////////////////////

	struct BMDModel // definition of our BMDModel format - this is a POD type
	{
		char name[32];		// model name
		char tex_name[32];	// Texture name
		int num_verts;	    // number of vertices
		int	num_indices;	// number of indices
		int off_verts;		// offset to vertices
		int off_indices;	// offset to indices
		// Vertex Data [num_verts    * sizeof(vertex_t)] follows
		// Index Data  [num_indices  * sizeof(index_t) ] follows
		
		vertex3d* vertices() const;
		index_t*  indices()  const;

		static unique_ptr<BMDModel> loadFromFile(const string& file);
	private:
		BMDModel() {}
	};

	struct StaticMesh
	{
		unique_ptr<BMDModel> MeshData;     // 
		Vertex3dBuffer       Vertex3dBuff; // buffer of vertex3d elements

		StaticMesh(const string& resourcePath);
		~StaticMesh();

		operator bool() const { return MeshData && Vertex3dBuff.vertexCount; }

	};

	////////////////////////////////////////////////////////////////////////////////
}
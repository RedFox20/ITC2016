#include "Model.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>


namespace itc
{
	////////////////////////////////////////////////////////////////////////////////

	vertex3d* BMDModel::vertices() const
	{
		return (vertex3d*)((char*)this + off_verts);
	}
	index_t* BMDModel::indices() const
	{
		return (index_t*)((char*)this + off_indices);
	}
	unique_ptr<BMDModel> BMDModel::loadFromFile(const string& file)
	{
		// read the 3D model data
		FILE* f = fopen(file.data(), "rb");
		if (f == 0) {
			printf("BMDModel::loadFromFile(): fopen failed %s\n", file.data());
			return false;
		}
		struct stat s;
		fstat(fileno(f), &s);
		int size = s.st_size;
		BMDModel* m = (BMDModel*)malloc(size);
		if (!m) {
			printf("BMDModel::loadFromFile(): malloc %dKB failed %s\n", size/1024, file.data());
			return false;
		}
		fread(m, size, 1, f);
		fclose(f);
		#if DEBUG
			printf("------------------\n");
			printf("FileSize: %d\n", size);
			printf("Loaded model   %s (%dKB)\n", m->name, size / 1024);
			printf("  Texture      %s\n", m->tex_name);
			printf("  NumVertices  %d\n", m->num_verts);
			printf("  NumIndices   %d\n", m->num_indices);
			printf("  Polys        %d\n", m->num_indices/3);
			printf("------------------\n");
		#endif
		return unique_ptr<BMDModel>(m);
	}

	////////////////////////////////////////////////////////////////////////////////
}


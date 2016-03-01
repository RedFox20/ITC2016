#include "StaticMesh.hpp"
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
		BMDModel* m = nullptr;
		if (FILE* f = fopen(file.data(), "rb"))
		{
			struct stat s;
			fstat(fileno(f), &s);
			if (m = (BMDModel*)malloc(s.st_size))
			{
				fread(m, s.st_size, 1, f);
				#if DEBUG
					printf("------------------\n");
					printf("FileSize: %d\n", s.st_size);
					printf("Loaded model   %s (%dKB)\n", m->name, s.st_size / 1024);
					printf("  Texture      %s\n", m->tex_name);
					printf("  NumVertices  %d\n", m->num_verts);
					printf("  NumIndices   %d\n", m->num_indices);
					printf("  Polys        %d\n", m->num_indices/3);
					printf("------------------\n");
				#endif
			}
			else fprintf(stderr, "BMDModel::loadFromFile(): malloc %dKB failed %s\n", s.st_size/1024, file.data());
			fclose(f);
		}
		else fprintf(stderr, "BMDModel::loadFromFile(): fopen failed %s\n", file.data());
		return unique_ptr<BMDModel>(m);
	}

	StaticMesh::StaticMesh(const string & resourcePath)
	{
	}

	StaticMesh::~StaticMesh()
	{
	}

	////////////////////////////////////////////////////////////////////////////////
}


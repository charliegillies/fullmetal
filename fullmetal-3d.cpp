#include "fullmetal-3d.h"
#include "fullmetal.h"
#include <fstream>
#include <cassert>

fm::PolyFace::Indexes::Indexes() : vertexIndex(-1), normalIndex(-1), texCoordIndex(-1) { }

fm::PolyFace::PolyFace() : indices() { }

fm::ObjModel * fm::ObjModelLoader::load(std::string fp)
{
	// try to open stream, if bad - assert out
	std::ifstream stream;
	stream.open(fp.c_str());
	assert(stream.good());

	// create a model, begin the readLine procedure to parse it
	ObjModel* model = new ObjModel();
	model->filepath = fp;
	model->switchedUvs = false;

	std::string data;
	while (std::getline(stream, data))
		readLine(data, model);

	stream.close();

	return model;
}

void fm::ObjModelLoader::readLine(std::string& line, ObjModel* model)
{
	// If line has no data, skip
	if (line.size() < 3) return;
	
	OBJ_PARAM param = getParam(line);

	switch (param) {
		// example: v 1.23 -0.341 9.2
		case OBJ_PARAM::VERTEX: {
			Vector3 vertex;
			int matches = sscanf_s(line.data(), "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);

			assert(matches == 3);
			model->vertices.push_back(vertex);
		}
		break;

		// example: f 1/1/1 2/2/2 3/3/3 
		case OBJ_PARAM::POLY_FACE: {
			PolyFace face;
			int matches = sscanf_s(line.data(), "f %d/%d/%d %d/%d/%d %d/%d/%d",
				&face.indices[0].vertexIndex, &face.indices[0].texCoordIndex, &face.indices[0].normalIndex,
				&face.indices[1].vertexIndex, &face.indices[1].texCoordIndex, &face.indices[1].normalIndex,
				&face.indices[2].vertexIndex, &face.indices[2].texCoordIndex, &face.indices[2].normalIndex);

			assert(matches == 9);
			model->polyFaces.push_back(face);
		}
		break;

		// example: vn -0.96 -1.2 2.2
		case OBJ_PARAM::VERTEX_NORMAL: { 
			Vector3 normal;
			int matches = sscanf_s(line.data(), "vn %f %f %f", &normal.x, &normal.y, &normal.z);

			assert(matches == 3);
			model->vertexNormals.push_back(normal);
		}
		break;
			
		// example: vt 2.0 2.0
		case OBJ_PARAM::TEX_COORD: {
			Vector3 coord;
			int matches = sscanf_s(line.data(), "vt %f %f", &coord.x, &coord.y);
			
			assert(matches == 2);
			model->textureCoords.push_back(coord);
		}
		break;
	}
}

fm::ObjModelLoader::OBJ_PARAM fm::ObjModelLoader::getParam(std::string & line)
{
	// vertex
	if (line.find("#") != std::string::npos) {
		return OBJ_PARAM::UNKNOWN;
	}
	else if (line.find("v ") != std::string::npos) {
		return OBJ_PARAM::VERTEX;
	} 
	else if (line.find("vt") != std::string::npos) {
		return OBJ_PARAM::TEX_COORD;
	}
	else if (line.find("vn") != std::string::npos) {
		return OBJ_PARAM::VERTEX_NORMAL;
	}
	else if (line.find("f ") != std::string::npos) {
		return OBJ_PARAM::POLY_FACE;
	}

	return OBJ_PARAM::UNKNOWN;
}

fm::ObjModel* fm::loadObjModel(std::string filepath)
{
	ObjModelLoader objLoader;
	ObjModel* model = objLoader.load(filepath);
	return model;
}

void fm::switchModelUvs(ObjModel * model)
{
	model->switchedUvs = !model->switchedUvs;

	for (auto& uv : model->textureCoords) {
		uv.x = 1.0f - uv.x;
		uv.y = 1.0f - uv.y;
	}
}

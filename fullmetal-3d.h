/*
 * Utilities for loading and handling 3D models.
 */

#pragma once
#include <string>
#include <vector>

namespace fm {
	class Vector3;

	/*
	 * 
	 */
	struct PolyFace {
		struct Indexes {
			Indexes();

			int vertexIndex;
			int normalIndex;
			int texCoordIndex;
		};

		PolyFace();

		Indexes indices[3];
	};

	/*
	 * 3D model data parsed from a .obj file.
	 */
	struct ObjModel {
	public:
		/*
		 * The filepath of the obj model.
		 */
		std::string filepath;

		std::vector<Vector3> vertices;
		std::vector<Vector3> vertexNormals;
		std::vector<Vector3> textureCoords;
		std::vector<PolyFace> polyFaces;
	};

	/*
	 * Utility for loading an ObjModel.
	 */
	class ObjModelLoader {
	public:
		enum OBJ_PARAM {
			VERTEX,
			VERTEX_NORMAL,
			TEX_COORD,
			POLY_FACE,
			UNKNOWN
		};


		ObjModel* load(std::string fp);

	private:
		void readLine(std::string& line, ObjModel* model);
		OBJ_PARAM getParam(std::string& line);
	};

	/*
	 * Loads an object model.
	 * Returns nullptr on failure to load.
	 */
	ObjModel* loadObjModel(std::string filepath);
}
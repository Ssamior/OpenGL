#pragma once
#include "../types.h"
#include "model.h"
#include "mesh.h"
#include "camera.h"
#include "shader.h"

class aiNode;
class aiMesh;
class aiMaterial;
class aiScene;

class Model
{
	public:
		Model();

		bool Load(const string& path);
		void Draw(const Camera& cam, const glm::mat4& model, const glm::vec3& lightPos, const glm::vec3& lightColor, bool bForceAmbient=false);
		const vector<Mesh>& getMeshs() const;

	private:
		vector<Mesh>			m_meshes;
		string					m_directory;
		map<string, Texture>	m_loaded_textures;
		Shader					m_ShaderColorPhong;
		Shader					m_ShaderColorAmbient;
		Shader					m_ShaderTextureDiffuse;
		Shader					m_ShaderTextureAmbient;

		void processNodes(const aiNode* node, const aiScene* scene);
		void processMesh(const aiMesh* mesh, const aiScene* scene);
		vector<Texture> loadMaterialTextures(aiMaterial* mat, int aiTexType, const string& type_name);
};

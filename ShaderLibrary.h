#ifndef SHADER_LIBRARY_H
#define SHADER_LIBRARY_H

#include <unordered_map>
#include <experimental/filesystem>

#include "Shader.h"
#include <thread>

typedef std::experimental::filesystem::path Path;

class ShaderLibrary
{
public:
	explicit ShaderLibrary(Path& path);
	~ShaderLibrary();

	void WatchShader(std::string id);
	void WatchShader(std::string id, const std::vector<const GLchar*>& feedbackVaryings);
	void WatchTesselationShader(std::string id);
	Shader* GetShader(std::string id) const;
	void UnloadShader(std::string id);

	void WatchDirectory();

	void Update(void);

protected:
	bool m_isWatching;
	std::unordered_map<std::string, Shader*> m_loadedShaders;
	std::unordered_map<std::string, bool> m_dirtyShaders;
	std::unordered_map<std::string, std::experimental::filesystem::file_time_type> m_lastModifiedDates;
	std::unordered_map<std::string, std::vector<const GLchar*>> m_shaderFeedbackVaryings;
	std::unordered_map<std::string, bool> m_isTesselationShader;
	std::thread* m_watcherThread;
	Path m_path;

	void AddShader(std::string id);
	void AddShader(std::string id, const std::string vertexPath, const std::string secondPath, Shader::ShaderType::Enum secondShaderType);
	void AddShader(std::string id, const std::string vertexPath, const std::string fragmentPath, const std::string geometryPath);
	void AddShader(std::string id, const std::string vertexPath, const std::string tcsPath, const std::string tesPath, const std::string fragmentPath);
};

#endif

#include "ShaderLibrary.h"

ShaderLibrary::ShaderLibrary(Path& path)
	: m_isWatching(true)
	, m_path(path)
{
	m_watcherThread = new std::thread(&ShaderLibrary::WatchDirectory, this);
}

ShaderLibrary::~ShaderLibrary()
{
	// stop watching
	m_isWatching = false;
	if(m_watcherThread && m_watcherThread->joinable())
	{
		m_watcherThread->join();
	}

	// delete shaders
	for(auto kv : m_loadedShaders)
	{
		delete kv.second;
	}
}

void ShaderLibrary::WatchShader(std::string id)
{
	// shader files
	std::string shaderFilePaths[3] = {
		(m_path / (id + ".vert")).string(),
		(m_path / (id + ".frag")).string(),
		(m_path / (id + ".geom")).string()
	};

	// get the highest last modified date of the shaders
	std::experimental::filesystem::file_time_type highestLastMadified;
	for (unsigned int i = 0u; i < 3; ++i)
	{
		if (std::experimental::filesystem::exists(shaderFilePaths[i]))
		{
			std::experimental::filesystem::file_time_type modified = std::experimental::filesystem::last_write_time(shaderFilePaths[i]);
			if (m_lastModifiedDates[id] < modified)
			{
				if (highestLastMadified < modified)
					highestLastMadified = modified;
			}
		}
	}

	m_lastModifiedDates[id] = highestLastMadified;
	m_dirtyShaders[id] = true;
}

void ShaderLibrary::WatchShader(std::string id, const std::vector<const GLchar*>& feedbackVaryings)
{
	m_shaderFeedbackVaryings[id] = feedbackVaryings;

	WatchShader(id);
}

void ShaderLibrary::WatchTesselationShader(std::string id)
{
	// shader files
	std::string shaderFilePaths[4] = {
		(m_path / (id + ".vert")).string(),
		(m_path / (id + ".tcs")).string(),
		(m_path / (id + ".tes")).string(),
		(m_path / (id + ".frag")).string()
	};

	// get the highest last modified date of the shaders
	std::experimental::filesystem::file_time_type highestLastMadified;
	for (unsigned int i = 0u; i < 3; ++i)
	{
		if (std::experimental::filesystem::exists(shaderFilePaths[i]))
		{
			std::experimental::filesystem::file_time_type modified = std::experimental::filesystem::last_write_time(shaderFilePaths[i]);
			if (m_lastModifiedDates[id] < modified)
			{
				if (highestLastMadified < modified)
					highestLastMadified = modified;
			}
		}
	}

	m_isTesselationShader[id] = true;
	m_lastModifiedDates[id] = highestLastMadified;
	m_dirtyShaders[id] = true;
}

void ShaderLibrary::AddShader(std::string id)
{
	m_dirtyShaders[id] = false;

	if(m_isTesselationShader[id])
	{
		std::string vertexPath = (m_path / (id + ".vert")).string();
		std::string tcsPath = (m_path / (id + ".tcs")).string();
		std::string tesPath = (m_path / (id + ".tes")).string();
		std::string fragementPath = (m_path / (id + ".frag")).string();

		AddShader(id, vertexPath, tcsPath, tesPath, fragementPath);
		return;
	}

	std::string vertexPath = (m_path / (id + ".vert")).string();
	std::string fragementPath = (m_path / (id + ".frag")).string();
	std::string geometryPath = (m_path / (id + ".geom")).string();

	bool fragmentExists = std::experimental::filesystem::exists(fragementPath);
	bool geometryExists = std::experimental::filesystem::exists(geometryPath);

	if(geometryExists && fragmentExists)
	{
		AddShader(id, vertexPath, fragementPath, geometryPath);
	}
	else if(geometryExists)
	{
		AddShader(id, vertexPath, geometryPath, Shader::ShaderType::Geometry);
	}
	else if(fragmentExists)
	{
		AddShader(id, vertexPath, fragementPath, Shader::ShaderType::Fragment);
	}
}

void ShaderLibrary::AddShader(std::string id, const std::string vertexPath, const std::string secondPath, Shader::ShaderType::Enum secondShaderType)
{
	// create new shader and add it to the list
	if(m_shaderFeedbackVaryings.find(id) != m_shaderFeedbackVaryings.end())
	{
		m_loadedShaders[id] = new Shader(vertexPath, secondPath, secondShaderType, &m_shaderFeedbackVaryings[id][0], m_shaderFeedbackVaryings[id].size());
	}
	else
	{
		m_loadedShaders[id] = new Shader(vertexPath, secondPath, secondShaderType);
	}
}

void ShaderLibrary::AddShader(std::string id, const std::string vertexPath, const std::string fragmentPath, const std::string geometryPath)
{
	// create new shader and add it to the list
	if (m_shaderFeedbackVaryings.find(id) != m_shaderFeedbackVaryings.end())
	{
		m_loadedShaders[id] = new Shader(vertexPath, fragmentPath, geometryPath, &m_shaderFeedbackVaryings[id][0], m_shaderFeedbackVaryings[id].size());
	}
	else
	{
		m_loadedShaders[id] = new Shader(vertexPath, fragmentPath, geometryPath);
	}
}

void ShaderLibrary::AddShader(std::string id, const std::string vertexPath, const std::string tcsPath, const std::string tesPath, const std::string fragmentPath)
{
	m_loadedShaders[id] = new Shader(vertexPath, tcsPath, tesPath, fragmentPath);
}

Shader* ShaderLibrary::GetShader(std::string id) const
{
	return m_loadedShaders.at(id);
}

void ShaderLibrary::UnloadShader(std::string id)
{
	m_loadedShaders.erase(id);
}

void ShaderLibrary::WatchDirectory()
{
	while(m_isWatching)
	{
		for (auto it = m_loadedShaders.begin(); it != m_loadedShaders.end(); ++it)
		{
			std::string id = it->first;
			std::string shaderFilePaths[3] = {
				(m_path / (id + ".vert")).string(), 
				(m_path / (id + ".frag")).string(), 
				(m_path / (id + ".geom")).string()
			};

			// check all shader files
			bool shaderDirty = false;
			std::experimental::filesystem::file_time_type highistLastMadified;
			for(unsigned int i = 0u; i < 3; ++i)
			{
				if (std::experimental::filesystem::exists(shaderFilePaths[i]))
				{
					std::experimental::filesystem::file_time_type modified = std::experimental::filesystem::last_write_time(shaderFilePaths[i]);
					if (m_lastModifiedDates[id] < modified)
					{
						shaderDirty = true;

						if (highistLastMadified < modified)
							highistLastMadified = modified;
					}
				}
			}

			if(shaderDirty)
			{
				std::cout << "SHADERLIB::SHADER_CHANGED: " << id << std::endl;

				m_lastModifiedDates[id] = highistLastMadified;
				m_dirtyShaders[id] = shaderDirty;
			}
		}

		// wait at least 1 second before checking again
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

void ShaderLibrary::Update()
{
	for(auto it = m_dirtyShaders.begin(); it != m_dirtyShaders.end(); ++it)
	{
		if(it->second)
		{
			std::cout << "SHADERLIB::COMPILING_SHADER: " << it->first << std::endl;

			if(m_loadedShaders[it->first] != nullptr)
				delete m_loadedShaders[it->first];

			AddShader(it->first);
		}
	}
}

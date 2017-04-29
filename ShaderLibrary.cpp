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
	std::experimental::filesystem::file_time_type highistLastMadified;
	for (unsigned int i = 0u; i < 3; ++i)
	{
		if (std::experimental::filesystem::exists(shaderFilePaths[i]))
		{
			std::experimental::filesystem::file_time_type modified = std::experimental::filesystem::last_write_time(shaderFilePaths[i]);
			if (m_lastModifiedDates[id] < modified)
			{
				if (highistLastMadified < modified)
					highistLastMadified = modified;
			}
		}
	}

	m_lastModifiedDates[id] = highistLastMadified;
	m_dirtyShaders[id] = true;
}

void ShaderLibrary::AddShader(std::string id)
{
	m_dirtyShaders[id] = false;

	std::string vertexPath = (m_path / (id + ".vert")).string();
	std::string fragementPath = (m_path / (id + ".frag")).string();
	std::string geometryPath = (m_path / (id + ".geom")).string();

	if(std::experimental::filesystem::exists(geometryPath))
	{
		AddShader(id, vertexPath, fragementPath, geometryPath);
	}
	else
	{
		AddShader(id, vertexPath, fragementPath);
	}
}

void ShaderLibrary::AddShader(std::string id, const std::string vertexPath, const std::string fragmentPath)
{
	// create new shader and add it to the list
	m_loadedShaders[id] = new Shader(vertexPath, fragmentPath);
}

void ShaderLibrary::AddShader(std::string id, const std::string vertexPath, const std::string fragmentPath, const std::string geometryPath)
{
	// create new shader and add it to the list
	m_loadedShaders[id] = new Shader(vertexPath, fragmentPath, geometryPath);
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

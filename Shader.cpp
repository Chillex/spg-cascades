#include "Shader.h"

Shader::Shader(std::string vertexPath, std::string secondShaderPath, ShaderType::Enum secondShaderType)
{
	// read shader files
	const std::string vertexShaderCode = this->GetShaderCode(vertexPath.c_str());
	const std::string secondShaderCode = this->GetShaderCode(secondShaderPath.c_str());

	// create shaders
	GLuint vertexShader = this->CreateVertexShader(vertexShaderCode.c_str());
	GLuint secondShader;
	if(secondShaderType == ShaderType::Geometry)
	{
		secondShader = this->CreateGeometryShader(secondShaderCode.c_str());
	}
	else
	{
		secondShader = this->CreateFragmentShader(secondShaderCode.c_str());
	}

	// link the shaders together
	this->program = glCreateProgram();

	glAttachShader(this->program, vertexShader);
	glAttachShader(this->program, secondShader);
	glLinkProgram(this->program);

	CheckLinkStatus();

	// shaders are no longer needed after the program is created
	glDeleteShader(vertexShader);
	glDeleteShader(secondShader);
}

Shader::Shader(std::string vertexPath, std::string secondShaderPath, ShaderType::Enum secondShaderType, const GLchar** feedbackVaryings, const GLuint feedbackCount)
{
	// read shader files
	const std::string vertexShaderCode = this->GetShaderCode(vertexPath.c_str());
	const std::string secondShaderCode = this->GetShaderCode(secondShaderPath.c_str());

	// create shaders
	GLuint vertexShader = this->CreateVertexShader(vertexShaderCode.c_str());
	GLuint secondShader;
	if (secondShaderType == ShaderType::Geometry)
	{
		secondShader = this->CreateGeometryShader(secondShaderCode.c_str());
	}
	else
	{
		secondShader = this->CreateFragmentShader(secondShaderCode.c_str());
	}

	// link the shaders together
	this->program = glCreateProgram();

	glAttachShader(this->program, vertexShader);
	glAttachShader(this->program, secondShader);

	// add feedback varyings
	glTransformFeedbackVaryings(this->program, feedbackCount, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);

	glLinkProgram(this->program);

	CheckLinkStatus();

	// shaders are no longer needed after the program is created
	glDeleteShader(vertexShader);
	glDeleteShader(secondShader);
}

Shader::Shader(std::string vertexPath, std::string fragmentPath, std::string geometryPath)
{
	// read shader files
	const std::string vertexShaderCode = this->GetShaderCode(vertexPath.c_str());
	const std::string geometryShaderCode = this->GetShaderCode(geometryPath.c_str());
	const std::string fragmentShaderCode = this->GetShaderCode(fragmentPath.c_str());

	// create shaders
	GLuint vertexShader = this->CreateVertexShader(vertexShaderCode.c_str());
	GLuint geometryShader = this->CreateGeometryShader(geometryShaderCode.c_str());
	GLuint fragmentShader = this->CreateFragmentShader(fragmentShaderCode.c_str());

	// link the shaders together
	this->program = glCreateProgram();

	glAttachShader(this->program, vertexShader);
	glAttachShader(this->program, geometryShader);
	glAttachShader(this->program, fragmentShader);
	glLinkProgram(this->program);

	CheckLinkStatus();

	// shaders are no longer needed after the program is created
	glDeleteShader(vertexShader);
	glDeleteShader(geometryShader);
	glDeleteShader(fragmentShader);
}

Shader::Shader(std::string vertexPath, std::string fragmentPath, std::string geometryPath, const GLchar** feedbackVaryings, const GLuint feedbackCount)
{
	// read shader files
	const std::string vertexShaderCode = this->GetShaderCode(vertexPath.c_str());
	const std::string geometryShaderCode = this->GetShaderCode(geometryPath.c_str());
	const std::string fragmentShaderCode = this->GetShaderCode(fragmentPath.c_str());

	// create shaders
	GLuint vertexShader = this->CreateVertexShader(vertexShaderCode.c_str());
	GLuint geometryShader = this->CreateGeometryShader(geometryShaderCode.c_str());
	GLuint fragmentShader = this->CreateFragmentShader(fragmentShaderCode.c_str());

	// link the shaders together
	this->program = glCreateProgram();

	glAttachShader(this->program, vertexShader);
	glAttachShader(this->program, geometryShader);
	glAttachShader(this->program, fragmentShader);

	// add feedback varyings
	glTransformFeedbackVaryings(this->program, feedbackCount, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);

	glLinkProgram(this->program);

	CheckLinkStatus();

	// shaders are no longer needed after the program is created
	glDeleteShader(vertexShader);
	glDeleteShader(geometryShader);
	glDeleteShader(fragmentShader);
}

Shader::Shader(std::string vertexPath, std::string tcsPath, std::string tesPath, std::string fragmentPath)
{
	// read shader files
	const std::string vertexShaderCode = this->GetShaderCode(vertexPath.c_str());
	const std::string tcsShaderCode = this->GetShaderCode(tcsPath.c_str());
	const std::string tesShaderCode = this->GetShaderCode(tesPath.c_str());
	const std::string fragmentShaderCode = this->GetShaderCode(fragmentPath.c_str());

	// create shaders
	GLuint vertexShader = this->CreateVertexShader(vertexShaderCode.c_str());
	GLuint tcsShader = this->CreateTCSShader(tcsShaderCode.c_str());
	GLuint tesShader = this->CreateTESShader(tesShaderCode.c_str());
	GLuint fragmentShader = this->CreateFragmentShader(fragmentShaderCode.c_str());

	// link the shaders together
	this->program = glCreateProgram();

	glAttachShader(this->program, vertexShader);
	glAttachShader(this->program, tcsShader);
	glAttachShader(this->program, tesShader);
	glAttachShader(this->program, fragmentShader);
	glLinkProgram(this->program);

	CheckLinkStatus();

	// shaders are no longer needed after the program is created
	glDeleteShader(vertexShader);
	glDeleteShader(tcsShader);
	glDeleteShader(tesShader);
	glDeleteShader(fragmentShader);
}

Shader::~Shader()
{
}

void Shader::CheckLinkStatus(void)
{
	GLint success;
	GLchar infoLog[512];

	glGetProgramiv(this->program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM: linking failed" << std::endl << infoLog << std::endl;
	}
}

void Shader::Use()
{
	glUseProgram(this->program);
}

const std::string Shader::GetShaderCode(const char* path)
{
	std::string shaderCode;
	std::ifstream shaderFile;

	// allow ifstream to throw exceptions
	shaderFile.exceptions(std::ifstream::badbit);
	try
	{
		shaderFile.open(path);

		std::stringstream shaderStream;
		shaderStream << shaderFile.rdbuf();

		shaderFile.close();

		shaderCode = shaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
	}

	return shaderCode;
}

GLuint Shader::CreateShader(GLuint shaderType, const GLchar* shaderSource, const char* errorType)
{
	GLint success;
	GLchar infoLog[512];

	// create and compile shader
	GLuint shader;
	shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderSource, NULL);
	glCompileShader(shader);

	// check for compilation errors
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::" << errorType << ": could not compile shader" << std::endl << infoLog << std::endl;
	}

	return shader;
}

GLuint Shader::CreateVertexShader(const GLchar* shaderSource)
{
	return CreateShader(GL_VERTEX_SHADER, shaderSource, "VERTEX");
}

GLuint Shader::CreateFragmentShader(const GLchar* shaderSource)
{
	return CreateShader(GL_FRAGMENT_SHADER, shaderSource, "FRAGMENT");
}

GLuint Shader::CreateGeometryShader(const GLchar* shaderSource)
{
	return CreateShader(GL_GEOMETRY_SHADER, shaderSource, "GEOMETRY");
}

GLuint Shader::CreateTCSShader(const GLchar* shaderSource)
{
	return CreateShader(GL_TESS_CONTROL_SHADER, shaderSource, "TCS");
}

GLuint Shader::CreateTESShader(const GLchar* shaderSource)
{
	return CreateShader(GL_TESS_EVALUATION_SHADER, shaderSource, "TES");
}

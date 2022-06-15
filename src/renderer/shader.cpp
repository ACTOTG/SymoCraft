#include "renderer/shader.h"

bool Shader::compile(ShaderType type, std::string_view shaderFilepath)
{
	// Report compilation
	std::cout << "Compiling shader : " << shaderFilepath << '\n';

	// Read the shader source code from the file
	std::ifstream shader_file(shaderFilepath.data(), std::ios::in | std::ios::binary);
	std::stringstream shader_stream;
	std::string shader_source_code;
	if( shader_file )
	{
		shader_stream << shader_file.rdbuf();
		shader_source_code = shader_stream.str();
	}
	else
	{
		std::cout << "Could not open file: " << shaderFilepath << '\n';
	}


	GLenum shaderType = toGlShaderType(type);
	if (shaderType == GL_INVALID_ENUM) {
		std::cerr << ("ShaderType is unknown");
		return false;
	}

	// Create an empty shader handle
	shaderId = glCreateShader(shaderType);

	// Send the shader source code to GL
	const GLchar* sourceCStr = shader_source_code.c_str();
	glShaderSource(shaderId, 1, &sourceCStr, nullptr);

	// Compile the shader
	glCompileShader(shaderId);

	// Check if the compilation succeeded
	GLint isCompiled = 0;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		// If compilation failed, find out why and log the error
		GLint maxLength = 0;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(shaderId, maxLength, &maxLength, &infoLog[0]);

		// We don't need the shader anymore if compilation failed
		glDeleteShader(shaderId);

		printf("Shader Compilation failed: \n%s", infoLog.data());

		shaderId = UINT32_MAX;
		return false;
	}

	return true;
}

void Shader::destroy()
{
	if (shaderId != UINT32_MAX)
	{
		glDeleteShader(shaderId);
		shaderId = UINT32_MAX;
	}
}


GLenum Shader::toGlShaderType(ShaderType type)
{
	switch (type)
	{
	case ShaderType::Vertex:
		return GL_VERTEX_SHADER;
	case ShaderType::Fragment:
		return GL_FRAGMENT_SHADER;
	}
	return GL_INVALID_ENUM;
}

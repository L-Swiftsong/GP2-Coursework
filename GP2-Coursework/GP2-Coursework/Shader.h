#pragma once
#include <string>
#include <GL\glew.h>
#include "transform.h"
#include "Camera.h"
#include <iostream>
#include <fstream>

class Shader
{
public:
	Shader(const std::string& vertex_file, const std::string& fragment_file);

	void Bind(); //Set gpu to use our shaders
	void Update(const Transform& transform, const Camera& camera);

	std::string Shader::LoadShader(const std::string& fileName);
	void Shader::CheckShaderError(GLuint shader, GLuint flag, bool is_program, const std::string& error_message);
	GLuint Shader::CreateShader(const std::string& text, unsigned int type);

    ~Shader();


	/*template <typename T> void setParam(const std::string& name, T& value) const
	{
		glUniform(glGetUniformLocation(shaderID, name.c_str()), &value);

		if ((glGetUniformLocation(shaderID, name.c_str()) == -1))
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}*/
	// ------------------------------------------------------------------------
	void set_bool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(shader_id_, name.c_str()), (int)value);

		if ((glGetUniformLocation(shader_id_, name.c_str()) == -1))
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	// ------------------------------------------------------------------------
	void set_int(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(shader_id_, name.c_str()), value);

		if ((glGetUniformLocation(shader_id_, name.c_str()) == -1))
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	// ------------------------------------------------------------------------
	void set_float(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(shader_id_, name.c_str()), value);

		//if ((glGetUniformLocation(shaderID, name.c_str()) == -1))
		//{
		//	std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
		//	__debugbreak();
		//}
	}
	// ------------------------------------------------------------------------
	void set_vec_2(const std::string& name, const glm::vec2& value) const
	{
		glUniform2fv(glGetUniformLocation(shader_id_, name.c_str()), 1, &value[0]);

		if ((glGetUniformLocation(shader_id_, name.c_str()) == -1))
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	void set_vec_2(const std::string& name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(shader_id_, name.c_str()), x, y);

		if ((glGetUniformLocation(shader_id_, name.c_str()) == -1))
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	// ------------------------------------------------------------------------
	void set_vec_3(const std::string& name, const glm::vec3& value) const
	{
		glUniform3fv(glGetUniformLocation(shader_id_, name.c_str()), 1, &value[0]);

		if ((glGetUniformLocation(shader_id_, name.c_str()) == -1))
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	void set_vec_3(const std::string& name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(shader_id_, name.c_str()), x, y, z);

		if ((glGetUniformLocation(shader_id_, name.c_str()) == -1))
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	// ------------------------------------------------------------------------
	void set_vec_4(const std::string& name, const glm::vec4& value) const
	{
		glUniform4fv(glGetUniformLocation(shader_id_, name.c_str()), 1, &value[0]);

		if ((glGetUniformLocation(shader_id_, name.c_str()) == -1))
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	void set_vec_4(const std::string& name, float x, float y, float z, float w)
	{
		glUniform4f(glGetUniformLocation(shader_id_, name.c_str()), x, y, z, w);

		if ((glGetUniformLocation(shader_id_, name.c_str()) == -1))
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	// ------------------------------------------------------------------------
	void set_mat_2(const std::string& name, const glm::mat2& mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(shader_id_, name.c_str()), 1, GL_FALSE, &mat[0][0]);

		if ((glGetUniformLocation(shader_id_, name.c_str()) == -1))
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	// ------------------------------------------------------------------------
	void set_mat_3(const std::string& name, const glm::mat3& mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(shader_id_, name.c_str()), 1, GL_FALSE, &mat[0][0]);

		if ((glGetUniformLocation(shader_id_, name.c_str()) == -1))
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	// ------------------------------------------------------------------------
	void set_mat_4(const std::string& name, const glm::mat4& mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(shader_id_, name.c_str()), 1, GL_FALSE, &mat[0][0]);

		if ((glGetUniformLocation(shader_id_, name.c_str()) == -1))
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}


protected:
private:
	Shader() = delete;

	static const unsigned int kNumShaders = 2; // Number of shaders.
	enum
	{
		kMVPMatrix,
		kModelMatrix,
		kViewMatrix,
		kProjectionMatrix,

		kNumTransforms
	};


	// Track the shader program.
	GLuint shader_id_;

	// Array of our shaders.
	GLuint shaders_[kNumShaders];
	
	// Number of uniform variables.
	GLuint uniforms_[kNumTransforms];
};

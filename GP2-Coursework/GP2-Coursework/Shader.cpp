#include "Shader.h"

Shader::Shader(const std::string& vertex_file, const std::string& fragment_file)
{
	shader_id_ = glCreateProgram(); // create shader program (openGL saves as ref number)
	shaders_[0] = CreateShader(LoadShader(vertex_file), GL_VERTEX_SHADER); // create vertex shader
	shaders_[1] = CreateShader(LoadShader(fragment_file), GL_FRAGMENT_SHADER); // create fragment shader

	for (unsigned int i = 0; i < kNumShaders; i++)
	{
		glAttachShader(shader_id_, shaders_[i]); //add all our shaders to the shader program "shaders" 
	}

	glBindAttribLocation(shader_id_, 0, "VertexPosition"); // associate attribute variable with our shader program attribute (in this case attribute vec3 position;)
	glBindAttribLocation(shader_id_, 1, "VertexTexCoord");
	glBindAttribLocation(shader_id_, 2, "VertexNormal");

	glLinkProgram(shader_id_); //create executables that will run on the GPU shaders
	CheckShaderError(shader_id_, GL_LINK_STATUS, true, "Error: Shader program linking failed"); // cheack for error

	glValidateProgram(shader_id_); //check the entire program is valid
	CheckShaderError(shader_id_, GL_VALIDATE_STATUS, true, "Error: Shader program not valid");

	// Associate with the location of uniform variable within a program.
	uniforms_[kMVPMatrix] = glGetUniformLocation(shader_id_, "transform");
	uniforms_[kModelMatrix] = glGetUniformLocation(shader_id_, "modelMatrix");
	uniforms_[kViewMatrix] = glGetUniformLocation(shader_id_, "viewMatrix");
	uniforms_[kProjectionMatrix] = glGetUniformLocation(shader_id_, "projectionMatrix");
}
Shader::~Shader()
{
	for (unsigned int i = 0; i < kNumShaders; i++)
	{
		glDetachShader(shader_id_, shaders_[i]); //detach shader from program
		glDeleteShader(shaders_[i]); //delete the sahders
	}
	glDeleteProgram(shader_id_); // delete the program
}


void Shader::Bind()
{
	glUseProgram(shader_id_); //installs the program object specified by program as part of rendering state
}

void Shader::Update(const Transform& transform, const Camera& camera)
{
	glm::mat4 mvp = camera.get_view_projection() * transform.get_model();
	glUniformMatrix4fv(uniforms_[kMVPMatrix], 1, GLU_FALSE, &mvp[0][0]);

	glUniformMatrix4fv(uniforms_[kModelMatrix], 1, GLU_FALSE, &transform.get_model()[0][0]);
	glUniformMatrix4fv(uniforms_[kViewMatrix], 1, GLU_FALSE, &camera.get_view()[0][0]);
	glUniformMatrix4fv(uniforms_[kProjectionMatrix], 1, GLU_FALSE, &camera.get_projection()[0][0]);
}


void Shader::SetShaderMatrix(const int& matrix_index, const glm::mat4& matrix_value)
{
	glUniformMatrix4fv(uniforms_[matrix_index], 1, GLU_FALSE, &matrix_value[0][0]);
}



GLuint Shader::CreateShader(const std::string& text, unsigned int type)
{
	GLuint shader = glCreateShader(type); //create shader based on specified type

	if (shader == 0) //if == 0 shader no created
		std::cerr << "Error type creation failed " << type << std::endl;

	const GLchar* stringSource[1]; //convert strings into list of c-strings
	stringSource[0] = text.c_str();
	GLint lengths[1];
	lengths[0] = text.length();

	glShaderSource(shader, 1, stringSource, lengths); //send source code to opengl
	glCompileShader(shader); //get open gl to compile shader code

	CheckShaderError(shader, GL_COMPILE_STATUS, false, "Error compiling shader!"); //check for compile error

	return shader;
}

std::string Shader::LoadShader(const std::string& file_name)
{
	std::ifstream file;
	file.open((file_name).c_str());

	std::string output;
	std::string line;

	if (file.is_open())
	{
		while (file.good())
		{
			getline(file, line);
			output.append(line + "\n");
		}
	}
	else
	{
		std::cerr << "Unable to load shader: " << file_name << std::endl;
	}

	return output;
}

void Shader::CheckShaderError(GLuint shader, GLuint flag, bool is_program, const std::string& error_message)
{
	GLint success = 0;
	GLchar error[1024] = { 0 };

	if (is_program)
		glGetProgramiv(shader, flag, &success);
	else
		glGetShaderiv(shader, flag, &success);

	if (success == GL_FALSE)
	{
		if (is_program)
			glGetProgramInfoLog(shader, sizeof(error), NULL, error);
		else
			glGetShaderInfoLog(shader, sizeof(error), NULL, error);

		std::cerr << error_message << ": '" << error << "'" << std::endl;
	}
}


// headers
#include "main.h"
#include "scene-triangle.h"

// scene variable
Scene *SceneSquare;

namespace square
{
	// scene state
	GLuint gShaderProgramObject;

	GLuint vaoSquare;
	GLuint vboPositionSquare;
	GLuint vboColorSquare;
	GLuint mvpMatrixUniform;

	mat4 perspectiveProjectionMatrix;

	float angleSquare = 0.0f;

	bool Init(void)
	{
		//// vertex shader
		// create shader
		GLuint gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

		// provide source code to shader
		const GLchar* vertexShaderSourceCode =
			"#version 450 core \n" \

			"in vec4 vPosition; \n" \
			"in vec4 vColor; \n" \

			"uniform mat4 u_mvpMatrix; \n" \

			"out vec4 out_Color; \n" \

			"void main (void) \n" \
			"{ \n" \
			"	gl_Position = u_mvpMatrix * vPosition; \n" \
			"	out_Color = vColor; \n" \
			"} \n";

		glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

		// compile shader
		glCompileShader(gVertexShaderObject);

		// compilation errors 
		GLint iShaderCompileStatus = 0;
		GLint iInfoLogLength = 0;
		GLchar* szInfoLog = NULL;

		glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
		if (iShaderCompileStatus == GL_FALSE)
		{
			glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
			if (iInfoLogLength > 0)
			{
				szInfoLog = (GLchar*)malloc(iInfoLogLength);
				if (szInfoLog != NULL)
				{
					GLsizei written;
					glGetShaderInfoLog(gVertexShaderObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);

					fprintf(gpFile, "Vertex Shader Compiler Info Log: \n%s\n", szInfoLog);
					free(szInfoLog);
					return false;
				}
			}
		}

		//// fragment shader
		// create shader
		GLuint gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

		// provide source code to shader
		const GLchar* fragmentShaderSourceCode =
			"#version 450 core \n" \

			"in vec4 out_Color; \n" \
			"out vec4 FragColor; \n" \

			"void main (void) \n" \
			"{ \n" \
			"	FragColor = out_Color; \n" \
			"} \n";

		glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

		// compile shader
		glCompileShader(gFragmentShaderObject);

		// compile errors
		iShaderCompileStatus = 0;
		iInfoLogLength = 0;
		szInfoLog = NULL;

		glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
		if (iShaderCompileStatus == GL_FALSE)
		{
			glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
			if (iInfoLogLength > 0)
			{
				szInfoLog = (GLchar*)malloc(iInfoLogLength);
				if (szInfoLog != NULL)
				{
					GLsizei written;
					glGetShaderInfoLog(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);

					fprintf(gpFile, "Fragment Shader Compiler Info Log: \n%s\n", szInfoLog);
					free(szInfoLog);
					return false;
				}
			}
		}

		//// shader program
		// create
		gShaderProgramObject = glCreateProgram();

		// attach shaders
		glAttachShader(gShaderProgramObject, gVertexShaderObject);
		glAttachShader(gShaderProgramObject, gFragmentShaderObject);

		// pre-linking binding to vertex attribute
		glBindAttribLocation(gShaderProgramObject, RMC_ATTRIBUTE_POSITION, "vPosition");
		glBindAttribLocation(gShaderProgramObject, RMC_ATTRIBUTE_COLOR, "vColor");

		// link shader
		glLinkProgram(gShaderProgramObject);

		// linking errors
		GLint iProgramLinkStatus = 0;
		iInfoLogLength = 0;
		szInfoLog = NULL;

		glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iProgramLinkStatus);
		if (iProgramLinkStatus == GL_FALSE)
		{
			glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
			if (iInfoLogLength > 0)
			{
				szInfoLog = (GLchar*)malloc(iInfoLogLength);
				if (szInfoLog != NULL)
				{
					GLsizei written;
					glGetProgramInfoLog(gShaderProgramObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);

					fprintf(gpFile, ("Shader Program Linking Info Log: \n%s\n"), szInfoLog);
					free(szInfoLog);
					return false;
				}
			}
		}

		// post-linking retrieving uniform locations
		mvpMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_mvpMatrix");

		// position array of square
		const GLfloat squareVertices[] = {
			 1.0f,  1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,
		};

		// color array of square
		const GLfloat squareColors[] = {
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 0.0f
		};

		// create vao for square
		glGenVertexArrays(1, &vaoSquare);
		glBindVertexArray(vaoSquare);

		// create vbo for position
		glGenBuffers(1, &vboPositionSquare);
		glBindBuffer(GL_ARRAY_BUFFER, vboPositionSquare);
		glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(RMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(RMC_ATTRIBUTE_POSITION);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// create vbo for color
		glGenBuffers(1, &vboColorSquare);
		glBindBuffer(GL_ARRAY_BUFFER, vboColorSquare);
		glBufferData(GL_ARRAY_BUFFER, sizeof(squareColors), squareColors, GL_STATIC_DRAW);
		glVertexAttribPointer(RMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(RMC_ATTRIBUTE_COLOR);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);

		// matrix
		perspectiveProjectionMatrix = mat4::identity();
		return true;
	}

	void Uninit(void)
	{
		if (vaoSquare)
		{
			glDeleteVertexArrays(1, &vaoSquare);
			vaoSquare = 0;
		}

		if (vboPositionSquare)
		{
			glDeleteBuffers(1, &vboPositionSquare);
			vboPositionSquare = 0;
		}

		if (vboColorSquare)
		{
			glDeleteBuffers(1, &vboColorSquare);
			vboColorSquare = 0;
		}

		// destroy shader programs
		if (gShaderProgramObject)
		{
			GLsizei shaderCount;
			GLsizei i;

			glUseProgram(gShaderProgramObject);
			glGetProgramiv(gShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);

			GLuint* pShaders = (GLuint*)malloc(shaderCount * sizeof(GLuint));
			if (pShaders)
			{
				glGetAttachedShaders(gShaderProgramObject, shaderCount, &shaderCount, pShaders);

				for (i = 0; i < shaderCount; i++)
				{
					// detach shader
					glDetachShader(gShaderProgramObject, pShaders[i]);

					// delete shader
					glDeleteShader(pShaders[i]);
					pShaders[i] = 0;
				}

				free(pShaders);
			}

			glDeleteProgram(gShaderProgramObject);
			gShaderProgramObject = 0;
			glUseProgram(0);
		}

		// free the scene
		if (SceneSquare) free(SceneSquare);
	}

	void Display(void)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// start using OpenGL program object
		glUseProgram(gShaderProgramObject);

		//declaration of matrices
		mat4 translateMatrix;
		mat4 modelViewMatrix;
		mat4 modelViewProjectionMatrix;

		// intialize above matrices to identity
		translateMatrix = mat4::identity();
		modelViewMatrix = mat4::identity();
		modelViewProjectionMatrix = mat4::identity();

		// transformations
		translateMatrix = translate(0.0f, 0.0f, -3.0f);
		static float a = 0.0f;
		modelViewMatrix = translateMatrix * rotate(angleSquare, 1.0f, 0.0f, 0.0f);

		// do necessary matrix multiplication
		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

		// send necessary matrices to shader in respective uniforms
		glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

		// bind with vao (this will avoid many binding to vbo)
		glBindVertexArray(vaoSquare);

		// draw necessary scene
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		// unbind vao
		glBindVertexArray(0);

		// stop using OpenGL program object
		glUseProgram(0);
	}

	bool Update(float delta)
	{
		angleSquare += 0.000002f * delta;
		if (angleSquare > 180.0f) return true;

		return false;
	}

	void Resize(int width, int height)
	{
		perspectiveProjectionMatrix = vmath::perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
	}

}

Scene *GetSquareScene()
{
	SceneSquare = (Scene*)malloc(sizeof(Scene));

	strcpy_s(SceneSquare->Name, "SquareScene");

	SceneSquare->InitFunc   = square::Init;
	SceneSquare->UninitFunc = square::Uninit;

	SceneSquare->DisplayFunc = square::Display;
	SceneSquare->UpdateFunc  = square::Update;
	SceneSquare->ResizeFunc  = square::Resize;

	return SceneSquare;
}

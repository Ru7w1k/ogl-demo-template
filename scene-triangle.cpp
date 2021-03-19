// headers
#include "main.h"
#include "scene-triangle.h"
#include "logger.h"

// scene variable
Scene *SceneTriangle;

namespace triangle
{
	// scene state
	GLuint gShaderProgramObject;

	GLuint vao;
	GLuint vboPosition;
	GLuint vboColor;
	GLuint mvpMatrixUniform;

	mat4 perspectiveProjectionMatrix;

	float angleTriangle = 0.0f;

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

					LogD("Vertex Shader Compiler Info Log: \n%s", szInfoLog);
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

					LogD("Fragment Shader Compiler Info Log: \n%s", szInfoLog);
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

					LogD(("Shader Program Linking Info Log: \n%s"), szInfoLog);
					free(szInfoLog);
					return false;
				}
			}
		}

		// post-linking retrieving uniform locations
		mvpMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_mvpMatrix");

		// position array
		const GLfloat triangleVertices[] = {
			 0.0f,  1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f
		};

		// color array
		const GLfloat triangleColors[] = {
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f
		};

		// create vao for square
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// create vbo for position
		glGenBuffers(1, &vboPosition);
		glBindBuffer(GL_ARRAY_BUFFER, vboPosition);
		glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(RMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(RMC_ATTRIBUTE_POSITION);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// create vbo for color
		glGenBuffers(1, &vboColor);
		glBindBuffer(GL_ARRAY_BUFFER, vboColor);
		glBufferData(GL_ARRAY_BUFFER, sizeof(triangleColors), triangleColors, GL_STATIC_DRAW);
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
		if (vao)
		{
			glDeleteVertexArrays(1, &vao);
			vao = 0;
		}

		if (vboPosition)
		{
			glDeleteBuffers(1, &vboPosition);
			vboPosition = 0;
		}

		if (vboColor)
		{
			glDeleteBuffers(1, &vboColor);
			vboColor = 0;
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
		if (SceneTriangle) free(SceneTriangle);
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
		modelViewMatrix = translateMatrix * rotate(angleTriangle, 0.0f, 1.0f, 0.0f);

		// do necessary matrix multiplication
		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

		// send necessary matrices to shader in respective uniforms
		glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

		// bind with vao (this will avoid many binding to vbo)
		glBindVertexArray(vao);

		// draw necessary scene
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// unbind vao
		glBindVertexArray(0);

		// stop using OpenGL program object
		glUseProgram(0);
	}

	bool Update(float delta)
	{
		angleTriangle += 0.000002f * delta;
		if (angleTriangle > 90.0f) return true;

		return false;
	}

	void Resize(int width, int height)
	{
		perspectiveProjectionMatrix = vmath::perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
	}
}

Scene *GetTriangleScene()
{
	SceneTriangle = (Scene*)malloc(sizeof(Scene));

	strcpy_s(SceneTriangle->Name, "TriangleScene");

	SceneTriangle->InitFunc   = triangle::Init;
	SceneTriangle->UninitFunc = triangle::Uninit;

	SceneTriangle->DisplayFunc = triangle::Display;
	SceneTriangle->UpdateFunc  = triangle::Update;
	SceneTriangle->ResizeFunc  = triangle::Resize;

	return SceneTriangle;
}

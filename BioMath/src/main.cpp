// DEFINICIONES //

// Contexto OpenGL//
 
// Un contexto de OpenGL es un estado de renderizado integral que encapsula todos los ajustes, 
// recursos (texturas, shaders, buffers) y la configuración de la tubería gráfica para una aplicación 
// específica. Funciona como un "puerto" o hilo de ejecución activo, obligatorio para que los comandos 
// de la GPU surtan efecto en una ventana específica, siguiendo reglas estrictas de un contexto por hilo.

#define WIN32_LEAN_AND_MEAN //Minimal basic WinApi
#include <windows.h>
#include <malloc.h>
#include <gl/GL.h>

#pragma comment(lib, "opengl32.lib")

// ----------------------------
// Declaraciones GL/WGL minimas
// ----------------------------

// Construcción manual de tipos básicos de OpenGL

typedef char GLchar;
typedef ptrdiff_t GLsizeiptr;

// Constantes simbólicas de OpenGL.

#define GL_ARRAY_BUFFER 0x8892 // 0x8892 Un valor entero único que el estándar OpenGL asigna al concepto “array buffer”. (Vertex data).
#define GL_STATIC_DRAW 0x88E4 // Pista (sugerencia) sobre como se va a usar el buffer para el driver.
#define GL_VERTEX_SHADER 0x8B31 // Se usa para crear shaders (un shader de tipo vértice)
#define GL_FRAGMENT_SHADER 0x8B30 // Se usa para crear shaders (un shader de tipo fragment)
#define GL_COMPILE_STATUS 0x8B81 // El estado de compilación de un shader (¿Este shader compiló bien?)
#define GL_LINK_STATUS 0x8B82 // El estado de linkeo de un programa de shaders
#define GL_INFO_LOG_LENGTH 0x8B84 // La longitud del log de errores o warnings
#define GL_TRUE 1 // Los valores booleanos de OpenGL (usa enteros).
#define GL_FALSE 0


// Constantes de la extensión WGL (permite pedir contextos OpenGL modernos)

#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091 // El número de versión mayor del contexto OpenGL que estamos pidiendo
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092 // El número de versión menor del contexto OpenGL
#define WGL_CONTEXT_PROFILE_MASK_ARB  0x9126 // Que perfil de OpenGL quiero
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001 // El perfil core de OpenGL


// Typedef de un puntero a función para el tipo de la función de extensión wglCreateContextAttribsARB, 
// que es la que te permite crear un contexto OpenGL moderno(ej : 3.3 core) en Windows.

typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int*);


// En algunos casos APIENTRYP no puede no estar definido, lo definimos.
// APIENTRY suele corresponder a __stdcall en 32 bits y a "*" en 64 bits. 
// Básicamente define como una función recibe argumentos, quién limpia la pila y cómo se nombra el símbolo a nivel binario.

#ifndef APIENTRYP
	#define APIENTRYP APIENTRY *
#endif

// Typedefs que describen la firma exacta de diversas funciones OpenGL modernas, para poder mas adelante obtener el puntero
// y llamarlas como funciones normales.

// PFNGL...PROC viene de la convención del "registry":
// PFN = Pointer to FuNction
// GL = OpenGL
// ... = nombre de la función
// PROC = procedure(histórico)


// BLoque de shaders:

typedef GLuint(APIENTRYP PFNGLCREATESHADERPROC)(GLenum); // Crea un objeto shader (vertex o fragment) y devuelve un ID (GLuint).
typedef void  (APIENTRYP PFNGLSHADERSOURCEPROC)(GLuint, GLsizei, const GLchar* const*, const GLint*); // Le pasa el código GLSL al shader (se puede pasar 1 o varias strings).
typedef void  (APIENTRYP PFNGLCOMPILESHADERPROC)(GLuint); // Compila el shader.
typedef void  (APIENTRYP PFNGLGETSHADERIVPROC)(GLuint, GLenum, GLint*); // Consulta propiedades, resultado de compilacion
typedef void  (APIENTRYP PFNGLGETSHADERINFOLOGPROC)(GLuint, GLsizei, GLsizei*, GLchar*); // Obtiene el texto del log de compilación
typedef void  (APIENTRYP PFNGLDELETESHADERPROC)(GLuint); // Libera el objeto shader

// Bloque de programas (linkeo de shaders)

typedef GLuint(APIENTRYP PFNGLCREATEPROGRAMPROC)(void); // Crea un "program object".
typedef void  (APIENTRYP PFNGLATTACHSHADERPROC)(GLuint, GLuint); // Adjunta shaders al programa (vertex + fragment).
typedef void  (APIENTRYP PFNGLLINKPROGRAMPROC)(GLuint); // Linkea el programa (verifica compatibilidad de inputs/outputs).
typedef void  (APIENTRYP PFNGLGETPROGRAMIVPROC)(GLuint, GLenum, GLint*); // Igual que con shaders, pero para el link.
typedef void  (APIENTRYP PFNGLGETPROGRAMINFOLOGPROC)(GLuint, GLsizei, GLsizei*, GLchar*);
typedef void  (APIENTRYP PFNGLUSEPROGRAMPROC)(GLuint); // Activa el programa para dibujar.
typedef void  (APIENTRYP PFNGLDELETEPROGRAMPROC)(GLuint); // Libera el programa.

// VAOs (Vertex Array Objects)

typedef void  (APIENTRYP PFNGLGENVERTEXARRAYSPROC)(GLsizei, GLuint*); // Genera IDs de VAOs.
typedef void  (APIENTRYP PFNGLBINDVERTEXARRAYPROC)(GLuint); // Selecciona el VAO actual.
typedef void  (APIENTRYP PFNGLDELETEVERTEXARRAYSPROC)(GLsizei, const GLuint*); // Libera VAOs.

// VBOs / Buffers

typedef void  (APIENTRYP PFNGLGENBUFFERSPROC)(GLsizei, GLuint*); // Genera IDs de buffers.
typedef void  (APIENTRYP PFNGLBINDBUFFERPROC)(GLenum, GLuint); // Bindea un buffer a un target (GL_ARRAY_BUFFER, etc.)
typedef void  (APIENTRYP PFNGLBUFFERDATAPROC)(GLenum, GLsizeiptr, const void*, GLenum); // Reserva y/o copia datos al buffer.
typedef void  (APIENTRYP PFNGLDELETEBUFFERSPROC)(GLsizei, const GLuint*); // Libera buffers.

// Vertex attributes (Le dicen al pipeline cómo leer el VBO para alimentar el vertex shader.)

typedef void  (APIENTRYP PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint); // Habilita un atributo (ej: location 0 para posición).
typedef void  (APIENTRYP PFNGLVERTEXATTRIBPOINTERPROC)(GLuint, GLint, GLenum, GLboolean, GLsizei, const void*); //Define el layout: cantidad de componentes, tipo, stride, offset, etc.

// Uniforms (variables globales para shaders)

typedef GLint(APIENTRYP PFNGLGETUNIFORMLOCATIONPROC)(GLuint, const GLchar*); //Busca la ubicación de un uniform por nombre.
typedef void  (APIENTRYP PFNGLUNIFORM1FPROC)(GLint, GLfloat); //Setea un float.
typedef void  (APIENTRYP PFNGLUNIFORM2FPROC)(GLint, GLfloat, GLfloat); //Setea dos floats (vec2).



// Punteros:
// Donde vamos a guardar la referencia a las funciones previamente definidas, que carguen dinámicamente
static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB_ptr = nullptr;

static PFNGLCREATESHADERPROC glCreateShader_ptr = nullptr;
static PFNGLSHADERSOURCEPROC glShaderSource_ptr = nullptr;
static PFNGLCOMPILESHADERPROC glCompileShader_ptr = nullptr;
static PFNGLGETSHADERIVPROC glGetShaderiv_ptr = nullptr;
static PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog_ptr = nullptr;
static PFNGLDELETESHADERPROC glDeleteShader_ptr = nullptr;

static PFNGLCREATEPROGRAMPROC glCreateProgram_ptr = nullptr;
static PFNGLATTACHSHADERPROC glAttachShader_ptr = nullptr;
static PFNGLLINKPROGRAMPROC glLinkProgram_ptr = nullptr;
static PFNGLGETPROGRAMIVPROC glGetProgramiv_ptr = nullptr;
static PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog_ptr = nullptr;
static PFNGLUSEPROGRAMPROC glUseProgram_ptr = nullptr;
static PFNGLDELETEPROGRAMPROC glDeleteProgram_ptr = nullptr;

static PFNGLGENVERTEXARRAYSPROC glGenVertexArrays_ptr = nullptr;
static PFNGLBINDVERTEXARRAYPROC glBindVertexArray_ptr = nullptr;
static PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays_ptr = nullptr;

static PFNGLGENBUFFERSPROC glGenBuffers_ptr = nullptr;
static PFNGLBINDBUFFERPROC glBindBuffer_ptr = nullptr;
static PFNGLBUFFERDATAPROC glBufferData_ptr = nullptr;
static PFNGLDELETEBUFFERSPROC glDeleteBuffers_ptr = nullptr;

static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray_ptr = nullptr;
static PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer_ptr = nullptr;

static PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation_ptr = nullptr;
static PFNGLUNIFORM1FPROC glUniform1f_ptr = nullptr;
static PFNGLUNIFORM2FPROC glUniform2f_ptr = nullptr;



// ---------------------------
// App globals
// ---------------------------

static const wchar_t* kClassName = L"BioMathWindow";

// Handlers (encargados de procesar eventos):

// HWND: Handle to Window
// HDC: Handle to device context
// HGLRC: Handle to OpenGL rendering context

static HDC   g_hdc = nullptr;
static HGLRC g_glrc = nullptr;
static bool  g_running = true;

static int g_width = 1280;
static int g_height = 720;

// IDs de objetos OpenGL Inicializados en 0 porque 0 = objeto nulo en OpenGL, convención estándar
static GLuint g_program = 0;
static GLuint g_vao = 0;
static GLuint g_vbo = 0;

// Uniform locations cacheadas. Guardan el location de uniforms en el shader Inicializados en -1 porque:
// -1 = uniform no encontrado / inválido, valor estándar de error
static GLint  g_uTime = -1;
static GLint  g_uRes = -1;

static float g_timeSeconds = 0.0f; //Tiempo global


// ---------------------------
// Helpers
// ---------------------------

// Dame el nombre de una función OpenGL y te devuelvo un puntero ejecutable a esa función, si existe
static void* GetGLProc(const char* name)
{
	void* p = (void*)wglGetProcAddress(name);
	if (p) return p;

	// Some core funcs can live in opengl32.dll (rare for modern ones, but fine to try)
	HMODULE mod = GetModuleHandleW(L"opengl32.dll");
	if (mod) return (void*)GetProcAddress(mod, name);

	return nullptr;
}


// Este helper hace exactamente lo que haría GLAD/GLEW, pero a mano y solo con lo que el programa necesita.
static bool LoadGLFunctions() 
{
	wglCreateContextAttribsARB_ptr = (PFNWGLCREATECONTEXTATTRIBSARBPROC)GetGLProc("wglCreateContextAttribsARB");

	glCreateShader_ptr = (PFNGLCREATESHADERPROC)GetGLProc("glCreateShader");
	glShaderSource_ptr = (PFNGLSHADERSOURCEPROC)GetGLProc("glShaderSource");
	glCompileShader_ptr = (PFNGLCOMPILESHADERPROC)GetGLProc("glCompileShader");
	glGetShaderiv_ptr = (PFNGLGETSHADERIVPROC)GetGLProc("glGetShaderiv");
	glGetShaderInfoLog_ptr = (PFNGLGETSHADERINFOLOGPROC)GetGLProc("glGetShaderInfoLog");
	glDeleteShader_ptr = (PFNGLDELETESHADERPROC)GetGLProc("glDeleteShader");

	glCreateProgram_ptr = (PFNGLCREATEPROGRAMPROC)GetGLProc("glCreateProgram");
	glAttachShader_ptr = (PFNGLATTACHSHADERPROC)GetGLProc("glAttachShader");
	glLinkProgram_ptr = (PFNGLLINKPROGRAMPROC)GetGLProc("glLinkProgram");
	glGetProgramiv_ptr = (PFNGLGETPROGRAMIVPROC)GetGLProc("glGetProgramiv");
	glGetProgramInfoLog_ptr = (PFNGLGETPROGRAMINFOLOGPROC)GetGLProc("glGetProgramInfoLog");
	glUseProgram_ptr = (PFNGLUSEPROGRAMPROC)GetGLProc("glUseProgram");
	glDeleteProgram_ptr = (PFNGLDELETEPROGRAMPROC)GetGLProc("glDeleteProgram");

	glGenVertexArrays_ptr = (PFNGLGENVERTEXARRAYSPROC)GetGLProc("glGenVertexArrays");
	glBindVertexArray_ptr = (PFNGLBINDVERTEXARRAYPROC)GetGLProc("glBindVertexArray");
	glDeleteVertexArrays_ptr = (PFNGLDELETEVERTEXARRAYSPROC)GetGLProc("glDeleteVertexArrays");

	glGenBuffers_ptr = (PFNGLGENBUFFERSPROC)GetGLProc("glGenBuffers");
	glBindBuffer_ptr = (PFNGLBINDBUFFERPROC)GetGLProc("glBindBuffer");
	glBufferData_ptr = (PFNGLBUFFERDATAPROC)GetGLProc("glBufferData");
	glDeleteBuffers_ptr = (PFNGLDELETEBUFFERSPROC)GetGLProc("glDeleteBuffers");

	glEnableVertexAttribArray_ptr = (PFNGLENABLEVERTEXATTRIBARRAYPROC)GetGLProc("glEnableVertexAttribArray");
	glVertexAttribPointer_ptr = (PFNGLVERTEXATTRIBPOINTERPROC)GetGLProc("glVertexAttribPointer");

	glGetUniformLocation_ptr = (PFNGLGETUNIFORMLOCATIONPROC)GetGLProc("glGetUniformLocation");
	glUniform1f_ptr = (PFNGLUNIFORM1FPROC)GetGLProc("glUniform1f");
	glUniform2f_ptr = (PFNGLUNIFORM2FPROC)GetGLProc("glUniform2f");

	// Minimal sanity check: shaders + VAO required for our path
	return glCreateShader_ptr && glShaderSource_ptr && glCompileShader_ptr &&
		glCreateProgram_ptr && glLinkProgram_ptr && glUseProgram_ptr &&
		glGenVertexArrays_ptr && glBindVertexArray_ptr &&
		glGenBuffers_ptr && glBindBuffer_ptr && glBufferData_ptr &&
		glEnableVertexAttribArray_ptr && glVertexAttribPointer_ptr &&
		glGetUniformLocation_ptr && glUniform1f_ptr && glUniform2f_ptr;
}


// Cuadro de mensaje modal para debug
static void DebugMessageBoxA(const char* title, const char* text)
{
	MessageBoxA(nullptr, text, title, MB_ICONERROR | MB_OK);
}


// Define el código GLSL de un vertex shader y un fragment shader, los compila, los linkea en un programa, 
// maneja errores mostrando logs, borra los objetos shader temporales y cachea los uniform locations.
static bool CompileAndLinkProgram()
{
	// TODO: ESTUDIAR
	const char* vsSrc =
		"#version 330 core\n"
		"layout(location=0) in vec2 aPos;\n"
		"out vec2 vUV;\n"
		"void main(){\n"
		"  gl_Position = vec4(aPos, 0.0, 1.0);\n"
		"  vUV = aPos * 0.5 + 0.5;\n"
		"}\n";


	// TODO: ESTUDIAR
	const char* fsSrc =
		"#version 330 core\n"
		"in vec2 vUV;\n"
		"out vec4 FragColor;\n"
		"uniform float uTime;\n"
		"uniform vec2  uResolution;\n"
		"\n"
		"float hash(vec2 p){ return fract(sin(dot(p, vec2(127.1,311.7))) * 43758.5453123); }\n"
		"float noise(vec2 p){\n"
		"  vec2 i = floor(p);\n"
		"  vec2 f = fract(p);\n"
		"  float a = hash(i);\n"
		"  float b = hash(i + vec2(1,0));\n"
		"  float c = hash(i + vec2(0,1));\n"
		"  float d = hash(i + vec2(1,1));\n"
		"  vec2 u = f*f*(3.0-2.0*f);\n"
		"  return mix(a,b,u.x) + (c-a)*u.y*(1.0-u.x) + (d-b)*u.x*u.y;\n"
		"}\n"
		"\n"
		"void main(){\n"
		"  vec2 uv = vUV;\n"
		"  float t = uTime;\n"
		"  float n = noise(uv*6.0 + vec2(t*0.15, t*0.07));\n"
		"  float vign = smoothstep(1.2, 0.2, length(uv - 0.5));\n"
		"  vec3 col = vec3(0.08,0.10,0.14);\n"
		"  col += 0.35 * vec3(0.20,0.55,0.95) * n;\n"
		"  col += 0.15 * vec3(sin(t + uv.x*6.0), sin(t*0.7 + uv.y*5.0), sin(t*1.3)) * 0.5;\n"
		"  col *= vign;\n"
		"  FragColor = vec4(col, 1.0);\n"
		"}\n";

	GLuint vs = glCreateShader_ptr(GL_VERTEX_SHADER);
	glShaderSource_ptr(vs, 1, &vsSrc, nullptr);
	glCompileShader_ptr(vs);

	GLint ok = GL_FALSE;
	glGetShaderiv_ptr(vs, GL_COMPILE_STATUS, &ok);
	if (!ok)
	{
		GLint len = 0;
		glGetShaderiv_ptr(vs, GL_INFO_LOG_LENGTH, &len);
		char* log = (char*)_alloca((size_t)len + 1);
		GLsizei outLen = 0;
		glGetShaderInfoLog_ptr(vs, len, &outLen, log);
		log[outLen] = 0;
		DebugMessageBoxA("Vertex shader compile failed", log);
		glDeleteShader_ptr(vs);
		return false;
	}

	GLuint fs = glCreateShader_ptr(GL_FRAGMENT_SHADER);
	glShaderSource_ptr(fs, 1, &fsSrc, nullptr);
	glCompileShader_ptr(fs);

	glGetShaderiv_ptr(fs, GL_COMPILE_STATUS, &ok);
	if (!ok)
	{
		GLint len = 0;
		glGetShaderiv_ptr(fs, GL_INFO_LOG_LENGTH, &len);
		char* log = (char*)_alloca((size_t)len + 1);
		GLsizei outLen = 0;
		glGetShaderInfoLog_ptr(fs, len, &outLen, log);
		log[outLen] = 0;
		DebugMessageBoxA("Fragment shader compile failed", log);
		glDeleteShader_ptr(fs);
		glDeleteShader_ptr(vs);
		return false;
	}

	g_program = glCreateProgram_ptr();
	glAttachShader_ptr(g_program, vs);
	glAttachShader_ptr(g_program, fs);
	glLinkProgram_ptr(g_program);

	glGetProgramiv_ptr(g_program, GL_LINK_STATUS, &ok);
	if (!ok)
	{
		GLint len = 0;
		glGetProgramiv_ptr(g_program, GL_INFO_LOG_LENGTH, &len);
		char* log = (char*)_alloca((size_t)len + 1);
		GLsizei outLen = 0;
		glGetProgramInfoLog_ptr(g_program, len, &outLen, log);
		log[outLen] = 0;
		DebugMessageBoxA("Program link failed", log);
		glDeleteProgram_ptr(g_program);
		g_program = 0;
		glDeleteShader_ptr(fs);
		glDeleteShader_ptr(vs);
		return false;
	}

	glDeleteShader_ptr(fs);
	glDeleteShader_ptr(vs);

	g_uTime = glGetUniformLocation_ptr(g_program, "uTime");
	g_uRes = glGetUniformLocation_ptr(g_program, "uResolution");

	return true;
}

static void CreateFullscreenTriangle()
{
	// Fullscreen triangle in clip space:
	// (-1,-1), (3,-1), (-1,3)
	const float verts[] = {
		-1.0f, -1.0f,
		 3.0f, -1.0f,
		-1.0f,  3.0f
	};

	// Genero VAO
	glGenVertexArrays_ptr(1, &g_vao); // Creo un VAO (ID)
	glBindVertexArray_ptr(g_vao); // Lo pongo activo

	// Genero VBO
	glGenBuffers_ptr(1, &g_vbo);
	glBindBuffer_ptr(GL_ARRAY_BUFFER, g_vbo);
	glBufferData_ptr(GL_ARRAY_BUFFER, (GLsizeiptr)sizeof(verts), verts, GL_STATIC_DRAW);

	// Conecto el VBO con el shder (location 0)
	glEnableVertexAttribArray_ptr(0);
	
	glVertexAttribPointer_ptr(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	// Le dice a OpenGL : location 0 ; tiene 2 componentes(x, y); cada componente es float
	// no normalices(solo aplica a ints); stride = 2 * sizeof(float) porque cada vértice tiene 2 floats
	// offset = 0 porque la posición empieza al inicio del buffer

	// desbindea (defensivo)
	glBindVertexArray_ptr(0);
	glBindBuffer_ptr(GL_ARRAY_BUFFER, 0);
}


// ---------------------------
// Win32 + WGL
// ---------------------------	

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_SIZE:
		g_width = LOWORD(lParam);
		g_height = HIWORD(lParam);
		return 0;

	case WM_CLOSE:
		DestroyWindow(hWnd);
		return 0;

	case WM_DESTROY:
		g_running = false;
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}

// Inicialización de WGL.
// Conecto OpenGL a la ventana.
// Creo un contexto viejo para poder cargar la función que crea un contexto moderno.
// Si puedo, creo OpenGL 3.3 core.Si no, me quedo con el viejo.
// Y finalmente verifico que tengo las funciones necesarias para renderizar.”

static bool InitWGL(HWND hWnd)
{
	g_hdc = GetDC(hWnd);
	if (!g_hdc) return false;

	PIXELFORMATDESCRIPTOR pfd{}; // Como sera el framebuffer?
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24; // Z-Buffer bits (mientras más, mas precisa la comparacion (menos Z fighting).

	// Una linda definición del stencil buffer:
	// Un buffer auxiliar, por píxel, que participa en pruebas (tests) durante el pipeline 
	// de rasterización para decidir si un fragmento se procesa o se descarta.
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int pf = ChoosePixelFormat(g_hdc, &pfd);
	if (pf == 0) return false;
	if (!SetPixelFormat(g_hdc, pf, &pfd)) return false;

	// 1) Create legacy context first (needed to load wglCreateContextAttribsARB)
	HGLRC legacy = wglCreateContext(g_hdc);
	if (!legacy) return false;

	// Asocio el contexto OpenGL (HGLRC) con el Device Context (HDC) en el thread actual, haciendo que OpenGL quede activo.
	if (!wglMakeCurrent(g_hdc, legacy)) return false;

	// Load WGL extensions + GL functions (from legacy context)
	LoadGLFunctions();

	// 2) Try to create a modern core context (3.3)
	if (wglCreateContextAttribsARB_ptr)
	{
		const int attribs[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 3,
			WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0
		};

		HGLRC modern = wglCreateContextAttribsARB_ptr(g_hdc, 0, attribs);
		if (modern)
		{
			wglMakeCurrent(nullptr, nullptr);
			wglDeleteContext(legacy);

			g_glrc = modern;
			if (!wglMakeCurrent(g_hdc, g_glrc)) return false;
		}
		else
		{
			// Fallback: keep legacy
			g_glrc = legacy;
		}
	}
	else
	{
		// No attribs extension: keep legacy
		g_glrc = legacy;
	}

	// Ensure GL funcs are loaded for the final current context
	if (!LoadGLFunctions())
	{
		DebugMessageBoxA("OpenGL init failed",
			"Could not load required OpenGL functions.\n"
			"Your driver/context may not support OpenGL 2.0+ or required entry points.");
		return false;
	}

	return true;
}

static void ShutdownGL(HWND hWnd)
{
	if (g_program)
	{
		glDeleteProgram_ptr(g_program);
		g_program = 0;
	}
	if (g_vbo)
	{
		glDeleteBuffers_ptr(1, &g_vbo);
		g_vbo = 0;
	}
	if (g_vao)
	{
		glDeleteVertexArrays_ptr(1, &g_vao);
		g_vao = 0;
	}

	if (g_glrc)
	{
		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(g_glrc);
		g_glrc = nullptr;
	}

	if (g_hdc)
	{
		ReleaseDC(hWnd, g_hdc);
		g_hdc = nullptr;
	}
}

// ---------------------------
// Entry point
// ---------------------------

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	WNDCLASS wc{};
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = kClassName;

	if (!RegisterClass(&wc))
		return 1;

	HWND hWnd = CreateWindowEx(
		0, kClassName, L"WIP BioMath",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, g_width, g_height,
		nullptr, nullptr, hInstance, nullptr
	);
	if (!hWnd) return 1;

	ShowWindow(hWnd, SW_SHOW);

	if (!InitWGL(hWnd))
		return 1;

	// Build program + geometry
	if (!CompileAndLinkProgram())
		return 1;

	CreateFullscreenTriangle();

	LARGE_INTEGER freq{};
	LARGE_INTEGER prev{};
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&prev); // reloj de alta precisión en Windows.

	MSG msg{};
	while (g_running)
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				g_running = false;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Calcular delta time y acumular tiempo
		LARGE_INTEGER now{};
		QueryPerformanceCounter(&now);
		const double dt = double(now.QuadPart - prev.QuadPart) / double(freq.QuadPart);
		prev = now;
		g_timeSeconds += (float)dt;

		// Preparar el frame (viewport y clear)
		glViewport(0, 0, g_width > 0 ? g_width : 1, g_height > 0 ? g_height : 1);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Usar programa y setear uniforms
		glUseProgram_ptr(g_program);
		if (g_uTime >= 0) glUniform1f_ptr(g_uTime, g_timeSeconds);
		if (g_uRes >= 0) glUniform2f_ptr(g_uRes, (float)g_width, (float)g_height);

		// Dibujar el fullscreen triangle
		glBindVertexArray_ptr(g_vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray_ptr(0);

		SwapBuffers(g_hdc);
		Sleep(1);
	}

	ShutdownGL(hWnd);
	return 0;
}
#ifndef _GLOBALS_H_
#define _GLOBALS_H_

//#include "glut/glut.h"
#include <glad/glad.h>
#include "Errors.h"

//Just in case -- Null redefinition
#ifdef NULL
	#undef NULL
#endif
#define NULL 0
#define NULLRECT {0,0,0,0}


// LOGGING -----------------------------------------------------------------------
#define LOG(format, ...) Cronos::Log(__FILE__, __LINE__, format, __VA_ARGS__);
#define COMPILATIONLOGINFO Cronos::LogCompilationFirstMessage()

namespace Cronos {
	
	void Log(const char file[], int line, const char* format, ...);
	void LogCompilationFirstMessage();
}
// -------------------------------------------------------------------------------

// ERROR HANDLING ----------------------------------------------------------------
#define CRONOS_ASSERT(x) if(!(x)) __debugbreak();

#ifdef _DEBUG
	#define GLCall(x) GLClearError(); x; CRONOS_ASSERT(GLLogCall(#x, __FILE__, __LINE__));
	#define GL_SETERRORHANDLER(majV, minV) SetErrorHandler(majV, minV);
#else
	#define GLCall(x) x;
	#define GL_SETERRORHANDLER(majV, minV) std::cout << "GL_SETERROHANDLER (glDebugMessageCallback) not Available: NOT IN DEBUG CONTEXT" << std::endl;
#endif

// -------------------------------------------------------------------------------

// DEFINES -----------------------------------------------------------------------
#define CN_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

/// Keep a value between 0.0f and 1.0f
#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

/// Conversors
#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define HAVE_M_PI
#define TO_BOOL( a )  ( (a != 0) ? true : false )

/// Useful helpers, InRange checker, min, max and clamp
#define IN_RANGE( value, min, max ) ( ((value) >= (min) && (value) <= (max)) ? 1 : 0 )
#define MIN( a, b ) ( ((a) < (b)) ? (a) : (b) )
#define MAX( a, b ) ( ((a) > (b)) ? (a) : (b) )
#define CLAMP(x, upper, lower) (MIN(upper, MAX(x, lower)))

/// Standard string size
#define SHORT_STR	32
#define MID_STR		255
#define HUGE_STR	8192

/// Performance macros
#define PERF_START(timer) timer.Start()
#define PERF_PEEK(timer) LOG("%s took %f ms", __FUNCTION__, timer.ReadMs())

/// Configuration
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 720
#define SCREEN_SIZE 1
#define WIN_FULLSCREEN false
#define WIN_RESIZABLE true
#define WIN_BORDERLESS false
#define WIN_FULLSCREEN_DESKTOP false
#define VSYNC true
#define TITLE "Cronos Engine"

// -------------------------------------------------------------------------------

// TYPEDEFS ----------------------------------------------------------------------
typedef unsigned int uint;
typedef unsigned int ushort;
typedef unsigned char uchar;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;

// -------------------------------------------------------------------------------

// OTHERS ------------------------------------------------------------------------
enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

/// Joins a path and file
inline const char* const PATH(const char* folder, const char* file)
{
	static char path[MID_STR];
	sprintf_s(path, MID_STR, "%s/%s", folder, file);
	return path;
}

template <class VALUE_TYPE> void SWAP(VALUE_TYPE& a, VALUE_TYPE& b)
{
	VALUE_TYPE tmp = a;
	a = b;
	b = tmp;
}


#endif
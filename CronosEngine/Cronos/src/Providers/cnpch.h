#ifndef _CNPCH_H_
#define _CNPCH_H_

#include <vector>

#include <stdio.h>
#include <cstdio>

#include <stdlib.h>
#include <unordered_map>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>

#include <utility>
#include <functional>

#include <math.h>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <tuple>

//#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <windows.h>

//Warning! If you wanna work with bullet, first, integrate it, then, go to MathBuildConfig.h, look for
//the definition of MATH_BULLET_INTEROP, and uncomment it. Should work :)
#ifdef _DEBUG
	#pragma comment (lib, "vendor/MathGeoLib/libx86/Debug/MathGeoLib.lib")
#else
	#pragma comment (lib, "vendor/MathGeoLib/libx86/Debug/MathGeoLib.lib")
#endif

#include "MathGeoLib/include/MathGeoLib.h"

#include "Timers/Timer.h"
#include "Timers/PerfTimer.h"
#include "Globals.h"

#endif

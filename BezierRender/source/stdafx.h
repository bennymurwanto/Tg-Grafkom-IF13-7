//////////////////////////////////////////////////////////////////////
// stdafx.h
// Description:		Precompiled header includes
//
// By:				Ben Woodhouse (0145394)
// Date:			12/2003
//////////////////////////////////////////////////////////////////////

#pragma once

typedef unsigned int uint;

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <gl\gl.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <strstream>
#include <assert.h>
#include <algorithm>

#include "data structs\String.h"
#include "data structs\Singleton.h"
#include "maths\maths.h"
#include "timer.h"
#include "log.h"

#include "input\keys.h"
#include "input\mouse.h"

#include "colour.h"
#include "gldisplay.h"
#include "input\keys.h"
#include "input\mouse.h"
#include "winapp.h"





// TODO: reference additional headers your program requires here

#pragma once

#ifdef _DEBUG
#define INITGUID
#endif
#include <d3d11.h>
#include <dxgi1_6.h>

// DEBUG STUFF
#include <dxgidebug.h>

// LIBS
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "DXGI.lib" )

#include <d3dcompiler.h>
#pragma comment( lib, "d3dcompiler.lib" )

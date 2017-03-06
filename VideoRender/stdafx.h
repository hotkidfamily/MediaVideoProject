// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
#include <stdio.h>

// Windows 头文件: 
#include <windows.h>
#include <tchar.h>
#include <timeapi.h>
#include <stdlib.h>

#include <ddraw.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <DxErr.h>

#include "common\common.h"

#include "videoprocess\IVPP.h"

#include "RenderUtils.h"

#include "iRenderCommon.h"
#include "IRender.h"

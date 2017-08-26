#pragma once

#ifdef MSVC_IMPLICIT_LINK
#ifndef _DEBUG
#pragma comment(lib, "ThreadPool.lib")
#else
#pragma comment(lib, "ThreadPool_d.lib")
#endif
#endif

#include "ThreadPool/ThreadPool.h"
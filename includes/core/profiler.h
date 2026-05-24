#pragma once
#include <SDL3/SDL.h>

#include <vector>
#include <numeric>
#include <windows.h>
#include <psapi.h>

#include "time/timer.h"

namespace core
{
	struct MemoryStats {
		float workingSetMB;   // Physical RAM in use
		float privateBytesMB; // Total committed (matches VS ~closely)
	};

	// Can be extended to keep data for longer time and output it into file at the end of session
	class Profiler
	{
	public:

		static Profiler& Instance()
		{
			static Profiler instance;
			return instance;
		}

		// delete copy/move
		Profiler(const Profiler&) = delete;
		Profiler& operator=(const Profiler&) = delete;

		void update()
		{
			Uint64 now = SDL_GetPerformanceCounter();
			Uint64 freq = SDL_GetPerformanceFrequency();

			deltaTime = (float)((now - last) / (float)freq); // Convert to seconds
			last = now;

			frames++;
			timer.step(deltaTime);

			if (timer.isTimeout())
			{
				fps = frames;
				frames = 0;
				timer.consumeTimeout();
			}

			size_t memBytes = GetMemoryUsageBytes();
			memStats.workingSetMB = memBytes / (1024.0f * 1024.0f);

			size_t memBytesEx = GetMemoryUsageBytesEx();
			memStats.privateBytesMB = memBytesEx / (1024.0f * 1024.0f);
		}

		void setDrawCalls(Uint64 value)
		{
			drawCalls = value;
		}

		Uint64 getDrawCalls()
		{
			return drawCalls;
		}

		Uint64 getFPS() const
		{
			return fps;
		}

		float getDeltaTime() const
		{
			return deltaTime;
		}

		MemoryStats getMemoryStats()
		{
			return memStats;
		}
	private:
		Profiler() = default;

		size_t GetMemoryUsageBytes()
		{
#if defined(_WIN32)
			PROCESS_MEMORY_COUNTERS info;
			GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info));
			return info.WorkingSetSize;
#else
			// fallback for now
			return 0;
#endif
		}

		size_t GetMemoryUsageBytesEx()
		{
#if defined(_WIN32)
			PROCESS_MEMORY_COUNTERS_EX info;  // _EX variant
			GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&info, sizeof(info));
			return info.PrivateUsage;
#else
			return 0;
#endif
		}

		MemoryStats memStats;

		Timer timer = Timer(1.0f);

		float deltaTime = 0;
		float memMB = 0;

		Uint64 last = SDL_GetPerformanceCounter();
		Uint64 frames = 0;
		Uint64 fps = 0;
		Uint64 drawCalls;
	};
}
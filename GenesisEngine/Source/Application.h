#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "Globals.h"
#include <vector>
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleSceneIntro.h"
#include "Editor.h"

#include <string>

struct json_array_t;
typedef json_array_t JSON_Array;

struct json_value_t;
typedef json_value_t JSON_Value;

struct json_object_t;
typedef json_object_t JSON_Object;

struct HardwareSpecs
{
	int cpu_count;
	int cache;
	float ram;
	const char* gpu;
	const char* gpu_brand;
	float vram_budget;
	float vram_usage;
	float vram_available;
	float vram_reserved;

	bool RDTSC;
	bool MMX;
	bool SSE;
	bool SSE2;
	bool SSE3;
	bool SSE41;
	bool SSE42;
	bool AVX;
	bool AVX2;
	bool AltiVec;

	std::string caps;
};

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleCamera3D* camera;
	ModuleSceneIntro* scene;
	Editor* editor;
	ModuleRenderer3D* renderer3D;

	const char* engine_name;

public:

	Application(int argc, char* args[]);
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

public:
	float GetFPS();
	float GetLastDt();
	int GetFPSCap();
	void SetFPSCap(int fps_cap);

	HardwareSpecs GetHardware();
	const char* GetEngineVersion();

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

	JSON_Array* PrepareConfig();
	JSON_Object* GetJSONObjectByName(const char* name, JSON_Array* modules_array);

private:
	int	   argc;
	char** args;

	Timer	ms_timer;
	float	dt;
	float	fps;
	float	capped_ms;
	std::vector<Module*> modules_vector;

	const char* version;

	const char* config_path;
	JSON_Value* config_root;
};

extern Application* App;

#endif 
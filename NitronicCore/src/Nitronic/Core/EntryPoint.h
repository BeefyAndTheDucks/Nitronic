#pragma once

#include "Nitronic/Core/Base.h"
#include "Nitronic/Core/Application.h"

#ifdef N_PLATFORM_WINDOWS

extern Nitronic::Application* Nitronic::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
	Nitronic::Log::Init();

	auto app = Nitronic::CreateApplication({ argc, argv });
	app->Run();
	delete app;
}

#else
#error Platform doesn't support entry point yet!
#endif

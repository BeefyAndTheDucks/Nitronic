#pragma once

#include "Nitronic/Core/Base.h"
#include "Nitronic/Events/Event.h"

int main(int argc, char** argv);

namespace Nitronic {

	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			N_CORE_ASSERT(index < Count);
			return Args[index];
		}
	};

	struct ApplicationSpecification
	{
		std::string Name = "Nitronic Application";
		std::string WorkingDirectory;
		ApplicationCommandLineArgs CommandLineArgs;
	};

	class Application
	{
	public:
		Application(const ApplicationSpecification& specification);
		virtual ~Application();

		void Close();

		static Application& Get() { return *s_Instance; }

		const ApplicationSpecification& GetSpecification() const { return m_Specification; }
	private:
		void Run();

		ApplicationSpecification m_Specification;

		bool m_Running = true;

		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

	// defined in client
	Application* CreateApplication(ApplicationCommandLineArgs args);

}

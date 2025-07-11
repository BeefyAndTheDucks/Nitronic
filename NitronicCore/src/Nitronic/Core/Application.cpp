#include "npch.h"
#include "Nitronic/Core/Application.h"

#include "Nitronic/Events/ApplicationEvent.h"
#include "Nitronic/Core/Log.h"

namespace Nitronic
{

	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification& specification)
		: m_Specification(specification)
	{
		N_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		if (!m_Specification.WorkingDirectory.empty())
			std::filesystem::current_path(m_Specification.WorkingDirectory);

		N_CORE_INFO("Application '{0}' started", m_Specification.Name);
	}

	Application::~Application() {
		// Shutdown
	}

	void Application::Close() {
		m_Running = false;
	}

	void Application::Run()
	{
		WindowResizeEvent e(1920, 1080);
		if (e.IsInCategory(EventCategoryApplication))
		{
			N_TRACE("{0}", e.ToString());
		}
		if (e.IsInCategory(EventCategoryInput))
		{
			N_TRACE("{0}", e.ToString());
		}

		while (m_Running)
		{
			// Main loop logic goes here
			// For example, update and render the application
		}
	}

}

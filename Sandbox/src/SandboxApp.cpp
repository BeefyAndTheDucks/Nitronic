#include "Nitronic.h"

class Sandbox : public Nitronic::Application
{
public:
	Sandbox(const Nitronic::ApplicationSpecification& specification)
		: Nitronic::Application(specification)
	{

	}

	~Sandbox()
	{

	}

};

Nitronic::Application* Nitronic::CreateApplication(Nitronic::ApplicationCommandLineArgs args)
{
	Nitronic::ApplicationSpecification spec;
	spec.Name = "Sandbox";
	spec.WorkingDirectory = "../Sandbox";
	spec.CommandLineArgs = args;

	return new Sandbox(spec);
}

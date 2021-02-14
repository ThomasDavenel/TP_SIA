#include <Application/ApplicationSelection.h>

#include <Application/GL3_TP1.h>
#include <Application/GL3_TP2.h>
#include <Application/GL3_TP3.h>
#include <Application/TP1_siaa.h>

int main(int argc, char ** argv)
{
  ::std::cout<<"Path of the executable: "<<System::Path::executable()<<::std::endl ;
	// Registers the application 
	Application::ApplicationSelection::registerFactory<Application::GL3_TP1>("OpenGL 3 - TP 1");
	Application::ApplicationSelection::registerFactory<Application::GL3_TP2>("OpenGL 3 - TP 2");
	Application::ApplicationSelection::registerFactory<Application::GL3_TP3>("OpenGL 3 - TP 3");
	Application::ApplicationSelection::registerFactory<Application::TP1_siaa>("SIA - TP 1");

	// Initializes GLUT and GLEW
	Application::Base::initializeGLUT(argc, argv) ;
	// Selection of the application and run
	Application::ApplicationSelection::selectAndRun() ;
}

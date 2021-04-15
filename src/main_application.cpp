#include <Application/ApplicationSelection.h>

#include <Application/GL3_TP1.h>
#include <Application/GL3_TP2.h>
#include <Application/GL3_TP3.h>
#include <Application/TP1_siaa.h>
#include <Application/TP2_siaa.h>
#include <Application/TP3_siaa.h>
#include <Application/SIAA_TP4_MotionPlanning.h>

int main(int argc, char ** argv)
{
  ::std::cout<<"Path of the executable: "<<System::Path::executable()<<::std::endl ;
	// Registers the application 
	/*SI*/
	/*
	Application::ApplicationSelection::registerFactory<Application::GL3_TP1>("OpenGL 3 - TP 1");
	Application::ApplicationSelection::registerFactory<Application::GL3_TP2>("OpenGL 3 - TP 2");
	Application::ApplicationSelection::registerFactory<Application::GL3_TP3>("OpenGL 3 - TP 3");
	*/
	/*SIA*/
	
	Application::ApplicationSelection::registerFactory<Application::TP1_siaa>("SIA - TP 1");
	Application::ApplicationSelection::registerFactory<Application::TP2_siaa>("SIA - TP 2");
	Application::ApplicationSelection::registerFactory<Application::TP3_siaa>("SIA - TP 3");
	
	/*SIA_partie2*/
	Application::ApplicationSelection::registerFactory<Application::SIAA_TP4_MotionPlanning>("SIA - TP 4");
	// Initializes GLUT and GLEW
	Application::Base::initializeGLUT(argc, argv) ;
	// Selection of the application and run
	Application::ApplicationSelection::selectAndRun() ;
}


#include "globalState.h"

int main()
{

	try
	{
		globalState::initGraphics();
		globalState::initWindow(1000u, 700u);
		globalState::initDrawingSurface();

		globalState::loadImage(0u, "Approximate_scheme.jpg");
		globalState::loadVertexShader(0u, L"vs.cso");
		globalState::loadPixelShader(0u, L"ps.cso");
		globalState::setVertexShader(0u);
		globalState::setPixelShader(0u);


		globalState::draw(0u);
		while (globalState::windowIsOpen()) {
			globalState::processState();
		}
	}
	catch (const std::exception& ex)
	{
		std::cout << ex.what();
		system("pause");
	}
	catch (...)
	{
		std::cout << "\n\n\tUNKNOWN ERROR OCCURED\n\n";
		system("pause");
	}

	return 0;
}
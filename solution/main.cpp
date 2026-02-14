
#include "globalState.h"

int main()
{

	try
	{
		globalState::initGraphics();
		globalState::initWindow(1000u, 700u);
		globalState::initDrawingSurface();

		globalState::draw();

		while (globalState::windowIsOpen()) {
			globalState::processState();
		}
	}
	catch (const std::exception& ex)
	{
		std::cout << ex.what();
		system("pause");
	}

	return 0;
}
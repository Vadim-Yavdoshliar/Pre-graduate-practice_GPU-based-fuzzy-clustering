
#include "globalState.h"

int main()
{
	globalState::initGraphics();
	globalState::initWindow(1000u, 700u);
	globalState::initDrawingSurface();

	globalState::draw();

	while (globalState::windowIsOpen()) {
		globalState::processState();
	}

	return 0;
}
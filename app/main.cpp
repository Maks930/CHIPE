#include "Application/Application.h"

#include <Application/AudioEngine/AEngine.h>

int main(int argc, char** argv) {

    Application app(argc, argv);
    return app.exec();
}
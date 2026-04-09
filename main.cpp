#include "backend/core/Application.h"

int main() {
    Application application;
    if (!application.Init()) {
        return 1;
    }

    application.Run();
    application.Shutdown();
    return 0;
}

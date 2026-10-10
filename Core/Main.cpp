#include "Core/Application.h"

int main() {
    vl::App::Application application({ .enableEditor = true });
    return application.Run();
}

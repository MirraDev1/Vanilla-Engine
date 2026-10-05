# Vanilla Engine

Vanilla Engine is a game engine, built while learning.
Inspired by Yan Chernikov porpularly known as "Cherno". I learned a lot from his tutorials.
so...i should give it a try and make my own engine. I hope this will be a good learning experience for me and also to you(yes you).

## Architecture

The engine is one static library, `VanillaEngine`, and two programs that link it.

- `editor` (`Core/Main.cpp`) starts the Editor.
- `vanilla` (`Sandbox/Main.cpp`) starts the same engine with a different proscess created by `CreateProscess()` function  provided by the Windows OS API

`Application` owns the window and the frame. Each frame polls input, updates the camera, clears the screen, uploads the Editor's Events, then presents.

- `Core/Platform/Windows/` — window, device, renderer, camera(didn't find a good name actually), and viewport.
- `Core/Platform/DirectX/` — shaders, GPU buffers, debug output, and the log.
- `Core/Resources/` — texture loading.(gonna have to change this soon)
- `Core/Shaders/` — vertex and pixel shaders.
- `Core/Platform/Windows/vlUserInterface.*` — dockable editor UI, settings, and the console.
- `Core/Vendor/` — third-party code.(glfw , ImGui e.t.c)

## Notes
(unnescessary maybe?)
- This is a 3D engine.
- The application owns the window. The editor only borrows it.
- Files in `Core/Shaders` are copied into a `Shaders` folder beside the executable at build time. The program loads them from there.
- `ApplicationConfig::enableEditor` is the switch between the editor and the runtime. Keep experiments in `Sandbox/` so `Core/` stays the engine.(one of the worst ways to separate the Editor from the runtime but also trying to Improve that)

## Contributing
if you want to contribute(i first appreciate for that) you can:
Use a different branch. Do not commit straight to the main branch.

1. Branch off `master`.
2. Do the work on that branch.
3. Open a pull request back into `master`(main branch).

## Running it

Install these first:

1. Visual Studio 2022, with the **Desktop development with C++** workload.
2. HLSL Tools.
3. A GPU that supports DirectX 11(I'm sure all GPU's have this).

Open this folder in Visual Studio, select the `x64-debug` preset, and build `editor` or `vanilla`(im going to change this name i promise maybe to `VanillaRuntime` or...just `Sandbox`).

### Tech stack

C++23, CMake, GLFW, DirectX 11, HLSL

It looks rough(or....ugly i can say) right now. but...I'll improve it over time.
My weakpoint mainly lies on shader writing HLSL so if your good at this you'll be of great help
peace☮
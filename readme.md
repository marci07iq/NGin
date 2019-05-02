## NGin

### Features

NGin is an open source, lightweight game engine with useful features packed together:
- Graphics
  - HTML like GUI elements
  - OpenGL Shader and Texture wrappers
  - Legacy OpenGL re-implementation for better future support
- Data transport and storage
  - Tree like data packets
  - Networking wrapper, with optional compression (currently only winsock supported)
  - Save and load from file
- Mathematics
  - 2-3D Vector; 4D Matrix algebra
  - Polynomials and polynomial solving
  - Flow systems (for energy and liquid networks)
  - Rational numbers
- Modding
  - WIP custom scripting language, with drag-and-drop interface for simple scripts
- Compilation
  - Add whole codebase to your project or
  - Static linked library


### Installation
#### Windows, Visual Studio

##### Static library

- Download the Eigen library, and configure the additional include directories to point to it
- Compile the static library
- Modify copy_res.bat to copy the include files, libraries, and resources to your output of choice

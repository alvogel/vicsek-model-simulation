<b>Vicsek Model written in C++</b>

<img src="https://github.com/alvogel/vicsek-model-simulation/blob/master/preview.png">

<a href="https://github.com/alvogel/vicsek-model-simulation/blob/master/demo.gif">DEMO</a>

<b>Dependencies:</b>

- <a href="https://www.libsdl.org/download-2.0.php">SDL2</a>
- <a href="https://www.libsdl.org/projects/SDL_ttf/">SDL ttf</a>
- <a href="https://www.libsdl.org/projects/SDL_image/">SDL image</a>

<b>There are four implementations of the model:</b>

- Vicsek: Vicsek-Model with naive neighbour search O(n^2)
- VicsekQT: Vicsek-Model using QuadTree for neighbour search O(log(n))
- VicsekQTMT: Vicsek-Model using QuadTree for neighbour search O(log(n)) and Multi-Threading
- VicsekOCL: Vicsek-Model using OpenCL to simulate it hardware accelerated on graphic card(s)

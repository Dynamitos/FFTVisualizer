glslangValidator.exe -V test.vert -o testVertex.spv
glslangValidator.exe -V test.frag -o testFragment.spv

glslangValidator.exe -V parallax.vert -o parallaxVertex.spv
glslangValidator.exe -V parallax.frag -o parallaxFragment.spv

glslangValidator.exe -V generated.vert -o generatedVertex.spv
glslangValidator.exe -V generated.frag -o generatedFragment.spv
pause
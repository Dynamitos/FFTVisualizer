slangc test.slang -profile glsl_vertex -entry vertexMain -o generated.vert
slangc test.slang -profile glsl_vertex -target spirv -entry vertexMain -o generatedVertex.spv
pause
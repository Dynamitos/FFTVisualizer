slangc test.slang -profile glsl_fragment -entry fragmentMain -o generated.frag
slangc test.slang -profile glsl_fragment -target spirv -entry fragmentMain -o generatedFragment.spv
pause
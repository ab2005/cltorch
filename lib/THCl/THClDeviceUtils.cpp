#include <string>

#include THClDeviceutils.h

std::string THClDeviceUtils_getKernelTemplate() {
  // [[[cog
  // import stringify
  // stringify.write_kernel( "kernel", "THClDeviceUtils.cl" )
  // ]]]
  // generated using cog, from THClDeviceUtils.cl:
  const char * kernelSource =  
  "{{IndexType}} THClCeilDiv({{IndexType}} a, {{IndexType}} b) {\n" 
  "  return (a + b - 1) / b;\n" 
  "}\n" 
  "IndexType getStartIndex(IndexType totalSize) {\n" 
  "  IndexType sizePerBlock = THClCeilDiv(totalSize, (IndexType) gridDim.x);\n" 
  "  return blockIdx.x * sizePerBlock;\n" 
  "}\n" 
  "\n" 
  "";
  // [[[end]]]
  return kernelSource;
}



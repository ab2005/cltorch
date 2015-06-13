#include "THClGeneral.h"
#include "TH.h"

#include <stdio.h>
#include "EasyCL.h"

//#include "THCTensorRandom.h"
//#include "THCBlas.h"
//#include "THCAllocator.h"

/* Size of scratch space available in global memory per each SM + stream */
#define FLOATS_PER_SCRATCH_SPACE 4
#define GLOBAL_SCRATCH_SPACE_PER_SM_STREAM (FLOATS_PER_SCRATCH_SPACE) * sizeof(float)

void THClInit(THClState* state)
{
  printf("*******************************************\n");
  printf("THClInit()\n");
  state->allocatedDevices = easycl::DevicesInfo::getNumDevices();
  state->clByDevice = new EasyCL *[state->allocatedDevices];
  for(int i = 0; i < state->allocatedDevices; i++) {
    state->clByDevice[i] = 0;
    state->scratchSpaceByDevice[i] = 0;
  }
  state->currentDevice = 0;
  //state->cl = EasyCL::createForFirstGpuOtherwiseCpu(); // obviously this should change...
}

void THClShutdown(THClState* state)
{
  for(int i = 0; i < state->allocatedDevices; i++) {
    delete state->clByDevice[i];
    if( state->scratchSpaceByDevice[i] != 0 ) {
      delete state->scratchSpaceByDevice[i]->wrapper;
      delete state->scratchSpaceByDevice[i]->data;
      delete state->scratchSpaceByDevice[i];
    }
  }
  delete state->clByDevice;
  printf("THClShutdown()\n");
  printf("*******************************************\n");
}

std::ostream &operator<<( std::ostream &os, const dim3 &obj ) {
  os << "dim3{" << obj.vec[0] << ", " << obj.vec[1] << ", " << obj.vec[2] << "}";
  return os;
}

int THClState_getNumDevices(THClState* state) {
  return state->allocatedDevices;
}
void THClState_setDevice(THClState* state, int device) {
  state->currentDevice = device;
}
int THClState_getDevice(THClState* state) {
  return state->currentDevice;
}
EasyCL *THClState_getCl(THClState* state) {
  if( state->clByDevice[state->currentDevice] == 0 ) {
    state->clByDevice[state->currentDevice] = EasyCL::createForIndexedDevice(state->currentDevice);
    THClScratchSpace *scratch = new THClScratchSpace();
    scratch->data = new float[FLOATS_PER_SCRATCH_SPACE];
    EasyCL *cl = THClState_getCl(state);
    scratch->wrapper = cl->wrap(FLOATS_PER_SCRATCH_SPACE, scratch->data);
    scratch->wrapper->createOnDevice();
    state->scratchSpaceByDevice[state->currentDevice] = scratch;
  }
  return state->clByDevice[state->currentDevice];
}

CLWrapper* THClState_getCurrentDeviceScratchSpace(THClState* state)
{
//  int device = -1;
//  THClCheck(cudaGetDevice(&device));
  int device = state->currentDevice;
//  int stream = THClState_getCurrentStreamIndex(state);
  int stream = 0;

  return THClState_getDeviceScratchSpace(state, device, stream);
}

CLWrapper* THClState_getDeviceScratchSpace(THClState* state, int device, int stream)
{
//  THCClResourcesPerDevice* res =
//    THClState_getDeviceResourcePtr(state, device);

//  if (stream > state->numUserStreams || stream < 0)
//  {
//    THError("%d is not a stream", stream);
//  }

  if( stream != 0 ) {
    THError("%d is not a stream", stream);
  }
  return state->scratchSpaceByDevice[state->currentDevice]->wrapper;
//  return res->devScratchSpacePerStream[stream];
}

size_t THClState_getCurrentDeviceScratchSpaceSize(THClState* state)
{
//  int device = -1;
  int device = state->currentDevice;
//  THClCheck(cudaGetDevice(&device));
  return THClState_getDeviceScratchSpaceSize(state, device);
}

size_t THClState_getDeviceScratchSpaceSize(THClState* state, int device)
{
//  THCClResourcesPerDevice* res =
//    THClState_getDeviceResourcePtr(state, device);

  return GLOBAL_SCRATCH_SPACE_PER_SM_STREAM; // true currently since we only have
             // one stream per device, currently
//  return res->scratchSpacePerStream;
}


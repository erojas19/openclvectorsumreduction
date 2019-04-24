#include <iostream>
#include <vector>
#include <math.h>
#include <random>
#include <ctime>
#include <time.h>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#include <iostream>
#define CL_TARGET_OPENCL_VERSION 200
using namespace std;


int main() {
  // initialize vector
  int SIZE = 250;
  cl_int ret;
  vector<float>vectorSum;
  vector<float>vectorEmpty(SIZE);
  vector<float>resSum(1);
  for (int i = 0; i < SIZE; ++i) {
    float x = (float)(i % 10);
    vectorSum.push_back(x);
  }
  time_t now_start = time(nullptr);
  // convert now to string form
  char* dt = ctime(&now_start);
  cout << "The local date and time is: " << dt << endl;
  // Allocate space for vectors A,B and C
  float* Hl0 = (float*) malloc(sizeof(float) * vectorSum.size());
  float* HY = (float*) malloc(sizeof(float) * resSum.size());
  // Get platform and device information
  cl_platform_id* platforms = NULL;
  cl_uint num_platforms;
  //Set up the Platform
  cl_int clStatus = clGetPlatformIDs(0, NULL, &num_platforms);
  platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id) * num_platforms);
  clStatus = clGetPlatformIDs(num_platforms, platforms, NULL);
  //Get the devices list and choose the device you want to run on
  cl_device_id* device_list = NULL;
  cl_uint num_devices;
  clStatus = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 0, NULL, &num_devices);
  device_list = (cl_device_id*)malloc(sizeof(cl_device_id) * num_devices);
  clStatus = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, num_devices, device_list, NULL);
  // Create one OpenCL context for each device in the platform
  cl_context context;
  context = clCreateContext(NULL, num_devices, device_list, NULL, NULL, &clStatus);
  // Create a command queue
  cl_command_queue command_queue = clCreateCommandQueue(context, device_list[0], 0, &clStatus);
  // Create memory buffers on the device for each vector
  cl_mem Hl0_clmem = clCreateBuffer(context, CL_MEM_READ_ONLY, (vectorSum.size()) * sizeof(float), NULL, &clStatus);
  cl_mem Hl1_clmen = clCreateBuffer(context, CL_MEM_READ_WRITE, (vectorSum.size()) * sizeof(float), NULL, &clStatus);
  cl_mem Hy_clmen = clCreateBuffer(context, CL_MEM_READ_WRITE, (resSum.size()) * sizeof(float), NULL, &clStatus);

  // Copy the Buffer Hl0, HSYN0,HSYN1,Y
  clStatus = clEnqueueWriteBuffer(command_queue, Hl0_clmem, CL_TRUE, 0, (vectorSum.size()) * sizeof(float), vectorSum.data(), 0, NULL, NULL);
  clStatus = clEnqueueWriteBuffer(command_queue, Hl1_clmen, CL_TRUE, 0, (vectorSum.size()) * sizeof(float), vectorEmpty.data(), 0, NULL, NULL);
  clStatus = clEnqueueWriteBuffer(command_queue, Hy_clmen, CL_TRUE, 0, (resSum.size()) * sizeof(float), resSum.data(), 0, NULL, NULL);
  // loading kernel file

  char fileKernel[] = "D:/development/cpp/OpenClVectorSum/kernel.cl";
  ifstream kernelFile(fileKernel, ios::in);
  if (!kernelFile.is_open()) {
    std::cerr << "Failed to open file for reading: " << fileKernel << std::endl;
    return NULL;
  }
  std::ostringstream oss;
  oss << kernelFile.rdbuf();
  std::string srcStdStr = oss.str();
  const char* source_str = srcStdStr.c_str();
  cout << "*********************************" << endl;
  cout << srcStdStr << endl;
  cout << "*********************************" << endl;
  // Create a program from the kernel source
  cl_program program = clCreateProgramWithSource(context, 1, (const char**)&source_str, NULL, &clStatus);
  if (program == NULL) {
    std::cerr << "Failed to create CL program from source." << std::endl;
    return NULL;
  }
  // Build the program
  clStatus = clBuildProgram(program, 1, device_list, NULL, NULL, NULL);
  // Create the OpenCL kernel
  cl_kernel kernel = clCreateKernel(program, "sum",  &clStatus);
  // Set the arguments of the kernel
  clStatus = clSetKernelArg(kernel, 0, sizeof(cl_mem), &Hl0_clmem);
  clStatus = clSetKernelArg(kernel, 1, sizeof(cl_mem), &Hl1_clmen);
  clStatus = clSetKernelArg(kernel, 2, sizeof(cl_mem), &Hy_clmen);
  // Execute the OpenCL kernel on the list
  size_t global_size = SIZE; // Process the entire lists
  size_t localWorkSize = 16;
  clStatus = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_size, &localWorkSize, 0, NULL, NULL);
  /* Read the result */
  // Read the cl memory C_clmem on device to the host variable C
  vector<float>l1_res(resSum.size());
  clStatus = clEnqueueReadBuffer(command_queue, Hy_clmen, CL_TRUE, 0, (resSum.size()) * sizeof(float), resSum.data(), 0, NULL, NULL);

  cout << "****************************" << endl;
  for (int var = 0; var < resSum.size(); ++var) {
    cout << resSum[var] << endl;
  }
  float sum = 0.0f;
  for (int var = 0; var < vectorSum.size(); ++var) {
    sum = sum + vectorSum[var];
  }
  cout << sum << endl;
  // Clean up and wait for all the comands to complete.
  ret = clReleaseKernel(kernel);
  ret = clReleaseProgram(program);
  ret = clReleaseMemObject(Hl0_clmem);
  ret = clReleaseMemObject(Hl1_clmen);
  ret = clReleaseMemObject(Hy_clmen);
  ret = clReleaseCommandQueue(command_queue);
  ret = clReleaseContext(context);
  free(Hl0);
  free(HY);
  return 0;
}

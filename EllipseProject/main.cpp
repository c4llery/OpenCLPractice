#include <CL/opencl.hpp>
#include <vector>
#include <iostream>
#include <fstream>


void save_ppm(const std::string& filename, const std::vector<cl_uchar4>& pixels, int width, int height) {
    std::ofstream file(filename, std::ios::binary);
    file << "P6\n" << width << " " << height << "\n255\n";
    
    std::vector<unsigned char> rgb_data;
    rgb_data.reserve(width * height * 3);
    for (const auto& p : pixels) {
        rgb_data.push_back(p.x); 
        rgb_data.push_back(p.y); 
        rgb_data.push_back(p.z); 
    }
    
    file.write(reinterpret_cast<const char*>(rgb_data.data()), rgb_data.size());
}

int main(){
    const int width = 800;
    const int height = 600;
    const int a = 200;
    const int b = 200;
    const int x0 = 400;
    const int y0 = 300;

    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    if (platforms.empty()) {
        std::cerr << "No OpenCL platforms found!" << std::endl;
        return 1;
    }

    std::vector<cl::Device> devices;
    platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);
    if (devices.empty()) {
        std::cerr << "No GPU devices found. Trying CPU..." << std::endl;
        platforms[0].getDevices(CL_DEVICE_TYPE_CPU, &devices);
        if (devices.empty()) {
            std::cerr << "No OpenCL devices available!" << std::endl;
            return 1;
        }
    }


    cl::Context context(devices);
    cl::CommandQueue queue(context, devices[0]);
    std::ifstream file("kernel.cl");
    std::string code((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    cl::Program::Sources kernel_code;
    kernel_code.push_back({code.c_str(), code.length()});

    
    cl::Program program(context, kernel_code);
    program.build(devices);
    std::vector<cl_uchar4> pixels(width * height);
    cl::Buffer buffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_uchar4) * width * height);

    cl::Kernel kernel(program, "draw_ellipse");
    kernel.setArg(0, buffer);
    kernel.setArg(1, width);
    kernel.setArg(2, height);
    kernel.setArg(3, x0);
    kernel.setArg(4, y0);
    kernel.setArg(5, a);
    kernel.setArg(6, b);

    queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(width, height));
    queue.enqueueReadBuffer(buffer, CL_TRUE, 0, sizeof(cl_uchar4) * width * height, pixels.data()); 
    
    save_ppm("ellipse.ppm", pixels, width, height);
    std::cout << "Image saved to ellipse.ppm" << std::endl;

    return 0;
}

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <functional>
#include <vector>
const int WIDTH = 800;
const int HEIGHT = 600;
class HelloTriangleApplication {
public:
    void run() {
	initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window;
    VkInstance instance;
    void initVulkan() {
	createInstance();
    }
    void createInstance() {
	VkApplicationInfo appInfo= {};
	VkInstanceCreateInfo createInfo = {};
	uint32_t glfwExtensionCount = 0;
	uint32_t extensionCount = 0;
	const char** glfwExtensions;
	
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion - VK_API_VERSION_1_0;
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	
	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;
	createInfo.enabledLayerCount = 0;
	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
	
	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
    	}
	
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
	
	std::cout << "Availble Extensions" << std::endl;
	for(const auto &extension: extensions)
		std::cout << "\t" << extension.extensionName << std::endl;
	
    }		
    void checkRequiredExtensionsPresent(std::vector<VkExtensionProperties> availableExt, const char** requiredExt, int requiredExtCount) {
	    for (int i = 0; i < requiredExtCount;i++) {
		    bool found = false;
		    for (const auto& extension : availableExt) {
			    if(strcmp(requiredExt[i],extension.extensionName)){
				    found = true;
			    }
		    }
		    if(!found){
			    throw std::runtime_error("missing vulkan extension");
		    }
	    }
    std::cout << "extension requirement fulfilled" << std::endl;
    }
    void initWindow() {
	    glfwInit();
	    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }
    void mainLoop() {
	while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    	}
    }

    void cleanup() {
	  vkDestroyInstance(instance, nullptr);
	  glfwDestroyWindow(window);
    	  glfwTerminate();
    }
};

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

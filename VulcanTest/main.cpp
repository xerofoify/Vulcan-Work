#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <functional>
#include <vector>

/*Constants for width and height of the complete rendered window*/
const int WIDTH = 800;
const int HEIGHT = 600;

/*Constants for the validation Layers used from Vulcan when debugging*/
const std::vector<const char*> validationLayers = {
	"VK_LAYER_LUNARG_standard_validation"
};

/*Passed by makefile if debugging is enabled*/
#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else 
	const bool enableValidationLayers = true;
#endif

/*Structure used for Queues by HelloTriangleApplication*/
struct QueueFamilyIndices {
	int graphicsFamily = 1;
	bool isComplete() {
	return graphicsFamily >= 0; 
	}
};

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
	VkDebugReportCallbackEXT callback;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

	/* Functions called for window startup in run*/
	void initVulkan() {
		createInstance();
		setDebugCall();
		pickPhysicalDevice();
		createLogicalDevice();
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
		DestroyDebugReportCallbackEXT(instance, callback, nullptr);
		vkDestroyInstance(instance, nullptr);
		glfwDestroyWindow(window);
		glfwTerminate();
	}
	
	/* Helper Functions for initVulcan*/
	void createInstance() {
		VkApplicationInfo appInfo= {};
	   VkInstanceCreateInfo createInfo = {};
	   uint32_t glfwExtensionCount = 0;
	   uint32_t extensionCount = 0;
	   const char** glfwExtensions;

	   if (enableValidationLayers && !checkValidationLayerSupport()) {
			throw std::runtime_error("validation layers requested, but not available!");
	   }

	   appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	   appInfo.pApplicationName = "Hello Triangle";
	   appInfo.pEngineName = "No Engine";
	   appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	   appInfo.apiVersion - VK_API_VERSION_1_0;
	   createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	   createInfo.pApplicationInfo = &appInfo;

	   glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	  auto extensions = getRequiredExtensions();
	  createInfo.enabledExtensionCount = static_cast<uint32_t> (extensions.size());
	  createInfo.ppEnabledExtensionNames = extensions.data();
	  createInfo.enabledLayerCount = 0;
	  VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
	  if (enableValidationLayers) {
	       createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
	       createInfo.ppEnabledLayerNames = validationLayers.data();
	  } else {
	       createInfo.enabledLayerCount = 0;
	  }

	  if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	  }

	  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	  std::vector<VkExtensionProperties> extensionProperties(extensionCount);
	  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionProperties.data());
	}		

	void setDebugCall() {
		if (!enableValidationLayers)
			return;

		VkDebugReportCallbackCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
		createInfo.pfnCallback = debugCallback;

		if (vkCreateDebugReportCallbackEXT(instance, &createInfo, nullptr, &callback) != VK_SUCCESS) {
			throw std::runtime_error("failed to set up debug callback!");
		}
	}

	void createLogicalDevice() {
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
		queueCreateInfo.queueCount = 1;
	}

	bool pickPhysicalDevice() {
		return true;
	}

	bool isDeviceSuitable(VkPhysicalDevice device) {
		 QueueFamilyIndices indices = findQueueFamilies(device);

		 return indices.isComplete();
	}

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
		 QueueFamilyIndices indices;
		 uint32_t queueFamilyCount = 0;
		 vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		 std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		 vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
		 return indices;
		}

	void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator) {
		auto func = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
			if (func != nullptr) {
			func(instance, callback, pAllocator);
		}
	}

	VkResult vkCreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback) {
	auto func = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	   if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pCallback);
	   } else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	   }
	}
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	       VkDebugReportFlagsEXT flags,
	       VkDebugReportObjectTypeEXT objType,
	       uint64_t obj,
	       size_t location,
	       int32_t code,
	       const char* layerPrefix,
	       const char* msg,
	       void* userData) {

	       std::cerr << "validation layer: " << msg << std::endl;

	       return VK_FALSE;
	}

	bool checkValidationLayerSupport() {
	       uint32_t layerCount;
	       vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	       std::vector<VkLayerProperties> availableLayers(layerCount);
	       vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
		       for (const char* layerName : validationLayers) {
			bool layerFound = false;

			       for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
				       layerFound = true;
				       break;
				}
			       }

			       if (!layerFound) {
				return false;
			       }
			}

	        return true;
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
	std::vector<const char*> getRequiredExtensions() {
	       uint32_t glfwExtensionCount = 0;
	       const char** glfwExtensions;
	       glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	       std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	        if (enableValidationLayers)
		 extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

	        return extensions;
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

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstring>
#include <stdio.h>
#include <cstdlib>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

// TODO Page 53

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

// Only use validation layers when debugging
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

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

	void initWindow() {
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
	}

	void initVulkan() {
		createInstance();
	}

	void mainLoop() {
		// Run until the window recives a close command
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
	}

	void cleanup() {
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);

		glfwTerminate();
	}

	void createInstance() {
		std::vector<const char*> requiredExtensions = getRequiredExtensions();

		uint32_t extensionCount = 0;
		// Determines how many extension are available
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		// Queries for all available extensions
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

		// Throw an error if not all required extensions were found
		if (!checkExtensionSupport(availableExtensions, requiredExtensions, static_cast<uint32_t>(requiredExtensions.size()))) {
			throw std::runtime_error("Some required extensions where not found!");
		}

		// Throw an error if validation layers are requested, but not available
		if (enableValidationLayers && !checkValidationLayerSupport()) {
			throw std::runtime_error("validation layers requestd, but not available!");
		}

		// Mostly optional vulkan init
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		// Required vulkan init
		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
		createInfo.ppEnabledExtensionNames = requiredExtensions.data();

		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			throw std::runtime_error("failed to create instance!");
		}
	}

	// Ensure all required extensions are available on the system
	bool checkExtensionSupport(std::vector<VkExtensionProperties> availExts, std::vector<const char*> reqExts, uint32_t reqExtsCnt) {

		if (reqExtsCnt == 0) {
			return true;
		}

		bool extFound = false;
#ifdef NDEBUG
		for (uint32_t i = 0; i < reqExtsCnt; i++) {
			extFound = false;
			for (const auto& extension : availExts) {
				if (strcmp(extension.extensionName, reqExts[i]) == 0) {
					extFound = true;
					break;
				}
			}
			if (!extFound) {
				return false;
			}
		}

		return true;
#else
		uint32_t numFound = 0;
		std::cout << "Enabled Extensions:\n";
		for (const VkExtensionProperties& extension : availExts) {
			extFound = false;
			if (numFound < reqExtsCnt) {
				for (uint32_t i = 0; i < reqExtsCnt; i++) {
					if (strcmp(extension.extensionName, reqExts[i]) == 0) {
						std::cout << "\t [x] " << extension.extensionName << '\n';
						extFound = true;
						numFound++;
						break;
					}
				}

				if (!extFound) {
					std::cout << "\t [ ] " << extension.extensionName << '\n';
				}
			}
			else {
				std::cout << "\t [ ] " << extension.extensionName << '\n';
			}
		}

		if (numFound == reqExtsCnt) {
			return true;
		}
		else {
			return false;
		}
#endif
	}

	std::vector<const char*> getRequiredExtensions() {
		uint32_t glfwExtensionsCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionsCount);

		if (enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	// Ensures all enabled validation layers are available
	bool checkValidationLayerSupport() {
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		bool layerFound = false;
		uint32_t numFound = 0;

		std::cout << "\nEnabled Validation Layers:\n";
		for (const VkLayerProperties& availLayer : availableLayers) {
			layerFound = false;
			if (numFound < validationLayers.size()) {
				for (const char* reqLayer : validationLayers) {
					if (strcmp(reqLayer, availLayer.layerName) == 0) {
						std::cout << "\t [x] " << reqLayer << '\n';
						layerFound = true;
						numFound++;
						break;
					}
				}

				if (!layerFound) {
					std::cout << "\t [ ] " << availLayer.layerName << '\n';
				}
			}
			else {
				std::cout << "\t [ ] " << availLayer.layerName << '\n';
			}
		}

		if (numFound == validationLayers.size()) {
			return true;
		}
		else {
			return false;
		}
	}
};

int main() {
	HelloTriangleApplication app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
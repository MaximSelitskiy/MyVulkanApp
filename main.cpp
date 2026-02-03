#include <vulkan/vulkan.h>  // подключаем библиотеку Vulkan
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <optional>
#include <vector>
#include <algorithm>
#include <limits>
#include <cstdint>
#include <set>
const uint32_t WIDTH = 350;
const uint32_t HEIGHT = 250;
void NewSpaceLine() {
	std::cout << "\n===========================================================================\n";
}
class FirstApplication {
public:
	void run() {
		initWindow();
		initVulkan();
		info();
		mainLoop();
		cleanup();
	}
	void info() {
		std::cout << "VulkanAPP info:";
		NewSpaceLine();
		printAvailableExtensions();
		printAvailableDevices();
	}
private:
#pragma region parametres
	VkInstance vkInstance;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	GLFWwindow* window;
	VkSurfaceKHR surface;
	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainFormat;
	VkExtent2D swapChainExtent;
#pragma endregion
#pragma region SwapChain
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
		SwapChainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
		//FORMATS
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}
		//PRESENT MODES
		uint32_t presentModesCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModesCount, nullptr);
		if (presentModesCount != 0) {
			details.presentModes.resize(presentModesCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModesCount, details.presentModes.data());
		}
		return details;
	}
	bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
		std::set<std::string>requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}
		return requiredExtensions.empty();
	}
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
				availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}
		return availableFormats[0];
	}
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		for (const auto& availablepresentMode : availablePresentModes) {
			if (availablepresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablepresentMode;
			}
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}
		else {
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);
			VkExtent2D actualExtent{
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
			return actualExtent;
		}
	}
	void createSwapChain() {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);
		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount < swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}
		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(),indices.presentFamily.value() };
		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}
		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;
		if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
			throw std::runtime_error("failed to create swap chain!");
		}

		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
		swapChainFormat = surfaceFormat.format;
		swapChainExtent = extent;
	}
#pragma endregion
#pragma region InitializeWindow
	void initWindow() {
		if (!glfwInit()) {
			throw std::runtime_error("Failed to initialize GLFW");
		}
		if (!glfwVulkanSupported()) {
			std::cout << "GLFW info: Vulkan dont  supported!" << std::endl;
		}
		else {
			std::cout << "GLFW info: Vulkan supported." << std::endl;
		}
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		window = glfwCreateWindow(WIDTH, HEIGHT, "I NEVER ASKED FOR THIS", nullptr, nullptr);
	}
#pragma endregion

	void initVulkan() {
		createInstance();
		createSurface();
		pickPhysicalDevice();
		createLogicalDevice();
		createSwapChain();
	}
	void mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			glfwWaitEvents();
		}
	}
	void cleanup() {
		vkDestroyDevice(device, nullptr);
		vkDestroySurfaceKHR(vkInstance, surface, nullptr);
		vkDestroyInstance(vkInstance, nullptr);
		glfwDestroyWindow(window);
		glfwTerminate();
	}
#pragma region VulkanLogicalDevice
	void createLogicalDevice() {
		QueueFamilyIndices index = findQueueFamilies(physicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = {
			index.graphicsFamily.value(),
			index.presentFamily.value()
		};

		float queuePriority = 1.0;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = index.graphicsFamily.value();
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

		if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device)) {
			throw std::runtime_error("Couldn't create logical device");
		}

		vkGetDeviceQueue(device, index.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(device, index.presentFamily.value(), 0, &presentQueue);
	}
#pragma endregion
#pragma region VulkanSurface
	void createSurface() {
		if (glfwCreateWindowSurface(vkInstance, window, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("Can't create window surface");
		}
	}
#pragma endregion
#pragma region VulkanPhysicalDeviceQueues
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
		QueueFamilyIndices indices;
		uint32_t familyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);
		std::vector<VkQueueFamilyProperties> families(familyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, families.data());
		int i = 0;
		for (const auto& family : families) {
			if (family.queueFlags && VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
			if (presentSupport) {
				indices.presentFamily = i;
			}
			if (indices.isComplete()) {
				break;
			}
			i++;
		}
		return indices;
	}
	void printQueueInfo(VkPhysicalDevice device) {
		uint32_t familyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);
		std::vector<VkQueueFamilyProperties> families(familyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, families.data());
		for (const auto& family : families) {
			if (family.queueFlags && VK_QUEUE_COMPUTE_BIT) {
				std::cout << "Queue count: " << family.queueCount << std::endl;
				std::cout << "Flags: " << std::hex << family.queueFlags << "\n";
			}
		}
	}
#pragma endregion
#pragma region VulkanPhysicalDevice
	void pickPhysicalDevice() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);//ПОЛУЧЕНИЕ КОЛИЧЕСТВА ФИЗИЧЕСКИХ УСТРОЙСТВ
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());
		for (const auto& device : devices) {
			if (isDeviceSuitable(device)) {
				physicalDevice = device;
				break;
			}
		}

		if (physicalDevice == VK_NULL_HANDLE) {
			throw std::runtime_error("failed to find a suitable GPU!");
		}
	}
	bool isDeviceSuitable(VkPhysicalDevice device) {
		QueueFamilyIndices indices = findQueueFamilies(device);
		bool extensionSupported = checkDeviceExtensionSupport(device);

		bool swapChainAdequate = false;
		if (extensionSupported) {
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}
		return indices.isComplete() && swapChainAdequate && extensionSupported;
	}
	void printAvailableDevices() {
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);//ПОЛУЧЕНИЕ КОЛИЧЕСТВА ФИЗИЧЕСКИХ УСТРОЙСТВ
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());
		for (const auto& device : devices) {
			vkGetPhysicalDeviceProperties(device, &deviceProperties);
			vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
			std::cout << deviceProperties.deviceName << '\n';
			printQueueInfo(device);
			std::cout << '\n';
		}
		NewSpaceLine();
	}
#pragma endregion
#pragma region VulkanInstance
	void createInstance() {
		//ИНФОРМАЦИЯ О ПРИЛОЖЕНИИ
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "ASUS_APP";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "ASUS_ENGINE";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_4;
		//ИНФОРМАЦИЯ ДРАЙВЕРУ
		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.enabledLayerCount = 0;
		createInfo.pApplicationInfo = &appInfo;
		//УСТАНОВКА РАСШИРЕНИЙ С БИБЛИОТЕКОЙ GLFW
		uint32_t glfwExtensionCount = 0;
		const char** _glfwExtensions;
		_glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		std::vector<const char*> glfwExtensions(_glfwExtensions, _glfwExtensions + glfwExtensionCount);
		createInfo.enabledExtensionCount = static_cast<uint32_t>(glfwExtensionCount);
		createInfo.ppEnabledExtensionNames = glfwExtensions.data();
		VkResult vkResult = vkCreateInstance(&createInfo, nullptr, &vkInstance);
		if (vkResult != VK_SUCCESS) {
			throw std::runtime_error("failed to create vkInstance! Error code: " + std::to_string(vkResult));
		}
	}
#pragma endregion
#pragma region VulkanExtensions
	//ИНФОРМАЦИЯ О РАСШИРЕНИЯХ ЭКЗЭМПЛЯРА
	struct ExtensionData {
		std::vector<VkExtensionProperties> extensions;
		uint32_t extensionCount;
	};
	ExtensionData getSupportedExtensions() {
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		return { std::move(extensions), extensionCount };
	}
	void printAvailableExtensions() {
		std::vector<VkExtensionProperties> extensions = getSupportedExtensions().extensions;
		std::cout << "Available extensions: \n";
		for (const auto& extension : extensions) {
			std::cout << extension.extensionName << std::endl;
		}
		//ИНФОРМАЦИЯ О РАСШИРЕНИЯХ GLFW
		uint32_t glfwExtensionCount = 0;
		const char** _glfwExtensions;
		_glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		std::vector<const char*> glfwExtensions(_glfwExtensions, _glfwExtensions + glfwExtensionCount);
		std::cout << "\nGLFW extensions requires " << glfwExtensionCount << " extensions: \n";
		for (const auto& glfwExtension : glfwExtensions) {
			std::cout << glfwExtension << '\n';
		}
		NewSpaceLine();
	}
#pragma endregion
};
int main() {
	FirstApplication app;
	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << "APPLICATION CRASHED: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;

}


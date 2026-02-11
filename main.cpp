#include <vulkan/vulkan.h>  // подключаем библиотеку Vulkan
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <optional>
#include <vector>
#include <algorithm>
#include <limits>
#include <cstdint>
#include <set>
const uint32_t WIDTH = 350;
const uint32_t HEIGHT = 250;
static std::vector<char> readFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if (!file.is_open()) {
		throw std::runtime_error("couldn't open file");
	}
	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();
	return buffer;
}
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
	VkInstance vkInstance = VK_NULL_HANDLE;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	GLFWwindow* window = VK_NULL_HANDLE;
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
	VkQueue graphicsQueue = VK_NULL_HANDLE;
	VkQueue presentQueue = VK_NULL_HANDLE;
	const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	VkSwapchainKHR swapChain = VK_NULL_HANDLE;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkPipeline graphicsPipeline = VK_NULL_HANDLE;
	VkRenderPass renderPass = VK_NULL_HANDLE;
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
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		window = glfwCreateWindow(WIDTH, HEIGHT, "I NEVER ASKED FOR THIS", nullptr, nullptr);
	}
#pragma endregion
#pragma region Main
	void initVulkan() {
		createInstance();
		createSurface();
		pickPhysicalDevice();
		createLogicalDevice();
		createSwapChain();
		createImageViews();
		createRenderPass();
		createGraphicsPipeline();
	}
	void mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			glfwWaitEvents();
		}
	}
	void cleanup() {
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		vkDestroySurfaceKHR(vkInstance, surface, nullptr);
		vkDestroyRenderPass(device, renderPass, nullptr);
		vkDestroyPipeline(device, graphicsPipeline, nullptr);
		for (auto framebuffer : swapChainFramebuffers) {
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}
		vkDestroyDevice(device, nullptr);
		vkDestroyInstance(vkInstance, nullptr);
		glfwDestroyWindow(window);
		glfwTerminate();
	}
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
	void createImageViews() {
		swapChainImageViews.resize((swapChainImages.size()));
		for (int i = 0; i < swapChainImages.size(); i++) {
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			createInfo.image = swapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = swapChainFormat;

			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 0;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create image views");
			}
		}
	}
#pragma endregion
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
#pragma region Shaders
	VkShaderModule createShaderModule(const std::vector<char>& code) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error("couldn't create shader module!");
		}
		return shaderModule;
	}

#pragma endregion
#pragma region GraphicsPipeline
	void createGraphicsPipeline() {
		auto vertexShader = readFile("shaders/vert.spv");
		auto fragShader = readFile("shaders/frag.spv");

		VkShaderModule vertShaderModule = createShaderModule(vertexShader);
		VkShaderModule fragShaderModule = createShaderModule(fragShader);


		VkPipelineShaderStageCreateInfo vertPipelineShaderStageInfo = {};
		vertPipelineShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertPipelineShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertPipelineShaderStageInfo.module = vertShaderModule;
		vertPipelineShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragPipelineShaderStageInfo = {};
		fragPipelineShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragPipelineShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragPipelineShaderStageInfo.module = fragShaderModule;
		fragPipelineShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertPipelineShaderStageInfo,fragPipelineShaderStageInfo };

		VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{};
		pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = 0;//НЕОБЯЗАТЕЛЬНО
		pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0.0f;//КОЛИЧЕСТВО ОПИСАНИЙ АТРИБУТОВ
		pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = 0;//НЕОБЯЗАТЕЛЬНО
		pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 0.0f;//КОЛИЧЕСТВО ПРИВЯЗОК

		VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
		pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{};
		pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		pipelineViewportStateCreateInfo.viewportCount = 1;
		pipelineViewportStateCreateInfo.scissorCount = 1;

		std::vector<VkDynamicState> dynamicStates = {
	VK_DYNAMIC_STATE_VIEWPORT,
	VK_DYNAMIC_STATE_SCISSOR
		};
		VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
		pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		pipelineDynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		pipelineDynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

		VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{};
		pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
		pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		pipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		pipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;//НЕОБЯЗАТЕЛЬНО
		pipelineRasterizationStateCreateInfo.depthBiasClamp = VK_FALSE;//НЕОБЯЗАТЕЛЬНО
		pipelineRasterizationStateCreateInfo.depthBiasConstantFactor = VK_FALSE;//НЕОБЯЗАТЕЛЬНО
		pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
		pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;

		VkPipelineMultisampleStateCreateInfo pipelineMultisampStateCretaeInfo{};
		pipelineMultisampStateCretaeInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		pipelineMultisampStateCretaeInfo.sampleShadingEnable = VK_FALSE;
		pipelineMultisampStateCretaeInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		pipelineMultisampStateCretaeInfo.minSampleShading = 1.0f;//НЕОБЯЗАТЕЛЬНО
		pipelineMultisampStateCretaeInfo.pSampleMask = nullptr;//НЕОБЯЗАТЕЛЬНО
		pipelineMultisampStateCretaeInfo.alphaToCoverageEnable = VK_FALSE;//НЕОБЯЗАТЕЛЬНО
		pipelineMultisampStateCretaeInfo.alphaToOneEnable = VK_FALSE;//НЕОБЯЗАТЕЛЬНО

		VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState{};
		pipelineColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		pipelineColorBlendAttachmentState.blendEnable = VK_TRUE;
		pipelineColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		pipelineColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		pipelineColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		pipelineColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		pipelineColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
		pipelineColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{};
		pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		pipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
		pipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;//НЕОБЯЗАТЕЛЬНО
		pipelineColorBlendStateCreateInfo.attachmentCount = 1;
		pipelineColorBlendStateCreateInfo.pAttachments = &pipelineColorBlendAttachmentState;
		pipelineColorBlendStateCreateInfo.blendConstants[0] = 0.0f;//НЕОБЯЗАТЕЛЬНО
		pipelineColorBlendStateCreateInfo.blendConstants[1] = 0.0f;//НЕОБЯЗАТЕЛЬНО
		pipelineColorBlendStateCreateInfo.blendConstants[2] = 0.0f;//НЕОБЯЗАТЕЛЬНО
		pipelineColorBlendStateCreateInfo.blendConstants[3] = 0.0f;//НЕОБЯЗАТЕЛЬНО

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = 0;//НЕОБЯЗАТЕЛЬНО 
		//pipelineLayoutCreateInfo.pSetLayouts = nullptr;//НЕОБЯЗАТЕЛЬНО
		pipelineLayoutCreateInfo.pushConstantRangeCount = 0;//НЕОБЯЗАТЕЛЬНО
		//pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;//НЕОБЯЗАТЕЛЬНО
		if (vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("couldn't create pipeline layout");
		}

		VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
		graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		graphicsPipelineCreateInfo.stageCount = 2;
		graphicsPipelineCreateInfo.pStages = shaderStages;
		graphicsPipelineCreateInfo.pVertexInputState = &pipelineVertexInputStateCreateInfo;
		graphicsPipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
		graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
		graphicsPipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
		graphicsPipelineCreateInfo.pMultisampleState = &pipelineMultisampStateCretaeInfo;
		graphicsPipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
		graphicsPipelineCreateInfo.pDynamicState = &pipelineDynamicStateCreateInfo;
		graphicsPipelineCreateInfo.pDepthStencilState = nullptr;//НЕОБЯЗАТЕЛЬНО
		graphicsPipelineCreateInfo.layout = pipelineLayout;
		graphicsPipelineCreateInfo.renderPass = renderPass;
		graphicsPipelineCreateInfo.subpass = 0;

		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
			throw std::runtime_error("couldn't create graphics pipeline");
		}

		vkDestroyShaderModule(device, fragShaderModule, nullptr);
		vkDestroyShaderModule(device, vertShaderModule, nullptr);
	}
#pragma endregion
#pragma region RenderPass
	void createRenderPass()
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = swapChainFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkRenderPassCreateInfo renderPassCreateInfo{};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.attachmentCount = 1;
		renderPassCreateInfo.pAttachments = &colorAttachment;
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpass;

		if (vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass) != VK_SUCCESS) {
			throw std::runtime_error("couldn't create render pass");
		}

	}


#pragma endregion
#pragma region FrameBuffer
	void createFrameBuffers() {
		swapChainFramebuffers.resize(swapChainImageViews.size());

		for (size_t i{}; i < swapChainImageViews.size(); i++) {
			VkImageView attachments[] = { swapChainImageViews[i] };
			VkFramebufferCreateInfo frameBufferCreateInfo{};
			frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			frameBufferCreateInfo.renderPass = renderPass;
			frameBufferCreateInfo.attachmentCount = 1;
			frameBufferCreateInfo.pAttachments = attachments;
			frameBufferCreateInfo.height = swapChainExtent.height;
			frameBufferCreateInfo.width = swapChainExtent.width;
			frameBufferCreateInfo.layers = 1;

			if (vkCreateFramebuffer(device, &frameBufferCreateInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("couldn't create frame buffer");
			}
		}
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

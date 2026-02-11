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
	std::vector<VkImageView> swapChainImageViews;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	VkRenderPass renderPass;
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
/*#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <set>
#include <cstdint> // для uint32_t
#include <limits> // для std::numeric_limits
#include <algorithm> // для std::clamp
#include <fstream>  // для загрузки файлов шейдеров

const uint32_t WIDTH = 300;
const uint32_t HEIGHT = 250;

// структура для проверки возможностей поверхности
struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;              // возможности поверхности
	std::vector<VkSurfaceFormatKHR> formats;      // доступные форматы
	std::vector<VkPresentModeKHR> presentModes;   // доступные режимы представления
	std::vector<VkImageView> swapChainImageViews;  // представления изображений
};

// структура для хранения индексов семейств очередей
struct QueueFamilyIndices {
	uint32_t graphicsFamilyIndex;
	uint32_t presentFamilyIndex;

	VkBool32 graphicsSupport;
	VkBool32 presentSupport;

	bool isComplete() {
		return graphicsSupport && presentSupport;
	}
};

// функция загрузки файлов шейдеров
static std::vector<char> readFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	// открываем файл
	if (!file.is_open()) {
		throw std::runtime_error("Не удалось загрузить файл!");
	}
	size_t fileSize = (size_t)file.tellg(); // получаем размер
	std::vector<char> buffer(fileSize);  // определяем буфер для считывания

	file.seekg(0);      // перемещаемся в начало файла
	file.read(buffer.data(), fileSize);  // считываем файл в буфер

	file.close();       // закрываем поток файла

	return buffer;
}

class HelloApplication {
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
	VkSurfaceKHR surface;       // поверхность рисования
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;  // физическое устройство
	VkDevice device;    // логическре устройство
	VkQueue graphicsQueue;  // графическая очередь
	VkQueue presentQueue;   // очередь представления
	VkSwapchainKHR swapChain;  // цепочка буферов

	VkFormat swapChainImageFormat; // формат изображений
	VkExtent2D swapChainExtent;     // размер изображений

	std::vector<VkImage> swapChainImages;  // полученные изображения
	std::vector<VkImageView> swapChainImageViews; // представления изображений

	VkRenderPass renderPass;  // проход рендеринга

	// создаем окно
	void initWindow() {
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		window = glfwCreateWindow(WIDTH, HEIGHT, "METANIT.COM", nullptr, nullptr);
	}

	// инициализация всех аспектов Vulkan
	void initVulkan() {
		createInstance();           // создание VkInstance
		createSurface();            // создание поверхности рисования
		selectPhysicalDevice();     // выбор физического устройства
		createLogicalDevice();      // создание логического устройства
		createSwapChain();         // создание цепочки буферов
		createImageViews();         // создание представлений изображений
		createRenderPass();         // создаем проход рендеринга
		createGraphicsPipeline();  // создаем графический конвейер
	}
	// функция графического конвейера
	void createGraphicsPipeline() {
		// загрузка файлов шейдеров
		auto vertShaderCode = readFile("shaders/vert.spv");
		auto fragShaderCode = readFile("shaders/frag.spv");

		// преобразование кода шейдеров в модули  шейдеров
		VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
		VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

		// определение этапа для вершинного шейдера
		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;     // этап вершинного шейдера
		vertShaderStageInfo.module = vertShaderModule;  // модуль шейдера
		vertShaderStageInfo.pName = "main";     // точка входа в шейдер - функция main

		// определение этапа для фрагментного шейдера
		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		// определяем массив для дальнейшего использования
		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		// удаление модулей шейдеров
		vkDestroyShaderModule(device, fragShaderModule, nullptr);
		vkDestroyShaderModule(device, vertShaderModule, nullptr);
	}
	// создаем проход рендеринга
	void createRenderPass() {
		// определяем цветовое прикрепление
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = swapChainImageFormat;  // формат изображений, полученный из цепочки буферов
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;  // 1 образец на изображение
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;  // очистить буфер кадра до черного цвета перед рисованием нового кадра
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;  // отрисованное содержимое будет сохранено в памяти и может быть прочитано позже
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;  // не имеет значения
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;  // не имеет значения
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;  // не имеет значения
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;   // для вывода изображений на экран

		// определяем ссылку на цветовое прикрепление
		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;     // индекс прикрепления в массиве
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// определяем подпроход рендеринга
		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // тип конвейера - графический конвейер
		subpass.colorAttachmentCount = 1;  // количество цветовых прикреплений
		subpass.pColorAttachments = &colorAttachmentRef; // выше созданная ссылка на цветовое прикрепление

		// определяем параметры для создания прохода рендеринга
		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;     // количество применяемых прикреплений
		renderPassInfo.pAttachments = &colorAttachment; // применяемые прикрепления
		renderPassInfo.subpassCount = 1;  // количество подпроходов
		renderPassInfo.pSubpasses = &subpass;   // используемые подпроходы
		// создаем проход рендеринга
		if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
			throw std::runtime_error("Не удалось создать проход рендеринга!");
		}
	}
	// создание модуля шейдера из байт-кода шейдера
	VkShaderModule createShaderModule(const std::vector<char>& code) {
		std::cout << "KO";
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size(); // устанавливаем размер байт-кода

		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data()); // приводим указатель к нужному типу

		VkShaderModule shaderModule; // создаваемый модуль шейдера
		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("Не удалось создать модуль шейдера!");
		}
		std::cout << "KO";

		return shaderModule;
	}
	// создание представлений из изображений цепочки буферов
	void createImageViews() {
		swapChainImageViews.resize(swapChainImages.size());  // устанавливаем размер вектора
		for (size_t i{}; i < swapChainImages.size(); i++) {
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = swapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;    // изображение рассматривается как 2D-текстура
			createInfo.format = swapChainImageFormat;   // формат изображений

			// rgba-компоненты получают значения по умолчанию
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;
			// создаем представление изображения
			if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
				throw std::runtime_error("Не удалось создать представление изображения!");
			}
		}
	}
	// установка формата изображений
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		return availableFormats[0];
	}
	// определение режима представления
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}
	// определение размеров для цепочки буферов
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
		// если текущая ширина не указана как максимальное значение для uint32_t
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			// то возвращаем текущие размеры
			return capabilities.currentExtent;
		}
		else {
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}
	// создаение цепочки буферов
	void createSwapChain() {
		// получаем поддерживаемые устройством форматы, режимы представления и размеры
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

		// выбираем формат поверхности
		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		// выбираем режим представления
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		// устанавливаем размеры
		VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

		// определяем минимальное количество изображения, которое требуется для работы
		// и увеличиваем его на 1 для оптимизации работы драйвера
		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		// проверяем, что не превышаем максимальное количество изображений,
		// делая это, где 0 — это особое значение, которое означает, что максимума нет
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		// определяем конфигурацию для создания цепочки буферов
		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount;  // минимальное количество изображений
		createInfo.imageFormat = surfaceFormat.format;      // формат изображений
		createInfo.imageColorSpace = surfaceFormat.colorSpace;  // цветовое пространство
		createInfo.imageExtent = extent;            // размеры изображений
		createInfo.imageArrayLayers = 1;            // поверхность с одним представлением
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;  // изображение используется в качестве цветового прикрепления

		// Указываем, как обрабатывать изображения цепочки буферов, которые будут использоваться в нескольких семействах очередей.
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
		uint32_t queueFamilyIndices[] = { indices.graphicsFamilyIndex, indices.presentFamilyIndex };

		// если семейство графической очереди отличается от очереди представления
		if (indices.graphicsFamilyIndex != indices.presentFamilyIndex) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT; // обе очереди совместно владеют изображением
			createInfo.queueFamilyIndexCount = 2;           // две очереди
			createInfo.pQueueFamilyIndices = queueFamilyIndices; // массив индексов семейств очередей
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // изображением владеет одна очередь
			createInfo.queueFamilyIndexCount = 0; // Необязательно
			createInfo.pQueueFamilyIndices = nullptr; // Необязательно
		}
		// не применяем никаких преобразований
		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		// игнорирум альфа-канал
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		createInfo.presentMode = presentMode;  // режим представления
		createInfo.clipped = VK_TRUE;        // включаем обрезку скрытых пикселей

		createInfo.oldSwapchain = VK_NULL_HANDLE; // нет старых цепочек буферов

		// создаем цепочку буферов
		if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
			throw std::runtime_error("Не удалось создать цепочку буферов!");
		}
		// сохраняем выбранные формат и размеры изображений
		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent = extent;

		// сначала получаем количество изображений
		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
		// на основе полученного количества определяем вектор
		swapChainImages.resize(imageCount);
		// получаем в вектор изображения
		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
	}
	void mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			glfwWaitEvents();
		}
	}

	void cleanup() {
		// удаляем проход рендеринга
		vkDestroyRenderPass(device, renderPass, nullptr);

		// удаляем все представления изображений
		for (auto imageView : swapChainImageViews) {
			vkDestroyImageView(device, imageView, nullptr);
		}
		// удаляем цепочку буферов
		vkDestroySwapchainKHR(device, swapChain, nullptr);
		// удаляем логическое устройство
		vkDestroyDevice(device, nullptr);
		// удаляем поверхность
		vkDestroySurfaceKHR(instance, surface, nullptr);
		// удаляем объект VkInstance
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);
		glfwTerminate();
	}

	// создание VkInstance
	void createInstance() {

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		// получаем расширения для GLFW
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		// устанавливаем расширения для VkInstance
		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;

		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			throw std::runtime_error("Не удалось создать VkInstance!");
		}
	}

	// создаем поверхность
	void createSurface() {
		if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("Не удалось создать поверхность для рисования!");
		}
	}
	// проверка поддержки цепочки буферов
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
		SwapChainSupportDetails details;

		// проверяем поддерживаемые возможности
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		uint32_t formatCount;
		// сначала получаем количество поддерживаемых форматов
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0) {
			details.formats.resize(formatCount); // расширяем вектор, чтобы вместить все данные
			// получаем все поддерживаемые форматы
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		// получаем количество поддерживаемых режимов представления
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			// получаем все поддерживаемые режимы представления
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}
	// выбираем физическое устройство
	void selectPhysicalDevice() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if (deviceCount == 0) {
			throw std::runtime_error("Подходящее устройство GPU отсутствует!");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		// выбираем первое подходящее устройство
		for (const auto& device : devices) {
			if (isDeviceSuitable(device)) {
				physicalDevice = device;
				break;
			}
		}

		if (physicalDevice == VK_NULL_HANDLE) {
			throw std::runtime_error("Подходящее устройство GPU отсутствует!");
		}
	}

	// создание логического устройства
	void createLogicalDevice() {
		// получаем обе очереди
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		// набор для индексов семейств очередей, если они совпадают, то остается только один индекс
		std::set<uint32_t> uniqueQueueFamilies = {
			indices.graphicsFamilyIndex,
			indices.presentFamilyIndex
		};

		float queuePriority = 1.0f;
		// проходим по всем индексам семейств очередей и создаем для каждого индекса структуру VkDeviceQueueCreateInfo
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		// устанавливаем обе очереди для создания устройства
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		// подключаемые расширения
		const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		// подключаем расширения
		// устанавливаем количество расширений
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		// устанавливаем сами расширения
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		// устанавливаем поддерживаемые возможности
		VkPhysicalDeviceFeatures deviceFeatures{};
		createInfo.pEnabledFeatures = &deviceFeatures;

		// создаем логическое устройство
		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
			throw std::runtime_error("Не удалось создать логическое устройство!");
		}

		// создаем графическую очередь
		vkGetDeviceQueue(device, indices.graphicsFamilyIndex, 0, &graphicsQueue);
		// создаем очередь представления
		vkGetDeviceQueue(device, indices.presentFamilyIndex, 0, &presentQueue);
	}

	// проверяем поддержку цепочки буферов
	bool checkSwapchainExtensionSupport(VkPhysicalDevice device) {
		uint32_t extensionCount;
		// сначала получаем количество расширений
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		// получаем все доступные расширения
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::string extensionName{ VK_KHR_SWAPCHAIN_EXTENSION_NAME }; // строка, с которой сравниванием

		for (const auto& extension : availableExtensions) {
			// сравнение строк
			if (extensionName == extension.extensionName) return true;
		}

		return false;
	}
	// проверка, является ли устройство подходящим
	bool isDeviceSuitable(VkPhysicalDevice device) {
		// получаем индексы графической очереди и очереди представления
		QueueFamilyIndices indices = findQueueFamilies(device);
		// если не поддерживаются графическая очередь и/или очередь представления
		if (!indices.isComplete()) return false;
		// если не поддерживаются расширения
		if (!checkSwapchainExtensionSupport(device)) return false;
		// получаем поддержку цепочки буферов
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		// считаем устройство подходящим, если поддерживается хотя бы один формат и хотя бы один режим представления
		return !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}
	// получаем индексы семейства очередей
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		// проходим по всем очередям
		for (const auto& queueFamily : queueFamilies) {
			// получаем индекс семейства с графической очередью
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamilyIndex = i;
				indices.graphicsSupport = true;
			}
			// смотрим, поддерживается ли очередь представления
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &indices.presentSupport);
			if (indices.presentSupport)  indices.presentFamilyIndex = i;
			if (indices.isComplete())  break; // если обе очереди установлены
			i++;
		}
		return indices;
	}
};

int main() {
	HelloApplication app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}*/


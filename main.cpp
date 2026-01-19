#include <vulkan/vulkan.h>  // подключаем библиотеку Vulkan
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
void NewSpaceLine() {
    std::cout << "\n===========================================================================\n";
}
class FirstApplication{
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }
    void info() {
        printAvailableExtensions();
        printAvailableDevices();
    }
private:
#pragma region parametres
    VkInstance vkInstance;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
#pragma endregion
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
    }
    void initVulkan() {
        createInstance();
    }
    void mainLoop() {

    }
    void cleanup() {

    }
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
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        return deviceFeatures.geometryShader && (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
            || deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
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
            std::cout << "UWUPS :3 ERROR(~_~)\n";
        }
        if (vkResult == VK_SUCCESS) {
            std::cout << "VKINSTANCE CREATED\n";
            NewSpaceLine();
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
    std::cout << "VulkanAPP info:";
    NewSpaceLine();
    FirstApplication app;
    app.run();
    app.info();
    //ИНФОРМАЦИЯ О РАСШИРЕНИЯХ УСТРОЙСТВА
    /*    uint32_t extensionDeviceCount = 0;
    vkEnumerateDeviceExtensionProperties(nullptr, &extensionDeviceCount, nullptr);//НУЖНО УСТРОЙСТВО(оставлю на потом)
    std::vector<VkExtensionProperties> extensionsDevice(extensionDeviceCount);
    vkEnumerateDeviceExtensionProperties(nulllptr, &extensionDeviceCount, extensionsDevice.data());
    */
    //ИНФОРМАЦИЯ О ФИЗИЧЕСКИХ УСТРОЙСТВАХ
    //uint32_t deviceCount = 0;
    //vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);//ПОЛУЧЕНИЕ КОЛИЧЕСТВА ФИЗИЧЕСКИХ УСТРОЙСТВ
    //std::vector<VkPhysicalDevice> devices(deviceCount);
    //vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());
    //VkPhysicalDeviceProperties deviceProperties;
    //VkPhysicalDeviceFeatures deviceFeatures;
    //VkPhysicalDevice AMD_RADEON_GRAPHICS = selectDevice(&vkInstance);
    //for (const auto& device : devices) {
    //    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    //    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    //    std::cout << deviceProperties.deviceName << std::endl;
    //    std::cout << deviceCount << std::endl;
    //    std::cout << "Support for geometry shader: " << (deviceFeatures.geometryShader ? "Yes" : "No") << std::endl;
    //    std::cout << "Support for robust Buffer Access: " << (deviceFeatures.robustBufferAccess ? "Yes" : "No") << std::endl;
    //    std::cout << "Support for image Cude Array: " << (deviceFeatures.imageCubeArray ? "Yes" : "No") << std::endl;
    //    std::cout << "Support for discrete GPU: " << (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? "Yes" : "No") << std::endl;
    //    std::cout << "Support for integrated GPU: " << (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ? "Yes" : "No") << std::endl;

    //}
    //NewSpaceLine();
    //ИНФОРМАЦИЯ ОБ ОЧЕРЕДЯХ ФИЗИЧЕСКОГО УСТРОЙСТВА
  /*  uint32_t familyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(AMD_RADEON_GRAPHICS, &familyCount, nullptr);
    std::vector<VkQueueFamilyProperties> families(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(AMD_RADEON_GRAPHICS, &familyCount, families.data());
    for (const auto& family : families) {
        if (family.queueFlags && VK_QUEUE_COMPUTE_BIT) {
            std::cout << "Queue count: " << family.queueCount << std::endl;
            std::cout << "Flags: " << std::hex << family.queueFlags << "\n\n";
        }
    }
    NewSpaceLine();
    vkDestroyInstance(vkInstance, nullptr);*/
    return 0;

}


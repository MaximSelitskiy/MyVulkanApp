#include <vulkan/vulkan.h>  // подключаем библиотеку Vulkan
#include <GLFW/glfw3.h>
#include <iostream>
#include <optional>
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
#pragma region VulkanPhysicalDeviceQueues
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        bool isComplete() {
            return graphicsFamily.has_value();
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
        return indices.isComplete();
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
    return 0;

}


#include <vulkan/vulkan.h>  // подключаем библиотеку Vulkan
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
VkInstance vkInstance;
void NewSpaceLine() {
    std::cout << "\n===========================================================================\n";
}
VkPhysicalDevice selectDevice(VkInstance *instance) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(*instance, &deviceCount, nullptr);
    if (!deviceCount)
    {
        return VK_NULL_HANDLE;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(*instance, &deviceCount, devices.data());

    return devices[0];
}
class FirstApplication{
public:
private:

};
int main() {
    std::cout << "VulkanAPP info:";
    NewSpaceLine();
    //ИНФОРМАЦИЯ ДРАЙВЕРУ
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.enabledLayerCount = 0;
    //ИНФОРМАЦИЯ О ПРИЛОЖЕНИИ
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "ASUS_APP";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "ASUS_ENGINE";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_4;
    createInfo.pApplicationInfo = &appInfo;
    //ИНФОРМАЦИЯ О РАСШИРЕНИЯХ ЭКЗЭМПЛЯРА
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    std::cout << "Available extensions: \n";
    for (const auto& extension : extensions) {
        std::cout << extension.extensionName << std::endl;
    }
    //ИНФОРМАЦИЯ О РАСШИРЕНИЯХ УСТРОЙСТВА
    /*    uint32_t extensionDeviceCount = 0;
    vkEnumerateDeviceExtensionProperties(nullptr, &extensionDeviceCount, nullptr);//НУЖНО УСТРОЙСТВО(оставлю на потом)
    std::vector<VkExtensionProperties> extensionsDevice(extensionDeviceCount);
    vkEnumerateDeviceExtensionProperties(nulllptr, &extensionDeviceCount, extensionsDevice.data());
    */
    //УСТАНОВКА РАСШИРЕНИЙ С БИБЛИОТЕКОЙ GLFW
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    VkInstance vkInstanceGLFW;
    VkInstanceCreateInfo createInfoGLFW{};
    createInfoGLFW.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfoGLFW.enabledExtensionCount = glfwExtensionCount;
    createInfoGLFW.ppEnabledExtensionNames = glfwExtensions;
    NewSpaceLine();
    if (vkCreateInstance(&createInfo, nullptr, &vkInstanceGLFW) != VK_SUCCESS) {
        std::cout << "Unable to create VKInstanceGLFW" << std::endl;
        return 1;
    }
    else if(vkCreateInstance(&createInfo, nullptr, &vkInstanceGLFW) == VK_SUCCESS) {
        std::cout << "VKInstanceGLFW created" << std::endl;
    }
    NewSpaceLine();
    VkResult vkResult = vkCreateInstance(&createInfo, nullptr, &vkInstance);
    //ИНФОРМАЦИЯ О ФИЗИЧЕСКИХ УСТРОЙСТВАХ
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);//ПОЛУЧЕНИЕ КОЛИЧЕСТВА ФИЗИЧЕСКИХ УСТРОЙСТВ
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    VkPhysicalDevice AMD_RADEON_GRAPHICS = selectDevice(&vkInstance);
    for (const auto& device : devices) {
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        std::cout << deviceProperties.deviceName << std::endl;
        std::cout << deviceCount << std::endl;
        std::cout << "Support for geometry shader: " << (deviceFeatures.geometryShader ? "Yes" : "No") << std::endl;
        std::cout << "Support for robust Buffer Access: " << (deviceFeatures.robustBufferAccess ? "Yes" : "No") << std::endl;
        std::cout << "Support for image Cude Array: " << (deviceFeatures.imageCubeArray ? "Yes" : "No") << std::endl;
        std::cout << "Support for discrete GPU: " << (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? "Yes" : "No") << std::endl;
        std::cout << "Support for integrated GPU: " << (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ? "Yes" : "No") << std::endl;

    }
    NewSpaceLine();
    //ИНФОРМАЦИЯ ОБ ОЧЕРЕДЯХ ФИЗИЧЕСКОГО УСТРОЙСТВА
    uint32_t familyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(AMD_RADEON_GRAPHICS, &familyCount, nullptr);
    std::vector<VkQueueFamilyProperties> families(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(AMD_RADEON_GRAPHICS, &familyCount, families.data());
    for (const auto& family : families) {
        if (family.queueFlags && VK_QUEUE_COMPUTE_BIT)  {
            std::cout << "Queue count: " << family.queueCount << std::endl;
            std::cout << "Flags: " << std::hex << family.queueFlags << "\n\n";
        }
    }
    NewSpaceLine();

    if (vkResult != VK_SUCCESS) {
        std::cout << "GG";
    }
    else {
        std::cout << "API VERSION: " << appInfo.apiVersion << "\n";
        NewSpaceLine();
        std::cout << "APPLICATION NAME: " << appInfo.pApplicationName << "\n";
        NewSpaceLine();
        std::cout << "VKINSTANCE CREATED\n";
        NewSpaceLine();
    }
    vkDestroyInstance(vkInstanceGLFW, nullptr);
    vkDestroyInstance(vkInstance, nullptr);
    return 0;
}
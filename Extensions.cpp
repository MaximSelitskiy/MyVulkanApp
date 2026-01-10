//typedef struct VkExtensionProperties { //Вообще расширения представляют структуру , которая имеет два поля
//    char        extensionName[VK_MAX_EXTENSION_NAME_SIZE];  // имя расширения
//    uint32_t    specVersion;                                // версия расширения
//} VkExtensionProperties;



//VkResult vkEnumerateInstanceExtensionProperties( //Чтобы получить список поддерживаемых расширений перед созданием объекта VKInstance
//    const char* pLayerName, //, можно использовать функцию vkEnumerateInstanceExtensionProperties():
//    uint32_t* pPropertyCount,
//    VkExtensionProperties* pProperties);
/*

    -pLayerName: указатель на строку с имем слоя, из которого нужно извлечь расширения. Может быть NULL

    -pPropertyCount: количество расширений

    -pProperties: массив VkExtensionProperties для хранения сведений о расширениях. Может быть NULL
*/


/*#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
 
int main() {
 
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
 
    std::vector<VkExtensionProperties> extensions(extensionCount);
 
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
 
    std::cout << "Available extensions:\n";
 
    for (const auto& extension : extensions) {
        std::cout << extension.extensionName << std::endl;
    }
 
    return 0;
}*/


//Установка расширений с примером glfw
/*#define GLFW_INCLUDE_VULKAN
// #include <vulkan/vulkan.h>  // макрос GLFW_INCLUDE_VULKAN уже подключает Vulkan
#include <GLFW/glfw3.h>
 
#include <iostream>
 
int main() {
 
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    // 1) получаем расширения библиотеки GLFW для Vulkan
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
 
    VkInstance instance; // объект, который надо создать
 
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
 
    // 2) устанавливаем расширения
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
 
    // создаем объект VkInstance
    if(vkCreateInstance(&createInfo, nullptr, &instance)!= VK_SUCCESS){
        std::cout << "Unable to create VKInstance" << std::endl;
        return 1;
    }
    else(vkCreateInstance(&createInfo, nullptr, &instance)!= VK_SUCCESS){
        std::cout << "VKInstance created" << std::endl;
    }
    // удаление VKInstance
    vkDestroyInstance(instance, nullptr);
 
    return 0;
}*/
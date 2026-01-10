/*VkResult vkCreateInstance(
    const VkInstanceCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkInstance* pInstance);*/ //Для создания объекта VkInstance применяется функция vkCreateInstance():
/*

    -pCreateInfo: указатель на структуру VkInstanceCreateInfo, которая управляет созданием объекта VkInstance

    -pAllocator: управляет распределением памяти хоста. Может принимать значение NULL

    -pInstance: указывает на дескриптор VkInstance, через который возвращается созданный объект VkInstance
*/



//typedef struct VkInstanceCreateInfo {//VkInstanceCreateInfo - она сообщает драйверу Vulkan, какие глобальные расширения и уровни проверки мы хотим использовать
//    VkStructureType             sType;
//    const void* pNext;
//    VkInstanceCreateFlags       flags;
//    const VkApplicationInfo* pApplicationInfo;
//    uint32_t                    enabledLayerCount;
//    const char* const* ppEnabledLayerNames;
//    uint32_t                    enabledExtensionCount;
//    const char* const* ppEnabledExtensionNames;
//} VkInstanceCreateInfo;
/*

    -sType: тип структуры. В данном случае должен представлять значение VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO

    -pNext: NULL или указатель на структуру, которая расширяет эту структуру.

    -flags: битовая маска VkInstanceCreateFlagBits, указывающая поведение экземпляра.

    -pApplicationInfo: NULL или указатель на структуру VkApplicationInfo, которая представляет информацию о приложении.

    -enabledLayerCount: количество подключаемых глобальных слоев.

    -ppEnabledLayerNames: указатель на массив строк, содержащих имена подключаемых слоев.

    -enabledExtensionCount: количество подключаемых глобальных расширений.

    -ppEnabledExtensionNames: указатель на массив строк, содержащих имена подключаемых расширений.
*/



//#include <vulkan/vulkan.h>  // подключаем библиотеку Vulkan
//#include <iostream>
//
//int main() {
//    VkInstance instance; // объект, который надо создать
//
//    VkInstanceCreateInfo createInfo{};
//    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
//    createInfo.enabledLayerCount = 0;       // пока уровни нам не нужны, устанавливаем 0
//
//    // создаем объект VkInstance
//    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
//    // если не удалось создать объект VKInstance
//    if (result != VK_SUCCESS) {
//        std::cout << "Unable to create VKInstance" << std::endl;
//    }
//    else {
//        std::cout << "VKInstance is created" << std::endl;
//    }
//
//    // удаление VKInstance
//    vkDestroyInstance(instance, nullptr);
//
//    return 0;
//}

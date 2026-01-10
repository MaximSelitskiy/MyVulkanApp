/*VkResult vkEnumeratePhysicalDevices(  // Прежде всего мы можем получить все доступные физические устройства и выбрать одно из них для запуска приложения. Для этого применяется функция vkEnumeratePhysicalDevices():
    VkInstance                                  instance,
    uint32_t*                                   pPhysicalDeviceCount,
    VkPhysicalDevice*                           pPhysicalDevices);*/
/*instance: объект VkInstance

pPhysicalDeviceCount: указатель на целое число, через которое возвращается количество доступных физических устройств

pPhysicalDevices: указатель на массив дескрипторов VkPhysicalDevice. Может быть равен NULL*/


//Например, получим количество устройств (GPU), которые поддерживают Vulkan:
/*#include <vulkan/vulkan.h> 
#include <iostream>
 
int main() {
     
    VkInstance instance;
 
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
 
    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if(result!= VK_SUCCESS){
        std::cout << "Unable to create VKInstance" << std::endl;
        return 1;
    }
 
    uint32_t deviceCount = 0;
    // получаем количество устройств
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
 
    if (!deviceCount) {
        std::cout << "Устройства с поддержкой Vulkan отсутствуют" << std::endl;
    }
    else{
        std::cout << "Устройств с поддержкой Vulkan:" << deviceCount << std::endl;
    }
 
    vkDestroyInstance(instance, nullptr);
    return 0;
}*/

//Получение информации об устройстве
//функция vkGetPhysicalDeviceProperties() позволяет получить базовые свойства устройства, такие как имя, тип и поддерживаемая версия Vulkan
//void vkGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties* pProperties);


//В функцию передается дескриптор физического устройства - VkPhysicalDevice и указатель на структуру VkPhysicalDeviceProperties,
//через которую возвращаются свойства. Для получения свойств применяются поля структуры:
/*typedef struct VkPhysicalDeviceProperties {
    uint32_t                            apiVersion;
    uint32_t                            driverVersion;
    uint32_t                            vendorID;
    uint32_t                            deviceID;
    VkPhysicalDeviceType                deviceType;
    char                                deviceName[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE];
    uint8_t                             pipelineCacheUUID[VK_UUID_SIZE];
    VkPhysicalDeviceLimits              limits;
    VkPhysicalDeviceSparseProperties    sparseProperties;
} VkPhysicalDeviceProperties;*/
/*

    -apiVersion: версия Vulkan, поддерживаемая устройством

    -driverVersion: версия драйвера, указанная поставщиком.

    -vendorID: идентификатор поставщика физического устройства.

    -deviceID: идентификатор физического устройства.

    -deviceType: тип устройства (объект VkPhysicalDeviceType). Может принимать следующие значения:

    typedef enum VkPhysicalDeviceType {
        VK_PHYSICAL_DEVICE_TYPE_OTHER = 0,   // не соответствует ни одному из ниже определенных типов  
        VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU = 1,  // встроенная видеокарта
        VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU = 2,   // дискретная видеокарта
        VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU = 3,    // виртуальная видеркарта в виртуальном окружении
        VK_PHYSICAL_DEVICE_TYPE_CPU = 4,    // устройство обычно работает на том же процессоре, что и хост
    } VkPhysicalDeviceType;

    -deviceName: имя устройства.

    -pipelineCacheUUID: универсальный уникальный идентификатор устройства

    -limits: структура VkPhysicalDeviceLimits, определяющая ограничения физического устройства, специфичные для конкретного устройства.

    -sparseProperties: структура VkPhysicalDeviceSparseProperties, определяющая различные свойства физического устройства.
*/
/*#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
 
void displayDevices(VkInstance* instance){
 
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(*instance, &deviceCount, nullptr);
    // Если устройств с поддержкой Vulkan нет, то нет смысла что-то еще делать
    if (!deviceCount) {
        std::cout << "Устройства с поддержкой Vulkan отсутствуют" << std::endl;
        return;
    }
    // определяем вектор для хранения всех дескрипторов VkPhysicalDevice.
    std::vector<VkPhysicalDevice> devices(deviceCount);
    // получаем устройства
    vkEnumeratePhysicalDevices(*instance, &deviceCount, devices.data());
 
    VkPhysicalDeviceProperties deviceProperties;
    // перебираем полученные устройства
    for (const auto& device : devices) {
        // для каждого устройства получаем набор свойств
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        // выводим имя устройства на консоль
        std::cout << deviceProperties.deviceName << std::endl;
    }
}
 
int main() {
     
    VkInstance instance;
 
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
 
    if(vkCreateInstance(&createInfo, nullptr, &instance)!= VK_SUCCESS){
        std::cout << "Unable to create VKInstance" << std::endl;
        return 1;
    }
 
    // отображаем доступные устройства
    displayDevices(&instance);
 
    vkDestroyInstance(instance, nullptr);
    return 0;
}*/
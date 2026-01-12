/*#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <set>

// размеры окна
const uint32_t WIDTH = 300;
const uint32_t HEIGHT = 250;

// структура дл€ хранени€ индекса семейства графичесой очереди
struct QueueFamilyIndex {
    uint32_t graphicsFamilyIndex;  // сам индекс 
    VkBool32 graphicsSupport;       // есть ли поддержка графической очереди на устройстве
};

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
    VkSurfaceKHR surface;       // поверхность рисовани€

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE; // физическое устройство
    VkDevice device;        // логическое устройство

    VkQueue graphicsQueue;  // графическа€ очередь

    // создаем окно
    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        window = glfwCreateWindow(WIDTH, HEIGHT, "METANIT.COM", nullptr, nullptr);
    }
    // инициализаци€ всех аспектов Vulkan
    void initVulkan() {
        createInstance();           // создание VkInstance
        createSurface();            // создание поверхности рисовани€
        selectPhysicalDevice();     // выбор физического устройства
        createLogicalDevice();      // создание логического устройства
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwWaitEvents();
        }
    }

    void cleanup() {
        // удал€ем логическое устройство
        vkDestroyDevice(device, nullptr);
        // удал€ем поверхность
        vkDestroySurfaceKHR(instance, surface, nullptr);
        // удал€ем объект VkInstance
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
        // получаем расширени€ дл€ GLFW
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        // устанавливаем расширени€ дл€ VkInstance
        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("Ќе удалось создать VkInstance!");
        }
    }

    // создаем поверхность
    void createSurface() {
        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
            throw std::runtime_error("Ќе удалось создать поверхность окна!");
        }
    }
    // выбираем физическое устройство
    void selectPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            throw std::runtime_error("ѕодход€щее устройство GPU отсутствует!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        // выбираем первое подход€щее устройство
        for (const auto& device : devices) {
            if (isDeviceSuitable(device)) {
                physicalDevice = device;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("ѕодход€щее устройство GPU отсутствует!");
        }
    }
    // создание логического устройства
    void createLogicalDevice() {
        // получаем индекс графической очереди
        QueueFamilyIndex index = findQueueFamily(physicalDevice);
        if (index.graphicsSupport == false) {
            throw std::runtime_error("√рафическа€ очередь не поддерживаетс€");
        }

        float queuePriority = 1.0f;  // ѕриоритет очереди
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = index.graphicsFamilyIndex; // »ндекс семейства графических очередей
        queueCreateInfo.queueCount = 1;   // Ќам нужна 1 очередь из этого семейства
        queueCreateInfo.pQueuePriorities = &queuePriority;

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = 1;
        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.enabledExtensionCount = 0;

        // создаем логическое устройство
        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
            throw std::runtime_error("Ќе удалось создать логическое устройство!");
        }
        // создаем графическую очередь
        vkGetDeviceQueue(device, index.graphicsFamilyIndex, 0, &graphicsQueue);
    }

    // проверка, €вл€етс€ ли устройство подход€щим
    bool isDeviceSuitable(VkPhysicalDevice device) {
        QueueFamilyIndex index = findQueueFamily(device);
        // считаем устройство подход€щим, если есть поддержка графической очереди
        return index.graphicsSupport;
    }

    // получаем индекс семейства очередей
    QueueFamilyIndex findQueueFamily(VkPhysicalDevice device) {
        QueueFamilyIndex index;
        index.graphicsSupport = false;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            // получаем графическую очередь
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                index.graphicsFamilyIndex = i;
                index.graphicsSupport = true;
                break;
            }
            i++;
        }
        return index;
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
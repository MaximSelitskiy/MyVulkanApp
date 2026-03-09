// Файл main.cpp
// g++ main.cpp -lvulkan -lglfw -o app && ./app
// 
//#define GLFW_INCLUDE_VULKAN
//#include <GLFW/glfw3.h>
// 
//#include <iostream>
//#include <stdexcept>
//#include <vector>
//#include <cstring>
//#include <cstdlib>
//#include <set>
//#include <cstdint> // для uint32_t
//#include <limits> // для std::numeric_limits
//#include <algorithm> // для std::clamp
//#include <fstream>  // для загрузки файлов шейдеров
// 
//const uint32_t WIDTH = 300;
//const uint32_t HEIGHT = 250;
//struct SwapChainSupportDetails {
//    VkSurfaceCapabilitiesKHR capabilities;              // возможности поверхности
//    std::vector<VkSurfaceFormatKHR> formats;      // доступные форматы
//    std::vector<VkPresentModeKHR> presentModes;   // доступные режимы представления
//    std::vector<VkImageView> swapChainImageViews;  // представления изображений
//};
// 
// структура для хранения индексов семейств очередей
//struct QueueFamilyIndices {
//    uint32_t graphicsFamilyIndex;
//    uint32_t presentFamilyIndex;
// 
//    VkBool32 graphicsSupport;
//    VkBool32 presentSupport;
// 
//    bool isComplete() {
//        return graphicsSupport && presentSupport;
//    }
//};
// 
// функция загрузки файлов шейдеров
//static std::vector<char> readFile(const std::string& filename) {
//    std::ifstream file(filename, std::ios::ate | std::ios::binary);
// 
//     открываем файл
//    if (!file.is_open()) {
//        throw std::runtime_error("Не удалось загрузить файл!");
//    }
//    size_t fileSize = (size_t) file.tellg(); // получаем размер
//    std::vector<char> buffer(fileSize);  // определяем буфер для считывания
// 
//    file.seekg(0);      // перемещаемся в начало файла
//    file.read(buffer.data(), fileSize);  // считываем файл в буфер
// 
//    file.close();       // закрываем поток файла
// 
//    return buffer;
//}
// 
//class HelloApplication {
//public:
//    void run() {
//        initWindow();
//        initVulkan();
//        mainLoop();
//        cleanup();
//    }
// 
//private:
//    GLFWwindow* window;
// 
//    VkInstance instance;
//    VkSurfaceKHR surface;       // поверхность рисования
//    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;  // физическое устройство
//    VkDevice device;    // логическре устройство
//    VkQueue graphicsQueue;  // графическая очередь
//    VkQueue presentQueue;   // очередь представления
//    VkSwapchainKHR swapChain;  // цепочка буферов
//    VkFormat swapChainImageFormat; // формат изображений
//    VkExtent2D swapChainExtent;     // размер изображений
//    std::vector<VkImage> swapChainImages;  // полученные изображения
//    std::vector<VkImageView> swapChainImageViews; // представления изображений
//    VkRenderPass renderPass;  // проход рендеринга 
//    VkPipelineLayout pipelineLayout;  // макет графического конвейера
//    VkPipeline graphicsPipeline; // графический конвейер
//    std::vector<VkFramebuffer> swapChainFramebuffers;  // буферы кадров
//    VkCommandPool commandPool;  // пул команд
//    VkCommandBuffer commandBuffer;  // буфер команд
// 
//    VkSemaphore imageAvailableSemaphore;  // для сигнала, что изображение получено из цепочки кадров и готово к рендерингу
//    VkSemaphore renderFinishedSemaphore;  // для сигнала о том, что рендеринг завершен и можно начинать отображение
//    VkFence inFlightFence;      // чтобы гарантировать, что одновременно рендерится только один кадр
// 
//     
//    void initWindow() {
//        glfwInit();
// 
//        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
//        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
//        window = glfwCreateWindow(WIDTH, HEIGHT, "METANIT.COM", nullptr, nullptr);
//    }
// 
//     инициализация всех аспектов Vulkan
//    void initVulkan() {
//        createInstance();           // создание VkInstance
//        createSurface();            // создание поверхности рисования
//        selectPhysicalDevice();     // выбор физического устройства
//        createLogicalDevice();      // создание логического устройства
//        createSwapChain();         // создание цепочки буферов
//        createImageViews();         // создание представлений изображений
//        createRenderPass();         // создаем проход рендеринга
//        createGraphicsPipeline();  // создаем графический конвейер
//        createFramebuffers();   // создаем буферы кадров
//        createCommandPool(); // создаем пул команд
//        createCommandBuffer(); // создаем буфер команд
// 
//        createSyncObjects();  // создаем примитивы синхронизации
//    }
//     отрисовка кадров
//    void drawFrame() {
//         ожидаем получения сигнала от барьера
//        vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
//         сбрасываем барьер в несигнальное состояние
//        vkResetFences(device, 1, &inFlightFence);
// 
//         получаем индекс текущего изображения из цепочки буферов
//        uint32_t imageIndex;  
//        vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
//         
//        vkResetCommandBuffer(commandBuffer, 0);  // сбрасываем буфер команд
//        recordCommandBuffer(commandBuffer, imageIndex);  // записываем команды в буфер
// 
//         определяем информацию для отправки буфера команд в очередь
//        VkSubmitInfo submitInfo{};
//        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
// 
//        VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};  // ожидаем один семафор
//        submitInfo.waitSemaphoreCount = 1; // ожидаем один семафор
//        submitInfo.pWaitSemaphores = waitSemaphores;
//         этапы конвейера
//         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT - этап получения окончательного цвета
//        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
//        submitInfo.pWaitDstStageMask = waitStages;
// 
//         устанавливаем выполняемые команды
//        submitInfo.commandBufferCount = 1;   // один буфер команд
//        submitInfo.pCommandBuffers = &commandBuffer;  // выполняемый буфер команд
// 
//         устанавливаем, какие семафоры следует сигнализировать после завершения выполнения буфера команд
//         renderFinishedSemaphore будет сигнализировать о том, что команды завершили выполнение
//        VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
//        submitInfo.signalSemaphoreCount = 1;
//        submitInfo.pSignalSemaphores = signalSemaphores;
// 
//         отправляем буфер команд в графическую очередь
//        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS) {
//            throw std::runtime_error("Не удалось отправить буфер команд!");
//        }
// 
//        VkPresentInfoKHR presentInfo{};
//        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
// 
//         указываем, какие семафоры ждать, прежде чем может произойти отображение
//        presentInfo.waitSemaphoreCount = 1;
//        presentInfo.pWaitSemaphores = signalSemaphores;
//         
//        VkSwapchainKHR swapChains[] = {swapChain};  // цепочки кадров с изображениями для отображения
//        presentInfo.swapchainCount = 1;   // одна цепочка кадров
//        presentInfo.pSwapchains = swapChains;   // цепочки кадров, из которых показываем изображения
//        presentInfo.pImageIndices = &imageIndex; // индекс изображения
// 
//         отправляем запрос на показ изображения
//        vkQueuePresentKHR(presentQueue, &presentInfo);
//    }
//     создаем примитивы синхронизации
//    void createSyncObjects() {
//         информация для создания семафора
//        VkSemaphoreCreateInfo semaphoreInfo{};
//        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
// 
//         информация для создания барьера
//        VkFenceCreateInfo fenceInfo{};
//        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
//        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;  // переводим барьер в сигнальное состояние
//         создаем семафоры и барьер
//        if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
//            vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
//            vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
//            throw std::runtime_error("Не удалось создать примитивы синхронизации!");
//        }
//    }
//     запись команд в буфер
//    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
// 
//        VkCommandBufferBeginInfo beginInfo{};
//        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//         начинаем запись команд буфера
//        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
//            throw std::runtime_error("Не удалось начать запись команд в буфер!");
//        }
// 
//         информация для запуска прохода рендеринга
//        VkRenderPassBeginInfo renderPassInfo{};
//        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//        renderPassInfo.renderPass = renderPass;     // используемый проход рендеринга
//        renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex]; // изображения для отрисовки
//        renderPassInfo.renderArea.offset = {0, 0};  // смещение области рендеринга
//        renderPassInfo.renderArea.extent = swapChainExtent;  // размер области рендеринга
//        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};  // черный цвет
//        renderPassInfo.clearValueCount = 1;  // одно значение для очистки (для одного прикрепления)
//        renderPassInfo.pClearValues = &clearColor;  // устанавливаем цвет очистки
// 
//         начинаем проход рендеринга
//        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
// 
//         привязка графического конвейера к буферу команд
//        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
// 
//         устанавливаем область просмотра для буфера команд
//        VkViewport viewport{};
//        viewport.x = 0.0f;
//        viewport.y = 0.0f;
//        viewport.width = static_cast<float>(swapChainExtent.width);
//        viewport.height = static_cast<float>(swapChainExtent.height);
//        viewport.minDepth = 0.0f;
//        viewport.maxDepth = 1.0f;
//        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
// 
//         устанавливаем ножницы для буфера команд
//        VkRect2D scissor{};
//        scissor.offset = {0, 0};
//        scissor.extent = swapChainExtent;
//        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
// 
//         отрисовка
//        vkCmdDraw(commandBuffer, 3, 1, 0, 0);  // 3 вершины (треугольник), 1 объект для отрисовки
// 
//         завершаем проход рендеринга
//        vkCmdEndRenderPass(commandBuffer);
//         завершаем отрисовку
//        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
//            throw std::runtime_error("Не удалось выполнить запись буфера команд!");
//        }
//    }
//     создаем буфер команд
//    void createCommandBuffer() {
// 
//        VkCommandBufferAllocateInfo allocInfo{};
//        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//        allocInfo.commandPool = commandPool;      // устанавливаем пул команд
//        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;  // создаем первичный буфер команд
//        allocInfo.commandBufferCount = 1;  // только 1 буфер
// 
//         создаем буфер команд
//        if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
//            throw std::runtime_error("Не удалось создать буферы команд!");
//        }
//    }
//     создаем пул команд
//    void createCommandPool() {
//         получаем индексы семейств очередей
//        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);
// 
//        VkCommandPoolCreateInfo poolInfo{};
//        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
//        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;  // команды можно сбросить в начальное состояние
//        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamilyIndex; // индекс используемого семейства очередей
// 
//         создаем пул команд
//        if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
//            throw std::runtime_error("Не удалось создать пул команд!");
//        }
//    }
//     создание буферов кадров по каждому из изображений
//    void createFramebuffers() {
//         устанавливаем вектор требуемого размера, чтобы вместить все буферы кадров
//        swapChainFramebuffers.resize(swapChainImageViews.size());
// 
//         перебираем все представления изображений и для каждого создаем буфер кадров
//        for (size_t i{}; i < swapChainImageViews.size(); i++) {
//            VkImageView attachments[] = { swapChainImageViews[i] }; // определяем массив представлений изображений
// 
//             определяем конфигурацию буфера кадров
//            VkFramebufferCreateInfo framebufferInfo{};     
//            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
//            framebufferInfo.renderPass = renderPass;        // устанавливаем проход рендеринга
//            framebufferInfo.attachmentCount = 1;            // количество прикреплений - одно цветовое прикрепление
//            framebufferInfo.pAttachments = attachments;     // сами прикрепления
//            framebufferInfo.width = swapChainExtent.width;  // ширина кадра
//            framebufferInfo.height = swapChainExtent.height;    // высота кадра
//            framebufferInfo.layers = 1;                     // только один уровень
//             создание буфера кадров
//            if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
//                throw std::runtime_error("Не удалось создать буфер кадров!");
//            }
//        }
//    }
//     функция графического конвейера
//    void createGraphicsPipeline() {
//         загрузка файлов шейдеров
//        auto vertShaderCode = readFile("shaders/vert.spv");
//        auto fragShaderCode = readFile("shaders/frag.spv");
// 
//         преобразование кода шейдеров в модули  шейдеров
//        VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
//        VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);
// 
//         определение этапа для вершинного шейдера
//        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
//        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;     // этап вершинного шейдера
//        vertShaderStageInfo.module = vertShaderModule;  // модуль шейдера 
//        vertShaderStageInfo.pName = "main";     // точка входа в шейдер - функция main
// 
//         определение этапа для фрагментного шейдера
//        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
//        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
//        fragShaderStageInfo.module = fragShaderModule;
//        fragShaderStageInfo.pName = "main";
// 
//         определяем массив для дальнейшего использования
//        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
// 
//         определяем данные для передачи в вершинный шейдер
//        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
//        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
//        vertexInputInfo.vertexBindingDescriptionCount = 0;  // количество привязок вершин
//        vertexInputInfo.vertexAttributeDescriptionCount = 0;    // количество описаний вершин
// 
//         определяем параметры сборки входных данных
//        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
//        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
//        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;  // топология - создаем список треугольников
//        inputAssembly.primitiveRestartEnable = VK_FALSE;  // без перезапуска сборки примитивов
// 
//         параметры для создания области просмотра и ножниц
//        VkPipelineViewportStateCreateInfo viewportState{};
//        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
//        viewportState.viewportCount = 1;  // количество областей просмотра, используемых конвейером
//        viewportState.scissorCount = 1;  // количество ножниц
// 
//         настраиваем растеризатор
//        VkPipelineRasterizationStateCreateInfo rasterizer{};
//        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
//        rasterizer.depthClampEnable = VK_FALSE;         // отрасываем фрагменты за пределами ближней и дальней плоскостей
//        rasterizer.rasterizerDiscardEnable = VK_FALSE;  // не отбрасываем примитивы до этапа растеризации
//        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;  // заполняем область полигона фрагментами
//        rasterizer.lineWidth = 1.0f;                    // ширина линий
//        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;    //  отбрасываем треугольники, направление назад
//        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;     // треугольник с отрицательной площадью считается обращенным вперед
//        rasterizer.depthBiasEnable = VK_FALSE;          // не учитываем глубину фрагмента
// 
//         Настройкка мультисэмплинга
//        VkPipelineMultisampleStateCreateInfo multisampling{};
//        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
//        multisampling.sampleShadingEnable = VK_FALSE;    // без подключения Sample Shading
//        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;  // изображение с одним образцом на пиксель
// 
//         конфигурация для цветового перехода
//        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
//        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
//        colorBlendAttachment.blendEnable = VK_FALSE; // без смешивания цветов
// 
//         определяем глобальные настройки смешивания цветов
//        VkPipelineColorBlendStateCreateInfo colorBlending{};
//        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
//        colorBlending.logicOpEnable = VK_FALSE;  // без побитового смешивания цветов
//        colorBlending.logicOp = VK_LOGIC_OP_COPY; // используем исходный цвет
//        colorBlending.attachmentCount = 1;
//        colorBlending.pAttachments = &colorBlendAttachment;
//        colorBlending.blendConstants[0] = 0.0f;
//        colorBlending.blendConstants[1] = 0.0f;
//        colorBlending.blendConstants[2] = 0.0f;
//        colorBlending.blendConstants[3] = 0.0f;
// 
//         Определяем динамические состояния для конвейера для области просмотра и ножниц
//        std::vector<VkDynamicState> dynamicStates = {
//            VK_DYNAMIC_STATE_VIEWPORT,
//            VK_DYNAMIC_STATE_SCISSOR
//        };
//        VkPipelineDynamicStateCreateInfo dynamicState{};
//        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
//        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
//        dynamicState.pDynamicStates = dynamicStates.data();
// 
//         определяем конфигурацию макета конвейера
//        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
//        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//        pipelineLayoutInfo.setLayoutCount = 0;
//        pipelineLayoutInfo.pushConstantRangeCount = 0;
// 
//         создаем макет конвейера
//        if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
//            throw std::runtime_error("Не удалось создать макет конвейера!");
//        }
// 
//         устанавливаем конфигурацию графического конвейера
//        VkGraphicsPipelineCreateInfo pipelineInfo{};
//        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
//        pipelineInfo.stageCount = 2;  // два этапа - вершинный и фрагментный шейдеры
//        pipelineInfo.pStages = shaderStages;
//        pipelineInfo.pVertexInputState = &vertexInputInfo;  // устанавливаем входные параметры для вершинного шейдера
//        pipelineInfo.pInputAssemblyState = &inputAssembly;  // устанавливаем параметры сборки входных данных
//        pipelineInfo.pViewportState = &viewportState;       // устанавливаем параметры для создания области просмотра и ножниц
//        pipelineInfo.pRasterizationState = &rasterizer;     // настраиваем растеризатор
//        pipelineInfo.pMultisampleState = &multisampling;     // настраиваем мультисэмплинг
//        pipelineInfo.pColorBlendState = &colorBlending;     // конфигурация для цветового перехода
//        pipelineInfo.pDynamicState = &dynamicState;         // устанавливаем динамические состояния для конвейера для области просмотра и ножниц
//        pipelineInfo.layout = pipelineLayout;  // установка макета конвейера
//        pipelineInfo.renderPass = renderPass;  // установка прохода рендеринга
//        pipelineInfo.subpass = 0;   // индекс подпрохода
// 
//         создаем графический конвейер
//        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
//            throw std::runtime_error("Не удалось создать графический конвейер!");
//        }
//         удаление модулей шейдеров
//        vkDestroyShaderModule(device, fragShaderModule, nullptr);
//        vkDestroyShaderModule(device, vertShaderModule, nullptr);
//    } 
//     создаем проход рендеринга
//    void createRenderPass() {
//         определяем цветовое прикрепление
//        VkAttachmentDescription colorAttachment{};
//        colorAttachment.format = swapChainImageFormat;  // формат изображений, полученный из цепочки буферов
//        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;  // 1 образец на изображение
//        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;  // очистить буфер кадра до черного цвета перед рисованием нового кадра
//        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;  // отрисованное содержимое будет сохранено в памяти и может быть прочитано позже
//        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;  // не имеет значения
//        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;  // не имеет значения
//        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;  // не имеет значения
//        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;   // для вывода изображений на экран
// 
//         определяем ссылку на цветовое прикрепление
//        VkAttachmentReference colorAttachmentRef{};
//        colorAttachmentRef.attachment = 0;     // индекс прикрепления в массиве
//        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
// 
//         определяем подпроход рендеринга
//        VkSubpassDescription subpass{};
//        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // тип конвейера - графический конвейер
//        subpass.colorAttachmentCount = 1;  // количество цветовых прикреплений
//        subpass.pColorAttachments = &colorAttachmentRef; // выше созданная ссылка на цветовое прикрепление 
// 
//         определяем параметры для создания прохода рендеринга
//        VkRenderPassCreateInfo renderPassInfo{};
//        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
//        renderPassInfo.attachmentCount = 1;     // количество применяемых прикреплений
//        renderPassInfo.pAttachments = &colorAttachment; // применяемые прикрепления
//        renderPassInfo.subpassCount = 1;  // количество подпроходов
//        renderPassInfo.pSubpasses = &subpass;   // используемые подпроходы
//         создаем проход рендеринга 
//        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
//            throw std::runtime_error("Не удалось создать проход рендеринга!");
//        }
//    }
//     создание модуля шейдера из байт-кода шейдера 
//    VkShaderModule createShaderModule(const std::vector<char>& code) {
// 
//        VkShaderModuleCreateInfo createInfo{};
//        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
//        createInfo.codeSize = code.size(); // устанавливаем размер байт-кода
// 
//        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data()); // приводим указатель к нужному типу
// 
//        VkShaderModule shaderModule; // создаваемый модуль шейдера
//        if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
//            throw std::runtime_error("Не удалось создать модуль шейдера!");
//        }
//        return shaderModule;
//    }
//     создание представлений из изображений цепочки буферов
//    void createImageViews() {
//        swapChainImageViews.resize(swapChainImages.size());  // устанавливаем размер вектора
//        for (size_t i{}; i < swapChainImages.size(); i++) {
//            VkImageViewCreateInfo createInfo{};
//            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//            createInfo.image = swapChainImages[i];
//            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;    // изображение рассматривается как 2D-текстура
//            createInfo.format = swapChainImageFormat;   // формат изображений
// 
//             rgba-компоненты получают значения по умолчанию
//            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
//            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
//            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
//            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
// 
//            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;  
//            createInfo.subresourceRange.baseMipLevel = 0;
//            createInfo.subresourceRange.levelCount = 1;
//            createInfo.subresourceRange.baseArrayLayer = 0;
//            createInfo.subresourceRange.layerCount = 1;
//             создаем представление изображения
//            if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
//                throw std::runtime_error("Не удалось создать представление изображения!");
//            }
//        }
//    }
//     установка формата изображений
//    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
//        for (const auto& availableFormat : availableFormats) {
//            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
//                return availableFormat;
//            }
//        }
// 
//        return availableFormats[0];
//    }
//     определение режима представления
//    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
//        for (const auto& availablePresentMode : availablePresentModes) {
//            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
//                return availablePresentMode;
//            }
//        }
// 
//        return VK_PRESENT_MODE_FIFO_KHR;
//    }
//     определение размеров для цепочки буферов
//    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
//         если текущая ширина не указана как максимальное значение для uint32_t
//        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
//             то возвращаем текущие размеры
//            return capabilities.currentExtent;
//        } else {
//            int width, height;
//            glfwGetFramebufferSize(window, &width, &height);
// 
//            VkExtent2D actualExtent = {
//                static_cast<uint32_t>(width),
//                static_cast<uint32_t>(height)
//            };
// 
//            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
//            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
// 
//            return actualExtent;
//        }
//    }
//     создаение цепочки буферов
//    void createSwapChain() {
//         получаем поддерживаемые устройством форматы, режимы представления и размеры 
//        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);
// 
//         выбираем формат поверхности
//        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
//         выбираем режим представления
//        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
//         устанавливаем размеры
//        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
// 
//         определяем минимальное количество изображения, которое требуется для работы 
//         и увеличиваем его на 1 для оптимизации работы драйвера
//        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
//         проверяем, что не превышаем максимальное количество изображений, 
//         делая это, где 0 — это особое значение, которое означает, что максимума нет
//        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
//            imageCount = swapChainSupport.capabilities.maxImageCount;
//        }
// 
//         определяем конфигурацию для создания цепочки буферов
//        VkSwapchainCreateInfoKHR createInfo{};
//        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
//        createInfo.surface = surface;
//        createInfo.minImageCount = imageCount;  // минимальное количество изображений
//        createInfo.imageFormat = surfaceFormat.format;      // формат изображений
//        createInfo.imageColorSpace = surfaceFormat.colorSpace;  // цветовое пространство 
//        createInfo.imageExtent = extent;            // размеры изображений
//        createInfo.imageArrayLayers = 1;            // поверхность с одним представлением
//        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;  // изображение используется в качестве цветового прикрепления
// 
//         Указываем, как обрабатывать изображения цепочки буферов, которые будут использоваться в нескольких семействах очередей.
//        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
//        uint32_t queueFamilyIndices[] = {indices.graphicsFamilyIndex, indices.presentFamilyIndex};
// 
//         если семейство графической очереди отличается от очереди представления
//        if (indices.graphicsFamilyIndex != indices.presentFamilyIndex) {
//            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT; // обе очереди совместно владеют изображением
//            createInfo.queueFamilyIndexCount = 2;           // две очереди
//            createInfo.pQueueFamilyIndices = queueFamilyIndices; // массив индексов семейств очередей
//        } else {
//            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // изображением владеет одна очередь
//            createInfo.queueFamilyIndexCount = 0; // Необязательно
//            createInfo.pQueueFamilyIndices = nullptr; // Необязательно
//        }
//         не применяем никаких преобразований
//        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
//         игнорирум альфа-канал
//        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
// 
//        createInfo.presentMode = presentMode;  // режим представления
//        createInfo.clipped = VK_TRUE;        // включаем обрезку скрытых пикселей
// 
//        createInfo.oldSwapchain = VK_NULL_HANDLE; // нет старых цепочек буферов
// 
//         создаем цепочку буферов
//        if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
//            throw std::runtime_error("Не удалось создать цепочку буферов!");
//        }
//         сохраняем выбранные формат и размеры изображений
//        swapChainImageFormat = surfaceFormat.format;
//        swapChainExtent = extent;
// 
//         сначала получаем количество изображений
//        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
//         на основе полученного количества определяем вектор
//        swapChainImages.resize(imageCount);
//         получаем в вектор изображения
//        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
//    }
//    void mainLoop() {
//        while (!glfwWindowShouldClose(window)) {
//            glfwWaitEvents();
//            drawFrame();
//        }
//    }
// 
//    void cleanup() {
// 
//         удаляем семафоры
//        vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
//        vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
//         удаляем барьер
//        vkDestroyFence(device, inFlightFence, nullptr);
// 
//         удаляем пул команд
//        vkDestroyCommandPool(device, commandPool, nullptr);
// 
//         удаляем буферы кадров
//        for (auto framebuffer : swapChainFramebuffers) {
//            vkDestroyFramebuffer(device, framebuffer, nullptr);
//        }
//         удаляем конвейер
//        vkDestroyPipeline(device, graphicsPipeline, nullptr);
//         удаляем макет графического конвейера
//        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
//         удаляем проход рендеринга
//        vkDestroyRenderPass(device, renderPass, nullptr);
//         удаляем все представления изображений
//        for (auto imageView : swapChainImageViews) {
//            vkDestroyImageView(device, imageView, nullptr);
//        }
//         удаляем цепочку буферов
//        vkDestroySwapchainKHR(device, swapChain, nullptr);
//         удаляем логическое устройство
//        vkDestroyDevice(device, nullptr);
//         удаляем поверхность
//        vkDestroySurfaceKHR(instance, surface, nullptr);
//         удаляем объект VkInstance
//        vkDestroyInstance(instance, nullptr);
// 
//        glfwDestroyWindow(window);
//        glfwTerminate();
//    }
// 
//     создание VkInstance
//    void createInstance() {
// 
//        VkInstanceCreateInfo createInfo{};
//        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
// 
//        uint32_t glfwExtensionCount = 0;
//        const char** glfwExtensions;
//         получаем расширения для GLFW
//        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
//         устанавливаем расширения для VkInstance
//        createInfo.enabledExtensionCount = glfwExtensionCount;
//        createInfo.ppEnabledExtensionNames = glfwExtensions;
// 
//        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
//            throw std::runtime_error("Не удалось создать VkInstance!");
//        }
//    }
// 
//     создаем поверхность
//    void createSurface() {
//        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
//            throw std::runtime_error("Не удалось создать поверхность для рисования!");
//        }
//    }
//     проверка поддержки цепочки буферов
//    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
//        SwapChainSupportDetails details;
// 
//         проверяем поддерживаемые возможности
//        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
// 
//        uint32_t formatCount;
//         сначала получаем количество поддерживаемых форматов
//        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
// 
//        if (formatCount != 0) {
//            details.formats.resize(formatCount); // расширяем вектор, чтобы вместить все данные
//             получаем все поддерживаемые форматы
//            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
//        }
// 
//        uint32_t presentModeCount;
//         получаем количество поддерживаемых режимов представления
//        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
// 
//        if (presentModeCount != 0) {
//            details.presentModes.resize(presentModeCount); 
//             получаем все поддерживаемые режимы представления
//            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
//        }
// 
//        return details;
//    }
//     выбираем физическое устройство
//    void selectPhysicalDevice() {
//        uint32_t deviceCount = 0;
//        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
// 
//        if (deviceCount == 0) {
//            throw std::runtime_error("Подходящее устройство GPU отсутствует!");
//        }
// 
//        std::vector<VkPhysicalDevice> devices(deviceCount);
//        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
// 
//         выбираем первое подходящее устройство
//        for (const auto& device : devices) {
//            if (isDeviceSuitable(device)) {
//                physicalDevice = device;
//                break;
//            }
//        }
// 
//        if (physicalDevice == VK_NULL_HANDLE) {
//            throw std::runtime_error("Подходящее устройство GPU отсутствует!");
//        }
//    }
// 
//     создание логического устройства
//    void createLogicalDevice() {
//         получаем обе очереди
//        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
// 
//        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
//         набор для индексов семейств очередей, если они совпадают, то остается только один индекс
//        std::set<uint32_t> uniqueQueueFamilies = {
//            indices.graphicsFamilyIndex,
//            indices.presentFamilyIndex
//        };
// 
//        float queuePriority = 1.0f;
//         проходим по всем индексам семейств очередей и создаем для каждого индекса структуру VkDeviceQueueCreateInfo
//        for (uint32_t queueFamily : uniqueQueueFamilies) {
//            VkDeviceQueueCreateInfo queueCreateInfo{};
//            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
//            queueCreateInfo.queueFamilyIndex = queueFamily;
//            queueCreateInfo.queueCount = 1;
//            queueCreateInfo.pQueuePriorities = &queuePriority;
//            queueCreateInfos.push_back(queueCreateInfo);
//        }
// 
//        VkDeviceCreateInfo createInfo{};
//        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
// 
//         устанавливаем обе очереди для создания устройства
//        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
//        createInfo.pQueueCreateInfos = queueCreateInfos.data();
//         
//         подключаемые расширения
//        const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME};
//         подключаем расширения 
//         устанавливаем количество расширений
//        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()); 
//         устанавливаем сами расширения
//        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
// 
//         устанавливаем поддерживаемые возможности
//        VkPhysicalDeviceFeatures deviceFeatures{};
//        createInfo.pEnabledFeatures = &deviceFeatures;
// 
//         создаем логическое устройство
//        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
//            throw std::runtime_error("Не удалось создать логическое устройство!");
//        }
// 
//         создаем графическую очередь
//        vkGetDeviceQueue(device, indices.graphicsFamilyIndex, 0, &graphicsQueue);
//         создаем очередь представления
//        vkGetDeviceQueue(device, indices.presentFamilyIndex, 0, &presentQueue);
//    }
// 
//     проверяем поддержку цепочки буферов
//    bool checkSwapchainExtensionSupport(VkPhysicalDevice device) {
//        uint32_t extensionCount;
//         сначала получаем количество расширений
//        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
// 
//         получаем все доступные расширения
//        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
//        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
// 
//        std::string extensionName {VK_KHR_SWAPCHAIN_EXTENSION_NAME}; // строка, с которой сравниванием
// 
//        for (const auto& extension : availableExtensions) {
//             сравнение строк
//            if(extensionName == extension.extensionName) return true;
//        }
// 
//        return false;
//    }
//    проверка, является ли устройство подходящим
//    bool isDeviceSuitable(VkPhysicalDevice device) {
//         получаем индексы графической очереди и очереди представления
//        QueueFamilyIndices indices = findQueueFamilies(device);
//         если не поддерживаются графическая очередь и/или очередь представления
//        if(!indices.isComplete()) return false; 
//         если не поддерживаются расширения
//        if(!checkSwapchainExtensionSupport(device)) return false;
//         получаем поддержку цепочки буферов
//        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
//         считаем устройство подходящим, если поддерживается хотя бы один формат и хотя бы один режим представления
//        return !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
//    }
//     получаем индексы семейства очередей
//    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
//        QueueFamilyIndices indices;
// 
//        uint32_t queueFamilyCount = 0;
//        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
// 
//        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
//        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
// 
//        int i = 0;
//         проходим по всем очередям
//        for (const auto& queueFamily : queueFamilies) {
//             получаем индекс семейства с графической очередью
//            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
//                indices.graphicsFamilyIndex = i;
//                indices.graphicsSupport = true;
//            }
//             смотрим, поддерживается ли очередь представления
//            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &indices.presentSupport);
//            if (indices.presentSupport)  indices.presentFamilyIndex = i;
//            if (indices.isComplete())  break; // если обе очереди установлены
//            i++;
//        }
//        return indices;
//    }
//};
// 
//int main() {
//    HelloApplication app;
// 
//    try {
//        app.run();
//    } catch (const std::exception& e) {
//        std::cerr << e.what() << std::endl;
//        return EXIT_FAILURE;
//    }
// 
//    return EXIT_SUCCESS;
//}
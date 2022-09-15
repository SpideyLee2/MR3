#include "Application.h"

// Reformatted from ImGui OpenGL3 example
#include <stdio.h>
#include <iostream>

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>

#include "Camera.h"
#include "Texture2D.h"

// Try to get rid of these and make them member variables where it makes sense
#pragma region Global_Vars
Camera g_Camera{ glm::vec3(0.0f, 0.0f, 0.0f) };

// Transform matrices
glm::mat4 g_Model;
glm::mat4 view;
glm::mat4 projection;

// Initial window specs
int g_ScreenWidth = 800;
int g_ScreenHeight = 600;
int g_RefreshRate = 200;
bool g_isFullscreen = false;

const char* const TITLE = "MR3 Rendering Engine";

// Camera controls
double lastXPos = g_ScreenWidth / 2;
double lastYPos = g_ScreenHeight / 2;
bool firstMouseMove = true;

bool wireframeOn = false;
bool cursorLocked = true;

float screenQuadVertices[] = {
    // Positions	// Tex coords
    -1.0, -1.0,		 0.0,  0.0,
     1.0, -1.0,		 1.0,  0.0,
     1.0,  1.0,		 1.0,  1.0,

    -1.0, -1.0,		 0.0,  0.0,
     1.0,  1.0,		 1.0,  1.0,
    -1.0,  1.0,		 0.0,  1.0
};
float cubeVertices[] = {
    // Positions            // Normals              // Tex Coords
    // Back face
    -1.0f, -1.0f, -1.0f,     0.0f,  0.0f, -1.0f,     0.0f, 0.0f, // bottom-left
     1.0f,  1.0f, -1.0f,     0.0f,  0.0f, -1.0f,     1.0f, 1.0f, // top-right
     1.0f, -1.0f, -1.0f,     0.0f,  0.0f, -1.0f,     1.0f, 0.0f, // bottom-right         
     1.0f,  1.0f, -1.0f,     0.0f,  0.0f, -1.0f,     1.0f, 1.0f, // top-right
    -1.0f, -1.0f, -1.0f,     0.0f,  0.0f, -1.0f,     0.0f, 0.0f, // bottom-left
    -1.0f,  1.0f, -1.0f,     0.0f,  0.0f, -1.0f,     0.0f, 1.0f, // top-left
    // Front face		  				  
    -1.0f, -1.0f,  1.0f,     0.0f,  0.0f,  1.0f,     0.0f, 0.0f, // bottom-left
     1.0f, -1.0f,  1.0f,     0.0f,  0.0f,  1.0f,     1.0f, 0.0f, // bottom-right
     1.0f,  1.0f,  1.0f,     0.0f,  0.0f,  1.0f,     1.0f, 1.0f, // top-right
     1.0f,  1.0f,  1.0f,     0.0f,  0.0f,  1.0f,     1.0f, 1.0f, // top-right
    -1.0f,  1.0f,  1.0f,     0.0f,  0.0f,  1.0f,     0.0f, 1.0f, // top-left
    -1.0f, -1.0f,  1.0f,     0.0f,  0.0f,  1.0f,     0.0f, 0.0f, // bottom-left
    // Left face		  				  
    -1.0f,  1.0f,  1.0f,    -1.0f,  0.0f,  0.0f,     1.0f, 0.0f, // top-right
    -1.0f,  1.0f, -1.0f,    -1.0f,  0.0f,  0.0f,     1.0f, 1.0f, // top-left
    -1.0f, -1.0f, -1.0f,    -1.0f,  0.0f,  0.0f,     0.0f, 1.0f, // bottom-left
    -1.0f, -1.0f, -1.0f,    -1.0f,  0.0f,  0.0f,     0.0f, 1.0f, // bottom-left
    -1.0f, -1.0f,  1.0f,    -1.0f,  0.0f,  0.0f,     0.0f, 0.0f, // bottom-right
    -1.0f,  1.0f,  1.0f,    -1.0f,  0.0f,  0.0f,     1.0f, 0.0f, // top-right
    // Right face		  				  
     1.0f,  1.0f,  1.0f,     1.0f,  0.0f,  0.0f,     1.0f, 0.0f, // top-left
     1.0f, -1.0f, -1.0f,     1.0f,  0.0f,  0.0f,     0.0f, 1.0f, // bottom-right
     1.0f,  1.0f, -1.0f,     1.0f,  0.0f,  0.0f,     1.0f, 1.0f, // top-right         
     1.0f, -1.0f, -1.0f,     1.0f,  0.0f,  0.0f,     0.0f, 1.0f, // bottom-right
     1.0f,  1.0f,  1.0f,     1.0f,  0.0f,  0.0f,     1.0f, 0.0f, // top-left
     1.0f, -1.0f,  1.0f,     1.0f,  0.0f,  0.0f,     0.0f, 0.0f, // bottom-left     
    // Bottom face		  				  
    -1.0f, -1.0f, -1.0f,     0.0f, -1.0f,  0.0f,     0.0f, 1.0f, // top-right
     1.0f, -1.0f, -1.0f,     0.0f, -1.0f,  0.0f,     1.0f, 1.0f, // top-left
     1.0f, -1.0f,  1.0f,     0.0f, -1.0f,  0.0f,     1.0f, 0.0f, // bottom-left
     1.0f, -1.0f,  1.0f,     0.0f, -1.0f,  0.0f,     1.0f, 0.0f, // bottom-left
    -1.0f, -1.0f,  1.0f,     0.0f, -1.0f,  0.0f,     0.0f, 0.0f, // bottom-right
    -1.0f, -1.0f, -1.0f,     0.0f, -1.0f,  0.0f,     0.0f, 1.0f, // top-right
    // Top face			  				  
    -1.0f,  1.0f, -1.0f,     0.0f,  1.0f,  0.0f,     0.0f, 1.0f, // top-left
     1.0f,  1.0f , 1.0f,     0.0f,  1.0f,  0.0f,     1.0f, 0.0f, // bottom-right
     1.0f,  1.0f, -1.0f,     0.0f,  1.0f,  0.0f,     1.0f, 1.0f, // top-right     
     1.0f,  1.0f,  1.0f,     0.0f,  1.0f,  0.0f,     1.0f, 0.0f, // bottom-right
    -1.0f,  1.0f, -1.0f,     0.0f,  1.0f,  0.0f,     0.0f, 1.0f, // top-left
    -1.0f,  1.0f,  1.0f,     0.0f,  1.0f,  0.0f,     0.0f, 0.0f  // bottom-left        
};

unsigned int ubo;

unsigned int cubeVAO, cubeVBO;
unsigned int screenQuadVAO, screenQuadVBO;
unsigned int envMap, irradianceMap, prefilterMap, brdfLUT;

unsigned int sphereVAO = 0;
unsigned int indexCount;

glm::vec3 sunColor{ 1.0f, 1.0f, 1.0f };
glm::vec3 pointLightPos{ 0.0f, 4.0f, 0.0f };
glm::vec3 pointLightColor{ 100.0f, 100.0f, 100.0f };

glm::vec3 pointLightPositions[] {
    glm::vec3(-3.5f, 3.5f, 10.0f),
    glm::vec3(-3.5f, -3.5f, 10.0f),
    glm::vec3(3.5f, -3.5f, 10.0f),
    glm::vec3(3.5f, 3.5f, 10.0f)
};

glm::vec3 pointLightColors[] {
    glm::vec3(100.0f, 100.0f, 100.0f),
    glm::vec3(120.0f, 20.0f, 20.0f),
    glm::vec3(20.0f, 120.0f, 20.0f),
    glm::vec3(20.0f, 20.0f, 120.0f)
};

static MR3::Application* s_pInstance = nullptr;
#pragma endregion

#pragma region Function_Decls
void glfwErrorCallback(int error, const char* description);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mousePosCallback(GLFWwindow* window, double xPos, double yPos);
//void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void messageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                     GLsizei length, GLchar const* message, void const* user_param);
void genCubeVAOandVBO(unsigned int& cubeVAO, unsigned int& cubeVBO);
void genQuadVAOandVBO(unsigned int& screenQuadVAO, unsigned int& screenQuadVBO);
void drawPointLight(const Shader& shader, const int& i);
void drawBlinnPhongCube(const Shader& shader, const Texture2D& diffuse, const Texture2D& specular);
void drawPBRCube(const glm::mat4& model, const Shader& shader, const Texture2D& albedo, const Texture2D& normal,
                 const Texture2D& metallic, const Texture2D& roughness, const Texture2D& ao);
void drawPBRDemo(const std::unordered_map<std::string, Shader>& shaderMap, const Texture2D& albedo,
                 const Texture2D& normal, const Texture2D& metallic, const Texture2D& roughness, const Texture2D& ao);
void drawPBRSphere(const Shader& shader, const glm::vec3& albedo, const float& metallicness, const float& roughness,
                   const float& ao);
void renderUVSphere();
void iblSetup(const Shader& equirectToCubemapShader, const Shader& convolutionShader,
                      const Shader& prefilterShader, const Shader& brdfConvolutionShader, 
                      const Texture2D& equirectMap);
void drawScreenQuad(const Shader& shader, unsigned int quadTexture);
void drawSkybox(const Shader& shader, const unsigned int& envMap);
#pragma endregion

namespace MR3 {
    #if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
    #pragma comment(lib, "legacy_stdio_definitions")
    #endif

    Application::Application(const AppSettings& appInit) : m_AppSettings(appInit) {
        init();
    }

    Application::~Application() {
        shutdown();

        s_pInstance = nullptr;
    }

    //Application* Application::start() {
    //    if (s_pInstance == nullptr) {
    //        s_pInstance = new Application();
    //    }
    //    return s_pInstance;
    //}

    Application* Application::start(AppSettings&& appInit) {
        if (s_pInstance == nullptr) {
            s_pInstance = new Application(appInit);
        }
        return s_pInstance;
    }

    void Application::init() {
        // ================ Initializing glfw window and glad =================
        if (!glfwInit()) {
            std::cout << "FATAL ERROR: Failed initialize GLFW library." << std::endl;
            exit(-1);
        }
        glfwSetErrorCallback(glfwErrorCallback);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        m_pWindowHandle = glfwCreateWindow(m_AppSettings.width, m_AppSettings.height, m_AppSettings.title, NULL, NULL);

        if (!m_pWindowHandle) {
            std::cout << "FATAL ERROR: Failed to create GLFW window." << std::endl;
            glfwTerminate();
            exit(-1);
        }

        glfwSetFramebufferSizeCallback(m_pWindowHandle, framebufferSizeCallback);
        glfwSetCursorPosCallback(m_pWindowHandle, mousePosCallback);

        //glfwSetScrollCallback(scrollCallback);
        glfwSetKeyCallback(m_pWindowHandle, keyCallback);
        glfwSetInputMode(m_pWindowHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        glfwMakeContextCurrent(m_pWindowHandle);

        glfwSwapInterval(true); // Enables vsync

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "FATAL ERROR: Failed to load Glad." << std::endl;
            glfwTerminate();
            exit(-1);
        }

        //glfwSetErrorCallback(glfwErrorCallback);

        // GL Debug
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(messageCallback, nullptr);
        // Use debug message control to enable/disable certain gl debug notifications
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);

        //Window window{ g_ScreenWidth, g_ScreenHeight, TITLE };
        //window.setFramebufferSizeCallback(framebufferSizeCallback);
        //window.setCursorPosCallback(mousePosCallback);
        ////window.setScrollCallback(scrollCallback);
        //window.setKeyCallback(keyCallback);
        //window.setInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        //window.swapInterval(true); // Enables vsync

        // ======================== Initializing ImGui ========================
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        imguiIO = &ImGui::GetIO(); (void)imguiIO;
        imguiIO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        //imguiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;        // Enable Gamepad Controls
        imguiIO->ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        imguiIO->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        //imguiIO.ConfigViewportsNoAutoMerge = true;
        //imguiIO.ConfigViewportsNoTaskBarIcon = true;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (imguiIO->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(m_pWindowHandle, true);
        ImGui_ImplOpenGL3_Init("#version 450");

        // Load Fonts
        // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use 
        //   ImGui::PushFont()/PopFont() to select them.
        // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among 
        //   multiple.
        // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application 
        //   (e.g. use an assertion, or display an error and quit).
        // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling 
        //   ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
        // - Read 'docs/FONTS.md' for more instructions and details.
        // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double 
        //   backslash \\ !
        //io.Fonts->AddFontDefault();
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
        //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, 
        //                                            io.Fonts->GetGlyphRangesJapanese());
        //IM_ASSERT(font != NULL);

        // Camera
        g_Camera = Camera(glm::vec3(0.0f, 0.0f, 5.0f));

        // Shaders
        m_Shaders.try_emplace("Basic", Shader("shaders/basic.vert", "shaders/basic.frag"));
        //m_Shaders.try_emplace("Blinn-Phong", Shader("shaders/blinn-phong.vert", "shaders/blinn-phong.frag"));
        m_Shaders.try_emplace("PBR Shapes", Shader("shaders/pbr.vert", "shaders/pbr_shapes.frag"));
        m_Shaders.try_emplace("PBR Models", Shader("shaders/pbr.vert", "shaders/pbr_models.frag"));
        m_Shaders.try_emplace("Equirect to CubeMap", Shader("shaders/equirect_to_cubemap.vert", 
                                                            "shaders/equirect_to_cubemap.frag"));
        m_Shaders.try_emplace("Convolute", Shader("shaders/equirect_to_cubemap.vert", "shaders/convolute.frag"));
        m_Shaders.try_emplace("Prefilter", Shader("shaders/equirect_to_cubemap.vert", "shaders/prefilter.frag"));
        m_Shaders.try_emplace("BRDF Convolution", Shader("shaders/screenQuad.vert", "shaders/brdf_convolution.frag"));
        m_Shaders.try_emplace("Skybox", Shader("shaders/skybox.vert", "shaders/skybox.frag"));
        // For Debugging
        m_Shaders.try_emplace("Screen Quad", Shader("shaders/screenQuad.vert", "shaders/screenQuad.frag"));

        stbi_set_flip_vertically_on_load(true);

        // UBO
        Shader& basicShader = m_Shaders.at("Basic");
        unsigned int matricesUniformBlockIndex = glGetUniformBlockIndex(basicShader.id, "Matrices");
        glUniformBlockBinding(basicShader.id, matricesUniformBlockIndex, 0);
        glCreateBuffers(1, &ubo);
        glNamedBufferStorage(ubo, m_SizeOfMat4 * 2, NULL, GL_DYNAMIC_STORAGE_BIT);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);
        //glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, sizeOfView + sizeOfProj);

        // Simple cube generation
        genCubeVAOandVBO(cubeVAO, cubeVBO);
        // Screen quad generation
        genQuadVAOandVBO(screenQuadVAO, screenQuadVBO);

        g_Model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    }

    void Application::run() {
        // Textures
        //Texture2D diffuse{ "textures/grid_diffuse.png", false };
        //Texture2D specular{ "textures/grid_diffuse.png", false };

        //Texture2D rustedIronAlbedo{ "textures/rustediron1-alt2-bl/rustediron2_basecolor.png" };
        //Texture2D rustedIronNormal{ "textures/rustediron1-alt2-bl/rustediron2_normal.png" };
        //Texture2D rustedIronMetallic{ "textures/rustediron1-alt2-bl/rustediron2_metallic.png" };
        //Texture2D rustedIronRoughness{ "textures/rustediron1-alt2-bl/rustediron2_roughness.png" };
        //Texture2D rustedIronAO{ "textures/placeholders/white.png" };

        const int NUM_EQUIRECT_MAPS = 5;
        const Texture2D equirectMaps[NUM_EQUIRECT_MAPS] = {
            { "textures/equirect_maps/Footprint_Court_8k_TMap.jpg", true },
            { "textures/equirect_maps/Milkyway_BG.jpg", true },
            { "textures/equirect_maps/Frozen_Waterfall_HiRes_TMap.jpg", true },
            { "textures/equirect_maps/Road_to_MonumentValley_8k.jpg", true },
            { "textures/equirect_maps/modern_house_indoors.png", true },
        };
        const char* equirectMapNames[NUM_EQUIRECT_MAPS] = {
            "Footprint Court",
            "Milky Way",
            "Frozen Waterfall",
            "Road to Monument Valley",
            "Modern House Indoors"
        };

        int selectedInd = 0;

        glm::vec3 clearColor{ 0.2f, 0.4f, 0.5f };
        glm::vec3 albedo{ 0.0f, 1.0f, 1.0f };
        float metallicness = 0.0f;
        float roughness = 0.001f;
        float ao = 1.0f;

        glDepthFunc(GL_LEQUAL);
        glEnable(GL_DEPTH_TEST);

        // Default Skybox
        iblSetup(m_Shaders.at("Equirect to CubeMap"), m_Shaders.at("Convolute"), m_Shaders.at("Prefilter"),
                 m_Shaders.at("BRDF Convolution"), equirectMaps[0]);

        glViewport(0, 0, g_ScreenWidth, g_ScreenHeight);
        // Render loop
        while (!glfwWindowShouldClose(m_pWindowHandle)) {
            // Delta-time
            m_CurrFrameTime = glfwGetTime();
            m_DeltaTime = m_CurrFrameTime - m_LastFrameTime;
            m_LastFrameTime = m_CurrFrameTime;

            glfwPollEvents();
            processInput();

            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            //ImGui::SetNextFrameWantCaptureMouse(false);

            // ========================================= ImGUI Window Objects =========================================
            {
                ImGui::Begin("Settings");
                // Clear Color Settings
                //if (ImGui::CollapsingHeader("Clear Color")) {
                //    ImGui::ColorEdit3("Clear Color", glm::value_ptr(clearColor));
                //}
                
                // PBR Shape Settings
                if (ImGui::CollapsingHeader("Object Settings")) {
                    ImGui::ColorEdit3("Albedo", glm::value_ptr(albedo));
                    ImGui::SliderFloat("Metallicness", &metallicness, 0.0f, 1.0f);
                    ImGui::SliderFloat("Roughness", &roughness, 0.001f, 1.0f); // 0 roughness causes black edge artifact
                    ImGui::SliderFloat("Ambient Occlusion", &ao, 0.0f, 1.0f);
                }
                // Skybox Settings
                if (ImGui::CollapsingHeader("Skybox Settings")) {
                    if (ImGui::BeginCombo("Skyboxes", equirectMapNames[selectedInd])) {
                        for (int i = 0; i < NUM_EQUIRECT_MAPS; ++i) {
                            const bool isSelected = selectedInd == i;
                            if (ImGui::Selectable(equirectMapNames[i], isSelected)) {
                                selectedInd = i;
                                std::cout << "Selected equirect map '" << equirectMapNames[selectedInd] << "' from list\n";
                                //glDeleteTextures(1, &envMap);
                                //glDeleteTextures(1, &irradianceMap);
                                //glDeleteTextures(1, &prefilterMap);
                                //glDeleteTextures(1, &brdfLUT);
                                glDisable(GL_CULL_FACE);
                                iblSetup(m_Shaders.at("Equirect to CubeMap"), m_Shaders.at("Convolute"),
                                         m_Shaders.at("Prefilter"), m_Shaders.at("BRDF Convolution"),
                                         equirectMaps[i]);
                                glEnable(GL_CULL_FACE);
                                glViewport(0, 0, g_ScreenWidth, g_ScreenHeight);
                            }
                            if (isSelected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }
                }
                ImGui::End();
            }
            // ========================================================================================================

            //ImGui::ShowDemoWindow();
            
            ImGui::Render();
            glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            view = g_Camera.getViewMatrix();
            glNamedBufferSubData(ubo, 0, m_SizeOfMat4, glm::value_ptr(view));
            projection = glm::perspective(glm::radians(g_Camera.m_Fov),
                                          static_cast<float>(g_ScreenWidth) / static_cast<float>(g_ScreenHeight),
                                          0.1f,
                                          100.0f);
            glNamedBufferSubData(ubo, m_SizeOfMat4, m_SizeOfMat4, glm::value_ptr(projection));

            //drawPBRDemo(m_Shaders, rustedIronAlbedo, rustedIronNormal, 
            //            rustedIronMetallic, rustedIronRoughness, rustedIronAO);
            //drawEquirectCubeMap(m_Shaders.at("Equirect to CubeMap"), equirectMap);
            glDisable(GL_CULL_FACE);
            drawSkybox(m_Shaders.at("Skybox"), envMap);
            glEnable(GL_CULL_FACE);

            drawPBRSphere(m_Shaders.at("PBR Shapes"), albedo, metallicness, roughness, ao);
            for (int i = 0; i < 4; ++i) {
                drawPointLight(m_Shaders.at("Basic"), i);
            }

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // Update and Render additional Platform Windows
            // Platform functions may change the current OpenGL context, so we save/restore it to make it easier to 
            // paste this code elsewhere.
            if (imguiIO->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                GLFWwindow* backup_current_context = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(backup_current_context);
            }

            glfwSwapBuffers(m_pWindowHandle);
        }
    }

    void Application::shutdown() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(m_pWindowHandle);
        glfwTerminate();
    }

    void Application::processInput() {
        // Camera Controls
        if (glfwGetKey(m_pWindowHandle, GLFW_KEY_W) == GLFW_PRESS) {
            g_Camera.input(CAMERA_MOVEMENT::Forward, m_DeltaTime);
        }
        if (glfwGetKey(m_pWindowHandle, GLFW_KEY_S) == GLFW_PRESS) {
            g_Camera.input(CAMERA_MOVEMENT::Backward, m_DeltaTime);
        }
        if (glfwGetKey(m_pWindowHandle, GLFW_KEY_A) == GLFW_PRESS) {
            g_Camera.input(CAMERA_MOVEMENT::Left, m_DeltaTime);
        }
        if (glfwGetKey(m_pWindowHandle, GLFW_KEY_D) == GLFW_PRESS) {
            g_Camera.input(CAMERA_MOVEMENT::Right, m_DeltaTime);
        }
        if (glfwGetKey(m_pWindowHandle, GLFW_KEY_SPACE) == GLFW_PRESS) {
            g_Camera.input(CAMERA_MOVEMENT::Up, m_DeltaTime);
        }
        if (glfwGetKey(m_pWindowHandle, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            g_Camera.input(CAMERA_MOVEMENT::Down, m_DeltaTime);
        }
        if (glfwGetKey(m_pWindowHandle, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            g_Camera.applySpeedMultiplier();
        }
        if (glfwGetKey(m_pWindowHandle, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
            g_Camera.removeSpeedMultiplier();
        }
    }
}

void glfwErrorCallback(int error, const char* description) {
    std::cout << "GLFW Error (" << error << "): " << description << std::endl;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Close window
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // Fullscreen
    if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
        if (!g_isFullscreen) {
            // Sets the window to display in fullscreen on the primary monitor
            const GLFWvidmode* vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            g_ScreenWidth = vidMode->width;
            g_ScreenHeight = vidMode->height;
            g_RefreshRate = vidMode->refreshRate;
            glfwSetWindowMonitor(window, NULL, 0, 0, g_ScreenWidth, g_ScreenHeight, g_RefreshRate);
            glViewport(0, 0, g_ScreenWidth, g_ScreenHeight);
        }
        else { 
            // Sets window to be displayed at 800x600 on the primary monitor
            const GLFWvidmode* vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            g_ScreenWidth = 800;
            g_ScreenHeight = 600;
            g_RefreshRate = vidMode->refreshRate;
            // Resizes window and move top-left corner away from edge of screen to prevent hiding the window bar
            glfwSetWindowMonitor(window, NULL, 50, 50, g_ScreenWidth, g_ScreenHeight, g_RefreshRate);
            glViewport(0, 0, g_ScreenWidth, g_ScreenHeight);
        }
        g_isFullscreen = !g_isFullscreen;
    }

    // Enable/Disable visible edge drawing
    if (key == GLFW_KEY_BACKSLASH && action == GLFW_PRESS) {
        wireframeOn ? glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) : glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        wireframeOn = !wireframeOn;
    }

    // Unlocks/Locks cursor in window
    if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS) {
        if (cursorLocked) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            firstMouseMove = false;
            ImGui::SetNextFrameWantCaptureMouse(true);
        }
        else {
            //glfwSetCursorPosCallback(window, mousePosCallback);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            firstMouseMove = true;
            ImGui::SetNextFrameWantCaptureMouse(false);
        }
        cursorLocked = !cursorLocked;
    }
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    g_ScreenWidth = width;
    g_ScreenHeight = height;
    glViewport(0, 0, g_ScreenWidth, g_ScreenHeight);
}

void mousePosCallback(GLFWwindow* window, double xPos, double yPos) {
    // Prevents jarring camera redirection when scene is in focus
    if (firstMouseMove) {
        lastXPos = xPos;
        lastYPos = yPos;
        firstMouseMove = false;
    }

    // Enables camera movement if the cursor is locked
    if (cursorLocked) {
        float xOffset = static_cast<float>(xPos - lastXPos);
        float yOffset = static_cast<float>(yPos - lastYPos);
        lastXPos = xPos;
        lastYPos = yPos;

        g_Camera.processMouseMove(xOffset, yOffset);
    }
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
	g_Camera.processMouseScroll(static_cast<float>(yOffset));
}

void messageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                     GLsizei length, GLchar const* message, void const* user_param) {
    auto const srcStr = [source]() {
        switch (source) {
            case GL_DEBUG_SOURCE_API: return "API";
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
            case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
            case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
            case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
            case GL_DEBUG_SOURCE_OTHER: return "OTHER";
            default: return "?";
        }
    }();

    auto const typeStr = [type]() {
        switch (type) {
            case GL_DEBUG_TYPE_ERROR: return "ERROR";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED BEHAVIOR";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED BEHAVIOR";
            case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
            case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
            case GL_DEBUG_TYPE_MARKER: return "MARKER";
            case GL_DEBUG_TYPE_OTHER: return "OTHER";
            default: return "?";
        }
    }();

    auto const severityStr = [severity]() {
        switch (severity) {
            case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
            case GL_DEBUG_SEVERITY_LOW: return "LOW";
            case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
            case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
            default: return "?";
        }
    }();
    std::cout << srcStr << "-" << typeStr << "-" << severityStr << " (" << id << "): " << message << '\n';
}

void genCubeVAOandVBO(unsigned int& cubeVAO, unsigned int& cubeVBO) {
    glCreateVertexArrays(1, &cubeVAO);
    glCreateBuffers(1, &cubeVBO);
    glVertexArrayVertexBuffer(cubeVAO, 0, cubeVBO, 0, sizeof(float) * 8);
    glNamedBufferStorage(cubeVBO, sizeof(cubeVertices), cubeVertices, GL_DYNAMIC_STORAGE_BIT);

    glVertexArrayAttribFormat(cubeVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribFormat(cubeVAO, 1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3);
    glVertexArrayAttribFormat(cubeVAO, 2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6);

    glEnableVertexArrayAttrib(cubeVAO, 0);
    glEnableVertexArrayAttrib(cubeVAO, 1);
    glEnableVertexArrayAttrib(cubeVAO, 2);

    glVertexArrayAttribBinding(cubeVAO, 0, 0);
    glVertexArrayAttribBinding(cubeVAO, 1, 0);
    glVertexArrayAttribBinding(cubeVAO, 2, 0);
}

void genQuadVAOandVBO(unsigned int& screenQuadVAO, unsigned int& screenQuadVBO) {
    glCreateVertexArrays(1, &screenQuadVAO);
    glCreateBuffers(1, &screenQuadVBO);
    glVertexArrayVertexBuffer(screenQuadVAO, 0, screenQuadVBO, 0, sizeof(float) * 4);
    glNamedBufferStorage(screenQuadVBO, sizeof(screenQuadVertices), screenQuadVertices, GL_DYNAMIC_STORAGE_BIT);

    glVertexArrayAttribFormat(screenQuadVAO, 0, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribFormat(screenQuadVAO, 1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2);

    glEnableVertexArrayAttrib(screenQuadVAO, 0);
    glEnableVertexArrayAttrib(screenQuadVAO, 1);

    glVertexArrayAttribBinding(screenQuadVAO, 0, 0);
    glVertexArrayAttribBinding(screenQuadVAO, 1, 0);
}

void drawPointLight(const Shader& shader, const int& i) {
    shader.use();

    glm::mat4 model = glm::translate(glm::mat4(1.0f), pointLightPositions[i]);
    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

    glUniformMatrix4fv(glGetUniformLocation(shader.id, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3fv(glGetUniformLocation(shader.id, "color"), 1, glm::value_ptr(pointLightColors[i]));

    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawPBRSphere(const Shader& shader, const glm::vec3& albedo, const float& metallicness, const float& roughness, 
                   const float& ao) {
    shader.use();
    glBindTextureUnit(0, irradianceMap);
    glBindTextureUnit(1, prefilterMap);
    glBindTextureUnit(2, brdfLUT);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

    glUniformMatrix4fv(glGetUniformLocation(shader.id, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3fv(glGetUniformLocation(shader.id, "material.albedo"), 1, glm::value_ptr(albedo));
    glUniform1f(glGetUniformLocation(shader.id, "material.metallicness"), metallicness);
    glUniform1f(glGetUniformLocation(shader.id, "material.roughness"), roughness);
    glUniform1f(glGetUniformLocation(shader.id, "material.ao"), ao);
    for (int i = 0; i < 4; ++i) {
        std::string index = std::to_string(i);
        glUniform3fv(glGetUniformLocation(shader.id, ("pointLights[" + index + "].position").c_str()), 1,
                     glm::value_ptr(pointLightPositions[i]));
        glUniform3fv(glGetUniformLocation(shader.id, ("pointLights[" + index + "].color").c_str()), 1,
                     glm::value_ptr(pointLightColors[i]));
    }
    glUniform3fv(glGetUniformLocation(shader.id, "cameraPos"), 1, glm::value_ptr(g_Camera.m_Position));
    glUniform1i(glGetUniformLocation(shader.id, "irradianceMap"), 0);
    glUniform1i(glGetUniformLocation(shader.id, "prefilterMap"), 1);
    glUniform1i(glGetUniformLocation(shader.id, "brdfLUT"), 2);

    renderUVSphere();
}

void renderUVSphere() {
    if (sphereVAO == 0) {
        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359f;
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                positions.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else {
                for (int x = X_SEGMENTS; x >= 0; --x) {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        indexCount = static_cast<unsigned int>(indices.size());

        std::vector<float> data;
        for (unsigned int i = 0; i < positions.size(); ++i) {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (normals.size() > 0) {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
            if (uv.size() > 0) {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
        }
        unsigned int vbo, ebo;
        glCreateVertexArrays(1, &sphereVAO);
        glCreateBuffers(1, &vbo);
        glCreateBuffers(1, &ebo);

        unsigned int stride = (3 + 2 + 3) * sizeof(float);
        glVertexArrayVertexBuffer(sphereVAO, 0, vbo, 0, stride);
        glVertexArrayElementBuffer(sphereVAO, ebo);
        glNamedBufferData(vbo, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glNamedBufferData(ebo, indices.size() * sizeof(float), &indices[0], GL_STATIC_DRAW);

        glEnableVertexArrayAttrib(sphereVAO, 0);
        glEnableVertexArrayAttrib(sphereVAO, 1);
        glEnableVertexArrayAttrib(sphereVAO, 2);

        glVertexArrayAttribFormat(sphereVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribFormat(sphereVAO, 1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3);
        glVertexArrayAttribFormat(sphereVAO, 2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6);

        glVertexArrayAttribBinding(sphereVAO, 0, 0);
        glVertexArrayAttribBinding(sphereVAO, 1, 0);
        glVertexArrayAttribBinding(sphereVAO, 2, 0);
    }

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}

void iblSetup(const Shader& equirectToCubemapShader, const Shader& convolutionShader,
                      const Shader& prefilterShader, const Shader& brdfConvolutionShader,
                      const Texture2D& equirectMap) {
    // ======================================= Creates the environment cubemap =======================================
    // Setup framebuffer for sampling environment map from cube
    unsigned int captureFBO, captureRBO;
    glCreateFramebuffers(1, &captureFBO);
    glCreateRenderbuffers(1, &captureRBO);
    glNamedRenderbufferStorage(captureRBO, GL_DEPTH_COMPONENT24, 512, 512);
    glNamedFramebufferRenderbuffer(captureFBO, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    // Generates the corresponding cubemap color textures and pre-allocates memory
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &envMap);
    glTextureStorage2D(envMap, 1, GL_RGB16F, 512, 512);

    glTextureParameteri(envMap, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(envMap, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(envMap, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTextureParameteri(envMap, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(envMap, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glm::mat4 captureProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[]{
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    // Converts HDR equirectangular environment map to cubemap
    equirectToCubemapShader.use();
    equirectMap.bind(0);

    glUniform1i(glGetUniformLocation(equirectToCubemapShader.id, "equirectangularMap"), 0);
    glNamedBufferSubData(ubo, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(captureProj));

    glViewport(0, 0, 512, 512);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    glBindVertexArray(cubeVAO);
    for (int i = 0; i < 6; ++i) {
        glNamedBufferSubData(ubo, 0, sizeof(glm::mat4), glm::value_ptr(captureViews[i]));

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenerateTextureMipmap(envMap); // Necessary for Chetan Jags' prefilter convolution fix to spotty prefiltering

    // =============== Creates the irradiance cubemap from the environment cubemap through convolution ===============
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &irradianceMap);
    // Texture can be low resolution since we're averaging the irradiance anyways
    glTextureStorage2D(irradianceMap, 1, GL_RGB16F, 32, 32);

    glTextureParameteri(irradianceMap, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(irradianceMap, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(irradianceMap, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTextureParameteri(irradianceMap, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(irradianceMap, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Might not be able to be called a second time
    glNamedRenderbufferStorage(captureRBO, GL_DEPTH_COMPONENT24, 32, 32);

    convolutionShader.use();
    glBindTextureUnit(0, envMap);

    glUniform1i(glGetUniformLocation(convolutionShader.id, "envMap"), 0);
    glNamedBufferSubData(ubo, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(captureProj));

    glViewport(0, 0, 32, 32);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    glBindVertexArray(cubeVAO);
    for (int i = 0; i < 6; ++i) {
        glNamedBufferSubData(ubo, 0, sizeof(glm::mat4), glm::value_ptr(captureViews[i]));

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ================================= Creates the pre-filtered HDR environment map =================================
    const unsigned int MAX_MIP_LEVELS = 5; // must be > 1

    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &prefilterMap);
    glTextureStorage2D(prefilterMap, MAX_MIP_LEVELS, GL_RGB16F, 128, 128);

    glTextureParameteri(prefilterMap, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(prefilterMap, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(prefilterMap, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTextureParameteri(prefilterMap, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Enables trilinear filtering
    glTextureParameteri(prefilterMap, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Allocates memory for the pre-filtered maps to be stored in the mip levels
    glGenerateTextureMipmap(prefilterMap);

    // Quasi Monte-Carlo prefiltering
    prefilterShader.use();
    glBindTextureUnit(0, envMap);

    glUniform1i(glGetUniformLocation(prefilterShader.id, "envMap"), 0);
    glNamedBufferSubData(ubo, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(captureProj));

    // Prevents cubemap edges from being so apparent at lower mip levels
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindVertexArray(cubeVAO);

    // Loads the mipmap of the prefilter environment map texture
    for (unsigned int mip = 0; mip < MAX_MIP_LEVELS; ++mip) {
        // Resizes the framebuffer according to the mip-level size
        unsigned int mipWidth  = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));

        glNamedRenderbufferStorage(captureRBO, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        // Determines the roughness based on the mip level
        float roughness = static_cast<float>(mip) / static_cast<float>(MAX_MIP_LEVELS - 1);
        glUniform1f(glGetUniformLocation(prefilterShader.id, "roughness"), roughness);

        for (int i = 0; i < 6; ++i) {
            glNamedBufferSubData(ubo, 0, sizeof(glm::mat4), glm::value_ptr(captureViews[i]));

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ======================================= Creates the Convoluted BRDF LUT =======================================
    glCreateTextures(GL_TEXTURE_2D, 1, &brdfLUT);
    glTextureStorage2D(brdfLUT, 1, GL_RG16F, 512, 512); // 16-bit recommended by Epic Games
    glTextureParameteri(brdfLUT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(brdfLUT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(brdfLUT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(brdfLUT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glNamedRenderbufferStorage(captureRBO, GL_DEPTH_COMPONENT24, 512, 512);
    glNamedFramebufferTexture(captureFBO, GL_COLOR_ATTACHMENT0, brdfLUT, 0);

    brdfConvolutionShader.use();
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glViewport(0, 0, 512, 512);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(screenQuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// (DEBUG) Draws a quad to the screen
void drawScreenQuad(const Shader& shader, unsigned int quadTexture) {
    shader.use();
    glBindTextureUnit(0, quadTexture);
    glUniform1i(glGetUniformLocation(shader.id, "quadTex"), 0);
    
    glBindVertexArray(screenQuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void drawSkybox(const Shader& shader, const unsigned int& envMap) {
    shader.use();
    glBindTextureUnit(0, envMap);
    glNamedBufferSubData(ubo, 0, sizeof(glm::mat4), glm::value_ptr(view));
    glNamedBufferSubData(ubo, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));

    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
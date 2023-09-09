#pragma once

#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>

#include "potatoengine/assets/assetsManager.h"
#include "potatoengine/pch.h"
#include "potatoengine/renderer/camera/camera.h"
#include "potatoengine/renderer/rendererAPI.h"
#include "potatoengine/renderer/shaderProgram.h"
#include "potatoengine/scene/components.h"

namespace potatoengine {

class Renderer {
   public:
    Renderer(std::weak_ptr<AssetsManager> am);

    void init() const noexcept;
    void shutdown() noexcept;

    void onWindowResize(uint32_t w, uint32_t h) const noexcept;

    void beginScene(const Camera& c) noexcept;
    void endScene() noexcept;

    glm::vec3 getCameraPosition() const noexcept { return m_cameraPosition; }  // TODO: Remove this

    void add(const std::string& shaderProgram);
    bool contains(const std::string& shaderProgram) const noexcept { return m_shaderPrograms.contains(shaderProgram); }
    std::unique_ptr<ShaderProgram>& get(const std::string& shaderProgram);

    void render(const std::shared_ptr<VAO>& vao, const glm::mat4& transform, const std::string& shaderProgram);
    static std::unique_ptr<Renderer> Create(std::weak_ptr<AssetsManager> am) noexcept;

   private:
    glm::mat4 m_viewMatrix{};
    glm::mat4 m_projectionMatrix{};
    glm::vec3 m_cameraPosition{};
    std::unordered_map<std::string, std::unique_ptr<ShaderProgram>> m_shaderPrograms;
    std::weak_ptr<AssetsManager> m_assetsManager;
};
}
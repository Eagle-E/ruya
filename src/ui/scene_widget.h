#ifndef SCENE_WIDGET_HPP
#define SCENE_WIDGET_HPP

#include <format>
#include <string>
#include <imgui/imgui.h>

#include "scene/light.hpp"
#include "scene/components.hpp"

#include "scene/scene.hpp"
#include "ui/model_widget.h"
#include "ui/light_widget.h"
#include "ui/widgets.hpp"

using ruya::scene::Scene;
using ruya::scene::Model;

namespace ruya::ui
{

void scene_widget(Scene& scene)
{
    ImGui::PushID("SceneWidget");

    if(ImGui::TreeNode("Scene"))
    {
        if(ImGui::TreeNode("General"))
        {
            // ImVec4 im_rgb {vec.x, vec.y, vec.z, 1.0f};
            ruya::ui::color_widget(scene.background_color, "Background color");
            ImGui::TreePop();
        }

        // light emitting entities
        if(ImGui::TreeNode("Lights"))
        {
            auto lights_view = scene.registry.view<BasicLight>();
            for (entt::entity light_entity : lights_view)
            {
                BasicLight& light = lights_view.get<BasicLight>(light_entity);
                Model* model = scene.registry.try_get<Model>(light_entity);
                int light_id = static_cast<int>(light_entity);
                ruya::ui::light_widget(light_id, light, model);
            }
            ImGui::TreePop();
        }

        // standard objects
        if(ImGui::TreeNode("Models"))
        {
            auto models_view = scene.registry.view<Model>(entt::exclude<BasicLight>);
            for (entt::entity model_entity : models_view)
            {
                Model& model = models_view.get<Model>(model_entity);
                int model_id = static_cast<int>(model_entity);
                auto label = std::format("Model {}", model_id);
                ruya::ui::model_widget(model, label);
            }
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }

    ImGui::PopID();
}


}



#endif // SCENE_WIDGET_HPP
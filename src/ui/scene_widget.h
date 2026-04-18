#ifndef SCENE_WIDGET_HPP
#define SCENE_WIDGET_HPP

#include <imgui/imgui.h>

#include "scene/object.h"
#include "scene/light.hpp"

#include "scene/scene.h"
#include "ui/object_widget.h"
#include "ui/light_widget.h"
#include "ui/widgets.hpp"

using ruya::scene::Scene;

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
            for (LightBasic* light : scene.get_light_sources())
            {
                ruya::ui::light_widget(*light);
            }
            ImGui::TreePop();
        }

        // standard objects
        if(ImGui::TreeNode("Objects"))
        {
            for (Object* obj : scene.get_scene_objects())
            {
                ruya::ui::object_widget(*obj);
            }
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }

    ImGui::PopID();
}


}



#endif // SCENE_WIDGET_HPP
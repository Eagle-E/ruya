#ifndef OBJECT_WIDGET_HPP
#define OBJECT_WIDGET_HPP

#include <format>
#include <string>
#include <ranges>
#include <imgui/imgui.h>

#include "scene/object.h"
#include "scene/components.hpp"

using ruya::scene::Model;
using ruya::scene::Element;

namespace
{
    static inline void obj_vec3_float_widget(vec3 & vec, const std::string& label = {})
    {
        ImVec4 im_vec {vec.x, vec.y, vec.z, 0.0f};
        if(ImGui::DragFloat3(label.c_str(), (float*)&im_vec))
        {
            vec.x = im_vec.x;
            vec.y = im_vec.y;
            vec.z = im_vec.z;
        }
    }

    inline void transform_settings(Transform& transform)
    {
        obj_vec3_float_widget(transform.position, "position");
        obj_vec3_float_widget(transform.scale, "scale");
        obj_vec3_float_widget(transform.rotation, "rotation");
    }

    inline void element_settings(Element& elem)
    {
        transform_settings(elem.transform);
    }

    inline void element_list_settings(std::vector<Element>& elem_list)
    {
        for (auto [i, elem] : std::views::enumerate(elem_list))
        {
            ImGui::PushID(i);

            // TODO WARNING: if the list of elements is mutated, the ui won't be in sync!!!
            std::string element_id = std::format("Element {}", i);
            
            if (ImGui::TreeNode(element_id.c_str()))
            {
                element_settings(elem);
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
    }
} 


namespace ruya::ui
{
    void model_widget(Model& model, const std::string& label)
    {
        ImGui::PushID(label.c_str());

        for (auto [i, elem] : std::views::enumerate(model.elements))
        {
            // TODO WARNING: if the list of elements is mutated, the ui won't be in sync!!!
            std::string element_id = std::format("Element {}-{}", label, i);
            
            if (ImGui::TreeNode(element_id.c_str()))
            {
                element_list_settings(model.elements);
                ImGui::TreePop();
            }
        }

        ImGui::PopID();
    }

    void object_widget(Object& object, const std::string& label = {})
    {
        ImGui::PushID(object.id.value);

        auto label_str = label;
        if (label.empty())
            label_str = std::format("Object {}", object.id.value);

        if (ImGui::TreeNode(label_str.c_str()))
        {
            element_list_settings(object.model.elements);
            ImGui::TreePop();
        }
        ImGui::PopID();
    }
}



#endif // OBJECT_WIDGET_HPP
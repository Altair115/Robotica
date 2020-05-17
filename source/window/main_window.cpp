#include <window/main_window.hpp>
#include <vision/parallax.hpp>
#include <vision/converters.hpp>
#include <world/robot.hpp>


namespace robotica {
    main_window& main_window::instance(void) {
        static main_window i;
        return i;
    }


    main_window::main_window(void) : window("Settings") {}


    void main_window::add_elements(void) {
        constexpr int topbar_height  = 19;
        constexpr int slider_height  = 23;
        constexpr int image_size     = 255;
        constexpr int padding_top    = 42;
        constexpr int padding_bottom = 8;
        constexpr int padding_side   = 8;

        int num_elems = 0;
        expand(settings, [&](const auto& v) { num_elems += v.size(); });

        const int target_height = padding_top + (slider_height * num_elems) + (2 * image_size) + padding_bottom;
        const int target_width = (4 * image_size) + (2 * padding_side);

        if (!has_resized) ImGui::SetWindowSize({ (float) target_width, (float) target_height });
        has_resized = true;

        ImGui::SetWindowPos({ 0, 0 });
        auto size = ImGui::GetWindowContentRegionMax();
        SDL_SetWindowSize(handle, size.x + 8, size.y + 8);

        left.set_image(robot::instance().get_camera_output(side::LEFT));
        right.set_image(robot::instance().get_camera_output(side::RIGHT));

        auto depth = parallax_depth_map(left.get_image(), right.get_image());

        left_disp.set_image(depth.left_disp);
        right_disp.set_image(depth.right_disp);
        filtered.set_image(depth.filtered);
        raw_vis.set_image(depth.raw_vis);
        filtered_vis.set_image(depth.filtered_vis);


        expand(settings, [](auto& vector) {
            using type = typename std::remove_reference_t<decltype(vector)>::value_type::type::type;

            for (auto& elem : vector) {
                auto& setting = elem.get();

                // TODO: Add more input types. (notably string and bool.)
                if constexpr (std::is_integral_v<type>)       ImGui::SliderInt(setting.name.c_str(), &setting.value, setting.min, setting.max);
                if constexpr (std::is_floating_point_v<type>) ImGui::SliderFloat(setting.name.c_str(), &setting.value, setting.min, setting.max);
            }
        });


        left.show();
        ImGui::SameLine();
        right.show();
        ImGui::SameLine();
        left_disp.show();
        ImGui::SameLine();
        right_disp.show();

        filtered.show();
        ImGui::SameLine();
        raw_vis.show();
        ImGui::SameLine();
        filtered_vis.show();
    }
}
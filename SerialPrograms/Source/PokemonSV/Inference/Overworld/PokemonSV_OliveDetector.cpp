/*  Olive Detector
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */
#include "Kernels/Waterfill/Kernels_Waterfill_Session.h"
#include "CommonFramework/Globals.h"
#include "CommonFramework/Exceptions/OperationFailedException.h"
#include "CommonFramework/VideoPipeline/VideoOverlayScopes.h"
#include "CommonFramework/ImageMatch/ImageDiff.h"
#include "CommonFramework/ImageMatch/ExactImageMatcher.h"
#include "CommonFramework/ImageTools/BinaryImage_FilterRgb32.h"
#include "CommonFramework/ImageTools/WaterfillUtilities.h"
#include "CommonFramework/ImageMatch/WaterfillTemplateMatcher.h"
#include "CommonFramework/ImageTools/ImageFilter.h"
#include "CommonFramework/Tools/ConsoleHandle.h"
#include "PokemonSV/Inference/Overworld/PokemonSV_DirectionDetector.h"
#include "NintendoSwitch/Commands/NintendoSwitch_Commands_PushButtons.h"
#include "PokemonSV_OliveDetector.h"

#include <iostream>
using std::cout;
using std::endl;

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonSV{


class OliveMatcher : public ImageMatch::WaterfillTemplateMatcher{
public:
    OliveMatcher() : WaterfillTemplateMatcher(
        "PokemonSV/Olive.png", Color(0,0,0), Color(255, 255, 255), 5
    ){
        m_aspect_ratio_lower = 0.5;
        m_aspect_ratio_upper = 1.5;
        m_area_ratio_lower = 0.5;
        m_area_ratio_upper = 1.5;
    }

    static const ImageMatch::WaterfillTemplateMatcher& instance(){
        static OliveMatcher matcher;
        return matcher;
    }
};


OliveDetector::OliveDetector(ConsoleHandle& console, Color color)
    : m_overlays(console.overlay())
    , m_color(color)
{}
void OliveDetector::make_overlays(VideoOverlaySet& items) const{
}


std::pair<double, double> OliveDetector::olive_location(ConsoleHandle& console, BotBaseContext& context, ImageFloatBox box){
    context.wait_for_all_requests();
    ImageFloatBox location = get_olive_floatbox(console, context, box);
    double x = location.x + (location.width / 2);
    double y = location.y + (location.height / 2);

    return std::make_pair(x, y);
}

std::pair<double, double> box_center(ImageFloatBox& box){
    double x = box.x + (box.width / 2);
    double y = box.y + (box.height / 2);

    return std::make_pair(x, y);
}

ImageFloatBox OliveDetector::get_olive_floatbox(ConsoleHandle& console, BotBaseContext& context, ImageFloatBox box){
    context.wait_for_all_requests();
    auto snapshot = console.video().snapshot();
    const ImageViewRGB32& screen = snapshot;

    const std::vector<std::pair<uint32_t, uint32_t>> filters = {
        {combine_rgb(0, 10, 0), combine_rgb(255, 255, 255)},
    };

    ImageRGB32 green_only = filter_green(screen, Color(0xff000000), 10);

    const double min_object_size = 1000;
    const double rmsd_threshold = 300;

    const double screen_rel_size = (screen.height() / 1080.0);
    const size_t min_size = size_t(screen_rel_size * screen_rel_size * min_object_size);

    size_t largest_size = 0;
    ImageFloatBox largest_green(0, 0, 0, 0);
    ImageViewRGB32 cropped = extract_box_reference(green_only, box);
    ImagePixelBox pixel_search_area = floatbox_to_pixelbox(screen.width(), screen.height(), box);
    match_template_by_waterfill(
        cropped, 
        OliveMatcher::instance(),
        filters,
        {min_size, SIZE_MAX},
        rmsd_threshold,
        [&](Kernels::Waterfill::WaterfillObject& object) -> bool {
            size_t object_area = object.area;
            if (object_area > largest_size){
                largest_size = object_area;
                // cout << largest_size << endl;
                ImagePixelBox found_box(
                    object.min_x + pixel_search_area.min_x, object.min_y + pixel_search_area.min_y,
                    object.max_x + pixel_search_area.min_x, object.max_y + pixel_search_area.min_y);
                largest_green = pixelbox_to_floatbox(screen.width(), screen.height(), found_box);
            }

            return false;
        }
    );

    m_overlays.clear();
    m_overlays.add(m_color, largest_green);
    
    return largest_green;
}

void OliveDetector::align_to_olive(
    const ProgramInfo& info, 
    ConsoleHandle& console, 
    BotBaseContext& context, 
    double direction_facing, 
    ImageFloatBox area_to_check
){
    size_t MAX_ATTEMPTS = 10;
    DirectionDetector direction;
    for (size_t i = 0; i < MAX_ATTEMPTS; i++){
        direction.change_direction(info, console, context, direction_facing);
        pbf_move_left_joystick(context, 128, 0, 5, 20);

        ImageFloatBox olive_box = get_olive_floatbox(console, context, area_to_check);

        std::pair<double, double> olive = box_center(olive_box);
        double olive_x = olive.first;
        double olive_y = olive.second;
        double olive_area = olive_box.width * olive_box.height;

        double x_diff = olive_x - 0.5;
        console.log("olive_x: " +  std::to_string(olive_x) + ", olive_y: " +  std::to_string(olive_y) + ", olive_area: " +  std::to_string(olive_area));
        if (std::abs(x_diff) < 0.01){
            return;
        }

        if (olive_x == 0 && olive_y == 0){
            throw OperationFailedException(
                ErrorReport::SEND_ERROR_REPORT, console,
                "align_to_olive(): Olive not detected.",
                true
            );
        }

        uint16_t scale_factor = 130;
        if (olive_y > 0.50){
            scale_factor = 100;

        }
        uint16_t push_duration = std::max(uint16_t((std::abs(x_diff) + 0.02) * scale_factor / (olive_y)), uint16_t(15));

        int16_t push_direction = (x_diff > 0) ? 1 : -1;
        double push_magnitude = 128; // std::max(double(128 / (i + 1)), double(20)); // push less with each iteration/attempt
        uint8_t push_x = uint8_t(std::max(std::min(int(128 + (push_direction * push_magnitude)), 255), 0));
        console.log("push x: " + std::to_string(push_x) + ", push duration: " +  std::to_string(push_duration));
        pbf_wait(context, 100);
        pbf_move_left_joystick(context, push_x, 128, push_duration, 100);
    
    }
    


}




}
}
}

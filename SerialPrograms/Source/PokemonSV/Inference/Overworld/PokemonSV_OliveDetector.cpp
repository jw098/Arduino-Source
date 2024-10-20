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
        m_aspect_ratio_upper = 3;
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

ImageFloatBox OliveDetector::align_to_olive(
    const ProgramInfo& info, 
    ConsoleHandle& console, 
    BotBaseContext& context, 
    double direction_facing, 
    ImageFloatBox area_to_check
){
    size_t MAX_ATTEMPTS = 10;
    ImageFloatBox olive_box;
    DirectionDetector direction;
    uint16_t scale_factor = 130;
    int16_t prev_push_direction = 0;
    for (size_t i = 0; i < MAX_ATTEMPTS; i++){
        direction.change_direction(info, console, context, direction_facing);
        pbf_move_left_joystick(context, 128, 0, 5, 20);

        olive_box = get_olive_floatbox(console, context, area_to_check);

        std::pair<double, double> olive = box_center(olive_box);
        double olive_x = olive.first;
        double olive_y = olive.second;
        double olive_area = olive_box.width * olive_box.height;

        double x_diff = olive_x - 0.5;
        console.log("olive_x: " +  std::to_string(olive_x) + ", olive_y: " +  std::to_string(olive_y) + ", olive_area: " +  std::to_string(olive_area));
        if (std::abs(x_diff) < 0.01){
            return olive_box;
        }

        if (olive_x == 0 && olive_y == 0){
            throw OperationFailedException(
                ErrorReport::SEND_ERROR_REPORT, console,
                "align_to_olive(): Olive not detected.",
                true
            );
        }

        int16_t push_direction = (x_diff > 0) ? 1 : -1;
        if (olive_y > 0.50 && scale_factor > 100){
            scale_factor = 100;
        }
        if (push_direction * -1 == prev_push_direction){  
            // if you overshot the olive, and are now pushing in the opposite direction
            // then reduce the push_duration.
            scale_factor = int16_t(scale_factor * 0.5);
        }
        
        // cout << "olive_height" << olive_box.height << endl;
        // if (olive_box.height > 0.4){
        //     scale_factor = 50;
        // }


        uint16_t push_duration = std::max(uint16_t((std::abs(x_diff) + 0.02) * scale_factor / (olive_y)), uint16_t(15));
        double push_magnitude = 128; // std::max(double(128 / (i + 1)), double(20)); // push less with each iteration/attempt
        uint8_t push_x = uint8_t(std::max(std::min(int(128 + (push_direction * push_magnitude)), 255), 0));
        console.log("scale_factor: " + std::to_string(scale_factor));
        console.log("push x: " + std::to_string(push_x) + ", push duration: " +  std::to_string(push_duration));
        // pbf_wait(context, 100);
        uint16_t wait_ticks = 20;
        if (std::abs(x_diff) < 0.05){
            wait_ticks = 100;
        }        
        pbf_move_left_joystick(context, push_x, 128, push_duration, wait_ticks);
        prev_push_direction = push_direction;
    
    }
    return olive_box;
}


// todo: detect and handle case where olive is stuck.
// todo: detect and handle case where olive is slightly to the left, and so we need to move on to the next phase
uint16_t OliveDetector::push_olive_forward(
    const ProgramInfo& info, 
    ConsoleHandle& console, 
    BotBaseContext& context, 
    double direction_facing, 
    uint16_t total_forward_distance,
    ImageFloatBox area_to_check,
    uint16_t push_olive
){
    uint16_t initial_push_olive = push_olive;
    uint16_t ticks_walked = 0;
    size_t MAX_ATTEMPTS = 10;
    for (size_t i = 0; i < MAX_ATTEMPTS; i++){
        align_to_olive(info, console, context, direction_facing, area_to_check);
        ticks_walked += walk_up_to_olive(info, console, context, direction_facing);
        

        if (ticks_walked > total_forward_distance){
            console.log("Distance walked: " + std::to_string(ticks_walked));
            return ticks_walked;
        }

        align_to_olive(info, console, context, direction_facing, area_to_check);
        // check location of olive before and after push
        // if olive is approximately in the same location, then the olive is stuck. try moving backward and running forward again.
        ImageFloatBox olive_box_1 = get_olive_floatbox(console, context, area_to_check);
        for (size_t j = 0; j < 3; j++){
            console.log("Distance walked: " + std::to_string(ticks_walked));
            console.log("Push the olive.");
            pbf_move_left_joystick(context, 128, 0, push_olive, 7 * TICKS_PER_SECOND);
            
            ticks_walked += push_olive;
            ImageFloatBox olive_box_2 = get_olive_floatbox(console, context, area_to_check);
            double x_diff = std::abs(olive_box_1.x - olive_box_2.x);
            double y_diff = std::abs(olive_box_1.y - olive_box_2.y);

            if (x_diff < 0.01 && y_diff < 0.01){
                console.log("Olive is stuck? Move backwards and try pushing again.");
                pbf_move_left_joystick(context, 128, 255, 75, 100);  // walk backwards
                ticks_walked -= initial_push_olive;
                push_olive = 200; // run forward more on the next push
            }else{
                break;
            }
        }
        
        if (ticks_walked > total_forward_distance){
            console.log("Distance walked: " + std::to_string(ticks_walked));
            return ticks_walked;
        }

        console.log("Distance walked: " + std::to_string(ticks_walked));
        
    }

    throw OperationFailedException(
        ErrorReport::SEND_ERROR_REPORT, console,
        "push_olive_forward(): Something went wrong. Failed to walk the Olive forward as expected.",
        true
    );

}

uint16_t OliveDetector::walk_up_to_olive(
    const ProgramInfo& info, 
    ConsoleHandle& console, 
    BotBaseContext& context, 
    double direction_facing, 
    ImageFloatBox area_to_check
){
    uint16_t ticks_walked = 0;
    size_t MAX_ATTEMPTS = 20;
    for (size_t i = 0; i < MAX_ATTEMPTS; i++){
        ImageFloatBox olive_box = get_olive_floatbox(console, context, area_to_check);
        std::pair<double, double> olive = box_center(olive_box);
        // double olive_x = olive.first;
        double olive_y = olive.second;

        uint16_t scale_factor = 2000;
        uint16_t push_duration = std::max(uint16_t(std::pow((0.57 - olive_y), 2) * scale_factor), uint16_t(20));
        console.log("olive_y: " + std::to_string(olive_y));
        if (olive_y > 0.51){
            return ticks_walked;
        }        
        console.log("push duration: " +  std::to_string(push_duration));
        ticks_walked += push_duration;

        uint16_t wait_ticks = 20;
        if (olive_y > 0.4){
            wait_ticks = 100;
        }        
        pbf_move_left_joystick(context, 128, 0, push_duration, wait_ticks);
    }    

    throw OperationFailedException(
        ErrorReport::SEND_ERROR_REPORT, console,
        "walk_up_to_olive(): Something went wrong. Failed to walk up to the Olive",
        true
    );

}



}
}
}

/*  Olive Detector
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "Kernels/Waterfill/Kernels_Waterfill_Session.h"
#include "CommonFramework/Globals.h"
#include "CommonFramework/VideoPipeline/VideoOverlayScopes.h"
#include "CommonFramework/ImageMatch/ImageDiff.h"
#include "CommonFramework/ImageMatch/ExactImageMatcher.h"
#include "CommonFramework/ImageTools/BinaryImage_FilterRgb32.h"
#include "CommonFramework/ImageTools/WaterfillUtilities.h"
#include "CommonFramework/ImageMatch/WaterfillTemplateMatcher.h"
#include "CommonFramework/ImageTools/ImageFilter.h"
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


OliveDetector::OliveDetector(Color color)
    : m_color(color)
{}
void OliveDetector::make_overlays(VideoOverlaySet& items) const{
}
ImageFloatBox OliveDetector::locate_olive(const ImageViewRGB32& screen, ImageFloatBox box) const{
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
                cout << largest_size << endl;
                ImagePixelBox found_box(
                    object.min_x + pixel_search_area.min_x, object.min_y + pixel_search_area.min_y,
                    object.max_x + pixel_search_area.min_x, object.max_y + pixel_search_area.min_y);
                largest_green = pixelbox_to_floatbox(screen.width(), screen.height(), found_box);
            }

            return false;
        }
    );

    return largest_green;
}




}
}
}

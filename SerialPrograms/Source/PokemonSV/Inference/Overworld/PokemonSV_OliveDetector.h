/*  Olive Detector
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_PokemonSV_OliveDetector_H
#define PokemonAutomation_PokemonSV_OliveDetector_H

#include "Common/Cpp/Color.h"
#include "ClientSource/Connection/BotBase.h"
#include "CommonFramework/ImageTools/ImageBoxes.h"
#include "CommonFramework/VideoPipeline/VideoFeed.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonSV{

class OliveDetector {
public:
    OliveDetector(ConsoleHandle& console, Color color = COLOR_RED);

    void make_overlays(VideoOverlaySet& items) const;

    std::pair<double, double> olive_location(ConsoleHandle& console, BotBaseContext& context, ImageFloatBox box = {0, 0.15, 1, 0.7});

    // return ImageFloatBox of the of the Olive, based on the largest blob of green
    ImageFloatBox get_olive_floatbox(ConsoleHandle& console, BotBaseContext& context, ImageFloatBox box = {0, 0.15, 1, 0.7});

    void align_to_olive(
        const ProgramInfo& info, 
        ConsoleHandle& console, 
        BotBaseContext& context, 
        double direction_facing, 
        ImageFloatBox area_to_check = {0, 0.15, 1, 0.7}
    );
    

protected:
    VideoOverlaySet m_overlays;
    const Color m_color;
};



}
}
}
#endif

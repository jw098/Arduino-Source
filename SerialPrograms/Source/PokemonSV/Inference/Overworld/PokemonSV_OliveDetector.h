/*  Olive Detector
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_PokemonSV_OliveDetector_H
#define PokemonAutomation_PokemonSV_OliveDetector_H

#include "Common/Cpp/Color.h"
#include "CommonFramework/ImageTools/ImageBoxes.h"
#include "CommonFramework/VideoPipeline/VideoFeed.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonSV{

class OliveDetector {
public:
    OliveDetector(Color color = COLOR_RED);

    void make_overlays(VideoOverlaySet& items) const;

    // return coordinates of the of the Olive, based on the largest blob of green
    ImageFloatBox locate_olive(const ImageViewRGB32& screen, ImageFloatBox box) const;

protected:
    const Color m_color;
};



}
}
}
#endif

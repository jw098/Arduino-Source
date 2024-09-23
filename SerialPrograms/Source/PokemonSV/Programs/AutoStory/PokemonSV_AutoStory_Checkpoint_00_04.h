/*  AutostoryTools
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_PokemonSV_AutoStory_Checkpoint_00_04_H
#define PokemonAutomation_PokemonSV_AutoStory_Checkpoint_00_04_H

#include "NintendoSwitch/NintendoSwitch_SingleSwitchProgram.h"
#include "PokemonSV_AutoStoryTools.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonSV{





// start: Received starter pokemon and changed move order. Cleared autoheal tutorial.
// end: Battled Nemona on the beach.
void checkpoint_04(SingleSwitchProgramEnvironment& env, BotBaseContext& context, EventNotificationOption& notif_status_update);

}
}
}
#endif

/*  AutoStory
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "CommonFramework/GlobalSettingsPanel.h"
#include "CommonFramework/Exceptions/FatalProgramException.h"
#include "CommonFramework/Exceptions/OperationFailedException.h"
#include "CommonFramework/InferenceInfra/InferenceRoutines.h"
#include "CommonFramework/Notifications/ProgramNotifications.h"
#include "CommonFramework/Tools/StatsTracking.h"
#include "CommonFramework/Tools/VideoResolutionCheck.h"
#include "NintendoSwitch/Commands/NintendoSwitch_Commands_PushButtons.h"
#include "Pokemon/Pokemon_Strings.h"
#include "PokemonSwSh/Inference/PokemonSwSh_IvJudgeReader.h"
#include "PokemonSV/Programs/PokemonSV_GameEntry.h"
#include "PokemonSV/Programs/PokemonSV_SaveGame.h"
#include "PokemonSV/Inference/Overworld/PokemonSV_OverworldDetector.h"
#include "PokemonSV_AutoStoryTools.h"
#include "PokemonSV_AutoStory_Segment_16.h"

//#include <iostream>
//using std::cout;
//using std::endl;
//#include <unordered_map>
//#include <algorithm>

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonSV{

using namespace Pokemon;




std::string AutoStory_Segment_16::name() const{
    return "13.1: Cascarrafa Gym (Water): Get Kofu's wallet";
}

std::string AutoStory_Segment_16::start_text() const{
    return "Start: Defeated Team Star (Dark). At Cascarrafa (West) Pokecenter.";
}

std::string AutoStory_Segment_16::end_text() const{
    return "End: Received Kofu's wallet. At Cascarrafa (North) Pokecenter.";
}

void AutoStory_Segment_16::run_segment(SingleSwitchProgramEnvironment& env, BotBaseContext& context, AutoStoryOptions options) const{
    AutoStoryStats& stats = env.current_stats<AutoStoryStats>();

    context.wait_for_all_requests();
    env.console.overlay().add_log("Start Segment 0", COLOR_ORANGE);

    checkpoint_32(env, context, options.notif_status_update);

    context.wait_for_all_requests();
    env.console.log("End Segment 0", COLOR_GREEN);
    stats.m_segment++;
    env.update_stats();

}


void checkpoint_32(
    SingleSwitchProgramEnvironment& env, 
    BotBaseContext& context, 
    EventNotificationOption& notif_status_update
){
    AutoStoryStats& stats = env.current_stats<AutoStoryStats>();
    bool first_attempt = true;
    while (true){
    try{
        if (first_attempt){
            checkpoint_save(env, context, notif_status_update);
            first_attempt = false;
        }         
        context.wait_for_all_requests();
        do_action_and_monitor_for_battles(env.program_info(), env.console, context,
            [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){
                realign_player(env.program_info(), env.console, context, PlayerRealignMode::REALIGN_NEW_MARKER, 80, 0, 40);
                pbf_move_left_joystick(context, 128, 0, 400, 100);
                realign_player(env.program_info(), env.console, context, PlayerRealignMode::REALIGN_NEW_MARKER, 255, 80, 100);
                pbf_move_left_joystick(context, 128, 0, 550, 100);                
        });

        realign_player(env.program_info(), env.console, context, PlayerRealignMode::REALIGN_NEW_MARKER, 210, 255, 50);
        pbf_press_button(context, BUTTON_PLUS, 20, 20);
        get_on_ride(env.program_info(), env.console, context);
        // walk towards elevator
        pbf_move_left_joystick(context, 128, 0, 500, 100);
        // jump to ensure you get on elevator
        pbf_controller_state(context, BUTTON_B, DPAD_NONE, 128, 0, 128, 128, 200);
        pbf_wait(context, 3 * TICKS_PER_SECOND);
        context.wait_for_all_requests();
        // wait for overworld to reappear after stepping off elevator
        OverworldWatcher        overworld(env.console, COLOR_CYAN);
        int ret = wait_until(
            env.console, context, 
            Milliseconds(20000),
            { overworld }
        );
        if (ret == 0){
            env.console.log("Overworld detected.");
        }else{
            throw OperationFailedException(
                ErrorReport::SEND_ERROR_REPORT, env.console,
                "Failed to detect overworld.",
                true
            );
        } 
        pbf_move_left_joystick(context, 128, 0, 100, 100);
        realign_player(env.program_info(), env.console, context, PlayerRealignMode::REALIGN_NEW_MARKER, 255, 67, 100);
        walk_forward_until_dialog(env.program_info(), env.console, context, NavigationMovementMode::DIRECTIONAL_ONLY, 20);
        mash_button_till_overworld(env.console, context, BUTTON_A);
       
        break;
    }catch (...){
        context.wait_for_all_requests();
        env.console.log("Resetting from checkpoint.");
        reset_game(env.program_info(), env.console, context);
        stats.m_reset++;
        env.update_stats();
    }           
    }

}

void checkpoint_33(
    SingleSwitchProgramEnvironment& env, 
    BotBaseContext& context, 
    EventNotificationOption& notif_status_update
){
    AutoStoryStats& stats = env.current_stats<AutoStoryStats>();
    bool first_attempt = true;
    while (true){
    try{
        if (first_attempt){
            checkpoint_save(env, context, notif_status_update);
            first_attempt = false;
        }         
        context.wait_for_all_requests();
       
        break;
    }catch (...){
        context.wait_for_all_requests();
        env.console.log("Resetting from checkpoint.");
        reset_game(env.program_info(), env.console, context);
        stats.m_reset++;
        env.update_stats();
    }            
    }

}

void checkpoint_34(
    SingleSwitchProgramEnvironment& env, 
    BotBaseContext& context, 
    EventNotificationOption& notif_status_update
){
    AutoStoryStats& stats = env.current_stats<AutoStoryStats>();
    bool first_attempt = true;
    while (true){
    try{
        if (first_attempt){
            checkpoint_save(env, context, notif_status_update);
            first_attempt = false;
        }         
        context.wait_for_all_requests();
       
        break;
    }catch (...){
        context.wait_for_all_requests();
        env.console.log("Resetting from checkpoint.");
        reset_game(env.program_info(), env.console, context);
        stats.m_reset++;
        env.update_stats();
    }         
    }

}




}
}
}

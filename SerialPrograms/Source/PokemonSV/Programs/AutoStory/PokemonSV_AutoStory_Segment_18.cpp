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
#include "PokemonSV_AutoStory_Segment_18.h"

#include <iostream>
//using std::cout;
//using std::endl;
//#include <unordered_map>
//#include <algorithm>

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonSV{

using namespace Pokemon;




std::string AutoStory_Segment_18::name() const{
    return "14: Great Tusk/Iron Treads titan";
}

std::string AutoStory_Segment_18::start_text() const{
    return "Start: Defeated Cascarrafa Gym (Water). At Cascarrafa Gym.";
}

std::string AutoStory_Segment_18::end_text() const{
    return "End: Defeated Great Tusk/Iron Treads. At Porto Marinada Pokecenter.";
}

void AutoStory_Segment_18::run_segment(SingleSwitchProgramEnvironment& env, BotBaseContext& context, AutoStoryOptions options) const{
    AutoStoryStats& stats = env.current_stats<AutoStoryStats>();

    context.wait_for_all_requests();
    env.console.overlay().add_log("Start Segment 14: Great Tusk/Iron Treads titan", COLOR_ORANGE);

   checkpoint_36(env, context, options.notif_status_update);

    context.wait_for_all_requests();
    env.console.log("End Segment 14: Great Tusk/Iron Treads titan", COLOR_GREEN);
    stats.m_segment++;
    env.update_stats();

}


void checkpoint_36(
    SingleSwitchProgramEnvironment& env, 
    BotBaseContext& context, 
    EventNotificationOption& notif_status_update
){
    AutoStoryStats& stats = env.current_stats<AutoStoryStats>();
    bool first_attempt = true;
    while (true){
    try{
        if (first_attempt){
            // checkpoint_save(env, context, notif_status_update);
            first_attempt = false;
        }         
        context.wait_for_all_requests();

        pbf_move_left_joystick(context, 128, 255, 500, 100);
        pbf_wait(context, 3 * TICKS_PER_SECOND);
        // wait for overworld after leaving Gym
        wait_for_overworld(env.program_info(), env.console, context, 30);

        // fly to Porto Marinada pokecenter
        move_cursor_towards_flypoint_and_go_there(env.program_info(), env.console, context, {ZoomChange::KEEP_ZOOM, 0, 80, 150});

        DirectionDetector direction;
        
        // section 1
        do_action_and_monitor_for_battles(env.program_info(), env.console, context,
            [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){
                direction.change_direction(env.console, context, 4.677921);
                pbf_move_left_joystick(context, 128, 0, 150, 100);
        });        

        // section 2
        realign_player(env.program_info(), env.console, context, PlayerRealignMode::REALIGN_NEW_MARKER, 255, 230, 120);
        overworld_navigation(env.program_info(), env.console, context, 
            NavigationStopCondition::STOP_MARKER, NavigationMovementMode::DIRECTIONAL_ONLY, 
            128, 0, 60, 10, false);         

        // section 3. enter circle
        realign_player_from_landmark(
            env.program_info(), env.console, context, 
            {ZoomChange::KEEP_ZOOM, 0, 0, 50},
            {ZoomChange::ZOOM_IN, 175, 255, 120}
        );        

        overworld_navigation(env.program_info(), env.console, context, 
            NavigationStopCondition::STOP_MARKER, NavigationMovementMode::DIRECTIONAL_ONLY, 
            128, 0, 60, 10, false);         


        while (true){


        std::cout << "1:00" << std::endl;
        realign_player_from_landmark(
            env.program_info(), env.console, context, 
            {ZoomChange::KEEP_ZOOM, 128, 0, 50},
            {ZoomChange::ZOOM_IN, 160, 255, 90}
        );           
        overworld_navigation(env.program_info(), env.console, context, 
            NavigationStopCondition::STOP_MARKER, NavigationMovementMode::DIRECTIONAL_ONLY, 
            128, 0, 20, 10, false);  

        // 135, 255, 90  12:00
        std::cout << "12:00" << std::endl;
        realign_player_from_landmark(
            env.program_info(), env.console, context, 
            {ZoomChange::KEEP_ZOOM, 128, 0, 50},
            {ZoomChange::ZOOM_IN, 135, 255, 90}
        );           
        overworld_navigation(env.program_info(), env.console, context, 
            NavigationStopCondition::STOP_MARKER, NavigationMovementMode::DIRECTIONAL_ONLY, 
            128, 0, 20, 10, false);       

        // 120, 255, 120  7:00
        std::cout << "7:00" << std::endl;
        realign_player_from_landmark(
            env.program_info(), env.console, context, 
            {ZoomChange::KEEP_ZOOM, 128, 0, 50},
            {ZoomChange::ZOOM_IN, 115, 255, 130}
        );           
        overworld_navigation(env.program_info(), env.console, context, 
            NavigationStopCondition::STOP_MARKER, NavigationMovementMode::DIRECTIONAL_ONLY, 
            128, 0, 20, 10, false);             

        // 150, 255, 135       6:00 
        std::cout << "6:00" << std::endl;
        realign_player_from_landmark(
            env.program_info(), env.console, context, 
            {ZoomChange::KEEP_ZOOM, 128, 0, 50},
            {ZoomChange::ZOOM_IN, 150, 255, 135}
        );           
        overworld_navigation(env.program_info(), env.console, context, 
            NavigationStopCondition::STOP_MARKER, NavigationMovementMode::DIRECTIONAL_ONLY, 
            128, 0, 20, 10, false);      

        std::cout << "2:00" << std::endl;
        realign_player_from_landmark(
            env.program_info(), env.console, context, 
            {ZoomChange::KEEP_ZOOM, 128, 0, 50},
            {ZoomChange::ZOOM_IN, 180, 255, 120}
        );           
        overworld_navigation(env.program_info(), env.console, context, 
            NavigationStopCondition::STOP_MARKER, NavigationMovementMode::DIRECTIONAL_ONLY, 
            128, 0, 20, 10, false);                  

        }        
       
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

void checkpoint_37(
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

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
#include "PokemonSV/Inference/Overworld/PokemonSV_DirectionDetector.h"
#include "PokemonSV/Inference/Dialogs/PokemonSV_DialogDetector.h"
#include "PokemonSV_AutoStoryTools.h"
#include "PokemonSV_AutoStory_Segment_18.h"

// #include <iostream>
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
    return "End: Defeated Great Tusk/Iron Treads. At Mesagoza East Pokecenter.";
}

void AutoStory_Segment_18::run_segment(SingleSwitchProgramEnvironment& env, BotBaseContext& context, AutoStoryOptions options) const{
    AutoStoryStats& stats = env.current_stats<AutoStoryStats>();

    context.wait_for_all_requests();
    env.console.overlay().add_log("Start Segment 14: Great Tusk/Iron Treads titan", COLOR_ORANGE);

    checkpoint_39(env, context, options.notif_status_update);
    checkpoint_40(env, context, options.notif_status_update);

    context.wait_for_all_requests();
    env.console.log("End Segment 14: Great Tusk/Iron Treads titan", COLOR_GREEN);
    stats.m_segment++;
    env.update_stats();

}


void checkpoint_39(
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
                direction.change_direction(env.program_info(), env.console, context, 4.677921);
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

        DialogBoxWatcher        dialog(COLOR_RED, true);

        // section 4
        int ret = run_until(
            env.console, context,
            [&](BotBaseContext& context){
                // run around in circles until you run into the titan
                WallClock start = current_time();

                while (true){
                    if (current_time() - start > std::chrono::minutes(30)){
                        break;
                    }
                    // std::cout << "1:00" << std::endl;
                    realign_player_from_landmark(
                        env.program_info(), env.console, context, 
                        {ZoomChange::KEEP_ZOOM, 128, 0, 50},
                        {ZoomChange::ZOOM_IN, 160, 255, 90}
                    );           
                    overworld_navigation(env.program_info(), env.console, context, 
                        NavigationStopCondition::STOP_MARKER, NavigationMovementMode::DIRECTIONAL_ONLY, 
                        128, 0, 36, 12, false);  


                    // std::cout << "11:00" << std::endl;
                    realign_player_from_landmark(
                        env.program_info(), env.console, context, 
                        {ZoomChange::KEEP_ZOOM, 128, 0, 50},
                        {ZoomChange::ZOOM_IN, 135, 255, 90}
                    );           
                    overworld_navigation(env.program_info(), env.console, context, 
                        NavigationStopCondition::STOP_MARKER, NavigationMovementMode::DIRECTIONAL_ONLY, 
                        128, 0, 30, 10, false);       


                    // std::cout << "10:00" << std::endl;
                    realign_player_from_landmark(
                        env.program_info(), env.console, context, 
                        {ZoomChange::KEEP_ZOOM, 128, 0, 50},
                        {ZoomChange::ZOOM_IN, 120, 255, 105}
                    );           
                    overworld_navigation(env.program_info(), env.console, context, 
                        NavigationStopCondition::STOP_MARKER, NavigationMovementMode::DIRECTIONAL_ONLY, 
                        128, 0, 30, 10, false);            

                    // std::cout << "7:00" << std::endl;
                    realign_player_from_landmark(
                        env.program_info(), env.console, context, 
                        {ZoomChange::KEEP_ZOOM, 128, 0, 50},
                        {ZoomChange::ZOOM_IN, 115, 255, 127}
                    );           
                    overworld_navigation(env.program_info(), env.console, context, 
                        NavigationStopCondition::STOP_MARKER, NavigationMovementMode::DIRECTIONAL_ONLY, 
                        128, 0, 48, 12, false);            

                    // std::cout << "6:00" << std::endl;
                    realign_player_from_landmark(
                        env.program_info(), env.console, context, 
                        {ZoomChange::KEEP_ZOOM, 128, 0, 50},
                        {ZoomChange::ZOOM_IN, 135, 255, 137}
                    );           
                    overworld_navigation(env.program_info(), env.console, context, 
                        NavigationStopCondition::STOP_MARKER, NavigationMovementMode::DIRECTIONAL_ONLY, 
                        128, 0, 30, 10, false);    

                    // std::cout << "2:00" << std::endl;
                    realign_player_from_landmark(
                        env.program_info(), env.console, context, 
                        {ZoomChange::KEEP_ZOOM, 128, 0, 50},
                        {ZoomChange::ZOOM_IN, 200, 255, 120}
                    );           
                    overworld_navigation(env.program_info(), env.console, context, 
                        NavigationStopCondition::STOP_MARKER, NavigationMovementMode::DIRECTIONAL_ONLY, 
                        128, 0, 50, 10, false);    
                }                 

            },
            {dialog}
        );
        if (ret < 0){
            throw OperationFailedException(
                ErrorReport::SEND_ERROR_REPORT, env.console,
                "checkpoint_39(): Failed to run into Great Tusk/Iron Treads.",
                true
            );            
        }

        // battle the titan phase 1
        clear_dialog(env.console, context, ClearDialogMode::STOP_BATTLE, 60, {CallbackEnum::BATTLE, CallbackEnum::BLACK_DIALOG_BOX});
        run_battle_press_A(env.console, context, BattleStopCondition::STOP_OVERWORLD);

        // section 5
        realign_player_from_landmark(
            env.program_info(), env.console, context, 
            {ZoomChange::KEEP_ZOOM, 128, 0, 40},
            {ZoomChange::ZOOM_IN, 122, 255, 130}
        );           
        overworld_navigation(env.program_info(), env.console, context, 
            NavigationStopCondition::STOP_MARKER, NavigationMovementMode::DIRECTIONAL_ONLY, 
            128, 0, 20, 10, false);  

        // section 6
        realign_player_from_landmark(
            env.program_info(), env.console, context, 
            {ZoomChange::KEEP_ZOOM, 128, 0, 50},
            {ZoomChange::ZOOM_IN, 90, 255, 170}
        );       

        overworld_navigation(env.program_info(), env.console, context, 
            NavigationStopCondition::STOP_DIALOG, NavigationMovementMode::DIRECTIONAL_ONLY, 
            128, 0, 40, 40, false);        

        // battle the titan phase 2
        clear_dialog(env.console, context, ClearDialogMode::STOP_BATTLE, 60, {CallbackEnum::BATTLE});  
        run_battle_press_A(env.console, context, BattleStopCondition::STOP_DIALOG, {CallbackEnum::DIALOG_ARROW});
        mash_button_till_overworld(env.console, context, BUTTON_A, 360);
       
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

void checkpoint_40(
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
        move_cursor_towards_flypoint_and_go_there(env.program_info(), env.console, context, {ZoomChange::KEEP_ZOOM, 255, 185, 440});
       
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
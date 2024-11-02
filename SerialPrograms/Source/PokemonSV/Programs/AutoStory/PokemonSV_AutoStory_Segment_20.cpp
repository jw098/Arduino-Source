/*  AutoStory
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "CommonFramework/GlobalSettingsPanel.h"
#include "CommonFramework/Exceptions/FatalProgramException.h"
#include "CommonFramework/Exceptions/OperationFailedException.h"
#include "CommonFramework/Exceptions/UnexpectedBattleException.h"
#include "CommonFramework/InferenceInfra/InferenceRoutines.h"
#include "CommonFramework/Notifications/ProgramNotifications.h"
#include "CommonFramework/Tools/StatsTracking.h"
#include "CommonFramework/Tools/VideoResolutionCheck.h"
#include "NintendoSwitch/Commands/NintendoSwitch_Commands_PushButtons.h"
#include "Pokemon/Pokemon_Strings.h"
#include "PokemonSwSh/Inference/PokemonSwSh_IvJudgeReader.h"
#include "PokemonSV/Inference/Overworld/PokemonSV_DirectionDetector.h"
#include "PokemonSV/Programs/PokemonSV_GameEntry.h"
#include "PokemonSV/Programs/PokemonSV_SaveGame.h"
#include "PokemonSV_AutoStoryTools.h"
#include "PokemonSV_AutoStory_Segment_20.h"

//#include <iostream>
//using std::cout;
//using std::endl;
//#include <unordered_map>
//#include <algorithm>

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonSV{

using namespace Pokemon;




std::string AutoStory_Segment_20::name() const{
    return "15.2: Klawf Titan: Battle Klawf";
}

std::string AutoStory_Segment_20::start_text() const{
    return "Start: At South Province (Area Three) Pokecenter.";
}

std::string AutoStory_Segment_20::end_text() const{
    return "End:";
}

void AutoStory_Segment_20::run_segment(SingleSwitchProgramEnvironment& env, BotBaseContext& context, AutoStoryOptions options) const{
    AutoStoryStats& stats = env.current_stats<AutoStoryStats>();

    context.wait_for_all_requests();
    env.console.overlay().add_log("Start Segment ", COLOR_ORANGE);

    checkpoint_42(env, context, options.notif_status_update);

    context.wait_for_all_requests();
    env.console.log("End Segment ", COLOR_GREEN);
    stats.m_segment++;
    env.update_stats();

}



void checkpoint_42(
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

        // section 1
        DirectionDetector direction;
        direction.change_direction(env.program_info(), env.console, context, 0.14);
        do_action_and_monitor_for_battles(env.program_info(), env.console, context,
            [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){
                pbf_move_left_joystick(context, 128, 0, 200, 100);
        });        

        // section 2. walk until hit dialog
        realign_player_from_landmark(
            env.program_info(), env.console, context, 
            {ZoomChange::KEEP_ZOOM, 0, 128, 70},
            {ZoomChange::ZOOM_IN, 255, 93, 170}
        );  
        handle_when_stationary_in_overworld(env.program_info(), env.console, context, 
            [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){           
                overworld_navigation(env.program_info(), env.console, context, 
                    NavigationStopCondition::STOP_DIALOG, NavigationMovementMode::DIRECTIONAL_ONLY, 
                    128, 0, 20, 10, false);
            }, 
            [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){           
                pbf_move_left_joystick(context, 0, 128, 40, 50);
                realign_player(env.program_info(), env.console, context, PlayerRealignMode::REALIGN_OLD_MARKER);
            }
        );   
        // clear_dialog(env.console, context, ClearDialogMode::STOP_OVERWORLD, 60, {CallbackEnum::BLACK_DIALOG_BOX});
        mash_button_till_overworld(env.console, context, BUTTON_A);


        // section 3. 
        realign_player(env.program_info(), env.console, context, PlayerRealignMode::REALIGN_OLD_MARKER);
        handle_when_stationary_in_overworld(env.program_info(), env.console, context, 
            [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){           
                overworld_navigation(env.program_info(), env.console, context, 
                    NavigationStopCondition::STOP_MARKER, NavigationMovementMode::DIRECTIONAL_ONLY, 
                    128, 0, 30, 10, false);
            }, 
            [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){           
                pbf_move_left_joystick(context, 0, 128, 40, 50);
                realign_player(env.program_info(), env.console, context, PlayerRealignMode::REALIGN_OLD_MARKER);
            }
        );          

        // section 4
        realign_player_from_landmark(
            env.program_info(), env.console, context, 
            {ZoomChange::KEEP_ZOOM, 255, 128, 100},
            {ZoomChange::ZOOM_IN, 0, 90, 157}
        );  
        handle_when_stationary_in_overworld(env.program_info(), env.console, context, 
            [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){           
                overworld_navigation(env.program_info(), env.console, context, 
                    NavigationStopCondition::STOP_MARKER, NavigationMovementMode::DIRECTIONAL_ONLY, 
                    128, 0, 30, 10, false);
            }, 
            [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){           
                pbf_move_left_joystick(context, 255, 128, 40, 50);
                realign_player(env.program_info(), env.console, context, PlayerRealignMode::REALIGN_OLD_MARKER);
            }
        );   

        // section 5
        realign_player_from_landmark(
            env.program_info(), env.console, context, 
            {ZoomChange::KEEP_ZOOM, 255, 150, 70},
            {ZoomChange::ZOOM_IN, 0, 95, 112}
        );  
        handle_when_stationary_in_overworld(env.program_info(), env.console, context, 
            [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){           
                overworld_navigation(env.program_info(), env.console, context, 
                    NavigationStopCondition::STOP_MARKER, NavigationMovementMode::DIRECTIONAL_ONLY, 
                    128, 0, 30, 10, false);
            }, 
            [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){           
                pbf_move_left_joystick(context, 0, 128, 40, 50);
                realign_player(env.program_info(), env.console, context, PlayerRealignMode::REALIGN_OLD_MARKER);
            }
        );   

        // section 6
        realign_player_from_landmark(
            env.program_info(), env.console, context, 
            {ZoomChange::KEEP_ZOOM, 255, 128, 60},
            {ZoomChange::ZOOM_IN, 0, 50, 105}
        );  
        handle_when_stationary_in_overworld(env.program_info(), env.console, context, 
            [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){           
                overworld_navigation(env.program_info(), env.console, context, 
                    NavigationStopCondition::STOP_MARKER, NavigationMovementMode::DIRECTIONAL_ONLY, 
                    128, 0, 24, 8, false);
            }, 
            [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){           
                pbf_move_left_joystick(context, 128, 255, 50, 50);
                pbf_move_left_joystick(context, 255, 128, 50, 50);
                realign_player(env.program_info(), env.console, context, PlayerRealignMode::REALIGN_OLD_MARKER);
            }
        );   

        // section 7. walk up to Klawf on the lower wall, so it moves to the high ground
        realign_player_from_landmark(
            env.program_info(), env.console, context, 
            {ZoomChange::KEEP_ZOOM, 255, 150, 50},
            {ZoomChange::ZOOM_IN, 0, 35, 117}
        );  
        handle_when_stationary_in_overworld(env.program_info(), env.console, context, 
            [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){           
                overworld_navigation(env.program_info(), env.console, context, 
                    NavigationStopCondition::STOP_MARKER, NavigationMovementMode::DIRECTIONAL_ONLY, 
                    128, 0, 30, 10, false);
            }, 
            [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){           
                pbf_move_left_joystick(context, 255, 128, 40, 50);
                realign_player(env.program_info(), env.console, context, PlayerRealignMode::REALIGN_OLD_MARKER);
            }
        );                                        

        // section 8. walk towards Klawf on lower wall
        size_t MAX_ATTEMPTS = 3;
        for (size_t i = 0; i < MAX_ATTEMPTS; i++){
            try {
                context.wait_for_all_requests();
                do_action_and_monitor_for_battles(env.program_info(), env.console, context,
                    [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){
                        direction.change_direction(env.program_info(), env.console, context, 5.95);
                        pbf_move_left_joystick(context, 128, 0, 300, 100);
                });
                break;
            }catch (UnexpectedBattleException&){
                run_battle_press_A(env.console, context, BattleStopCondition::STOP_OVERWORLD);
                direction.change_direction(env.program_info(), env.console, context, 5.95);
                pbf_move_left_joystick(context, 128, 0, 300, 100);                
            }
        }

        // section 9
        realign_player_from_landmark(
            env.program_info(), env.console, context, 
            {ZoomChange::KEEP_ZOOM, 255, 180, 50},
            {ZoomChange::ZOOM_IN, 0, 35, 100}
        );  
        handle_when_stationary_in_overworld(env.program_info(), env.console, context, 
            [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){           
                overworld_navigation(env.program_info(), env.console, context, 
                    NavigationStopCondition::STOP_MARKER, NavigationMovementMode::DIRECTIONAL_ONLY, 
                    128, 0, 12, 12, false);
            }, 
            [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){           
                pbf_move_left_joystick(context, 255, 128, 40, 50);
                realign_player(env.program_info(), env.console, context, PlayerRealignMode::REALIGN_OLD_MARKER);
            }
        );          

        // section 10
        realign_player_from_landmark(
            env.program_info(), env.console, context, 
            {ZoomChange::KEEP_ZOOM, 255, 180, 50},
            {ZoomChange::ZOOM_IN, 0, 20, 95}
        );  
        handle_when_stationary_in_overworld(env.program_info(), env.console, context, 
            [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){           
                overworld_navigation(env.program_info(), env.console, context, 
                    NavigationStopCondition::STOP_MARKER, NavigationMovementMode::DIRECTIONAL_ONLY, 
                    128, 0, 12, 12, false);
            }, 
            [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){           
                pbf_move_left_joystick(context, 255, 128, 40, 50);
                realign_player(env.program_info(), env.console, context, PlayerRealignMode::REALIGN_OLD_MARKER);
            }
        );    

        // section 11
        realign_player_from_landmark(
            env.program_info(), env.console, context, 
            {ZoomChange::KEEP_ZOOM, 255, 180, 50},
            {ZoomChange::ZOOM_IN, 0, 0, 100}
        );  
        handle_when_stationary_in_overworld(env.program_info(), env.console, context, 
            [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){           
                overworld_navigation(env.program_info(), env.console, context, 
                    NavigationStopCondition::STOP_MARKER, NavigationMovementMode::DIRECTIONAL_ONLY, 
                    128, 0, 12, 12, false);
            }, 
            [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){           
                pbf_move_left_joystick(context, 255, 128, 40, 50);
                realign_player(env.program_info(), env.console, context, PlayerRealignMode::REALIGN_OLD_MARKER);
            }
        );   

        // section 12
        direction.change_direction(env.program_info(), env.console, context, 5.76);      
        pbf_move_left_joystick(context, 128, 0, 300, 100);

        direction.change_direction(env.program_info(), env.console, context, 0.37);      
        pbf_move_left_joystick(context, 128, 0, 900, 100);  

        direction.change_direction(env.program_info(), env.console, context, 2.6);      
        overworld_navigation(env.program_info(), env.console, context, 
            NavigationStopCondition::STOP_BATTLE, NavigationMovementMode::DIRECTIONAL_ONLY, 
            128, 0, 25, 25, false);    

        

       
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

// todo: uncomment checkpoint_save
void checkpoint_43(
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

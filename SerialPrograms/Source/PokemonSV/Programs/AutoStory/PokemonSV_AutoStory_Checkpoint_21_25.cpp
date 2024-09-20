/*  AutoStoryTools
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
#include "CommonFramework/ImageTools/SolidColorTest.h"
#include "NintendoSwitch/Commands/NintendoSwitch_Commands_PushButtons.h"
#include "NintendoSwitch/Commands/NintendoSwitch_Commands_Superscalar.h"
#include "NintendoSwitch/Programs/NintendoSwitch_SnapshotDumper.h"
#include "PokemonSwSh/Inference/PokemonSwSh_IvJudgeReader.h"
#include "PokemonSV/Inference/Battles/PokemonSV_NormalBattleMenus.h"
#include "PokemonSV/Inference/Dialogs/PokemonSV_DialogDetector.h"
#include "PokemonSV/Inference/Overworld/PokemonSV_OverworldDetector.h"
#include "PokemonSV/Inference/PokemonSV_MainMenuDetector.h"
#include "PokemonSV/Inference/Map/PokemonSV_MapMenuDetector.h"
#include "PokemonSV/Programs/PokemonSV_Navigation.h"
#include "PokemonSV/Programs/PokemonSV_GameEntry.h"
#include "PokemonSV/Programs/PokemonSV_SaveGame.h"
#include "PokemonSV/Programs/Battles/PokemonSV_Battles.h"
#include "PokemonSV/Programs/AutoStory/PokemonSV_MenuOption.h"
#include "PokemonSV/Inference/PokemonSV_TutorialDetector.h"
#include "PokemonSV/Inference/PokemonSV_PokemonMovesReader.h"
#include "PokemonSV/Inference/Map/PokemonSV_DestinationMarkerDetector.h"
#include "PokemonSV_AutoStoryTools.h"

//#include <iostream>
//using std::cout;
//using std::endl;
//#include <unordered_map>
//#include <algorithm>

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonSV{

using namespace Pokemon;



void checkpoint_21(
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
        fly_to_overlapping_flypoint(env.program_info(), env.console, context);

        context.wait_for_all_requests();
        pbf_press_button(context, BUTTON_L, 20, 20);
        // move forward
        pbf_move_left_joystick(context, 128, 0, 30, 100);
        // get on ride
        pbf_press_button(context, BUTTON_PLUS, 20, 20);
        get_on_ride(env.program_info(), env.console, context);
        // turn left
        realign_player(env.program_info(), env.console, context, PlayerRealignMode::REALIGN_NO_MARKER, 0, 128, 50);
        // move forward
        pbf_move_left_joystick(context, 128, 0, 100, 100);

        realign_player(env.program_info(), env.console, context, PlayerRealignMode::REALIGN_NEW_MARKER, 0, 200, 70);
        pbf_move_left_joystick(context, 128, 0, 400, 100);

        realign_player(env.program_info(), env.console, context, PlayerRealignMode::REALIGN_NEW_MARKER, 0, 128, 70);
        pbf_move_left_joystick(context, 128, 0, 700, 100);

        // turn towards wall
        realign_player(env.program_info(), env.console, context, PlayerRealignMode::REALIGN_NEW_MARKER, 128, 0, 50);
        pbf_move_left_joystick(context, 128, 0, 200, 100);
        // run and jump over wall
        pbf_controller_state(context, BUTTON_B, DPAD_NONE, 128, 0, 128, 128, 100);

        BlackScreenOverWatcher black_screen(COLOR_RED, { 0.2, 0.2, 0.6, 0.6 });
        int ret = wait_until(
            env.console, context,
            std::chrono::seconds(30),
            { black_screen }
        );
        if (ret < 0){
            throw OperationFailedException(
                ErrorReport::SEND_ERROR_REPORT, env.console,
                "checkpoint_21(): Failed to jump the East Mesagoza wall.",
                true
            );
        }
        context.wait_for_all_requests();
        fly_to_overlapping_flypoint(env.program_info(), env.console, context);
       
        break;
    }catch(...){
        context.wait_for_all_requests();
        env.console.log("Resetting from checkpoint.");
        reset_game(env.program_info(), env.console, context);
        stats.m_reset++;
        env.update_stats();
    }             
    }

}

void checkpoint_22(
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
        // section 1
        realign_player_from_landmark(
            env.program_info(), env.console, context,
            {ZoomChange::ZOOM_IN, 0, 128, 80},
            {ZoomChange::KEEP_ZOOM, 255, 80, 37}            
        );
        overworld_navigation(env.program_info(), env.console, context, 
            NavigationStopCondition::STOP_MARKER, NavigationMovementMode::DIRECTIONAL_ONLY, 
            128, 0, 40, 20, false);

        // section 2
        realign_player_from_landmark(
            env.program_info(), env.console, context,
            {ZoomChange::ZOOM_IN, 0, 128, 40},
            {ZoomChange::KEEP_ZOOM, 255, 255, 27}            
        );        
        overworld_navigation(env.program_info(), env.console, context, 
            NavigationStopCondition::STOP_MARKER, NavigationMovementMode::DIRECTIONAL_ONLY, 
            128, 0, 30, 15, false);
        
        // section 3. set marker to pokecenter
        realign_player_from_landmark(
            env.program_info(), env.console, context,
            {ZoomChange::ZOOM_IN, 128, 128, 0},
            {ZoomChange::KEEP_ZOOM, 128, 128, 0}            
        );         
        overworld_navigation(env.program_info(), env.console, context, 
            NavigationStopCondition::STOP_MARKER, NavigationMovementMode::DIRECTIONAL_ONLY, 
            128, 0, 30, 15, false);

        // section 3. set marker past pokecenter
        handle_unexpected_battles(env.program_info(), env.console, context,
        [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){                        
            realign_player(env.program_info(), env.console, context, PlayerRealignMode::REALIGN_NEW_MARKER, 0, 70, 30);
        });            
        overworld_navigation(env.program_info(), env.console, context, 
            NavigationStopCondition::STOP_TIME, NavigationMovementMode::DIRECTIONAL_ONLY, 
            128, 20, 12, 12, false);        

        fly_to_overlapping_flypoint(env.program_info(), env.console, context);
       
        break;
    }catch(...){
        context.wait_for_all_requests();
        env.console.log("Resetting from checkpoint.");
        reset_game(env.program_info(), env.console, context);
        stats.m_reset++;
        env.update_stats();
    }             
    }

}

void checkpoint_23(
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
    }catch(...){
        context.wait_for_all_requests();
        env.console.log("Resetting from checkpoint.");
        reset_game(env.program_info(), env.console, context);
        stats.m_reset++;
        env.update_stats();
    }             
    }

}

void checkpoint_24(
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
    }catch(...){
        context.wait_for_all_requests();
        env.console.log("Resetting from checkpoint.");
        reset_game(env.program_info(), env.console, context);
        stats.m_reset++;
        env.update_stats();
    }             
    }

}

void checkpoint_25(
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
    }catch(...){
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

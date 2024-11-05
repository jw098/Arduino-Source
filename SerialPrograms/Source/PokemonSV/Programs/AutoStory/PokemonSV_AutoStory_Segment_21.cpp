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
#include "PokemonSV_AutoStory_Segment_21.h"

//#include <iostream>
//using std::cout;
//using std::endl;
//#include <unordered_map>
//#include <algorithm>

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonSV{

using namespace Pokemon;




std::string AutoStory_Segment_21::name() const{
    return "16.1: Artazon Gym (Grass): Gym challenge";
}

std::string AutoStory_Segment_21::start_text() const{
    return "Start: Defeated Klawf. At Artazon (West) Pokecenter.";
}

std::string AutoStory_Segment_21::end_text() const{
    return "End: ";
}

void AutoStory_Segment_21::run_segment(SingleSwitchProgramEnvironment& env, BotBaseContext& context, AutoStoryOptions options) const{
    AutoStoryStats& stats = env.current_stats<AutoStoryStats>();

    context.wait_for_all_requests();
    env.console.overlay().add_log("Start Segment ", COLOR_ORANGE);

    checkpoint_44(env, context, options.notif_status_update);

    context.wait_for_all_requests();
    env.console.log("End Segment ", COLOR_GREEN);
    stats.m_segment++;
    env.update_stats();

}


void checkpoint_44(
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
        }else{
            enter_menu_from_overworld(env.program_info(), env.console, context, -1);
            // we wait 10 seconds then save, so that the initial conditions are slightly different on each reset.
            env.log("Wait 10 seconds.");
            context.wait_for(Milliseconds(10 * 1000));
            save_game_from_overworld(env.program_info(), env.console, context);
        }
        context.wait_for_all_requests();

        // place the marker somewhere else. the current location disrupts the Stationary detector
        realign_player(env.program_info(), env.console, context, PlayerRealignMode::REALIGN_NEW_MARKER, 0, 128, 50);

        DirectionDetector direction;
        do_action_and_monitor_for_battles(env.program_info(), env.console, context,
            [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){
                direction.change_direction(env.program_info(), env.console, context, 6.198);
                pbf_move_left_joystick(context, 128, 0, 400, 100);
                direction.change_direction(env.program_info(), env.console, context, 4.693);
                pbf_move_left_joystick(context, 128, 0, 1000, 100);
        });
        // walk up right set of stairs
        direction.change_direction(env.program_info(), env.console, context, 4.276);
        pbf_move_left_joystick(context, 128, 0, 700, 100);

        // realign using lamp-post
        direction.change_direction(env.program_info(), env.console, context, 2.34);
        pbf_move_left_joystick(context, 128, 0, 200, 100);
        direction.change_direction(env.program_info(), env.console, context, 1.81);
        pbf_move_left_joystick(context, 255, 0, 600, 100);

        // move toward gym building
        direction.change_direction(env.program_info(), env.console, context, 4.26);
        pbf_move_left_joystick(context, 128, 0, 900, 100);
        direction.change_direction(env.program_info(), env.console, context, 3.05);
        pbf_move_left_joystick(context, 128, 0, 200, 100);
        pbf_wait(context, 7 * TICKS_PER_SECOND);
        context.wait_for_all_requests();

        handle_when_stationary_in_overworld(env.program_info(), env.console, context, 
            [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){           
                walk_forward_until_dialog(env.program_info(), env.console, context, NavigationMovementMode::DIRECTIONAL_ONLY, 20);
            }, 
            [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){           
                pbf_move_left_joystick(context, 0, 0, 100, 50);
            },
            5, 5
        );          

        // enter gym building. talk go Nemona and battle her.
        clear_dialog(env.console, context, ClearDialogMode::STOP_BATTLE, 60, {CallbackEnum::BLACK_DIALOG_BOX, CallbackEnum::PROMPT_DIALOG, CallbackEnum::DIALOG_ARROW, CallbackEnum::BATTLE});        
        run_battle_press_A(env.console, context, BattleStopCondition::STOP_DIALOG, {CallbackEnum::GRADIENT_ARROW});
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

// todo: uncomment checkpoint_save
void checkpoint_45(
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
        }else{
            enter_menu_from_overworld(env.program_info(), env.console, context, -1);
            // we wait 10 seconds then save, so that the initial conditions are slightly different on each reset.
            env.log("Wait 10 seconds.");
            context.wait_for(Milliseconds(10 * 1000));
            save_game_from_overworld(env.program_info(), env.console, context);
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

// todo: uncomment checkpoint_save
void checkpoint_46(
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
        }else{
            enter_menu_from_overworld(env.program_info(), env.console, context, -1);
            // we wait 10 seconds then save, so that the initial conditions are slightly different on each reset.
            env.log("Wait 10 seconds.");
            context.wait_for(Milliseconds(10 * 1000));
            save_game_from_overworld(env.program_info(), env.console, context);
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


// todo: uncomment checkpoint_save
void checkpoint_47(
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
        }else{
            enter_menu_from_overworld(env.program_info(), env.console, context, -1);
            // we wait 10 seconds then save, so that the initial conditions are slightly different on each reset.
            env.log("Wait 10 seconds.");
            context.wait_for(Milliseconds(10 * 1000));
            save_game_from_overworld(env.program_info(), env.console, context);
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


// todo: uncomment checkpoint_save
void checkpoint_48(
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
        }else{
            enter_menu_from_overworld(env.program_info(), env.console, context, -1);
            // we wait 10 seconds then save, so that the initial conditions are slightly different on each reset.
            env.log("Wait 10 seconds.");
            context.wait_for(Milliseconds(10 * 1000));
            save_game_from_overworld(env.program_info(), env.console, context);
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


// todo: uncomment checkpoint_save
void checkpoint_49(
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
        }else{
            enter_menu_from_overworld(env.program_info(), env.console, context, -1);
            // we wait 10 seconds then save, so that the initial conditions are slightly different on each reset.
            env.log("Wait 10 seconds.");
            context.wait_for(Milliseconds(10 * 1000));
            save_game_from_overworld(env.program_info(), env.console, context);
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

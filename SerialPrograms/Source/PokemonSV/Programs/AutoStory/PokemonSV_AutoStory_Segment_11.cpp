/*  AutoStory
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "Common/Cpp/DateTime.h"
#include "CommonFramework/GlobalSettingsPanel.h"
#include "CommonFramework/Exceptions/FatalProgramException.h"
#include "CommonFramework/Exceptions/OperationFailedException.h"
#include "CommonFramework/InferenceInfra/InferenceRoutines.h"
#include "CommonFramework/Notifications/ProgramNotifications.h"
#include "CommonFramework/Tools/StatsTracking.h"
#include "CommonFramework/Tools/VideoResolutionCheck.h"
#include "NintendoSwitch/Commands/NintendoSwitch_Commands_PushButtons.h"
#include "NintendoSwitch/Commands/NintendoSwitch_Commands_Routines.h"
#include "NintendoSwitch/Programs/NintendoSwitch_GameEntry.h"
#include "Pokemon/Pokemon_Strings.h"
#include "PokemonSwSh/Inference/PokemonSwSh_IvJudgeReader.h"
#include "PokemonSV/Programs/PokemonSV_GameEntry.h"
#include "PokemonSV/Programs/PokemonSV_SaveGame.h"
#include "PokemonSV/PokemonSV_Settings.h"
#include "PokemonSV/Inference/Overworld/PokemonSV_DirectionDetector.h"
#include "PokemonSV/Inference/Overworld/PokemonSV_OliveDetector.h"
#include "PokemonSV_AutoStoryTools.h"
#include "PokemonSV_AutoStory_Segment_11.h"

//#include <iostream>
//using std::cout;
//using std::endl;
//#include <unordered_map>
//#include <algorithm>

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonSV{

using namespace Pokemon;




std::string AutoStory_Segment_11::name() const{
    return "10.2: Cortondo Gym - Gym challenge (unfinished)";
}

std::string AutoStory_Segment_11::start_text() const{
    return "Start: At Cortondo East Pokecenter.";
}

std::string AutoStory_Segment_11::end_text() const{
    return "End: Beat Cortondo Gym challenge.";
}

void AutoStory_Segment_11::run_segment(SingleSwitchProgramEnvironment& env, BotBaseContext& context, AutoStoryOptions options) const{
    AutoStoryStats& stats = env.current_stats<AutoStoryStats>();

    context.wait_for_all_requests();
    env.console.overlay().add_log("Start Segment 10.2: Cortondo Gym - Gym challenge", COLOR_ORANGE);

    checkpoint_24(env, context, options.notif_status_update);

    context.wait_for_all_requests();
    env.console.log("End Segment 10.2: Cortondo Gym - Gym challenge", COLOR_GREEN);
    stats.m_segment++;
    env.update_stats();

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
        DirectionDetector direction;
        do_action_and_monitor_for_battles(env.program_info(), env.console, context,
            [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){
                direction.change_direction(env.program_info(), env.console, context, 2.71);
                pbf_move_left_joystick(context, 128, 0, 375, 100);
                direction.change_direction(env.program_info(), env.console, context, 1.26);
                pbf_move_left_joystick(context, 128, 0, 1750, 100);                
        });        
       
        direction.change_direction(env.program_info(), env.console, context, 2.73);
        pbf_move_left_joystick(context, 128, 0, 200, 100);
        pbf_wait(context, 5 * TICKS_PER_SECOND);
        context.wait_for_all_requests();
        handle_when_stationary_in_overworld(env.program_info(), env.console, context, 
            [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){           
                walk_forward_until_dialog(env.program_info(), env.console, context, NavigationMovementMode::DIRECTIONAL_ONLY, 20);
            }, 
            [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){           
                pbf_move_left_joystick(context, 0, 0, 100, 20);
            },
            5, 5
        ); 

        mash_button_till_overworld(env.console, context, BUTTON_A, 360);
        walk_forward_until_dialog(env.program_info(), env.console, context, NavigationMovementMode::DIRECTIONAL_SPAM_A, 10);
        clear_dialog(env.console, context, ClearDialogMode::STOP_OVERWORLD, 60, {CallbackEnum::OVERWORLD});

        pbf_move_left_joystick(context, 128, 255, 500, 100);
        pbf_wait(context, 3 * TICKS_PER_SECOND);        
        // wait for overworld after leaving gym
        wait_for_overworld(env.program_info(), env.console, context, 30);

        pbf_move_left_joystick(context, 128, 0, 450, 100);
        direction.change_direction(env.program_info(), env.console, context, 1.26);
        pbf_move_left_joystick(context, 128, 0, 1600, 100);
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

        // section 1. align to Olive roll NPC
        realign_player(env.program_info(), env.console, context, PlayerRealignMode::REALIGN_NEW_MARKER, 157, 0, 40);
        overworld_navigation(env.program_info(), env.console, context, 
            NavigationStopCondition::STOP_MARKER, NavigationMovementMode::DIRECTIONAL_ONLY, 
            128, 0, 40, 10);
        
        // section 1.1. keep walking forward and talk to Olive roll NPC
        do_action_and_monitor_for_battles(env.program_info(), env.console, context,
            [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){
                walk_forward_until_dialog(env.program_info(), env.console, context, NavigationMovementMode::DIRECTIONAL_SPAM_A, 10);
            }
        );     
        mash_button_till_overworld(env.console, context, BUTTON_A);

        // section 2
        pbf_move_left_joystick(context, 128, 0, 1300, 100);

        // section 3
        DirectionDetector direction;
        direction.change_direction(env.program_info(), env.console, context, 6.0);
        pbf_move_left_joystick(context, 128, 0, 700, 100);

        // section 4. align to corner
        direction.change_direction(env.program_info(), env.console, context,  4.69);
        pbf_move_left_joystick(context, 128, 0, 150, 100);

        // section 5. battle first NPC
        direction.change_direction(env.program_info(), env.console, context,  1.485);
        walk_forward_until_dialog(env.program_info(), env.console, context, NavigationMovementMode::DIRECTIONAL_SPAM_A, 10, 128, 20);
        clear_dialog(env.console, context, ClearDialogMode::STOP_BATTLE, 60, {CallbackEnum::BATTLE, CallbackEnum::DIALOG_ARROW});
        run_battle_press_A(env.console, context, BattleStopCondition::STOP_DIALOG);
        mash_button_till_overworld(env.console, context, BUTTON_A);

        // section 6
        direction.change_direction(env.program_info(), env.console, context, 5.95);
        pbf_move_left_joystick(context, 128, 0, 1000, 100);

        // section 7
        direction.change_direction(env.program_info(), env.console, context,  1.327);
        pbf_move_left_joystick(context, 128, 0, 700, 100);

        // section 8
        direction.change_direction(env.program_info(), env.console, context,  6.106);
        pbf_move_left_joystick(context, 128, 0, 200, 100);

        // section 9. battle second NPC
        direction.change_direction(env.program_info(), env.console, context,  4.275);
        walk_forward_until_dialog(env.program_info(), env.console, context, NavigationMovementMode::DIRECTIONAL_SPAM_A, 10, 128, 20);
        clear_dialog(env.console, context, ClearDialogMode::STOP_BATTLE, 60, {CallbackEnum::BATTLE, CallbackEnum::DIALOG_ARROW});
        run_battle_press_A(env.console, context, BattleStopCondition::STOP_DIALOG);
        mash_button_till_overworld(env.console, context, BUTTON_A);

        // section 10. leave Olive roll
        pbf_mash_button(context, BUTTON_Y, 100);
        clear_dialog(env.console, context, ClearDialogMode::STOP_PROMPT, 60, {CallbackEnum::PROMPT_DIALOG});
        clear_dialog(env.console, context, ClearDialogMode::STOP_TIMEOUT, 15, {CallbackEnum::PROMPT_DIALOG});
        wait_for_overworld(env.program_info(), env.console, context);
        enter_menu_from_overworld(env.program_info(), env.console, context, -1);

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

// todo: uncomment checkpoint_save
void checkpoint_26(
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

        // change the time of day: close game, change time to 5:45 am.
        pbf_press_button(context, BUTTON_HOME, 20, GameSettings::instance().GAME_TO_HOME_DELAY);
        change_date(env, context, {2025, 1, 1, 5, 45, 0});
        reset_game_from_home(env.program_info(), env.console, context);

        // talk to Olive roll NPC
        walk_forward_until_dialog(env.program_info(), env.console, context, NavigationMovementMode::DIRECTIONAL_SPAM_A, 10, 128, 20);
        mash_button_till_overworld(env.console, context, BUTTON_A);

        // section 1
        pbf_move_left_joystick(context, 128, 0, 400, 50);
        OliveDetector green(env.console);
        green.push_olive_forward(env.program_info(), env.console, context, 4.5, 800);

        // section 2. realign using fence corner
        DirectionDetector direction;
        direction.change_direction(env.program_info(), env.console, context,  2.74);
        pbf_move_left_joystick(context, 128, 0, 200, 50);
        direction.change_direction(env.program_info(), env.console, context,  4.328);
        pbf_move_left_joystick(context, 128, 0, 200, 50);
        direction.change_direction(env.program_info(), env.console, context,  1.22);
        pbf_move_left_joystick(context, 128, 0, 150, 50);

        // section 3. push olive past first NPC
        green.push_olive_forward(env.program_info(), env.console, context, 5.43, 100);
        green.push_olive_forward(env.program_info(), env.console, context, 5.95, 900);


        // section 4. realign using fence corner
        direction.change_direction(env.program_info(), env.console, context,  4.5);
        pbf_move_left_joystick(context, 128, 0, 200, 50);
        direction.change_direction(env.program_info(), env.console, context, 5.86);
        pbf_move_left_joystick(context, 128, 0, 300, 50);
        direction.change_direction(env.program_info(), env.console, context,  2.76);
        pbf_move_left_joystick(context, 128, 0, 80, 50);

        // section 5. push olive across the hump
        green.push_olive_forward(env.program_info(), env.console, context, 1.27, 700);        

        // section 6. realign using fence.
        direction.change_direction(env.program_info(), env.console, context,  3.0);
        pbf_move_left_joystick(context, 128, 0, 150, 50);
        direction.change_direction(env.program_info(), env.console, context,  1.17);
        pbf_move_left_joystick(context, 128, 0, 200, 50);

        // section 7. past second NPC
        green.push_olive_forward(env.program_info(), env.console, context, 5.8, 900);

        // todo: trigger action once overworld no longer detected

        break;
    }catch (...){
        context.wait_for_all_requests();
        env.console.log("Resetting from checkpoint.");
        // reset_game(env.program_info(), env.console, context); // the checkpoint itself already resets the game, so need to reset twice
        stats.m_reset++;
        env.update_stats();
    }         
    }

}




}
}
}

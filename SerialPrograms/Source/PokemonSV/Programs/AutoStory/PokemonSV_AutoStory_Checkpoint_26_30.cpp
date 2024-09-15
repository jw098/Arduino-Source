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




// todo: uncomment checkpoint_save and fly_to_overlapping_flypoint
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
            checkpoint_save(env, context, notif_status_update);
            first_attempt = false;
        }         
        context.wait_for_all_requests();

        fly_to_overlapping_flypoint(env.program_info(), env.console, context);

        // align for long stretch 1, part 1
        do_action_and_monitor_for_battles(env.program_info(), env.console, context,
        [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){     
            realign_player(env.program_info(), env.console, context, PlayerRealignMode::REALIGN_NEW_MARKER, 75, 0, 50);
        });

        overworld_navigation(env.program_info(), env.console, context, 
            NavigationStopCondition::STOP_MARKER, NavigationMovementMode::CLEAR_WITH_LETS_GO, 
            128, 0, 60, 10, false);

        // align for long stretch 1, part 2
        do_action_and_monitor_for_battles(env.program_info(), env.console, context,
        [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){     
            realign_player_from_landmark(
                env.program_info(), env.console, context, 
                {ZoomChange::ZOOM_IN, 128, 255, 40},
                {ZoomChange::KEEP_ZOOM, 80, 0, 75}
            );            
        });
        
        overworld_navigation(env.program_info(), env.console, context, 
            NavigationStopCondition::STOP_MARKER, NavigationMovementMode::CLEAR_WITH_LETS_GO, 
            128, 0, 60, 10, false);            

        // align for long stretch 1, part 3
        do_action_and_monitor_for_battles(env.program_info(), env.console, context,
        [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){     
            realign_player_from_landmark(
                env.program_info(), env.console, context, 
                {ZoomChange::ZOOM_IN, 128, 255, 60},
                {ZoomChange::KEEP_ZOOM, 95, 0, 115}
            );
        });
        
        overworld_navigation(env.program_info(), env.console, context, 
            NavigationStopCondition::STOP_MARKER, NavigationMovementMode::CLEAR_WITH_LETS_GO, 
            128, 0, 60, 10, false);

        // align for long stretch 2
        do_action_and_monitor_for_battles(env.program_info(), env.console, context,
        [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){     
            realign_player_from_landmark(
                env.program_info(), env.console, context, 
                {ZoomChange::ZOOM_IN, 128, 255, 100},
                {ZoomChange::KEEP_ZOOM, 0, 105, 65}
            );              
        });

        overworld_navigation(env.program_info(), env.console, context, 
            NavigationStopCondition::STOP_MARKER, NavigationMovementMode::CLEAR_WITH_LETS_GO, 
            128, 0, 60, 10, false);

        // align for long stretch 3, part 1
        do_action_and_monitor_for_battles(env.program_info(), env.console, context,
        [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){     
            realign_player_from_landmark(
                env.program_info(), env.console, context, 
                {ZoomChange::ZOOM_IN, 255, 128, 65},
                {ZoomChange::KEEP_ZOOM, 20, 0, 110}
            ); 

        });
        
        overworld_navigation(env.program_info(), env.console, context, 
            NavigationStopCondition::STOP_MARKER, NavigationMovementMode::CLEAR_WITH_LETS_GO, 
            128, 0, 60, 10, false);

        // align for long stretch 3, part 2 
        do_action_and_monitor_for_battles(env.program_info(), env.console, context,
        [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){     
            realign_player_from_landmark(
                env.program_info(), env.console, context, 
                {ZoomChange::ZOOM_IN, 0, 110, 110},
                {ZoomChange::KEEP_ZOOM, 255, 128, 115}
            );

        });

        overworld_navigation(env.program_info(), env.console, context, 
            NavigationStopCondition::STOP_MARKER, NavigationMovementMode::CLEAR_WITH_LETS_GO, 
            128, 0, 60, 10, false);                

        // align for long stretch 3, part 3 
        do_action_and_monitor_for_battles(env.program_info(), env.console, context,
        [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){     
            realign_player_from_landmark(
                env.program_info(), env.console, context, 
                {ZoomChange::ZOOM_IN, 0, 128, 100},
                {ZoomChange::KEEP_ZOOM, 255, 70, 90}
            );

        });

        overworld_navigation(env.program_info(), env.console, context, 
            NavigationStopCondition::STOP_MARKER, NavigationMovementMode::CLEAR_WITH_LETS_GO, 
            128, 0, 60, 10, false);        

        // align to cross bridge
        do_action_and_monitor_for_battles(env.program_info(), env.console, context,
        [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){     
            realign_player_from_landmark(
                env.program_info(), env.console, context, 
                {ZoomChange::ZOOM_IN, 0, 128, 90},
                {ZoomChange::KEEP_ZOOM, 255, 35, 67}
            );

        });


        // attempt to cross bridge. If fall into water, go back to start position (just before bridge) and try again
        WallClock start_to_cross_bridge = current_time();
        while (true){
            if (current_time() - start_to_cross_bridge > std::chrono::minutes(6)){
                throw OperationFailedException(
                    ErrorReport::SEND_ERROR_REPORT, env.console,
                    "checkpoint_26(): Failed to cross bridge after 6 minutes.",
                    true
                );
            }        

            try {
                overworld_navigation(env.program_info(), env.console, context, 
                    NavigationStopCondition::STOP_MARKER, NavigationMovementMode::CLEAR_WITH_LETS_GO, 
                    128, 0, 30, 30, false);         

                break;

            }catch (...){ // try again if fall into water
                pbf_mash_button(context, BUTTON_A, 250);

                // walk back to start position before bridge
                do_action_and_monitor_for_battles(env.program_info(), env.console, context,
                [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){     
                    realign_player_from_landmark(
                        env.program_info(), env.console, context, 
                        {ZoomChange::ZOOM_IN, 255, 255, 180},
                        {ZoomChange::KEEP_ZOOM, 30, 0, 175}
                    );

                });    

                overworld_navigation(env.program_info(), env.console, context, 
                    NavigationStopCondition::STOP_MARKER, NavigationMovementMode::CLEAR_WITH_LETS_GO, 
                    128, 0, 30, 30, false);          


                // align to cross bridge
                do_action_and_monitor_for_battles(env.program_info(), env.console, context,
                [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){     
                    realign_player_from_landmark(
                        env.program_info(), env.console, context, 
                        {ZoomChange::ZOOM_IN, 0, 128, 90},
                        {ZoomChange::KEEP_ZOOM, 255, 35, 67}
                    );

                });             

            }
        }

        confirm_no_overlapping_flypoint(env.program_info(), env.console, context);
        pbf_press_button(context, BUTTON_B, 20, 100);
        press_Bs_to_back_to_overworld(env.program_info(), env.console, context);

        env.console.log("Successfully crossed the bridge.");

        // align for post-bridge section 1
        do_action_and_monitor_for_battles(env.program_info(), env.console, context,
        [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){     
            realign_player_from_landmark(
                env.program_info(), env.console, context, 
                {ZoomChange::ZOOM_IN, 0, 150, 60},
                {ZoomChange::KEEP_ZOOM, 255, 60, 50} // {ZoomChange::KEEP_ZOOM, 255, 60, 50}
            );

        });    

        overworld_navigation(env.program_info(), env.console, context, 
            NavigationStopCondition::STOP_MARKER, NavigationMovementMode::CLEAR_WITH_LETS_GO, 
            128, 0, 30, 10, false);   

        // align for post-bridge section 2
        do_action_and_monitor_for_battles(env.program_info(), env.console, context,
        [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){     
            realign_player_from_landmark(
                env.program_info(), env.console, context, 
                {ZoomChange::ZOOM_IN, 0, 150, 60},
                {ZoomChange::KEEP_ZOOM, 255, 105, 50}
            );

        });    

        overworld_navigation(env.program_info(), env.console, context, 
            NavigationStopCondition::STOP_MARKER, NavigationMovementMode::CLEAR_WITH_LETS_GO, 
            128, 0, 30, 10, false);        

        // align for post-bridge section 3
        do_action_and_monitor_for_battles(env.program_info(), env.console, context,
        [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){     
            realign_player_from_landmark(
                env.program_info(), env.console, context, 
                {ZoomChange::ZOOM_IN, 0, 128, 50},
                {ZoomChange::KEEP_ZOOM, 255, 90, 40}
            );

        });          

        overworld_navigation(env.program_info(), env.console, context, 
            NavigationStopCondition::STOP_MARKER, NavigationMovementMode::CLEAR_WITH_LETS_GO, 
            128, 0, 30, 10, false);        

        // align for post-bridge section 4
        do_action_and_monitor_for_battles(env.program_info(), env.console, context,
        [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){     
            realign_player_from_landmark(
                env.program_info(), env.console, context, 
                {ZoomChange::ZOOM_IN, 0, 128, 50},
                {ZoomChange::KEEP_ZOOM, 255, 65, 35}
            );

        });    

        overworld_navigation(env.program_info(), env.console, context, 
            NavigationStopCondition::STOP_MARKER, NavigationMovementMode::CLEAR_WITH_LETS_GO, 
            128, 0, 30, 10, false);     



        // align for post-bridge section 5. set marker to pokecenter.
        do_action_and_monitor_for_battles(env.program_info(), env.console, context,
        [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){     
            realign_player_from_landmark(
                env.program_info(), env.console, context, 
                {ZoomChange::ZOOM_IN, 0, 128, 50},
                {ZoomChange::KEEP_ZOOM, 128, 128, 0}
            );

        });    

        overworld_navigation(env.program_info(), env.console, context, 
            NavigationStopCondition::STOP_MARKER, NavigationMovementMode::CLEAR_WITH_LETS_GO, 
            128, 0, 30, 10, false);    


        // align for post-bridge section 6. set marker past pokecenter
        do_action_and_monitor_for_battles(env.program_info(), env.console, context,
        [&](const ProgramInfo& info, ConsoleHandle& console, BotBaseContext& context){    
            realign_player(env.program_info(), env.console, context, PlayerRealignMode::REALIGN_NEW_MARKER, 0, 180, 50);
        });

        overworld_navigation(env.program_info(), env.console, context, 
            NavigationStopCondition::STOP_TIME, NavigationMovementMode::CLEAR_WITH_LETS_GO, 
            128, 0, 5, 5, false);    



        fly_to_overlapping_flypoint(env.program_info(), env.console, context);
              
       
        break;
    }catch(OperationFailedException& e){
        context.wait_for_all_requests();
        env.console.log(e.m_message, COLOR_RED);
        env.console.log("Resetting from checkpoint.");
        reset_game(env.program_info(), env.console, context);
        stats.m_reset++;
        env.update_stats();
    }             
    }

}

void checkpoint_27(
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
    }catch(OperationFailedException& e){
        context.wait_for_all_requests();
        env.console.log(e.m_message, COLOR_RED);
        env.console.log("Resetting from checkpoint.");
        reset_game(env.program_info(), env.console, context);
        stats.m_reset++;
        env.update_stats();
    }             
    }

}

void checkpoint_28(
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
    }catch(OperationFailedException& e){
        context.wait_for_all_requests();
        env.console.log(e.m_message, COLOR_RED);
        env.console.log("Resetting from checkpoint.");
        reset_game(env.program_info(), env.console, context);
        stats.m_reset++;
        env.update_stats();
    }             
    }

}

void checkpoint_29(
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
    }catch(OperationFailedException& e){
        context.wait_for_all_requests();
        env.console.log(e.m_message, COLOR_RED);
        env.console.log("Resetting from checkpoint.");
        reset_game(env.program_info(), env.console, context);
        stats.m_reset++;
        env.update_stats();
    }             
    }

}

void checkpoint_30(
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
    }catch(OperationFailedException& e){
        context.wait_for_all_requests();
        env.console.log(e.m_message, COLOR_RED);
        env.console.log("Resetting from checkpoint.");
        reset_game(env.program_info(), env.console, context);
        stats.m_reset++;
        env.update_stats();
    }             
    }

}

void checkpoint_31(
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
    }catch(OperationFailedException& e){
        context.wait_for_all_requests();
        env.console.log(e.m_message, COLOR_RED);
        env.console.log("Resetting from checkpoint.");
        reset_game(env.program_info(), env.console, context);
        stats.m_reset++;
        env.update_stats();
    }             
    }

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
       
        break;
    }catch(OperationFailedException& e){
        context.wait_for_all_requests();
        env.console.log(e.m_message, COLOR_RED);
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
    }catch(OperationFailedException& e){
        context.wait_for_all_requests();
        env.console.log(e.m_message, COLOR_RED);
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
    }catch(OperationFailedException& e){
        context.wait_for_all_requests();
        env.console.log(e.m_message, COLOR_RED);
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

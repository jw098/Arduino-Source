/*  Turbo Macro
 *
 *  From: https://github.com/PokemonAutomation/
 *
 */

#include "NintendoSwitch_TurboMacro.h"
#include "CommonTools/Async/InferenceRoutines.h"
#include "PokemonLA/Inference/Sounds/PokemonLA_ShinySoundDetector.h"
#include "PokemonLZA/Options/PokemonLZA_ShinyDetectedAction.h"
#include "NintendoSwitch/Programs/NintendoSwitch_GameEntry.h"
//#include <iostream>
//using std::cout;
//using std::endl;

namespace PokemonAutomation{
namespace NintendoSwitch{


TurboMacro_Descriptor::TurboMacro_Descriptor()
    : SingleSwitchProgramDescriptor(
        "NintendoSwitch:TurboMacro",
        "Nintendo Switch", "Turbo Macro",
        "Programs/NintendoSwitch/TurboMacro.html",
        "Create macros",
        ProgramControllerClass::StandardController_NoRestrictions,
        FeedbackType::NONE,
        AllowCommandsWhenRunning::DISABLE_COMMANDS
    )
{}

TurboMacro::TurboMacro()
    : LOOP(
        "<b>Number of times to loop:</b>",
        LockMode::UNLOCK_WHILE_RUNNING,
        100, 0
    )
    , TABLE(
        "Command Schedule:",
        {
            ControllerClass::NintendoSwitch_ProController,
            ControllerClass::NintendoSwitch_LeftJoycon,
            ControllerClass::NintendoSwitch_RightJoycon,
        }
    )
    , RUN_UNTIL_CALLBACK(
        "Trigger to stop program:",
        {
            {RunUntilCallback::NONE,         "none",           "None"},
            {RunUntilCallback::LZA_SHINY_SOUND,            "lza-shiny-sound",              "LZA: Shiny Sound Detected"},
        },
        LockMode::LOCK_WHILE_RUNNING,
        RunUntilCallback::NONE
    )
{
    PA_ADD_OPTION(LOOP);
    PA_ADD_OPTION(RUN_UNTIL_CALLBACK);
    PA_ADD_OPTION(TABLE);
}


void TurboMacro::program(SingleSwitchProgramEnvironment& env, CancellableScope& scope){
    switch (RUN_UNTIL_CALLBACK){
    case RunUntilCallback::NONE:
        run_table(env, scope);
        break;
    case RunUntilCallback::LZA_SHINY_SOUND:
        run_table_stop_when_lza_shiny_sound(env, scope);
        break;
    default:
        throw InternalProgramError(nullptr, PA_CURRENT_FUNCTION, "TurboMacro::program(): Unknown RunUntilCallback");
        
    }
}


void TurboMacro::run_table(SingleSwitchProgramEnvironment& env, CancellableScope& scope){
    for (uint32_t c = 0; c < LOOP; c++){
        TABLE.run(scope, env.console.controller());
    }
}

void TurboMacro::run_table_stop_when_lza_shiny_sound(SingleSwitchProgramEnvironment& env, CancellableScope& scope){
    PokemonLZA::ShinySoundDetectedActionOption shiny_detected_option("Shiny Detected", "", "1000 ms", PokemonLZA::ShinySoundDetectedAction::NOTIFY_ON_FIRST_ONLY);
    PokemonLZA::ShinySoundHandler shiny_sound_handler(shiny_detected_option);
    PokemonLA::ShinySoundDetector shiny_detector(env.console, [&](float error_coefficient) -> bool {
        //  Warning: This callback will be run from a different thread than this function.
        // env.console.overlay().add_log("Shiny Sound Detected!", COLOR_YELLOW);
        return shiny_sound_handler.on_shiny_sound(
            env, env.console,
            0,
            error_coefficient
        );
    });

    ProControllerContext context(scope, env.console.controller<ProController>());

    int ret = run_until(
        env.console, scope,
        [&](CancellableScope& scope){
            run_table(env, scope);
        },
        {shiny_detector}
    );

    if (ret == 0){
        go_home(env.console, context);
    }
}

}
}

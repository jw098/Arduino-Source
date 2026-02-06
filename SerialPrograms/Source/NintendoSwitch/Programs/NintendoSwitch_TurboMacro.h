/*  Turbo Macro
 *
 *  From: https://github.com/PokemonAutomation/
 *
 */

#ifndef PokemonAutomation_NintendoSwitch_TurboMacro_H
#define PokemonAutomation_NintendoSwitch_TurboMacro_H

#include "Common/Cpp/Options/SimpleIntegerOption.h"
#include "Controllers/ControllerStateTable.h"
#include "NintendoSwitch/NintendoSwitch_SingleSwitchProgram.h"

namespace PokemonAutomation{
namespace NintendoSwitch{


class TurboMacro_Descriptor : public SingleSwitchProgramDescriptor{
public:
    TurboMacro_Descriptor();
};


class TurboMacro : public SingleSwitchProgramInstance{
public:
    TurboMacro();

    virtual void program(SingleSwitchProgramEnvironment& env, CancellableScope& scope) override;

    void run_table(SingleSwitchProgramEnvironment& env, CancellableScope& scope);

    void run_table_stop_when_lza_shiny_sound(SingleSwitchProgramEnvironment& env, CancellableScope& scope);


private:
    SimpleIntegerOption<uint32_t> LOOP;
    ControllerCommandTables TABLE;

    enum class RunUntilCallback{
        NONE,
        LZA_SHINY_SOUND,
    };
    EnumDropdownOption<RunUntilCallback> RUN_UNTIL_CALLBACK;
};



}
}
#endif // PokemonAutomation_NintendoSwitch_TurboMacro_H

/*  Test Program (Switch)
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_NintendoSwitch_TestProgram_H
#define PokemonAutomation_NintendoSwitch_TestProgram_H

#include "Common/Cpp/Options/BatchOption.h"
#include "Common/Cpp/Options/StaticTextOption.h"
#include "Common/Cpp/Options/BooleanCheckBoxOption.h"
#include "CommonFramework/Options/StringSelectOption.h"
#include "CommonFramework/Options/LanguageOCROption.h"
#include "CommonFramework/Notifications/EventNotificationsTable.h"
#include "NintendoSwitch/NintendoSwitch_SingleSwitchProgram.h"
#include "NintendoSwitch/NintendoSwitch_MultiSwitchProgram.h"
#include "PokemonSwSh/Options/PokemonSwSh_BallSelectOption.h"
#include "PokemonSwSh/Options/EncounterFilter/PokemonSwSh_EncounterFilterOption.h"
#include "CommonFramework/Tools/StatsTracking.h"

namespace PokemonAutomation{
namespace NintendoSwitch{

//using namespace PokemonSwSh;





class TestProgram_Descriptor : public MultiSwitchProgramDescriptor{
public:
    TestProgram_Descriptor();
};


class TestProgram : public MultiSwitchProgramInstance{
public:
    TestProgram();

//    std::unique_ptr<StatsTracker> make_stats() const override{
//        return std::unique_ptr<StatsTracker>(new StatsTracker());
//    }
    virtual void program(MultiSwitchProgramEnvironment& env, CancellableScope& scope) override;

private:
    struct Stats : public StatsTracker{
        Stats()
            : m_resets(m_stats["Resets"])
        {
            m_display_order.emplace_back("Resets");
        }
        std::atomic<uint64_t>& m_resets;
    };

private:
    OCR::LanguageOCR LANGUAGE;

    StaticTextOption STATIC_TEXT;

    StringSelectOption SELECT;

    EventNotificationOption NOTIFICATION_TEST;
    EventNotificationsOption NOTIFICATIONS;
};



}
}
#endif

/*  Nintendo Switch Controller (Serial PABotBase)
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */


#ifndef PokemonAutomation_NintendoSwitch_Controller_SerialPABotBase_H
#define PokemonAutomation_NintendoSwitch_Controller_SerialPABotBase_H

#include <QSerialPortInfo>
#include "ClientSource/Connection/BotBase.h"
#include "Controllers/ControllerDescriptor.h"
#include "Controllers/ControllerConnection.h"
#include "Controllers/SerialPABotBase/SerialPABotBase_Connection.h"
#include "NintendoSwitch_ControllerWithScheduler.h"

namespace PokemonAutomation{
namespace NintendoSwitch{



class SwitchController_SerialPABotBase_Descriptor : public ControllerDescriptor
{
public:
    static const char* TYPENAME;

public:
    SwitchController_SerialPABotBase_Descriptor() = default;
    SwitchController_SerialPABotBase_Descriptor(const QSerialPortInfo& info)
        : m_port(info)
    {}

    const QSerialPortInfo& port() const{
        return m_port;
    }

    virtual bool operator==(const ControllerDescriptor& x) const override;
    virtual const char* type_name() const override;
    virtual std::string display_name() const override;

    virtual void load_json(const JsonValue& json) override;
    virtual JsonValue to_json() const override;

    virtual std::unique_ptr<ControllerConnection> open_connection(
        Logger& logger
    ) const override;
    virtual std::unique_ptr<AbstractController> make_controller(
        Logger& logger,
        ControllerConnection& connection,
        ControllerType controller_type,
        const ControllerRequirements& requirements
    ) const override;

private:
    QSerialPortInfo m_port;
};




template <typename Type>
PA_FORCE_INLINE Type milliseconds_to_ticks_8ms(Type milliseconds){
    return milliseconds / 8 + (milliseconds % 8 + 7) / 8;
}









class SwitchController_SerialPABotBase : public SwitchControllerWithScheduler
{
public:
    using ContextType = SwitchControllerContext;

public:
    ~SwitchController_SerialPABotBase();
    SwitchController_SerialPABotBase(
        Logger& logger,
        SerialPABotBase::SerialPABotBaseConnection& connection,
        const ControllerRequirements& requirements
    );

    virtual Logger& logger() override{
        return m_logger;
    }

    virtual bool is_ready() const override{
        return m_serial && m_handle.is_ready() && m_error_string.empty();
    }
    virtual std::string error_string() const override{
        return m_error_string;
    }


public:
    //  General Control

    virtual void wait_for_all(const Cancellable* cancellable) override;
    virtual void cancel_all_commands(const Cancellable* cancellable) override;
    virtual void replace_on_next_command(const Cancellable* cancellable) override;


public:
    //  Basic Commands

    virtual void issue_controller_state(
        const Cancellable* cancellable,
        Button button,
        DpadPosition position,
        uint8_t left_x, uint8_t left_y,
        uint8_t right_x, uint8_t right_y,
        Milliseconds duration
    ) override;
    virtual void send_botbase_request(
        const Cancellable* cancellable,
        const BotBaseRequest& request
    ) override;
    virtual BotBaseMessage send_botbase_request_and_wait(
        const Cancellable* cancellable,
        const BotBaseRequest& request
    ) override;


private:
    Logger& m_logger;
    const ControllerRequirements& m_requirements;

    SerialPABotBase::SerialPABotBaseConnection& m_handle;
    BotBaseController* m_serial;

    std::string m_error_string;
};




}
}
#endif

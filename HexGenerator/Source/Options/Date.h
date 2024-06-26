/*  Switch Date
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_SwitchDate_H
#define PokemonAutomation_SwitchDate_H

#include "Common/Cpp/Options/DateOption.h"
#include "SingleStatementOption.h"

namespace PokemonAutomation{
namespace HexGenerator{


class SwitchDate : public SingleStatementOption{
public:
    static const std::string OPTION_TYPE;

public:
    SwitchDate(const JsonObject& obj);

    virtual const std::string& type() const override{ return OPTION_TYPE; }
    virtual std::string check_validity() const override;
    virtual void restore_defaults() override;

    virtual JsonObject to_json() const override;
    virtual std::string to_cpp() const override;

    virtual QWidget* make_ui(QWidget& parent) override;

private:
    DateTimeOption m_option;
};


}
}
#endif



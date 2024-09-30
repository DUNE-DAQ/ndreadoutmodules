/**
 * @file DummyModule.cpp
 *
 * Implementations of DummyModule's functions
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "DummyModule.hpp"

#include "ndreadoutmodules/dummymodule/Nljs.hpp"
#include "ndreadoutmodules/dummymoduleinfo/InfoNljs.hpp"

#include <string>

namespace dunedaq::ndreadoutmodules {

DummyModule::DummyModule(const std::string& name)
  : dunedaq::appfwk::DAQModule(name)
{
  register_command("conf", &DummyModule::do_conf);
}

void
DummyModule::init(const data_t& /* structured args */)
{}

void
DummyModule::generate_opmon_data()
{
  opmon::DummyModuleInfo info;
  info.total_amount = m_total_amount;
  info.new_amount = m_amount_since_last_get_info_call.exchange(0);

  publish(std::move(info));
}

void
DummyModule::do_conf(const data_t& conf_as_json)
{
  auto conf_as_cpp = conf_as_json.get<dummymodule::Conf>();
  m_some_configured_value = conf_as_cpp.some_configured_value;
}

} // namespace dunedaq::ndreadoutmodules

DEFINE_DUNE_DAQ_MODULE(dunedaq::ndreadoutmodules::DummyModule)

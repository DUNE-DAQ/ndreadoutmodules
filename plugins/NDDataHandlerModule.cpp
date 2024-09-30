/**
 * @file NDDataLinkHandler.cpp NDDataLinkHandler class implementation
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#include "NDDataHandlerModule.hpp"

#include "appfwk/app/Nljs.hpp"
#include "appfwk/cmd/Nljs.hpp"
#include "appfwk/cmd/Structs.hpp"
#include "rcif/cmd/Nljs.hpp"

#include "iomanager/IOManager.hpp"
#include "datahandlinglibs/opmon/datahandling_info.pb.h"
#include "datahandlinglibs/DataHandlingIssues.hpp"
#include "datahandlinglibs/ReadoutLogging.hpp"
#include "datahandlinglibs/concepts/DataHandlingConcept.hpp"
#include "datahandlinglibs/models/DataHandlingModel.hpp"
#include "datahandlinglibs/models/SkipListLatencyBufferModel.hpp"

#include "ndreadoutlibs/NDReadoutPACMANTypeAdapter.hpp"
#include "ndreadoutlibs/NDReadoutMPDTypeAdapter.hpp"
#include "ndreadoutlibs/pacman/PACMANFrameProcessor.hpp"
#include "ndreadoutlibs/pacman/PACMANListRequestHandler.hpp"
#include "ndreadoutlibs/mpd/MPDFrameProcessor.hpp"
#include "ndreadoutlibs/mpd/MPDListRequestHandler.hpp"

#include <memory>
#include <sstream>
#include <string>
#include <vector>

using namespace dunedaq::datahandlinglibs::logging;

namespace dunedaq {

DUNE_DAQ_TYPESTRING(dunedaq::ndreadoutlibs::types::NDReadoutPACMANTypeAdapter, "PACMANFrame")
DUNE_DAQ_TYPESTRING(dunedaq::ndreadoutlibs::types::NDReadoutMPDTypeAdapter, "MPDFrame")

namespace ndreadoutmodules {

NDDataHandlerModule::NDDataHandlerModule(const std::string& name)
  : DAQModule(name)
  , RawDataHandlerBase(name)
{ 
  //inherited_dlh::m_readout_creator = make_readout_creator("nd");

  inherited_mod::register_command("conf", &inherited_dlh::do_conf);
  inherited_mod::register_command("scrap", &inherited_dlh::do_scrap);
  inherited_mod::register_command("start", &inherited_dlh::do_start);
  inherited_mod::register_command("stop_trigger_sources", &inherited_dlh::do_stop);
  inherited_mod::register_command("record", &inherited_dlh::do_record);
}

void
NDDataHandlerModule::init(std::shared_ptr<appfwk::ModuleConfiguration> cfg)
{

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering init() method";
  inherited_dlh::init(cfg);
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting init() method";
}

void
NDDataHandlerModule::generate_opmon_data()
 {
 }

std::unique_ptr<datahandlinglibs::DataHandlingConcept>
NDDataHandlerModule::create_readout(const appmodel::DataHandlerModule* modconf, std::atomic<bool>& run_marker)
{
  namespace rol = dunedaq::datahandlinglibs;
  namespace ndt = dunedaq::ndreadoutlibs::types;

  // Acquire input connection and its DataType

  std::string raw_dt = modconf->get_module_configuration()->get_input_data_type();
  TLOG()<<"RAW DT :: "<<raw_dt;
  // IF ND LAr PACMAN
  if (raw_dt.find("PACMANFrame") != std::string::npos) {
    TLOG_DEBUG(TLVL_WORK_STEPS) << "Creating readout for a pacman";
    auto readout_model =
      std::make_unique<rol::DataHandlingModel<ndt::NDReadoutPACMANTypeAdapter,
                                         ndreadoutlibs::PACMANListRequestHandler,
                                         rol::SkipListLatencyBufferModel<ndt::NDReadoutPACMANTypeAdapter>,
                                         ndreadoutlibs::PACMANFrameProcessor>>(run_marker);
    readout_model->init(modconf);
    return readout_model;
  }

  // IF ND LAr MPD
  else if (raw_dt.find("MPDFrame") != std::string::npos) {
    TLOG_DEBUG(TLVL_WORK_STEPS) << "Creating readout for a mpd";
    auto readout_model =
    std::make_unique<rol::DataHandlingModel<ndt::NDReadoutMPDTypeAdapter,
                                       ndreadoutlibs::MPDListRequestHandler,
                                       rol::SkipListLatencyBufferModel<ndt::NDReadoutMPDTypeAdapter>,
                                       ndreadoutlibs::MPDFrameProcessor>>(run_marker);
    readout_model->init(modconf);
    return readout_model;
  }

  return nullptr;
}

} // namespace ndreadoutmodules
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::ndreadoutmodules::NDDataHandlerModule)

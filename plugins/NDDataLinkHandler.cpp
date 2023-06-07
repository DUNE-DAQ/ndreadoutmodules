/**
 * @file NDDataLinkHandler.cpp NDDataLinkHandler class implementation
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#include "NDDataLinkHandler.hpp"

#include "appfwk/app/Nljs.hpp"
#include "appfwk/cmd/Nljs.hpp"
#include "appfwk/cmd/Structs.hpp"
#include "rcif/cmd/Nljs.hpp"

#include "iomanager/IOManager.hpp"

#include "readoutlibs/ReadoutIssues.hpp"
#include "readoutlibs/ReadoutLogging.hpp"
#include "readoutlibs/concepts/ReadoutConcept.hpp"
#include "readoutlibs/models/ReadoutModel.hpp"
#include "readoutlibs/models/SkipListLatencyBufferModel.hpp"

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

using namespace dunedaq::readoutlibs::logging;

namespace dunedaq {

DUNE_DAQ_TYPESTRING(dunedaq::ndreadoutlibs::types::NDReadoutPACMANTypeAdapter, "PACMANFrame")
DUNE_DAQ_TYPESTRING(dunedaq::ndreadoutlibs::types::NDReadoutMPDTypeAdapter, "MPDFrame")

namespace ndreadoutmodules {

NDDataLinkHandler::NDDataLinkHandler(const std::string& name)
  : DAQModule(name)
  , DataLinkHandlerBase(name)
{ 
  //inherited_dlh::m_readout_creator = make_readout_creator("nd");

  inherited_mod::register_command("conf", &inherited_dlh::do_conf);
  inherited_mod::register_command("scrap", &inherited_dlh::do_scrap);
  inherited_mod::register_command("start", &inherited_dlh::do_start);
  inherited_mod::register_command("stop_trigger_sources", &inherited_dlh::do_stop);
  inherited_mod::register_command("record", &inherited_dlh::do_record);
}

void
NDDataLinkHandler::init(const data_t& args)
{

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering init() method";
  inherited_dlh::init(args);
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting init() method";
}

void
NDDataLinkHandler::get_info(opmonlib::InfoCollector& ci, int level)
{
  inherited_dlh::get_info(ci, level);
}

std::unique_ptr<readoutlibs::ReadoutConcept>
NDDataLinkHandlercreate_readout(const nlohmann::json& args, std::atomic<bool>& run_marker)
{
  namespace rol = dunedaq::readoutlibs;
  namespace ndt = dunedaq::ndreadoutlibs::types;

  // Acquire input connection and its DataType
  auto ci = appfwk::connection_index(args, {"raw_input"});
  auto datatypes = dunedaq::iomanager::IOManager::get()->get_datatypes(ci["raw_input"]);
  if (datatypes.size() != 1) {
    ers::error(dunedaq::readoutlibs::GenericConfigurationError(ERS_HERE,
      "Multiple raw_input queues specified! Expected only a single instance!"));
  }
  std::string raw_dt{ *datatypes.begin() };
  TLOG() << "Choosing specializations for ReadoutModel with raw_input"
         << " [uid:" << ci["raw_input"] << " , data_type:" << raw_dt << ']';

  // IF ND LAr PACMAN
  if (raw_dt.find("PACMANFrame") != std::string::npos) {
    TLOG_DEBUG(TLVL_WORK_STEPS) << "Creating readout for a pacman";
    auto readout_model =
      std::make_unique<rol::ReadoutModel<ndt::NDReadoutPACMANTypeAdapter,
                                         ndreadoutlibs::PACMANListRequestHandler,
                                         rol::SkipListLatencyBufferModel<ndt::NDReadoutPACMANTypeAdapter>,
                                         ndreadoutlibs::PACMANFrameProcessor>>(run_marker);
    readout_model->init(args);
    return readout_model;
  }

  // IF ND LAr MPD
  if (raw_dt.find("MPDFrame") != std::string::npos) {
    TLOG_DEBUG(TLVL_WORK_STEPS) << "Creating readout for a mpd";
    auto readout_model =
    std::make_unique<rol::ReadoutModel<ndt::NDReadoutMPDTypeAdapter,
                                       ndreadoutlibs::MPDListRequestHandler,
                                       rol::SkipListLatencyBufferModel<ndt::NDReadoutMPDTypeAdapter>,
                                       ndreadoutlibs::MPDFrameProcessor>>(run_marker);
    readout_model->init(args);
    return readout_model;
  }

  return nullptr;
}

} // namespace ndreadoutmodules
} // namespace dunedaq

DEFINE_DUNE_DAQ_MODULE(dunedaq::ndreadoutmodules::NDDataLinkHandler)

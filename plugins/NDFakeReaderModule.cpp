/**
 * @file NDFakeReaderModule.cpp FDFakeReaderModule class implementation
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "NDFakeReaderModule.hpp"

#include "logging/Logging.hpp"

#include "datahandlinglibs/ReadoutLogging.hpp"
#include "datahandlinglibs/DataHandlingIssues.hpp"
#include "datahandlinglibs/models/SourceEmulatorModel.hpp"
#include "appmodel/DataReaderModule.hpp"

#include "ndreadoutlibs/NDReadoutPACMANTypeAdapter.hpp"
#include "ndreadoutlibs/NDReadoutMPDTypeAdapter.hpp"

#include <chrono>
#include <fstream>
#include <iomanip>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace dunedaq::datahandlinglibs::logging;

namespace dunedaq{
DUNE_DAQ_TYPESTRING(dunedaq::ndreadoutlibs::types::NDReadoutPACMANTypeAdapter, "PACMANFrame")
DUNE_DAQ_TYPESTRING(dunedaq::ndreadoutlibs::types::NDReadoutMPDTypeAdapter, "MPDFrame")

namespace ndreadoutmodules{

NDFakeReaderModule::NDFakeReaderModule(const std::string& name)
  : DAQModule(name)
  , FakeCardReaderBase(name)
{
  inherited_mod::register_command("conf", &inherited_fcr::do_conf);
  inherited_mod::register_command("scrap", &inherited_fcr::do_scrap);
  inherited_mod::register_command("start", &inherited_fcr::do_start);
  inherited_mod::register_command("stop_trigger_sources", &inherited_fcr::do_stop);
}

void
NDFakeReaderModule::init(std::shared_ptr<appfwk::ModuleConfiguration> cfg)
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Entering init() method";
  inherited_fcr::init(cfg);
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << get_name() << ": Exiting init() method";
}

std::shared_ptr<datahandlinglibs::SourceEmulatorConcept>
NDFakeReaderModule::create_source_emulator(std::string q_id, std::atomic<bool>& run_marker){
    //! values suitable to emulation
    // TODO: FIND VALUES
    static constexpr int pacman_time_tick_diff = 1;    
    static constexpr double pacman_dropout_rate = 1.0;
    static constexpr double pacman_rate_khz = 1.0;
    static constexpr int pacman_frames_per_tick = 1;

    static constexpr int mpd_time_tick_diff = 1;    
    static constexpr double mpd_dropout_rate = 1.0;
    static constexpr double mpd_rate_khz = 1.0;
    static constexpr int mpd_frames_per_tick = 1;

    static constexpr double emu_frame_error_rate = 0.0;

    auto datatypes = dunedaq::iomanager::IOManager::get()->get_datatypes(q_id);

    if (datatypes.size()!=1){
        ers::error(dunedaq::datahandlinglibs::GenericConfigurationError(ERS_HERE,
            "Multiple output data types specified! Expected only a single type!"));
    }

    std::string raw_dt{ *datatypes.begin() };
          TLOG() << "Choosing specialization for SourceEmulator with raw_input"
             << " [uid:" << q_id << " , data_type:" << raw_dt << ']';

    // IF PACMAN
    if(raw_dt.find("PACMANFrame")!= std::string::npos){
        TLOG_DEBUG(TLVL_WORK_STEPS) << "Creating fake PACMAN link";

        auto source_emu_model =
        std::make_shared<datahandlinglibs::SourceEmulatorModel<ndreadoutlibs::types::NDReadoutPACMANTypeAdapter>>(
            q_id, run_marker, pacman_time_tick_diff, pacman_dropout_rate, emu_frame_error_rate, pacman_rate_khz, pacman_frames_per_tick);
    
        register_node(q_id, source_emu_model);
        return source_emu_model;

    }


    // IF MPD
    if(raw_dt.find("MPDFrame")!= std::string::npos){
        TLOG_DEBUG(TLVL_WORK_STEPS) << "Creating fake MPD link";

        auto source_emu_model =
        std::make_shared<datahandlinglibs::SourceEmulatorModel<ndreadoutlibs::types::NDReadoutMPDTypeAdapter>>(
            q_id, run_marker, mpd_time_tick_diff, mpd_dropout_rate, emu_frame_error_rate, mpd_rate_khz, mpd_frames_per_tick);
    
        register_node(q_id, source_emu_model);
        return source_emu_model;

    }




    return nullptr;
}




} // ndreadoutmodules
} // dunedaq
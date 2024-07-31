/**
 * @file NDDataLinkHandler.hpp FarDetector Generic readout
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef NDREADOUTMODULES_PLUGINS_NDDATALINKHANDLER_HPP_
#define NDREADOUTMODULES_PLUGINS_NDDATALINKHANDLER_HPP_

#include "appfwk/DAQModule.hpp"

#include "datahandlinglibs/RawDataHandlerBase.hpp"

#include <string>

namespace dunedaq {
namespace ndreadoutmodules {

class NDDataHandlerModule : public dunedaq::appfwk::DAQModule,
                          public dunedaq::readoutmodules::RawDataHandlerBase
{
public:
  using inherited_dlh = dunedaq::readoutmodules::RawDataHandlerBase;
  using inherited_mod = dunedaq::appfwk::DAQModule;
  /**
   * @brief NDDataHandlerModule Constructor
   * @param name Instance name for this NDDataHandlerModule instance
   */
  explicit NDDataHandlerModule(const std::string& name);

  NDDataHandlerModule(const NDDataHandlerModule&) = delete;            ///< NDDataHandlerModule is not copy-constructible
  NDDataHandlerModule& operator=(const NDDataHandlerModule&) = delete; ///< NDDataHandlerModule is not copy-assignable
  NDDataHandlerModule(NDDataHandlerModule&&) = delete;                 ///< NDDataHandlerModule is not move-constructible
  NDDataHandlerModule& operator=(NDDataHandlerModule&&) = delete;      ///< NDDataHandlerModule is not move-assignable

  void init(std::shared_ptr<appfwk::ModuleConfiguration> cfg) override;
  void get_info(opmonlib::InfoCollector& ci, int level) override;

  std::unique_ptr<readoutlibs::ReadoutConcept>
  create_readout(const appmodel::DataHandlerModule* modconf, std::atomic<bool>& run_marker) override;

};

} // namespace ndreadoutmodules
} // namespace dunedaq

#endif // NDREADOUTMODULES_PLUGINS_NDDATALINKHANDLER_HPP_

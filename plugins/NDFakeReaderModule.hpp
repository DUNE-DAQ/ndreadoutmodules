#ifndef NDREADOUTMODULES_PLUGINS_NDFAKECARDREADER_HPP_
#define NDREADOUTMODULES_PLUGINS_NDFAKECARDREADER_HPP_

#include "appfwk/DAQModule.hpp"
#include "appfwk/ModuleConfiguration.hpp"

#include "datahandlinglibs/FakeCardReaderBase.hpp"

#include <string>

namespace dunedaq{
namespace ndreadoutmodules{

class NDFakeReaderModule : public dunedaq::appfwk::DAQModule,
                            public dunedaq::datahandlinglibs::FakeCardReaderBase
{
   public:
    using inherited_fcr = dunedaq::datahandlinglibs::FakeCardReaderBase;
    using inherited_mod = dunedaq::appfwk::DAQModule;

    explicit NDFakeReaderModule(const std::string& name);

    NDFakeReaderModule(const NDFakeReaderModule&) = delete;            ///< NDFakeReaderModule is not copy-constructible
    NDFakeReaderModule& operator=(const NDFakeReaderModule&) = delete; ///< NDFakeReaderModule is not copy-assignable
    NDFakeReaderModule(NDFakeReaderModule&&) = delete;                 ///< NDFakeReaderModule is not move-constructible
    NDFakeReaderModule& operator=(NDFakeReaderModule&&) = delete;      ///< NDFakeReaderModule is not move-assignable

    void init(std::shared_ptr<appfwk::ModuleConfiguration> cfg) override;

    std::shared_ptr<datahandlinglibs::SourceEmulatorConcept>
    create_source_emulator(std::string qi, std::atomic<bool>& run_marker) override;

};


} //ndreadoutmodules
}  //dunedaq


#endif
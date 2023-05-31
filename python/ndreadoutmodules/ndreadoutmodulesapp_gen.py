# This module facilitates the generation of ndreadoutmodules DAQModules within ndreadoutmodules apps


# Set moo schema search path                                                                              
from dunedaq.env import get_moo_model_path
import moo.io
moo.io.default_load_path = get_moo_model_path()

# Load configuration types                                                                                
import moo.otypes
moo.otypes.load_types("ndreadoutmodules/dummymodule.jsonnet")

import dunedaq.ndreadoutmodules.dummymodule as dummymodule

from daqconf.core.app import App, ModuleGraph
from daqconf.core.daqmodule import DAQModule
#from daqconf.core.conf_utils import Endpoint, Direction

def get_ndreadoutmodules_app(nickname, num_dummymodules, some_configured_value, host="localhost"):
    """
    Here the configuration for an entire daq_application instance using DAQModules from ndreadoutmodules is generated.
    """

    modules = []

    for i in range(num_dummymodules):
        modules += [DAQModule(name = f"nickname{i}", 
                              plugin = "DummyModule", 
                              conf = dummymodule.Conf(some_configured_value = some_configured_value
                                )
                    )]

    mgraph = ModuleGraph(modules)
    ndreadoutmodules_app = App(modulegraph = mgraph, host = host, name = nickname)

    return ndreadoutmodules_app

/*
    Copyright (c) 2002-2009 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file DegisnSpaceExlorer.cc
 *
 * Implementation of explorer.
 *
 * The command line version of the Design Space Explorer.
 *
 * @author Jari M�ntyneva 2007 (jari.mantyneva-no.spam-tut.fi)
 * @author Esa M��tt� 2008 (esa.maatta-no.spam-tut.fi)
 * @note rating: red
 */


#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

#include "StringTools.hh"
#include "Application.hh"
#include "Exception.hh"
#include "FileSystem.hh"
#include "DesignSpaceExplorer.hh"
#include "DesignSpaceExplorerPlugin.hh"
#include "ExplorerCmdLineOptions.hh"
#include "DSDBManager.hh"
#include "HDBRegistry.hh"
#include "MachineImplementation.hh"

#include "ExplorerPluginParameter.hh"
#include "Environment.hh"

using std::vector;
using std::cout;
using std::cerr;
using std::endl;
using std::string;

/**
 * Loads the output Design Space Database file and creates a DSDB from it.
 * 
 * If the given filename doesn't exists creates a new DSDB file whit the given
 * name.
 * 
 * @param dsdbFile The DSDB file name.
 * @return DSDBManager of the DSDB file.
 */
DSDBManager*
loadDSDB(const std::string& dsdbFile) 
    throw (IOException) {

    if (FileSystem::fileExists(dsdbFile)) {
        return new DSDBManager(dsdbFile);
    } else {
        return DSDBManager::createNew(dsdbFile);
    }
}

/**
 * Parses the given parameter which has form 'paramname=paramvalue" to
 * different strings.
 *
 * @param param The parameter.
 * @param paramName Parameter name is stored here.
 * @param paramValue Parameter value is stored here.
 * @exception InvalidData If the given parameter is not in the correct form.
 */
void
parseParameter(
    const std::string& param,
    std::string& paramName,
    std::string& paramValue)
    throw (InvalidData) {

    string::size_type separatorPos = param.find("=", 0);
    if (separatorPos == string::npos) {
        string errorMsg = 
            "Explorer plugin parameters must be in form "
            "'parametername=parametervalue'.";
        throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
    }

    paramName = param.substr(0, separatorPos);
    paramValue = param.substr(separatorPos+1, param.length());
}

/**
 * Determines the widest column so that column widths can be tuned.
 *
 * Lengths of the data values are returned through the parameters.
 *
 * @param confs Configuration costs which lengths are checked.
 * @configurationID Length of the configuration ID string.
 * @applicationPath Length of the path string of an application.
 * @cycleCount Length of the program cycle count string.
 * @energyEstimate Length of the programs energy estimate string.
 */
void
determineLongest(
    const vector<DSDBManager::ConfigurationCosts>& confs, int& configurationID,
    int& applicationPath, int& cycleCount, int& energyEstimate) {

    configurationID = 0;
    applicationPath = 0;
    cycleCount = 0;
    energyEstimate = 0;
    for (unsigned int i = 0; i < confs.size(); i++) {
        int size = Conversion::toString(confs[i].configurationID).size();
        if (size > configurationID) {
            configurationID = size;
        }
        size = confs[i].application.size();
        if (size > applicationPath) {
            applicationPath = size;
        }
        size = Conversion::toString(confs[i].cycleCount).size();
        if (size > cycleCount) {
            cycleCount = size;
        }
        size = Conversion::toString(confs[i].energyEstimate).size();
        if (size > energyEstimate) {
            energyEstimate = size;
        }
    }
}

/**
 * Outputs given number of empty characters to cout.
 *
 * @param numberOfSpaces The number of empty characters to output.
 */
void
printSpaces(unsigned int numberOfSpaces) {
    while (numberOfSpaces != 0) {
        cout << " ";
        numberOfSpaces--;
    }
}

/**
 * Returns the ordering of data indicated by the string.
 *
 * @order Option string given by user that tells the ordering.
 * @return The ordering of the data.
 */
DSDBManager::Order orderingOfData(const string& order) {
    if (order == "I") {
        return DSDBManager::ORDER_BY_CONFIGURATION;
    } else if (order == "P") {
        return DSDBManager::ORDER_BY_APPLICATION;
    } else if (order == "C") {
        return DSDBManager::ORDER_BY_CYCLE_COUNT;
    } else if (order == "E") {
        return DSDBManager::ORDER_BY_ENERGY_ESTIMATE;
    } else {
        return DSDBManager::ORDER_BY_CONFIGURATION;
    }
}

/**
 * Parses parameters given from command line and passes them to the plugin.
 *
 * @param plugin Explorer plugin that receives the parameters.
 * @param options Explorer command line options where parameters are read.
 */
bool
loadPluginParameters(
    DesignSpaceExplorerPlugin* plugin, 
    const ExplorerCmdLineOptions& options) {

    // Check the parameters to be passed to the explorer plugin.
    for (int i = 0; i < options.explorerPluginParameterCount(); i++) {
        string param = options.explorerPluginParameter(i);
        string paramName;
        string paramValue;
        try {
            parseParameter(param, paramName, paramValue);
            plugin->giveParameter(paramName, paramValue);
        } catch (const Exception& e) {
            std::cerr << e.errorMessage() << std::endl;
            return false;
        }
    }
    return true;
}


/**
 * Loads explorer plugin.
 *
 * @param plugin Explorer plugin name to be loaded.
 * @param dsdb DSDB that plugin is associated with.
 * @return explorer plugin as a pointer.
 */
DesignSpaceExplorerPlugin*
loadExplorerPlugin(const std::string& plugin, DSDBManager* dsdb) {
    
    // Try to load the explorer plugin.
    DesignSpaceExplorerPlugin* explorer = NULL;
    try {
        explorer = DesignSpaceExplorer::loadExplorerPlugin(plugin, dsdb);
    } catch (const FileNotFound& e) {
        std::cerr << "No explorer plugin file named '" << plugin
                  << ".so' found."  << std::endl;
        delete dsdb;
        return NULL;
    } catch (const Exception& e) {
        std::string msg = "Error while trying to load the explorer plugin "
            "named '" + plugin + ".so'.";
        verboseLog(msg)
        msg = "With reason: " + e.errorMessage();
        verboseLogC(msg, 1)
        delete dsdb;
        delete explorer;
        return NULL;
    }
    return explorer;
}


/**
 * Prints explorer plugins and their descriptions.
 */
void
printPlugins() {
    vector<string> found_plugins;
    vector<string> searchPaths = Environment::explorerPluginPaths();
    for (vector<string>::const_iterator iter = searchPaths.begin();
            iter != searchPaths.end(); iter++) {

        if (FileSystem::fileExists(*iter)) {
            // now list all files with postfix ".so"
            verboseLogC("Fetching plugins from directory: " + *iter, 1)
            FileSystem::findFromDirectory(".*\\.so$", *iter, found_plugins);
        }
    }
    cout << "| Plugin name                 | Description " << endl
         << "--------------------------------------------" << endl;
    cout.flags(std::ios::left);
    for (unsigned int i = 0; i < found_plugins.size(); ++i) {
        std::string pluginName = FileSystem::fileNameBody(found_plugins[i]);
        DesignSpaceExplorerPlugin* plugin = loadExplorerPlugin(pluginName, NULL);
        if (!plugin) {
            return;
        }
        cout << std::setw(30) << pluginName << plugin->description() << endl;
    }
}


/**
 * Prints explorer plugin parameter info.
 *
 * @param plugin Explorer plugin which parameter info is to be printed.
 */
void
printPluginParamInfo(DesignSpaceExplorerPlugin& plugin) {
    using std::setw;
    DesignSpaceExplorerPlugin::ParameterMap pm = plugin.parameters();
    DesignSpaceExplorerPlugin::PMCIt it = pm.begin();
    cout << "| Parameter name |      Type       | Default value | Description " << endl
         << "-----------------------------------------------------------------" << endl;
    cout.flags(std::ios::left);
    int fw = 18;
    while (it != pm.end()) {
        cout << setw(fw) << it->first;
        switch (it->second.type()) {
            case UINT:
                cout << setw(fw) << "unsigned int"; 
                break;
            case INT:
                cout << setw(fw) << "int"; 
                break;
            case STRING:
                cout << setw(fw) << "string"; 
                break;
            case BOOL:
                cout << setw(fw) << "boolean"; 
                break;
            default:
                cout << setw(fw) << "unknown type"; 
        }
        if (!it->second.isCompulsory()) {
            if (it->second.type() == BOOL) {
                cout << setw(fw) << 
                    (it->second.value() == "1" || it->second.value() == "true"
                     ? "true" : "false"); 
            } else {
                cout << setw(fw) << it->second.value();
            }
        }
        cout << std::endl;
        ++it; 
    }
}


/**
 * Main function.
 *
 * Parses the command line and figures out what to do.
 *
 * @param argc The command line argument count.
 * @param argv The command line arguments (passed to the interpreter).
 * @return The return status.
 */
int main(int argc, char* argv[]) {

    Application::initialize();    

    // boolean to check if done something useful
    bool doneUseful = false;

    // Parses the command line options.
    ExplorerCmdLineOptions options;
    try {
        options.parse(argv, argc);
    } catch (ParserStopRequest) {
        return EXIT_SUCCESS;
    } catch (const IllegalCommandLine& i) {
        std::cerr << i.errorMessage() <<  std::endl;
        return EXIT_FAILURE;
    }

    int verboseLevel = options.verboseLevel();
    if (verboseLevel < 0) {
        Application::setVerboseLevel();
    } else {
        Application::setVerboseLevel(verboseLevel);
    }

    if (options.printPlugins()) {
        printPlugins();
        return EXIT_SUCCESS;
    }

    if (options.pluginInfo().length() != 0) {
        std::string plugin = options.pluginInfo();
        DesignSpaceExplorerPlugin* explorer = loadExplorerPlugin(plugin, NULL);
        if (!explorer) {
            return EXIT_FAILURE;
        }
        printPluginParamInfo(*explorer);
        delete explorer;
        explorer = NULL;
        return EXIT_SUCCESS;
    }

    // Only argument should be dsdb.
    if (options.numberOfArguments() != 1) {
        std::cerr << "Illegal number of arguments." << std::endl << std::endl;
        options.printHelp();
        return EXIT_FAILURE;
    }
    // Get the database file.
    std::string dsdbFile = "";
    dsdbFile = options.argument(1);

    // Loads the database.
    DSDBManager* dsdb;
    try {
        dsdb = loadDSDB(dsdbFile);
    } catch (const IOException& e) {
        std::cerr << e.errorMessage() << std::endl;
        return EXIT_FAILURE;
    }

    // adds new configuration to the DSDB
    if (options.adfFile()) {
        TTAMachine::Machine* adf = NULL;
        IDF::MachineImplementation* idf = NULL;
        try {
            DSDBManager::MachineConfiguration conf;
            conf.hasImplementation = false;
            adf = TTAMachine::Machine::loadFromADF(options.adfFileName());
            conf.architectureID = dsdb->addArchitecture(*adf);
            if (options.idfFile()) {
                idf = IDF::MachineImplementation::loadFromIDF(options.idfFileName());
                conf.implementationID = dsdb->addImplementation(*idf, 0, 0);
                conf.hasImplementation = true;
            }
            RowID confID = dsdb->addConfiguration(conf);
            std::cout << "Added configuration " << confID << " into the DSDB." << std::endl;
            delete adf;
            delete idf;
            doneUseful = true;
        } catch (const Exception& e) {
            std::cout << "Error occured reading ADF or IDF. " 
                      << e.errorMessage() << std::endl;
            delete dsdb;
            return EXIT_FAILURE;           
        }
    }

    // Check and add the test application directories.
    int testDirectories = options.testApplicationDirectoryCount();
    for (int i = 0; i < testDirectories; i++) {
        std::string testDir = options.testApplicationDirectory(i);
        if (FileSystem::fileExists(testDir) &&
                FileSystem::fileIsDirectory(testDir)) {
            if (!dsdb->hasApplication(testDir)) {
                dsdb->addApplication(testDir);
            }
        } else {
            std::cerr << "Application directory '" << testDir
                      << "' does not exists." << std::endl;
        }
        doneUseful = true;
    }

    // Prints the summary of the configurations in the database.
    if (options.printSummary()) {
        DSDBManager::Order ordering = orderingOfData(options.summaryOrdering());
        cout << "Configurations in DSDB: " << endl;
        int idLength = 0;
        int pathLength = 0;
        int cycleLength = 0;
        int energyLength = 0;
        cout << "| Conf ID | Application path | cycle count | energy estimate |"
             << endl;
        cout << "--------------------------------------------------------------"
             << endl;
        vector<DSDBManager::ConfigurationCosts> confCosts =
            dsdb->applicationCostEstimatesByConf(ordering);
        // Checks the longes strings of all data values that will be printed.
        // Values are used to create a clean output of the results.
        determineLongest(
            confCosts, idLength, pathLength, cycleLength, energyLength);
        for (unsigned int i = 0; i < confCosts.size(); i++) {
            cout << "| ";
            cout << confCosts[i].configurationID;
            printSpaces(
                idLength - Conversion::toString(
                    confCosts[i].configurationID).size());
            cout << " | ";
            cout << confCosts[i].application;
            printSpaces(
                pathLength - confCosts[i].application.size());
            cout << " | ";
            cout << confCosts[i].cycleCount;
            printSpaces(
                cycleLength - Conversion::toString(
                    confCosts[i].cycleCount).size());
            cout << " | ";
            cout << confCosts[i].energyEstimate;
            printSpaces(
                energyLength - Conversion::toString(
                    confCosts[i].energyEstimate).size());
            cout << " |" << endl;
        }
        cout << "--------------------------------------------------------------"
             << endl;
    }
    
    // Prints the total amount of configurations in the database.
    if (options.printSummary() || options.numberOfConfigurations()) {
        cout << "Total: " << dsdb->configurationIDs().size() 
             << " configurations "
             << "in the database." << endl;
        delete dsdb;
        return EXIT_SUCCESS;
    }

    // Write the configuration ADF and IDF to files.
    if (options.writeOutConfiguration()) {
        for (int i = 0; i < options.numberOfConfigurationsToWrite(); i++) {
            if (dsdb->hasConfiguration(options.configurationToWrite(i))) {
                DSDBManager::MachineConfiguration configuration = 
                    dsdb->configuration(options.configurationToWrite(i));
                std::string adfFileName = 
                    Conversion::toString(options.configurationToWrite(i))
                    + ".adf";
                try {
                    dsdb->writeArchitectureToFile(
                        configuration.architectureID, adfFileName);
                    std::cout << "Written ADF file of configuration "
                              << options.configurationToWrite(i)
                              << std::endl;
                } catch (const Exception& e) {
                    std::cerr << "Error occured while writing the ADF." 
                              << std::endl;
                    delete dsdb;
                    return EXIT_FAILURE;
                }

                if (configuration.hasImplementation) {
                    std::string idfFileName = 
                        Conversion::toString(options.configurationToWrite(i))
                        + ".idf";
                    try {
                        dsdb->writeImplementationToFile(
                            configuration.implementationID, idfFileName);
                        std::cout << "Written IDF file of configuration "
                                  << options.configurationToWrite(i)
                                  << std::endl;
                    } catch (const Exception& e) {
                        std::cerr << "Error occured while writing the IDF."
                                  << std::endl;
                        delete dsdb;
                        return EXIT_FAILURE;
                    }
                }
            } else {
                // dsdb didn't have requested configuration
                std::cerr << "No configuration found with id: " 
                          << options.configurationToWrite(i) << "." 
                          << std::endl;
                delete dsdb;
                return EXIT_FAILURE;
            }
        }
        delete dsdb;
        return EXIT_SUCCESS;
    }
    
    // Remove applications from DSDB if requested
    if (options.applicationIDToRemoveCount() > 0) {
        for (int i = 0; i < options.applicationIDToRemoveCount(); i++) {
            RowID id = options.applicationIDToRemove(i);
            try {
                dsdb->removeApplication(id);
            } catch (const KeyNotFound&) {
                std::cerr << "No application with ID: " << id << " in DSDB."
                          << std::endl;
            }
        }
        return EXIT_SUCCESS;
    }

    if (testDirectories < 1 && !dsdb->applicationCount()) {
        // used plugin may not need a test application, or the app is
        // provided to the plugin with its own parameter
        std::cout << "Warning: No test application paths given or found in "
                     "dsdb."
                  << std::endl;
    }

    // If the list applications option is given.
    if (options.printApplications()) {
        std::cout << "Applications in the DSDB:" << std::endl;
        std::cout << " ID | Application" << std::endl;
        std::cout << "---------------------------------" << std::endl;
        std::set<RowID> appIDs = dsdb->applicationIDs();
        std::set<RowID>::const_iterator appIter = appIDs.begin();
        for (; appIter != appIDs.end(); appIter++) {
            std::cout << " " << (*appIter) << " | " 
                      << dsdb->applicationPath(*appIter)
                      << std::endl;
        }
        std::cout << "---------------------------------" << std::endl;
        std::cout << "Total: " << dsdb->applicationCount() 
                  << " applications in DSDB." << std::endl;
        return EXIT_SUCCESS;
    }

    // Check the explorer plugin.
    std::string pluginToUse = "";
    pluginToUse = options.explorerPlugin();
    if (pluginToUse == "") {
        if (!doneUseful) {
            std::cerr << "No explorer plugin given." << std::endl;
            return EXIT_FAILURE;
        } else {
            return EXIT_SUCCESS;
        }
    }
    
    // Try to load the explorer plugin.
    DesignSpaceExplorerPlugin* explorer = loadExplorerPlugin(pluginToUse, dsdb);
    if (!explorer || !loadPluginParameters(explorer, options)) {
        return EXIT_FAILURE;
    }
    
    // Load the HDB files if given as option
    if (options.hdbFileNames()) {
        for (int i = 0; i < options.hdbFileNameCount(); i++) {
            try {
                HDB::HDBRegistry::instance().hdb(options.hdbFileName(i));
            } catch (const FileNotFound& e) {
                std::cerr << "Could not find HDB file " 
                          << options.hdbFileName(i) << std::endl;
            }
        }
    }

    try {
        RowID startPointConfigurationID = options.startConfiguration();        
        vector<RowID> result =
            explorer->explore(startPointConfigurationID);
        if (result.empty()) {
            cout << "No new configurations could be created." << endl;
        } else {
            std::cout << "Best result configurations:" << std::endl;
            for (unsigned int i = 0; i < result.size(); i++) {
                cout << " " << result[i] << endl;
            }
        }
    } catch (const Exception& e) {
        std::cerr << e.errorMessage()
                  << " " << e.fileName()
                  << " " << e.lineNum() << std::endl;
        delete dsdb;
        delete explorer;
        return EXIT_FAILURE;
    }

    delete dsdb;
    delete explorer;
    return EXIT_SUCCESS;
}

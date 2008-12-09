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
 * @file BaseSchedulerModule.cc
 *
 * Implementation of BaseSchedulerModule class.
 *
 * @author Ari Mets�halme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "Application.hh"
#include "BaseSchedulerModule.hh"

using std::string;

/**
 * Constructor.
 */
BaseSchedulerModule::BaseSchedulerModule():
    target_(NULL), program_(NULL), loader_(NULL), interPassData_(NULL) {
}

/**
 * Destructor.
 */
BaseSchedulerModule::~BaseSchedulerModule() {
}

/**
 * Return true if the module needs a machine object model of the
 * target processor.
 *
 * @return True if the module needs a machine object model of the
 * target processor.
 */
bool
BaseSchedulerModule::needsTarget() const {
    return false;
}

/**
 * Register the machine object model of the target processor to the module.
 *
 * @param target The target.
 */
void
BaseSchedulerModule::setTarget(const TTAMachine::Machine& target) {
    target_ = &target;
}

/**
 * Return true if the module needs the source program object model.
 *
 * @return True if the module needs the source program object model.
 */
bool
BaseSchedulerModule::needsProgram() const {
    return false;
}

/**
 * Register the program object model of the source program to the module.
 *
 * @param program The program.
 */
void
BaseSchedulerModule::setProgram(TTAProgram::Program& program) {
    program_ = &program;
}

/**
 * Return true if the module needs the source program in the
 * scheduler-specific program representation.
 *
 * @return True if the module needs the source program in the
 * scheduler-specific program representation.
 */
bool
BaseSchedulerModule::needsProgramRepresentation() const {
    return false;
}

/**
 * Return true if the module needs the plugin loader.
 *
 * @return True if the module needs the plugin loader for loading its helper
 * plugin modules.
 */
bool
BaseSchedulerModule::needsPluginLoader() const {
    return false;
}

/**
 * Register the plugin loader to the module.
 *
 * @param loader The plugin loader.
 */
void
BaseSchedulerModule::setPluginLoader(const SchedulerPluginLoader& loader) {
    loader_ = &loader;
}

/**
 * Register given module as a helper module.
 *
 * This method is normally invoked by the Scheduler front-end on a
 * startable module, but it could be recursively invoked by a module
 * on its helper modules. Base implementation assumes that the module
 * doesn't require helper modules. Each module that needs helpers should
 * reimplement this method.
 *
 * @exception IllegalRegistration If the given module is not a helper
 *            module of correct type, the module does not require helper
 *            modules or some other occurs while registering.
 */
void
BaseSchedulerModule::registerHelperModule(HelperSchedulerModule&)
    throw (IllegalRegistration) {
    string method = "BaseSchedulerModule::registerHelperModule()";
    string msg = "Module doesn't require helper modules.";
    throw IllegalRegistration(__FILE__, __LINE__, method, msg);
}

/**
 * Sets the object that containts the inter-pass data passed between
 * schedule passes.
 *
 * @param data The inter-pass data.
 */
void
BaseSchedulerModule::setInterPassData(InterPassData& data) {
    interPassData_ = &data;
}

/**
 * Returns the inter-pass data container that can be used by the pass.
 *
 * The object is a generic container for data passed between
 * scheduler passes.
 *
 * @param data The data object.
 */
InterPassData&
BaseSchedulerModule::interPassData() {
    assert(interPassData_ != NULL);
    return *interPassData_;
}

/**
 * Gives the options specified in the config file to the pass
 *
 * @param options vector of all parameters given to the scheduler pass.
 */
void
BaseSchedulerModule::setOptions(const std::vector<ObjectState*>&) {
}

/**
 * A short description of the module, usually the module name,
 * such as "LinearScanRegisterAllocator".
 *
 * @return The description as a string.
 */   
std::string
BaseSchedulerModule::shortDescription() const {
    return "";
}

/**
 * Optional longer description of the Module.
 *
 * This description can include usage instructions, details of choice of
 * helper modules, etc.
 *
 * @return The description as a string.
 */
std::string
BaseSchedulerModule::longDescription() const {
    return "";
}

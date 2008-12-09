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
 * @file FUExternalPort.cc
 *
 * Implementation of FUExternalPort class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "FUExternalPort.hh"
#include "FUImplementation.hh"
#include "ContainerTools.hh"

namespace HDB {

/**
 * The constructor.
 *
 * Registers the port automatically to the given FUImplementation instance.
 *
 * @param name Name of the port.
 * @param direction Direction of the port.
 * @param widthFormula The formula for the width of the port.
 * @param description Description of the port.
 * @param parent The parent FUImplementation instance.
 */
FUExternalPort::FUExternalPort(
    const std::string& name,
    Direction direction,
    const std::string& widthFormula,
    const std::string& description,
    FUImplementation& parent) :
    name_(name), direction_(direction), widthFormula_(widthFormula), 
    description_(description) {

    parent.addExternalPort(this);
}


/**
 * The destructor.
 */
FUExternalPort::~FUExternalPort() {
}


/**
 * Sets the name of the port.
 *
 * @param name Name of the port.
 */
void
FUExternalPort::setName(const std::string& name) {
    name_ = name;
}


/**
 * Returns the name of the port.
 *
 * @return The name of the port.
 */
std::string
FUExternalPort::name() const {
    return name_;
}


/**
 * Sets the direction of the port.
 *
 * @param direction The new direction.
 */
void
FUExternalPort::setDirection(Direction direction) {
    direction_ = direction;
}


/**
 * Returns the direction of the port.
 *
 * @return The direction of the port.
 */
Direction
FUExternalPort::direction() const {
    return direction_;
}


/**
 * Sets the width formula of the port.
 *
 * @param widthFormula The new width formula.
 */
void
FUExternalPort::setWidthFormula(const std::string& widthFormula) {
    widthFormula_ = widthFormula;
}


/**
 * Returns the formula for the width of the port.
 *
 * @return The formula.
 */
std::string
FUExternalPort::widthFormula() const {
    return widthFormula_;
}


/**
 * Sets description of the port.
 *
 * @param description The new description.
 */
void
FUExternalPort::setDescription(const std::string& description) {
    description_ = description;
}


/**
 * Returns the description of the port.
 *
 * @return The description.
 */
std::string
FUExternalPort::description() const {
    return description_;
}


/**
 * Sets a parameter dependency for the port.
 *
 * @param parameter Name of the parameter the port is dependent on.
 * @return True if parameter dependency was added, false if already existed.
 */
bool
FUExternalPort::setParameterDependency(const std::string& parameter) {
    if (!ContainerTools::containsValue(parameterDeps_, parameter)) {
        parameterDeps_.push_back(parameter);
        return true;
    }
    return false;
}


/**
 * Unsets the dependency of the given parameter.
 *
 * @param parameter The parameter.
 * @return True if parameter dependency was removed, false if it didn't exist.
 */
bool
FUExternalPort::unsetParameterDependency(const std::string& parameter) {
    return ContainerTools::removeValueIfExists(parameterDeps_, parameter);
}


/**
 * Returns the number of parameters the port is dependent on.
 *
 * @return The number of parameters.
 */
int
FUExternalPort::parameterDependencyCount() const {
    return parameterDeps_.size();
}


/**
 * Returns name of a parameter the port is dependent on.
 *
 * @param index Determines which parameter is returned.
 * @return The name of the parameter.
 * @exception OutOfRange If the index is negative or not smaller than the
 *                       number of parameters the port is dependent on.
 */
std::string
FUExternalPort::parameterDependency(int index) const 
    throw (OutOfRange) {

    if (index < 0 || index >= parameterDependencyCount()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }
    return parameterDeps_[index];
}
}

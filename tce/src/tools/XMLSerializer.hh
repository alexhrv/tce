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
 * @file XMLSerializer.hh
 *
 * Declaration of XMLSerializer class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 8 Jun 2004 by tr, jm, am, ll
 * @note rating: red
 */

#ifndef TTA_XML_SERIALIZER_HH
#define TTA_XML_SERIALIZER_HH

#include <string>

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMBuilder.hpp>

#include "Serializable.hh"
#include "ObjectState.hh"
#include "Serializer.hh"
#include "Exception.hh"

#if _XERCES_VERSION >= 20200
XERCES_CPP_NAMESPACE_USE
#endif


/**
 * This class is used to read and write XML. This is a base class for
 * different kind of XML serializers.
 */
class XMLSerializer : public Serializer {
public:
    XMLSerializer();
    virtual ~XMLSerializer();

    void setSourceFile(const std::string& fileName);
    void setSourceString(const std::string& source);

    void setDestinationFile(const std::string& fileName);
    void setDestinationString(std::string& destination);

    void setSchemaFile(const std::string& fileName);
    void setUseSchema(bool useSchema);

    virtual ObjectState* readState()
        throw (SerializerException);

    virtual void writeState(const ObjectState* rootState)
        throw (SerializerException);

protected:
    std::string sourceFile() const;

private:
    /// Copying forbidden.
    XMLSerializer(const XMLSerializer&);
    /// Assignment forbidden.
    XMLSerializer& operator=(const XMLSerializer&);

    void initializeParser();

    virtual ObjectState* readFile(const std::string& fileName)
        throw (SerializerException);

    virtual ObjectState* readString(const std::string& source)
        throw (SerializerException);

    virtual void writeFile(
        const std::string& fileName, const ObjectState* rootState)
        throw (SerializerException);

    virtual void writeString(
        std::string& target, const ObjectState* rootState)
        throw (SerializerException);

    DOMDocument* createDOMDocument(const ObjectState* state) const;
    void ensureValidStream(const std::string& fileName) const
        throw (UnreachableStream);

    DOMElement* createDOM(
        ObjectState* state,
        DOMDocument* doc) const;
    ObjectState* createState(const DOMNode* node) const;
    bool hasChildElementNodes(const DOMNode* node) const;

    /// Source file path.
    std::string sourceFile_;
    /// Destination file path.
    std::string destinationFile_;
    /// Schema file path.
    std::string schemaFile_;
    /// Indicates if xml file is validated using schema.
    bool useSchema_;
    /// The parser that checks the XML file for errors with the Schema.
    DOMBuilder* parser_;
    /// Implementation of the DOM.
    DOMImplementation* domImplementation_;

    /// Source string to read.
    const std::string* sourceString_;
    /// Destination string to write.
    std::string* destinationString_;
};

#endif


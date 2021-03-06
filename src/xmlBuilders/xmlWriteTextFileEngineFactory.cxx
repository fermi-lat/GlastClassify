/**@file xmlWriteTextFileEngineFactory.cxx

@brief implementation of class xmlWriteTextFileEngineFactory

$Header: /nfs/slac/g/glast/ground/cvs/GlastClassify/src/xmlBuilders/xmlWriteTextFileEngineFactory.cxx,v 1.3 2006/06/30 18:06:31 usher Exp $
*/

#include "xmlWriteTextFileEngineFactory.h"
#include "../ImActivityNodes/WriteTextFileEngineNode.h"
#include "xmlBase/XmlParser.h"
#include "facilities/Util.h"
#include <xercesc/dom/DOMElement.hpp>
#include "xmlBase/Dom.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <set> 
#include <cmath>  // for M_PI, among others

XERCES_CPP_NAMESPACE_USE


namespace {

    // output errors to console. (note: m_ prefex for data members )
    // only for statistics to show in debug output
    int  nesting_level, max_depth, node_count;  
    double min_prob, max_prob;
} // anonomous namespace

xmlWriteTextFileEngineFactory::xmlWriteTextFileEngineFactory(XTExprsnParser& parser) : xmlFactoryBase(parser)
{
}

IImActivityNode* xmlWriteTextFileEngineFactory::operator()(const DOMElement* xmlActivityNode)
{
    // Retrieve name and node id
    DOMElement* displayInfo = xmlBase::Dom::findFirstChildByName(xmlActivityNode, "DisplayInfo");
    std::string sType       = "WriteTextFileEngineNode";
    std::string sName       = xmlBase::Dom::getAttribute(displayInfo, "labelText");
    std::string sId         = xmlBase::Dom::getAttribute(xmlActivityNode, "id");

    // Create the node
    WriteTextFileEngineNode* node = new WriteTextFileEngineNode(sType, sName, sId);

    // Set up to be able to output the ntuple
    std::string          sVarName     = "WriteTupleRow";
    XTcolumnValBase*     basePtr      = XprsnParser().getXtTupleVars()[sVarName];
    XTcolumnVal<REALNUM>* xtColumnVal = dynamic_cast<XTcolumnVal<REALNUM>*>(basePtr);

    if (xtColumnVal == 0)
    {
        xtColumnVal = new XTcolumnVal<REALNUM>(sVarName);
        XprsnParser().getXtTupleVars()[sVarName] = xtColumnVal;
    }

    node->setXtColumnVal(xtColumnVal);

    return node;
}

xmlWriteTextFileEngineFactory::~xmlWriteTextFileEngineFactory()
{
    ///@TODO: delete trees
}

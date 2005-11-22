/**@file xmlTreeFactory.cxx

@brief implementation of class xmlTreeFactory

$Header: /nfs/slac/g/glast/ground/cvs/GlastClassify/src/xmlTreeFactory.cxx,v 1.5 2005/11/09 00:10:53 usher Exp $
*/

#include "GlastClassify/xmlTreeFactory.h"
#include "classifier/DecisionTree.h"
#include "ImActivityNodes/PredictEngineNode.h"
#include "xmlBuilders/ImSheetBuilder.h"
#include <fstream>
#include <cassert>
#include <stdexcept>
#include <algorithm>
#include <cmath>  // for M_PI, among others

#include "xmlBase/XmlParser.h"
#include <xercesc/dom/DOMElement.hpp>
#include "xmlBase/Dom.h"

using namespace GlastClassify;
XERCES_CPP_NAMESPACE_USE


/** @class GleamValues
@brief local definition of class which handles pointers to values
       NOTE: this needs modification - separate out into own class
       (in progress for next check in)
*/
class xmlTreeFactory::GleamValues : public DecisionTree::Values 
{
public:
    GleamValues(const ImSheetBuilder::StringList& names, 
                const xmlTreeFactory::LocalTupleValues& localVals,
                const std::map<std::string,std::string>& imToTobyMap,
                ITupleInterface&  tuple) :
                m_localVals(localVals)
    {
        // Loop over list of variable names given to us in names
        for( ImSheetBuilder::StringList::const_iterator it = names.begin();
            it != names.end(); ++it)
        {
            std::string varName = *it;

            // Does this variable need to have its name changed?
            if (imToTobyMap.find(varName) != imToTobyMap.end())
            {
                varName = imToTobyMap.find(varName)->second;
            }

            // Use a "try" to catch the exception if not in the tuple
            try
            {
                const Item* item = tuple.getItem(varName);
                if( item==0)
                {
                    throw std::invalid_argument("TreeFactory::GleamValues: did not find variable "+varName);
                }

                m_varAction.push_back(std::pair<std::string, bool>(varName, true));
                m_pval.push_back(std::pair<bool, const Item*>(true,item));
            }
            catch (std::invalid_argument&)
            {
                // Is this value a valid variable to be computed?
                if (localVals.isValue(varName))
                {
                    //Need a place holder for indexing to work
                    std::pair<bool, const Item*> entry(false, (const Item*)0);
                    
                    m_varAction.push_back(std::pair<std::string, bool>(varName, false));
                    m_pval.push_back(entry);
                }
                // Otherwise, an error
                else
                {
                    int j = 0;
                    //throw std::invalid_argument("xmlTreeFactory: did not find variable "+*it);
                }
            }
        }

    }
    /// @brief callback from tree evaluation

    double operator[](int index)const
    {
        double val = 0;
        const std::pair<std::string, bool>& varActionPair = m_varAction[index];

        if (varActionPair.second)
        {
            val = *((m_pval[index]).second);
        }
        else
        {
            val = m_localVals.getValue(varActionPair.first);
        }

        return val;
    }
    void print(std::ostream& out=std::cout)const
    {
        for(std::vector<std::pair<std::string, bool> >::const_iterator varIter = m_varAction.begin();
            varIter != m_varAction.end(); varIter++)
        {
            out << (*varIter).first << std::endl;
        }
    }
private:
    std::vector<std::pair<std::string, bool> > m_varAction;
    std::vector<std::pair<bool, const Item*> > m_pval;
    const xmlTreeFactory::LocalTupleValues&    m_localVals;
};


xmlTreeFactory::xmlTreeFactory(const std::string& path, ITupleInterface& tuple)
                               : m_lookup(tuple), 
                                 m_localVals(tuple),
                                 m_xtTupleMap()
{
    std::string sFileName = path+"/"+"DC2_Analysis.imw";
    
    xmlBase::XmlParser xmlParser;

    m_domDocument = xmlParser.parse(sFileName.c_str());
    
    if(m_domDocument == 0)
    {
        // Error checking usually for a missing file.
        // Or sometimes due to a bad XML document.
        // Remember <P><BR></P> is only valid
        // in poorly formed html, not xml.
        // When we get a schema for UserLibrary, we'll
        // be able to use that as a validator also.
        std::invalid_argument("xmlTreeFactory: could no open input file: " + sFileName);
    }

    // Testing
    m_xtTupleMap.clear();

    m_imSheet = new ImSheetBuilder(m_domDocument, m_xtTupleMap);

    // Set up cross reference to the ntuple
    for(XTcolumnVal<double>::XTtupleMap::iterator dataIter = m_xtTupleMap.begin(); 
        dataIter != m_xtTupleMap.end(); dataIter++)
    {
        const std::string& varName = dataIter->first;

        try
        {
            const GlastClassify::Item* item = m_lookup.getItem(varName);

            if (item != 0) m_nTupleMap[varName] = item;
        }
        catch (std::invalid_argument&)
        {
            int j = 0;
        }
    }

//    m_xtTupleVals.print();

    //Testing...
    std::ofstream outFile("IMsheetTest.txt");
    m_imSheet->print(outFile);
    outFile.close();

    // Make a map between the names Toby has for the trees, and Bill's names...
    m_TobyToBillMap.clear();

    m_TobyToBillMap["energy/param"]         = "CT: Energy Param";
    m_TobyToBillMap["energy/lastlayer"]     = "CT: Energy Last Layer";
    m_TobyToBillMap["energy/profile"]       = "CT: Energy Profile";
    m_TobyToBillMap["energy/tracker"]       = "CT: Energy Tracker";

    m_TobyToBillMap["vertex/thin"]          = "CT: PSF VTX/1Tkr Thin";
    m_TobyToBillMap["vertex/thick"]         = "CT: PSF VTX/1Tkr Thick";

    m_TobyToBillMap["psf/vertex/thin"]      = "CT: PSF CORE VTX Thin";  
    m_TobyToBillMap["psf/vertex/thick"]     = "CT: PSF CORE VTX Thick"; 
    m_TobyToBillMap["psf/track/thin"]       = "CT: PSF CORE 1TKR Thin"; 
    m_TobyToBillMap["psf/track/thick"]      = "CT:  PSF CORE 1TKR Thick";

    // Don't know what to do here but need at least a placeholder...
    m_TobyToBillMap["gamma/vertex/highcal"] = "CT: BKG VTX CAL-Med";
    m_TobyToBillMap["gamma/vertex/medcal"]  = "CT: BKG VTX CAL-Med";      
    m_TobyToBillMap["gamma/vertex/thin"]    = "CT: BKG VTX CAL-Low Thin  ";
    m_TobyToBillMap["gamma/vertex/thick"]   = "CT: BKG VTX CAL-Low Thick";
    m_TobyToBillMap["gamma/track/highcal"]  = "CT: BKG 1Tkr CAL-Hi CT ";      
    m_TobyToBillMap["gamma/track/medcal"]   = "CT: BKG 1Tkr CAL-Med";         
    m_TobyToBillMap["gamma/track/thin"]     = "CT: BKG 1Tkr CAL-Low Thin";    
    m_TobyToBillMap["gamma/track/thick"]    = "CT: BKG Bkg 1Tkr CAL-Low Thick";

    // Mapping between the CT output value names in Toby's scheme to IM's
    m_ImToTobyOutMap.clear();
    m_ImToTobyOutMap["Pr(GoodEnergy)"] = "CTgoodCal";
    m_ImToTobyOutMap["Pr(CORE)"]       = "CTgoodPsf";
    m_ImToTobyOutMap["Pr(VTX)"]        = "CTvertex";
    m_ImToTobyOutMap["Pr(GAM)"]        = "CTgamma";
}

const ITreeFactory::ITree& xmlTreeFactory::operator()(const std::string& name)
{
    std::string newName = m_TobyToBillMap[name];

    if (newName != "")
    {
        //std::string predEng = "PredictEngineNode";
        std::string predEng = "Toby";

        // Retrieve vector of PredictEngineNodes 
        std::vector<IImActivityNode*> nodeVec = m_imSheet->getActivityINodeVec(predEng);

        for (std::vector<IImActivityNode*>::iterator nodeIter = nodeVec.begin(); nodeIter != nodeVec.end(); nodeIter++)
        {
            IImActivityNode* iNode = *nodeIter;

            if (iNode->getName() != newName) continue;

            // Convert to a PredictEngineNode
            PredictEngineNode* node = dynamic_cast<PredictEngineNode*>(iNode);

            // Parse the desired tree from the xml document
            DecisionTree* tree = node->getDecisionTree();

            // Retrieve the list of variables used by this tree
            const ImSheetBuilder::StringList& tmpVarNames = node->getInputVarList();

            ImSheetBuilder::StringList& varNames = const_cast<ImSheetBuilder::StringList&>(tmpVarNames);

            GleamValues* gleamVals = new GleamValues(varNames, m_localVals, m_ImToTobyOutMap, m_lookup);

            m_trees.push_back(new Tree(tree, gleamVals));

            break;
        }
    }

    return *m_trees.back();
}

double xmlTreeFactory::getTupleVal(const std::string& name) 
{
    double value = 0;

    //XTcolumnVal<double>* xtValue = m_xtTupleVals.getColumnVal(name);
    //if (xtValue) value = *(*xtValue)();
        
    XTcolumnVal<double>::XTtupleMap::iterator dataIter = m_xtTupleMap.find(name);       
    if (dataIter != m_xtTupleMap.end()) value = *(*(dataIter->second))();

    return value;
}


void xmlTreeFactory::execute()
{
    // Transfer tuple variables to local tuple
    for(XTcolumnVal<double>::XTtupleMap::iterator dataIter = m_xtTupleMap.begin(); 
        dataIter != m_xtTupleMap.end(); dataIter++)
    {
        const std::string& varName = dataIter->first;

        std::map<std::string,const GlastClassify::Item*>::const_iterator nTupleIter = m_nTupleMap.find(varName);

        if (nTupleIter != m_nTupleMap.end()) dataIter->second->setDataValue(*(nTupleIter->second));
        else
        {
            dataIter->second->setDataValue(0.);
            dataIter->second->clearValidFlag();
        }
    }

    // Execute the sheet
    m_imSheet->getHeadNode()->execute();

    // Make a pass through to clear out the "invalid" variables
    for(XTcolumnVal<double>::XTtupleMap::iterator dataIter = m_xtTupleMap.begin(); 
        dataIter != m_xtTupleMap.end(); dataIter++)
    {
        if (!(dataIter->second->dataIsValid())) dataIter->second->setDataValue(0.);
    }

    // Done
    return;
}


double xmlTreeFactory::Tree::operator()()const
{
    return (*m_dt)(*m_vals);
}
std::string xmlTreeFactory::Tree::title()const
{
    return m_dt->title();
}
xmlTreeFactory::Tree::~Tree()
{
    delete m_dt;
    delete m_vals;
}
void xmlTreeFactory::Tree::printFile(std::string treeName) const
{
    std::string sFileName = treeName + ".txt";

    std::ofstream treeFile(sFileName.c_str());
    m_dt->print(treeFile);
    treeFile.close();

    sFileName = treeName + "_var.txt";
    std::ofstream varFile(sFileName.c_str());
    m_vals->print(varFile);
    varFile.close();

    return;
}

xmlTreeFactory::~xmlTreeFactory()
{
    ///@TODO: delete trees
}

xmlTreeFactory::LocalTupleValues::LocalTupleValues(ITupleInterface& lookup) 
{
    m_valsMap.clear();

    // Create the entries in the local variables map
    //m_valsMap["BestEnergy"]       = 0.;
    m_valsMap["EvtLogEnergyRaw"]  = 0.;
    m_valsMap["BestLogEnergy"]    = 0.;
    m_valsMap["BestEnergyProb"]   = 0.;
    m_valsMap["TkrEnergyFrac"]    = 0.;
    m_valsMap["TkrTotalHitsNorm"] = 0.;
    m_valsMap["TkrTotalHitRatio"] = 0.;
    m_valsMap["Tkr12DiffHits"]    = 0.;
    m_valsMap["TkrLATEdge"]       = 0.;
    m_valsMap["PSFEneProbPrd"]    = 0.;
    m_valsMap["CalMaxXtalRatio"]  = 0.;

    m_tupleVals["EvtEnergyCorr"]  = lookup.getItem("EvtEnergyCorr");
    m_tupleVals["TkrEnergyCorr"]  = lookup.getItem("EvtEnergyCorr");
    m_tupleVals["TkrTotalHits"]   = lookup.getItem("TkrTotalHits");
    m_tupleVals["Tkr1FirstLayer"] = lookup.getItem("Tkr1FirstLayer");
    m_tupleVals["Tkr1Hits"]       = lookup.getItem("Tkr1Hits");
    m_tupleVals["Tkr2Hits"]       = lookup.getItem("Tkr2Hits");
    m_tupleVals["Tkr1X0"]         = lookup.getItem("Tkr1X0");
    m_tupleVals["Tkr1Y0"]         = lookup.getItem("Tkr1Y0");
    m_tupleVals["CTgoodPsf"]      = lookup.getItem("CTgoodPsf");
    m_tupleVals["CalXtalMaxEne"]  = lookup.getItem("CalXtalMaxEne");
    m_tupleVals["CalEnergyRaw"]   = lookup.getItem("CalEnergyRaw");
}

double xmlTreeFactory::LocalTupleValues::getValue(const std::string& name) const
{
    double value = 0.;

    // I would prefer not to do it his way but this is expedient for now...
    //BestEnergy
    if (name == "BestEnergy")
    {
        value = *(m_tupleVals.find("EvtEnergyCorr")->second);

        // Here is what we are supposed to do... currently can't?
        //BestEnergy = ifelse(ProfileProb > ParamProb & ProfileProb > LastLayerProb & 
        //                    ProfileProb > TrackerProb,  CalCfpEnergy, BestEnergy)
    }
    // EvtLogEnergyRaw here
    else if (name == "EvtLogEnergyRaw")
    {
        double CalEnergy = *(m_tupleVals.find("EvtEnergyCorr")->second);
        value = log(CalEnergy) / 2.3026;
    }
    //BestLogEnergy
    else if (name == "BestLogEnergy")
    {
        std::string sBestEnergy = "BestEnergy";
        double BestEnergy = getValue(sBestEnergy);
        value = log(BestEnergy) / 2.3026;
    }
    //BestEnergyProb
    else if (name == "BestEnergyProb")
    {
        value = *(m_tupleVals.find("EvtEnergyCorr")->second);
    }

    //TkrEnergyFrac 
    else if (name == "TkrEnergyFrac")
    {
        value = *(m_tupleVals.find("TkrEnergyCorr")->second) / *(m_tupleVals.find("EvtEnergyCorr")->second);
    }

    //TkrTotalHitsNorm
    else if (name == "TkrTotalHitsNorm")
    {
        value = *(m_tupleVals.find("TkrTotalHits")->second) / sqrt(*(m_tupleVals.find("Tkr1FirstLayer")->second) - 1);
    }

    //TkrTotalHitRatio
    else if (name == "TkrTotalHitRatio")
    {
        value = (*(m_tupleVals.find("Tkr1Hits")->second) + *(m_tupleVals.find("Tkr2Hits")->second))/ *(m_tupleVals.find("TkrTotalHits")->second);
    }

    //Tkr12DiffHits
    else if (name == "Tkr12DiffHits")
    {
        value = *(m_tupleVals.find("Tkr1Hits")->second) - *(m_tupleVals.find("Tkr2Hits")->second);
    }

    //TkrLATEdge
    else if (name == "TkrLATEdge")
    {
        value = 740. - std::max(fabs(*(m_tupleVals.find("Tkr1X0")->second)), fabs(*(m_tupleVals.find("Tkr1Y0")->second)));
    }

    //PSFEneProbPrd
    else if (name == "PSFEneProbPrd")
    {
        double      CTgoodPsf       = *(m_tupleVals.find("CTgoodPsf")->second);
        std::string sBestEnergyProb = "BestEnergyProb";
        double      BestEnergyProb  = getValue(sBestEnergyProb);
        value = sqrt(BestEnergyProb*BestEnergyProb + CTgoodPsf*CTgoodPsf);
    }

    //CalMaxXtalRatio
    else if (name == "CalMaxXtalRatio")
    {
        value = *(m_tupleVals.find("CalXtalMaxEne")->second) / *(m_tupleVals.find("CalEnergyRaw")->second);
    }

    return value;
}


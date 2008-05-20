/** @file AtwoodTrees.cxx
@brief Implement tree definition and evaluation 

$Header: /nfs/slac/g/glast/ground/cvs/GlastClassify/src/AtwoodTrees.cxx,v 1.44 2008/03/21 22:19:26 usher Exp $

*/
#include "AtwoodTrees.h"
#include "TreeAnalysis.h"
#include "xmlBuilders/xmlTreeAnalysisFactory.h"

#include "classifier/DecisionTree.h"
#include "facilities/Util.h" // for expandEnvVar    


#include <sstream>
#include <cassert>

namespace {
    // This should be changed each time there is a new file, to make it defautl
    std::string default_xml("$(GLASTCLASSIFYROOT)/xml/Pass6_Analysis_Complete_RRmod_3_NB.xml");
}
/* 
*/
using namespace GlastClassify;

//_________________________________________________________________________

/** @page meritTuple_CTB_variables
@section ctbVars CTB Variables
This list of CTB variables is up-to-date as of GlastClassify v6r2p1, April 30, 2008.
We will <em>attempt</em> to keep it that way, but it's not yet guaranteed, since the
list is generated by hand from an IM worksheet.<br><br>
<table>
 <tr><td> CTBBestEnergy
 <td>F<td> analysis choice of the best estimate of the energy from 
           EvtEnergyCorr, CalCfpCalEnergy and CalLkHdEnergy
 <tr><td> CTBBestLogEnergy
 <td>F<td> log(base10) of Best Energy: log(CTBBestEnergy)/2.3026
 <tr><td> CTBBestEnergyRatio
 <td>F<td> CTBBestEnergy/max(1., CalEnergyRaw)
 <tr><td> CTBProfEnergyRatio
 <td>F<td> CalCfpEnergy/max(1., CalEnergyRaw)
 <tr><td> CTBLkhdEnergyRatio
 <td>F<td> CalLkHdEnergy/max(1.,CalEnergyRaw)
 <tr><td> CTBCalPSFErr
 <td>F<td> acos(-McXDir*CalXDir-McYDir*CalYDir-McZDir*CalZDir)
 <tr><td> CTBCalFrontBackRatio
 <td>F<td> (CalELayer0+CalELayer1+CalELayer+CalELayer3)/
           <br>max((CalELayer4+CalELayer5+CalELayer6+CalELayer7), 1)
 <tr><td> CTBTkrCoreCalDoca
 <td>F<td> CalTrackDoca - 2.5*Tkr1CoreHC
 <tr><td> CTBCalDocaAngle
 <td>F<td> CalTrackDoca + 80*CalTrackAngle
 <tr><td> CTBCalTransTCCD
 <td>F<td> CalTransRms + .1*(CalTrackDoca - 2.5*Tkr1CoreHC)
 <tr><td> CTBDiffCalDOCACalXtalRms
 <td>F<td> CalTrackDoca - CalTrkXtalRmsETrunc
 <tr><td> CTBTkrEnergyFrac
 <td>F<td> TkrEnergyCorr/max(1., EvtEnergyCorr)
 <tr><td> CTBTkrSHRCalAngle
 <td>F<td> CalTrackAngle - .2*TkrSurplusHitRatio
 <tr><td> CTBVtxTkr1Angle
 <td>F<td> acos(min((1.,VtxXDir*Tkr1XDir+VtxYDir*Tkr1YDir+VtxZDir*Tkr1ZDir))
 <tr><td> CTBBest[X/Y/Z]Dir
 <td>F<td> analysis choice for best direction estimate: 
           <br> [x/y/z] direction cosine
 <tr><td> CTBBestS[XX/YY/XY]
 <td>F<td> analysis choice for best direction covariance matrix: 
           <br>[XX/YY/XY] slope element
 <tr><td> CTBBestPSFCat
 <td>F<td> analysis choice for best direction catagory: 
           <br>Vtx, 1Tkr, VtxNeut, VtxNeut1, CalDir
 <tr><td> CTBBestPSFErr
 <td>F<td> angular error of best solution calculated using MC truth
 <tr><td> CTBCORE
 <td>F<td> CT based variable giving the probability that the direction solutions is good
 <tr><td> CTBCPFGamProb
 <td>F<td> if > 0, probability the event is a gamma using ACD + TKR variables. 
           <br>if == -1, events fails one or more prefilter ACD Cuts.
 <tr><td> CTBCPFCornerVeto 
 <td>F<td> CPF prefilter cut to remove events entering through ACD corner  gaps: 
           <br>ifelse(((Tkr1LATEdge/1.5)^2 + (AcdCornerDocaENorm-10)^2<6400) 
           <br>&nbsp;&nbsp;& Tkr1SSDVeto<3, 
           <br>&nbsp;&nbsp;-1., 1.) 
 <tr><td> CTBTKRISVeto
 <td>F<td> Tracker Interupted Shower Veto:
           <br>ifelse(AcdTileEventEnergyRatio > max(.003, (6-TkrUpstreamHC)*.006) 
           <br>&nbsp;&nbsp;& AcdTileEventEnergyRatio>(-.015-.00002*AcdTkr1ActiveDistENorm) 
           <br>&nbsp;&nbsp;& TkrUpstreamHC>0,
           <br>&nbsp;&nbsp;-1., 1.)
 <tr><td> CTBTKRHRVeto
 <td>F<td> Tracker heavy ion & range-out veto:
           <br>ifelse((Tkr1ToTFirst < .2 
           <br>&nbsp;&nbsp;& CTBBestEnergy<25000) 
           <br>&nbsp;&nbsp;| Tkr1ToTFirst*2.5/CTBBestLogEnergy>6.5, 
           <br>&nbsp;&nbsp;-1., 1.)
 <tr><td> CTBTKRGamProb
 <td>F<td> if > 0, probability event is a gamma using TKR variables. 
           <br>if < 0, event fails Tracker prefilters. 
           <br> <br>Note that two TKR prefilters were separated out (see above 2 variables)
 <tr><td> CTBCalMaxXtalRatio
 <td>F<td> CalXtalMaxEne/max(1., CalEnergyRaw)
 <tr><td> CTBCALSeal
 <td>F<td> Veto for particles enter the LAT from the CAL side. 
           Compares energies in layers 6 & 7 of the CAL. 
           Also includes particles entering CAL sides by considering 
           energy deposit near CAL edges (sides) 
 <tr><td> CTBCALGamProb
 <td>F<td> if > 0, probability event is a gamma using TKR & CAL variables
           <br>if < 0, event fails CAL prefilters. 
 <tr><td> CTBAllProb
 <td>F<td> CT based variables combining CTBCPFGamProb, CTBTKRGamProb and CTBCALGamProb
 <tr><td> CTBClassLevel
 <td>F<td> Each event is classified into one of four classes, defined using varying degrees of the above 
           GamProb variables: 
           <br> <br>&nbsp;&nbsp;0 = background 
           <br>&nbsp;&nbsp;1 = transient
           <br>&nbsp;&nbsp;2 = source 
           <br>&nbsp;&nbsp;3 = diffuse
           <br> <br>Note that the transient class is defined as
           CTBClassLevel >= 1, and so on, since these classes are strictly nested.
 <tr><td> CTBParticleType
 <td>F<td> Particle type determination: 
           <br>1 = e+/e-, 2 = proton, 3 = alpha, and 4 = heavy ion
 <tr><td> CTBChargedEMProb
 <td>F<td> CT based variable giving probability that incoming 
           particle was an electron or positron, as distinct from a proton
 </table>
*/

AtwoodTrees::AtwoodTrees(ITupleInterface& tuple, std::ostream& log, std::string imfile, bool printTreeInfo)
                       : m_log(log)
{
    // these are used for preliminary cuts to select the tree to use
    m_TkrNumTracks    = tuple.getItem("TkrNumTracks");
    m_CalEnergyRaw    = tuple.getItem("CalEnergyRaw");
    m_CalCsIRLn       = tuple.getItem("CalCsIRLn");
    m_FilterStatus_HI = tuple.getItem("FilterStatus_HI");

    m_AcdActiveDist3D  = tuple.getItem("AcdActiveDist3D");
    m_AcdRibbonActDist = tuple.getItem("AcdRibbonActDist");
    m_AcdCornerDoca    = tuple.getItem("AcdCornerDoca");
    m_Tkr1SSDVeto      = tuple.getItem("Tkr1SSDVeto");

    m_eventId          = tuple.getItem("EvtEventId");
    m_run              = tuple.getItem("EvtRun");

    // Default values as of 4/5/2007 at request of Bill Atwood
    m_calEnergyCut     = 5.;
    m_csiRadLenCut     = 4.;
    m_numTracksCut     = 0.;
        
    m_executeTreeCnt = 0;
    m_goodVals       = 0;
    m_caughtVals     = 0;

    //m_xmlFactory = new GlastClassify::xmlTreeFactory(treepath, tuple);
    if( imfile.empty() ){
        imfile = default_xml; 
    }

    facilities::Util::expandEnvVar(&imfile);

    log << "GlastClassify::AtwoodTrees-- Loading Atwood's IM classification trees from " << imfile ;

    xmlTreeAnalysisFactory treeFactory(imfile, tuple);

    m_treeAnalysis = treeFactory.buildTreeAnalysis();

    log << "\n\t\tloaded " << treeFactory.nodeCount() <<" nodes";

    //Testing...
    if (printTreeInfo)
    {
        //std::ofstream outFile("IMsheetTest.txt");
        m_treeAnalysis->print(log);
        //outFile.close();
    }

    return;
}

AtwoodTrees::~AtwoodTrees() 
{
    delete m_treeAnalysis;
}


//_________________________________________________________________________

bool AtwoodTrees::execute()
{
    // Output flag default is to not write row (if pruning)
    bool writeTupleRow = false;

    // initialize CT output variables
    m_bestEnergyProb    = 0.;
    m_CORE              = 0.;
    m_evtLogEnergyRaw   = 0.;

    double tkrNumTracks = *m_TkrNumTracks;
    double calenergy    = *m_CalEnergyRaw;
    double calCsiRln    = *m_CalCsIRLn;

    int    eventId      = *m_eventId;
    int    run          = *m_run;

    // Always zero the CTB output values in case cuts below fail
    m_treeAnalysis->zeroCTvals();

    // These are the "standard" selection cuts
    if( calenergy > m_calEnergyCut && calCsiRln > m_csiRadLenCut && tkrNumTracks > m_numTracksCut)
    {
        m_treeAnalysis->execute();

        m_executeTreeCnt++;

        // Recover the results?
        try
        {
            // Retrieve the energy classification results (needed below)
            m_bestEnergyProb  = m_treeAnalysis->getTupleVal("CTBBestEnergyProb");
            m_CORE            = m_treeAnalysis->getTupleVal("CTBCORE");
            m_evtLogEnergyRaw = m_treeAnalysis->getTupleVal("EvtLogEnergyRaw");

            m_goodVals++;
        }
        catch(...)
        {
            // Keeps on executing;
            m_caughtVals++;
        }

        double FilterStatus_HI = *m_FilterStatus_HI;

        FilterStatus_HI = 0;

        // First cuts on Filter status and failures for energy and tails
        if (FilterStatus_HI == 0 && m_bestEnergyProb > 0.1 && m_CORE > 0.1)
        {
            double AcdActiveDist3D  = *m_AcdActiveDist3D;
            double AcdRibbonActDist = *m_AcdRibbonActDist;
            double Tkr1SSDVeto      = *m_Tkr1SSDVeto;

            // A series of selections on the ACD 
            if (!((AcdActiveDist3D > 0 || AcdRibbonActDist > 0) && Tkr1SSDVeto < 2))
            {
                double AcdCornerDoca = *m_AcdCornerDoca;

                if (!(AcdCornerDoca > -5 && AcdCornerDoca < 50))
                {
                    // Finally, check the result of running the Analysis Sheet
                    double dWriteTupleRow = m_treeAnalysis->getTupleVal("WriteTupleRow");

                    if (dWriteTupleRow != 0.) writeTupleRow = true;
                }
            }
        }
    }

    // Last step, always copy CTB back to ntuple
    m_treeAnalysis->storeCTvals();

    return writeTupleRow;
}

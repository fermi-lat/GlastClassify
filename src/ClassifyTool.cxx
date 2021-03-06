/**
 * @class ClassifyTool
 *
 * @brief Implements a Gaudi Tool to allow for "standard" GLAST classification/analysis to be run
 *        from within the context of reconstruction
 *
 * @author The Tracking Software Group
 *
 * File and Version Information:
 *      $Header: /nfs/slac/g/glast/ground/cvs/GlastRelease-scons/GlastClassify/src/ClassifyTool.cxx,v 1.9 2011/11/18 17:41:23 usher Exp $
 */

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/GaudiException.h" 
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "TreeAnalysis.h"
#include "xmlBuilders/xmlTreeAnalysisFactory.h"

#include "GlastSvc/GlastClassify/ITupleInterface.h"
#include "GlastSvc/GlastClassify/IClassifyTool.h"
#include "facilities/Util.h"

#include "ntupleWriterSvc/INTupleWriterSvc.h"

#include <errno.h>

//***************************************************************************
// Begin by defining the "tuple" code here
//

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template <class T> class ClassifyItem : public GlastClassify::Item {
public:
    ClassifyItem<T>(const std::string& name, const std::string& type, T* data) :
      m_pdata(data), m_name(name), m_type(type)   {}
    
    virtual ~ClassifyItem<T>() {}

    operator double()const {
        return (double)*m_pdata;
    }

    const std::string& getDataName() const {return m_name;}
          void*        getDataAddr() const {return m_pdata;}

    const std::string& getDataType() const {return m_type;}

// LSR 14-Jul-08 code for ntuple types

    void setDataValue(void* data) 
    {
        if (m_type == "UInt_t")
        {
            *m_pdata = *(reinterpret_cast<int*>(data));
        }
        else if (m_type == "ULong64_t")
        {
            *m_pdata = *(reinterpret_cast<unsigned long long*>(data));
        }
        else if (m_type == "Float_t")
        {
            *m_pdata = *(reinterpret_cast<float*>(data));
        }
        else if (m_type == "Double_t")
        {
            *m_pdata = *(reinterpret_cast<double*>(data));
        }
        else if (m_type == "UChar_t")
        {
            memset(m_pdata, ' ', 80);
            strcpy(reinterpret_cast<char*>(m_pdata), reinterpret_cast<char*>(data));
        }
        else if (m_type == "Char_t")
        {
            memset(m_pdata, ' ', 80);
            strcpy(reinterpret_cast<char*>(m_pdata), reinterpret_cast<char*>(data));
        }
        else
        {
            throw std::invalid_argument("ClassifyAlg::Item: attempting to set an unrecognized data type");
        }
    }

private:
    T*          m_pdata;
    std::string m_name;
    std::string m_type;
    void*       m_treePtr;
};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class ClassifyTuple : public GlastClassify::ITupleInterface {
public:
    ClassifyTuple( INTupleWriterSvc* tuple, const std::string& treeName, const std::string fileName = "")
        : m_tuple(tuple)
        , m_treeName(treeName)
        , m_fileName(fileName)
        , m_saveRow(false)
    {
        if (m_fileName != "") m_saveRow = true;

        m_tuple->getOutputTreePtr(m_treePtr, m_treeName);

        if (!m_treePtr) 
            throw std::invalid_argument("GleamTuple constructor: can not get pointer to output tree!");
    }

// LSR 14-Jul-08 code for ntuple types

    const GlastClassify::Item* getItem(const std::string& name)const
    {
        const GlastClassify::Item* item = 0;
        void* dummy;

        std::string type = m_tuple->getItem(m_treeName, name, dummy, m_treePtr);

        if (type == "Float_t")
        {
            float* data = (float*)dummy;
            item =new ClassifyItem<float>(name, type, data);
        }
        else if (type == "Double_t")
        {
            double* data = (double*)dummy;
            item =new ClassifyItem<double>(name, type, data);
        }
        else if (type == "UInt_t")
        {
            unsigned int* data = (unsigned int*)dummy;
            item =new ClassifyItem<unsigned int>(name, type, data);
        }
        else if (type == "ULong64_t")
        {
            unsigned long long* data = (unsigned long long*)dummy;
            item =new ClassifyItem<unsigned long long>(name, type, data);
        }
        else if (type == "Int_t")
        {
            int* data = (int*)dummy;
            item =new ClassifyItem<int>(name, type, data);
        }
        else if (type == "UChar_t")
        {
            char* data = (char*)dummy;
            item = new ClassifyItem<char>(name, type, data);
        }
        else if (type == "Char_t")
        {
            char* data = (char*)dummy;
            item = new ClassifyItem<char>(name, type, data);
        }
        else
        {
            throw std::invalid_argument("GleamTuple::getItem: attempting to set an unrecognized data type");
        }

        return item;
    }

// LSR 14-Jul-08 code for ntuple types

    /// create a new item (float only for now) in the tuple, which will take the given value
   void addItem(const std::string& name, float & value)
    {
        m_tuple->addItem(m_treeName, name, &value, m_fileName, m_saveRow);
    }
    
    void addItem(const std::string& name, double & value)
    {
        m_tuple->addItem(m_treeName, name, &value, m_fileName, m_saveRow);
    }
    void addItem(const std::string& name, unsigned long long & value)
    {
        m_tuple->addItem(m_treeName, name, &value, m_fileName, m_saveRow);
    }
    void addItem(const std::string& name, char & value)
    {
        m_tuple->addItem(m_treeName, name, &value, m_fileName, m_saveRow);
    }
private:
    INTupleWriterSvc*  m_tuple;
    void*              m_treePtr;
    const std::string& m_treeName;
    const std::string  m_fileName;
    bool               m_saveRow;
};


//***************************************************************************
// Now get on to the work of defining the actual tool here
//

class ClassifyTool : public AlgTool, virtual public IClassifyTool, virtual public IIncidentListener
{
public:
    /// Standard Gaudi Tool interface constructor
    ClassifyTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~ClassifyTool();

    /// @brief Intialization of the tool
    StatusCode initialize();

    /// @brief Method to set up the classification tree analysis for this tool 
    ///        (separating from "initialize" allows for multiple instances with different analyses)
    StatusCode setUpClassification(VarNameToValueMap& varMap,
                                   std::string&       xmlFileName,
                                   std::string&       tupleName,
                                   std::string        tupleFileName = "");

    /// @brief Method to association the Monte Carlo hits into Pattern Candidate tracks
    StatusCode runClassification();

    /// @brief Once classification run this will look up and return the value of a given
    ///        variable. If the variable has been found successfully then it returns true
    bool       getVariable(const std::string& varName, GlastClassify::Item* &varItem);

    /// @brief Called by incident service at signalled times in event processing
    void handle(const Incident& inc);

    /// @brief Finalize method for outputting run statistics
    StatusCode finalize();

private:

    ///*** PRIVATE METHODS ***

    ///*** PRIVATE DATA MEMBERS ***
    // Pointer to the tuple service
    INTupleWriterSvc*               m_rootTupleSvc;

    // Keep track of the tuple name
    std::string                     m_tupleName;

    /// Pointer to the ntuple set up to enable classification trees
    GlastClassify::ITupleInterface* m_tuple;

    // Pointer to the classification tree analysis
    GlastClassify::TreeAnalysis*    m_treeAnalysis;

    // Are we saving rows?
    bool                            m_saveRows;

    bool                            m_printInfo;
};


//static ToolFactory<ClassifyTool> s_factory;
//const IToolFactory& ClassifyToolFactory = s_factory;
DECLARE_TOOL_FACTORY(ClassifyTool);

//
// Class constructor, no initialization here
//
ClassifyTool::ClassifyTool(const std::string& type, const std::string& name, const IInterface* parent) :
                    AlgTool(type, name, parent),
                    m_rootTupleSvc(0), m_tupleName(""), m_tuple(0), m_treeAnalysis(0), m_saveRows(false)
{
    declareProperty("PrintInfo", m_printInfo = true);

    //Declare additional interface
    declareInterface<IClassifyTool>(this);

    return;
}

ClassifyTool::~ClassifyTool()
{
    delete m_treeAnalysis;

    return;
}

//
// Initialization of the tool here
//
StatusCode ClassifyTool::initialize()
{
    StatusCode sc   = StatusCode::SUCCESS;

    // Recover the root tuple service
    IService* iService = 0;
    if ((sc = serviceLocator()->getService("RootTupleSvc", iService)).isFailure())
    {
        throw GaudiException("Service [RootTupleSvc] not found", name(), sc);
    }

    // get a pointer to RootTupleSvc 
    m_rootTupleSvc = dynamic_cast<INTupleWriterSvc*>(iService);

    if ((sc = serviceLocator()->getService("IncidentSvc", iService)).isFailure())
    {
        throw GaudiException("Service [IncidentSvc] not found", name(), sc);
    }
    IIncidentSvc* incSvc = dynamic_cast<IIncidentSvc*>(iService);

    //set up listener for IncidentSvc
    incSvc->addListener(this, "BeginEvent", 100);

    return sc;
}

StatusCode ClassifyTool::setUpClassification(VarNameToValueMap& varMap,
                                             std::string&       xmlFileName,
                                             std::string&       tupleName,
                                             std::string        tupleFileName)
{
    StatusCode sc   = StatusCode::SUCCESS;
        
    MsgStream log(msgSvc(), name());

    // Keep track of the tuple name (which is also the tree name)
    m_tupleName = tupleName;

    // The tuple service does not have a direct method for creating a new output file and tree
    // so we have to do this on the side here... 
    int* dummyInt = new int();

    if (tupleFileName == "") m_saveRows = false;

    sc = m_rootTupleSvc->addItem(tupleName, "Dummy", dummyInt, tupleFileName, m_saveRows);

    // create our interface to the tuple
    m_tuple = new ClassifyTuple(m_rootTupleSvc, m_tupleName, tupleFileName);

    // Loop through the input var map to add variables to our tuple
    for(VarNameToValueMap::iterator varMapItr = varMap.begin(); varMapItr != varMap.end(); varMapItr++)
    {
        // The following song and dance is intended to be a way to avoid having to also keep track
        // of the interface to root tuple stuff in the calling code. It involves a bit of a song and
        // dance here but hopefully this is less painful that the alternative... (FLW)
        GlastClassify::Item* item = varMapItr->second;

        if (item->getDataType() == "Float_t")
        {
            m_tuple->addItem(varMapItr->first, *reinterpret_cast<float*>(item->getDataAddr()));
        }
        else if (item->getDataType() == "Double_t")
        {
            m_tuple->addItem(varMapItr->first, *reinterpret_cast<double*>(item->getDataAddr()));
        }
        else if (item->getDataType() == "Char_t")
        {
            m_tuple->addItem(varMapItr->first, *reinterpret_cast<char*>(item->getDataAddr()));
        }
    }

    // Sort out the path to the input xml file
	facilities::Util::expandEnvVar(&xmlFileName);

    log << MSG::INFO << "GlastClassify::ClassifyTool -- Loading Atwood's IM classification trees from " << xmlFileName << endreq;

    GlastClassify::xmlTreeAnalysisFactory treeFactory(xmlFileName, *m_tuple);

    m_treeAnalysis = treeFactory.buildTreeAnalysis();

    log << MSG::INFO << "\n\t\tloaded " << treeFactory.nodeCount() <<" nodes";

    //Testing...
    if (m_printInfo)
    {
        m_treeAnalysis->print(log.stream());
        //outFile.close();
    }

    log << endreq;

    return sc;
}

StatusCode ClassifyTool::runClassification()
{
    StatusCode sc = StatusCode::SUCCESS;

    // Always zero the CTB output values in case cuts below fail
    m_treeAnalysis->zeroCTvals();

    // Note that as of July 18, 2008, we no longer make any selection cuts before calling the IM analysis
    m_treeAnalysis->execute();

    // Last step, always copy CTB back to ntuple
    m_treeAnalysis->storeCTvals();

    return sc;
}

bool ClassifyTool::getVariable(const std::string& varName, GlastClassify::Item* &varItem)
{
    bool foundIt = false;

    // Look up the variable in the tuple 
    const GlastClassify::Item* item = m_tuple->getItem(varName);

    if (item)
    {
        varItem = const_cast<GlastClassify::Item*>(item);
        foundIt = true;
    }

    return foundIt;
}

void ClassifyTool::handle(const Incident & inc) 
{    
    MsgStream log(msgSvc(), name());

    if(inc.type()=="BeginEvent") 
    {
        // Always zero the CTB output values in case cuts below fail
        m_treeAnalysis->zeroAllVals();

        // Always set the store row flag to true (so we can cross reference to merit?)
        if (m_saveRows) m_rootTupleSvc->storeRowFlag(m_tupleName, true);
    }
}
StatusCode ClassifyTool::finalize()
{
    StatusCode sc = StatusCode::SUCCESS;

    return sc;
}

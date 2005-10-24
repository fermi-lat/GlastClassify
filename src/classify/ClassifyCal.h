/**@file ClassifyCal.h
@brief 

*/
#include "GlastClassify.h"

#include <cmath>

class ClassifyCal : public GlastClassify
{
public:
    typedef enum {LOW, MED, HIGH, ALL} etype;

    ClassifyCal(const std::string& info_path, etype energyrange=ALL)
        : GlastClassify(info_path)
        , m_energyrange(energyrange)
    {
        if     ( info_path.find("high")>=0) m_energyrange=HIGH;
        else if( info_path.find("med")>=0) m_energyrange=MED;
        else if( info_path.find("low")>=0) m_energyrange=LOW;
        else    throw std::invalid_argument("unrecognized enegy range");
        m_energyIndex   =find_index("EvtEnergyCorr");
        m_calEnergyIndex= find_index("CalEnergyRaw");
        m_CalTotRLnIndex = find_index("CalTotRLn");
        m_mcEnergy = add_index( "McEnergy");
    }

    //function to generate goodcal test

    virtual bool isgood()
    {
        double energyRatio = datum(m_energyIndex)/datum(m_mcEnergy);
        return fabs(energyRatio-1)<0.35;
    }

    virtual bool accept()
    {
        double energy = datum(m_calEnergyIndex);
        if(energy<5. || datum(m_CalTotRLnIndex)<4.) return false;
        else if(m_energyrange==LOW) return (energy<350.);
        else if(m_energyrange==MED) return ( energy<3500.);
        else if(m_energyrange==HIGH) return (energy>3500.);
        else return true;
        //		return (energy>5. && datum(m_CalTotRLnIndex)>4.);
    }

    float calEnergy()const{ return datum(m_calEnergyIndex); }

private:
    int m_energyIndex;
    int m_mcEnergy;
    int m_calEnergyIndex;
    int m_CalTotRLnIndex;
    etype m_energyrange;
};


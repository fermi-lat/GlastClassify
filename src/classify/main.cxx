/**@file main.cxx
@brief main program for application to create GLAST classification trees

$Header: /nfs/slac/g/glast/ground/cvs/GlastClassify/src/classify/main.cxx,v 1.14 2005/10/31 16:45:49 burnett Exp $
*/

#include "ClassifyCal.h"
#include "ClassifyVertex.h"
#include "ClassifyCore.h"
#include "ClassifyGamma.h"
#include "classifier/AdaBoost.h"
#include "classifier/Classifier.h"
#include "ClassifyEnergy.h"

#include "ParseOptions.h"


int main(int argc , char * argv[])
{
    int rc=0;
    ParseOptions opts(argc, argv);
    if( ! opts.valid) return 2;

    std::string name(opts.casename);
    GlastClassify::setPaths(opts.datapath, opts.infopath);
    GlastClassify::s_train = opts.train;
    GlastClassify::s_boost = opts.n_boosts;
    GlastClassify::s_events = opts.events;
    GlastClassify::s_normalize = opts.normalize;
    AdaBoost::set_purity(opts.leaf_purity);
    Classifier::Node::s_improvement_minimum = opts.improv_min;
    GlastClassify::s_train_sample= opts.train_sample; 
    GlastClassify::s_test_sample= opts.test_sample; 


    std::cout << "classifier invoked with data, tree paths: "<< opts.datapath << ", " << opts.infopath ;
    std::cout << "  In "<< (GlastClassify::s_train? " training" : "testing") << " mode" ;
    if( opts.train && opts.n_boosts>0) std::cout << ", boosting "<< opts.n_boosts<< " times";
    std::cout << std::endl;

    try {
        bool all(name=="all"); 
        bool psf(name=="psf");
        bool energy(name=="energy");
        bool vertex(name=="vertex");


        // the categories

        if( name=="energy/param/low" || all || energy) ClassifyEnergy("energy/param/low").run();
        if( name=="energy/param/med" || all || energy) ClassifyEnergy("energy/param/med").run();
        if( name=="energy/param/high"|| all || energy) ClassifyEnergy("energy/param/high").run();
        if( name=="energy/param"    || all || energy) ClassifyEnergy("energy/param").run();
        if( name=="energy/lastlayer"|| all || energy) ClassifyEnergy("energy/lastlayer").run();
        if( name=="energy/profile"  || all || energy) ClassifyEnergy("energy/profile").run();
        if( name=="energy/tracker"  || all || energy) ClassifyEnergy("energy/tracker").run();

        if( name=="vertex/thin"     || all || vertex) ClassifyVertex("vertex/thin" ).run();
        if( name=="vertex/thick"    || all || vertex) ClassifyVertex("vertex/thick").run();

        if( name=="psf/vertex/thin" || all || psf)    ClassifyCore("psf/vertex/thin" ).run();
        if( name=="psf/vertex/thick"|| all || psf)    ClassifyCore("psf/vertex/thick").run();
        if( name=="psf/track/thin"  || all || psf)    ClassifyCore("psf/track/thin"  ).run();
        if( name=="psf/track/thick" || all || psf)    ClassifyCore("psf/track/thick" ).run();

        if( name.substr(0,6)=="gamma/" || all) ClassifyGamma(name).run();
        if( name=="gamma" || all ) {
            std::string gamma_subset_name[]={"highcal", "medcal", "thin", "thick"};
            for( int i=0; i<4; ++i){
                ClassifyGamma("gamma/vertex/"+gamma_subset_name[i]).run();
                ClassifyGamma("gamma/track/"+gamma_subset_name[i]).run();
            }
        }
    }
    catch (const std::exception & error)
    {
        std::cerr << "Caught exception \"" << error.what() << "\"" <<std::endl;
        rc= 1;
    }
    return rc;
}

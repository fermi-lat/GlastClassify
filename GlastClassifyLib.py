# $Header: /nfs/slac/g/glast/ground/cvs/GlastRelease-scons/GlastClassify/GlastClassifyLib.py,v 1.1 2008/08/15 21:42:35 ecephas Exp $
def generate(env, **kw):
    if not kw.get('depsOnly', 0):
        env.Tool('addLibrary', library = ['GlastClassify'])
    env.Tool('facilitiesLib')
    env.Tool('classifierLib')
    env.Tool('ntupleWriterSvcLib')
    env.Tool('xmlBaseLib')
def exists(env):
    return 1;

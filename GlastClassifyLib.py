# $Header: /nfs/slac/g/glast/ground/cvs/GlastRelease-scons/GlastClassify/GlastClassifyLib.py,v 1.3 2009/01/23 00:21:11 ecephas Exp $
def generate(env, **kw):
    if not kw.get('noGaudi', 0):
        env.Tool('addLibrary', library = env['gaudiLibs'])
        if not kw.get('depsOnly', 0):
            env.Tool('addLibrary', library = ['GlastClassify'])
    env.Tool('classifierLib')
    env.Tool('ntupleWriterSvcLib')
    env.Tool('xmlBaseLib')
    env.Tool('facilitiesLib')
    env.Tool('addLibrary', library = env['xercesLibs'])
def exists(env):
    return 1;

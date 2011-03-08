# $Header: /nfs/slac/g/glast/ground/cvs/GlastRelease-scons/GlastClassify/GlastClassifyLib.py,v 1.4 2009/11/21 00:35:19 jrb Exp $
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
    env.Tool('addLibrary', library = env['TMineLibs'])
def exists(env):
    return 1;

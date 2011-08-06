# $Header: /nfs/slac/g/glast/ground/cvs/GlastClassify/GlastClassifyLib.py,v 1.5 2011/03/08 00:10:23 jrb Exp $
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

# $Header: /nfs/slac/g/glast/ground/cvs/GlastRelease-scons/GlastClassify/GlastClassifyLib.py,v 1.6 2012/08/15 05:06:11 heather Exp $
def generate(env, **kw):
#    if not kw.get('noGaudi', 0):
#        env.Tool('addLibrary', library = env['gaudiLibs'])
    if not kw.get('depsOnly', 0):
        env.Tool('addLibrary', library = ['GlastClassify'])
    env.Tool('classifierLib')
    env.Tool('ntupleWriterSvcLib')
    env.Tool('xmlBaseLib')
    env.Tool('facilitiesLib')
    env.Tool('addLibrary', library = env['xercesLibs'])
    env.Tool('addLibrary', library = env['TMineLibs'])
    if env['PLATFORM']=="win32" and env.get('CONTAINERNAME','')=='GlastRelease':
        env.Tool('findPkgPath', package = 'GlastSvc') 

def exists(env):
    return 1;

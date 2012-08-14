# -*- python -*-
# $Header: /nfs/slac/g/glast/ground/cvs/GlastRelease-scons/GlastClassify/SConscript,v 1.45 2012/05/11 19:58:38 kadrlica Exp $
# Authors: Tracy Usher <usher@slac.stanford.edu>
# Version: GlastClassify-07-09-04

Import('baseEnv')
Import('listFiles')
Import('packages')
staticLibEnv = baseEnv.Clone()    # static library
staticProgEnv = baseEnv.Clone()   # progs built on static library
libEnv = baseEnv.Clone()          # gaudi cmp. library 

staticProgEnv.Tool('GlastClassifyLib', depsOnly = 1, noGaudi = 1)
libEnv.Tool('addLinkDeps', package='GlastClassify', toBuild='component')
staticProgEnv.Tool('addLibrary', library = ['ClassificationTree'])

ClassificationTree = staticLibEnv.StaticLibrary('ClassificationTree',
                                                listFiles(['src/*.cxx',
                                                           'src/ImActivityNodes/*.cxx',
                                                           'src/XT/*.cxx',
                                                           'src/xmlBuilders/*.cxx']))
apply = staticProgEnv.Program('apply', listFiles(['src/apply/*.cxx']))

# Might need to recompile for shared lib
if baseEnv['PLATFORM'] == 'win32':
    libEnv.Tool('addLibrary', library = ['ClassificationTree'])
    GlastClassify = libEnv.ComponentLibrary('GlastClassify', ClassificationTree)
else:
    GlastClassify = libEnv.ComponentLibrary('GlastClassify',
                                            listFiles(['src/*.cxx',
                                                       'src/ImActivityNodes/*.cxx',
                                                       'src/XT/*.cxx',
                                                       'src/xmlBuilders/*.cxx']))




staticProgEnv.Tool('registerTargets', package = 'GlastClassify',
                   staticLibraryCxts = [[ClassificationTree, staticLibEnv]],
                   libraryCxts = [[GlastClassify, libEnv]], 
                   binaryCxts = [[apply, staticProgEnv]],
                   xml = listFiles(['xml/*.xml']),
                   includes = listFiles(['GlastClassify/*.h']))





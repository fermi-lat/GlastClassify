# -*- python -*-
# $Header: /nfs/slac/g/glast/ground/cvs/GlastClassify/SConscript,v 1.33 2011/03/15 21:30:22 lsrea Exp $
# Authors: Tracy Usher <usher@slac.stanford.edu>
# Version: GlastClassify-07-07-05
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
libEnv.Tool('addLibrary', library = ['ClassificationTree'])

GlastClassify = libEnv.SharedLibrary('GlastClassify',
                                     listFiles(['src/Dll/*.cxx']))



staticProgEnv.Tool('registerTargets', package = 'GlastClassify',
                   staticLibraryCxts = [[ClassificationTree, staticLibEnv]],
                   libraryCxts = [[GlastClassify, libEnv]], 
                   binaryCxts = [[apply, staticProgEnv]],
                   xml = listFiles(['xml/*.xml']),
                   includes = listFiles(['GlastClassify/*.h']))





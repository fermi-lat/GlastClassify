# -*- python -*-
# $Header: /nfs/slac/g/glast/ground/cvs/GlastRelease-scons/GlastClassify/SConscript,v 1.19 2009/10/16 15:58:55 usher Exp $
# Authors: Tracy Usher <usher@slac.stanford.edu>
# Version: GlastClassify-07-05-00
Import('baseEnv')
Import('listFiles')
Import('packages')
staticLibEnv = baseEnv.Clone()    # static library
staticProgEnv = baseEnv.Clone()   # progs built on static library
libEnv = baseEnv.Clone()          # gaudi cmp. library 

staticLibEnv.Tool('GlastClassifyLib', depsOnly = 1, noGaudi = 1)
staticProgEnv.Tool('GlastClassifyLib', depsOnly = 1, noGaudi = 1)
libEnv.Tool('GlastClassifyLib', depsOnly = 1)
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
                   includes = listFiles(['GlastClassify/*.h']))





# -*- python -*-
# $Header: /nfs/slac/g/glast/ground/cvs/GlastRelease-scons/GlastClassify/SConscript,v 1.3 2008/08/29 23:24:21 ecephas Exp $
# Authors: Tracy Usher <usher@slac.stanford.edu>
# Version: GlastClassify-06-05-02
Import('baseEnv')
Import('listFiles')
Import('packages')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

libEnv.Tool('GlastClassifyLib', depsOnly = 1)

ClassificationTree = libEnv.StaticLibrary('ClassificationTree', listFiles(['src/*.cxx',
			'src/ImActivityNodes/*.cxx',
			'src/XT/*.cxx',
			'src/xmlBuilders/*.cxx']))


GlastClassify = libEnv.SharedLibrary('GlastClassify', listFiles(['src/Dll/*.cxx']))

progEnv.Tool('GlastClassifyLib')
progEnv.Tool('addLibrary', library = ['ClassificationTree'])
apply = progEnv.Program('apply', listFiles(['src/apply/*.cxx']))
progEnv.Tool('registerObjects', package = 'GlastClassify', 
	libraries = [ClassificationTree, GlastClassify], 
	binaries = [apply], 
	includes = listFiles(['GlastClassify/*.h']))





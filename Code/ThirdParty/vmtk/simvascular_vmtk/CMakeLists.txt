# Copyright (c) Stanford University, The Regents of the University of
#               California, and others.
#
# All Rights Reserved.
#
# See Copyright-SimVascular.txt for additional details.
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject
# to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
# IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
# TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
# PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
# OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#-----------------------------------------------------------------------------
# ThirdParty - VMTK
#-----------------------------------------------------------------------------
set(lib ${SV_LIB_THIRDPARTY_VMTK_NAME})
LIST(APPEND PLUGINLIBS ${lib})

set(CXXSRCS vtkvmtkBoundaryLayerGenerator.cxx
	vtkvmtkMath.cxx vtkvmtkItem.cxx
	vtkvmtkStencil.cxx vtkvmtkDataSetItem.cxx vtkvmtkNeighborhood.cxx
	vtkvmtkPolyBallLine.cxx
	vtkvmtkPolyDataManifoldExtendedNeighborhood.cxx vtkvmtkPolyDataManifoldNeighborhood.cxx
	vtkvmtkPolyDataManifoldStencil.cxx vtkvmtkPolyDataUmbrellaStencil.cxx
	vtkvmtkPolyDataDistanceToCenterlines.cxx
	vtkvmtkPolyDataSurfaceRemeshing.cxx vtkvmtkPolyDataBoundaryExtractor.cxx
	vtkvmtkPolyDataSizingFunction.cxx vtkvmtkBoundaryReferenceSystems.cxx
	vtkvmtkCapPolyData.cxx vtkvmtkSurfaceProjection.cxx
	vtkvmtkPolyDataToUnstructuredGridFilter.cxx vtkvmtkSimpleCapPolyData.cxx
	vtkvmtkUnstructuredGridTetraFilter.cxx vtkvmtkAppendFilter.cxx
  vtkvmtkMinHeap.cxx vtkvmtkNonManifoldSteepestDescent.cxx
  vtkvmtkVoronoiDiagram3D.cxx vtkvmtkNonManifoldFastMarching.cxx
  vtkvmtkSimplifyVoronoiDiagram.cxx vtkvmtkInternalTetrahedraExtractor.cxx
	vtkvmtkSteepestDescentLineTracer.cxx vtkvmtkPolyDataCenterlines.cxx
	vtkvmtkCenterlineBranchExtractor.cxx vtkvmtkCenterlineSphereDistance.cxx
	vtkvmtkCenterlineUtilities.cxx vtkvmtkCenterlineSplittingAndGroupingFilter.cxx
  vtkvmtkPolyDataCenterlineGroupsClipper.cxx vtkvmtkMergeCenterlines.cxx
  vtkvmtkCenterlineBifurcationReferenceSystems.cxx vtkvmtkReferenceSystemUtilities.cxx
  vtkvmtkPolyDataBranchSections.cxx vtkvmtkPolyDataCenterlineSections.cxx vtkvmtkPolyDataBranchUtilities.cxx
	)
SET(HDRS vtkvmtkAppendFilter.h
	vtkvmtkBoundaryLayerGenerator.h vtkvmtkBoundaryReferenceSystems.h
	vtkvmtkCapPolyData.h vtkvmtkConfigure.h
	vtkvmtkConstants.h vtkvmtkDataSetItem.h
	vtkvmtkItem.h vtkvmtkMath.h
	vtkvmtkNeighborhood.h vtkvmtkPolyBallLine.h
	vtkvmtkPolyDataBoundaryExtractor.h vtkvmtkPolyDataDistanceToCenterlines.h
	vtkvmtkPolyDataManifoldExtendedNeighborhood.h vtkvmtkPolyDataManifoldNeighborhood.h
	vtkvmtkPolyDataManifoldStencil.h vtkvmtkPolyDataSizingFunction.h
	vtkvmtkPolyDataSurfaceRemeshing.h vtkvmtkPolyDataToUnstructuredGridFilter.h
	vtkvmtkPolyDataUmbrellaStencil.h vtkvmtkSimpleCapPolyData.h
	vtkvmtkStencil.h vtkvmtkSurfaceProjection.h
	vtkvmtkUnstructuredGridTetraFilter.h vtkvmtkWin32Header.h
	vtkvmtkMinHeap.h vtkvmtkNonManifoldSteepestDescent.h
	vtkvmtkVoronoiDiagram3D.h vtkvmtkNonManifoldFastMarching.h
	vtkvmtkSimplifyVoronoiDiagram.h vtkvmtkSteepestDescentLineTracer.h
	vtkvmtkInternalTetrahedraExtractor.h vtkvmtkPolyDataCenterlines.h
	vtkvmtkCenterlineBranchExtractor.h vtkvmtkCenterlineSphereDistance.h
	vtkvmtkCenterlineUtilities.h vtkvmtkCenterlineSplittingAndGroupingFilter.h
  vtkvmtkPolyDataCenterlineGroupsClipper.h vtkvmtkMergeCenterlines.h
  vtkvmtkCenterlineBifurcationReferenceSystems.h vtkvmtkReferenceSystemUtilities.h
  vtkvmtkPolyDataBranchSections.h vtkvmtkPolyDataCenterlineSections.h vtkvmtkPolyDataBranchUtilities.h 
	)

if(SV_USE_THIRDPARTY_SHARED_LIBRARIES)
  add_library(${lib} SHARED ${CXXSRCS})
else()
  add_library(${lib} STATIC ${CXXSRCS})
endif()

target_link_libraries(${lib}
	${VTK_LIBRARIES}
	)
if(SV_INSTALL_LIBS)
	install(TARGETS ${lib}
		RUNTIME DESTINATION ${SV_INSTALL_RUNTIME_DIR} COMPONENT ThirdPartyExecutables
		LIBRARY DESTINATION ${SV_INSTALL_LIBRARY_DIR} COMPONENT ThirdPartyLibraries
		ARCHIVE DESTINATION ${SV_INSTALL_ARCHIVE_DIR} COMPONENT ThirdPartyLibraries
		)
endif()
if(SV_INSTALL_HEADERS)
	install(FILES ${HDRS}
		DESTINATION ${SV_INSTALL_INCLUDE_DIR}/thirdparty/vmtk COMPONENT ThirdPartyHeaders
		)
endif()

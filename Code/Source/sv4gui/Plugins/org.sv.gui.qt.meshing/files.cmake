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

set(CPP_FILES
    sv4gui_MeshCreate.cxx
    sv4gui_MeshCreateAction.cxx
    sv4gui_MeshEdit.cxx
    sv4gui_VtkMeshSphereWidget.cxx
    sv4gui_LocalTableDelegate.cxx
    sv4gui_MeshLegacySaveAction.cxx
    sv4gui_MeshLoadSurfaceAction.cxx
    sv4gui_MeshLoadVolumeAction.cxx
    sv4gui_MeshingPluginActivator.cxx
)

set(MOC_H_FILES
    sv4gui_MeshCreate.h
    sv4gui_MeshCreateAction.h
    sv4gui_MeshEdit.h
    sv4gui_LocalTableDelegate.h
    sv4gui_MeshLegacySaveAction.h
    sv4gui_MeshLoadSurfaceAction.h
    sv4gui_MeshLoadVolumeAction.h
    sv4gui_MeshingPluginActivator.h
)

set(UI_FILES
    sv4gui_MeshCreate.ui
    sv4gui_MeshEdit.ui
)

set(CACHED_RESOURCE_FILES
  plugin.xml
  resources/mesh.png
)

set(QRC_FILES
)

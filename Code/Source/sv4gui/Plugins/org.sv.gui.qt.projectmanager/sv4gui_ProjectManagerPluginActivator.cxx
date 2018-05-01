/* Copyright (c) Stanford University, The Regents of the University of
 *               California, and others.
 *
 * All Rights Reserved.
 *
 * See Copyright-SimVascular.txt for additional details.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "sv4gui_ProjectManagerPluginActivator.h"
#include "sv4gui_ProjectAddImageAction.h"
#include "sv4gui_ProjectCloseAction.h"
#include "sv4gui_ProjectSaveAction.h"
#include "sv4gui_ProjectDuplicateAction.h"
#include "sv4gui_ProjectShowModelEdgesAction.h"
#include "sv4gui_ProjectShowModelFullAction.h"

//sv4guiProjectManagerPluginActivator* sv4guiProjectManagerPluginActivator::m_Instance = nullptr;
//ctkPluginContext* sv4guiProjectManagerPluginActivator::m_Context = nullptr;

sv4guiProjectManagerPluginActivator::sv4guiProjectManagerPluginActivator()
{
}

sv4guiProjectManagerPluginActivator::~sv4guiProjectManagerPluginActivator()
{
}

void sv4guiProjectManagerPluginActivator::start(ctkPluginContext* context)
{
//    m_Instance = this;
//    m_Context = context;

    BERRY_REGISTER_EXTENSION_CLASS(sv4guiProjectAddImageAction, context)
    BERRY_REGISTER_EXTENSION_CLASS(sv4guiProjectCloseAction, context)
    BERRY_REGISTER_EXTENSION_CLASS(sv4guiProjectSaveAction, context)
    BERRY_REGISTER_EXTENSION_CLASS(sv4guiProjectDuplicateAction, context)
    BERRY_REGISTER_EXTENSION_CLASS(sv4guiProjectShowModelEdgesAction, context)
    BERRY_REGISTER_EXTENSION_CLASS(sv4guiProjectShowModelFullAction, context)
}

void sv4guiProjectManagerPluginActivator::stop(ctkPluginContext* context)
{
//    Q_UNUSED(context)

//    m_Context = nullptr;
//    m_Instance = nullptr;
}

//ctkPluginContext* sv4guiProjectManagerPluginActivator::GetContext()
//{
//  return m_Context;
//}

//sv4guiProjectManagerPluginActivator* sv4guiProjectManagerPluginActivator::GetInstance()
//{
//    return m_Instance;
//}

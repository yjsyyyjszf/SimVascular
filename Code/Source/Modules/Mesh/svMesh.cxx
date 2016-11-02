#include "svMesh.h"

#include "svStringUtils.h"

svMesh::svMesh()
    : m_Type("")
    , m_ModelName("")
    , m_ModelElement(NULL)
    , m_SurfaceMesh(NULL)
    , m_VolumeMesh(NULL)
{
}

svMesh::svMesh(const svMesh &other)
    : m_Type(other.m_Type)
    , m_ModelName(other.m_ModelName)
    , m_ModelElement(other.m_ModelElement)
    , m_CommandHistory(other.m_CommandHistory)
{
    m_SurfaceMesh=NULL;
    if(other.m_SurfaceMesh)
    {
        m_SurfaceMesh=vtkSmartPointer<vtkPolyData>::New();
        m_SurfaceMesh->DeepCopy(other.m_SurfaceMesh);
    }

    m_VolumeMesh=NULL;
    if(other.m_VolumeMesh)
    {
        m_VolumeMesh=vtkSmartPointer<vtkUnstructuredGrid>::New();
        m_VolumeMesh->DeepCopy(other.m_VolumeMesh);
    }
}

svMesh::~svMesh()
{
}

svMesh* svMesh::Clone()
{
    return new svMesh(*this);
}

std::string svMesh::GetType() const
{
    return m_Type;
}

svModelElement* svMesh::GetModelElement() const
{
    return m_ModelElement;
}

bool svMesh::SetModelElement(svModelElement* modelElement)
{
    m_ModelElement=modelElement;
    return true;
}

void svMesh::CalculateBoundingBox(double *bounds)
{
    bounds[0]=0;
    bounds[1]=0;
    bounds[2]=0;
    bounds[3]=0;
    bounds[4]=0;
    bounds[5]=0;

    if (m_SurfaceMesh != nullptr && m_SurfaceMesh->GetNumberOfPoints() > 0)
    {
      m_SurfaceMesh->ComputeBounds();
      m_SurfaceMesh->GetBounds(bounds);
    }

}

bool svMesh::ExecuteCommands(std::vector<std::string> cmds)
{
    for(int i=0;i<cmds.size();i++)
    {
        svStringUtils::Trim(cmds[i]);

        if(cmds[i]=="")
            continue;

        if(!ExecuteCommand(cmds[i]))
            return false;
    }

    return true;
}

std::vector<std::string> svMesh::GetCommandHistory() const
{
    return m_CommandHistory;
}

void svMesh::SetCommandHistory(std::vector<std::string> history)
{
    m_CommandHistory=history;
}

bool svMesh::ExcuteCommandHistory()
{
    ExecuteCommands(m_CommandHistory);
}


/*=========================================================================

Program:   VMTK
Module:    $RCSfile: vtkvmtkPolyDataCenterlineSections.cxx,v $
Language:  C++
Date:      $Date: 2006/10/17 15:16:16 $
Version:   $Revision: 1.1 $

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENSE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
//SimVascular Changes: completely re-wrote code except for parts of ComputeCenterlineSections

#include "vtkvmtkPolyDataCenterlineSections.h"
#include "vtkvmtkPolyDataBranchSections.h"
#include "vtkPolyData.h"
#include "vtkPolyLine.h"
#include "vtkPolygon.h"
#include "vtkTriangle.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkPlane.h"
#include "vtkStripper.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkPolyDataNormals.h"
#include "vtkMath.h"
#include "vtkCleanPolyData.h"
#include "vtkAppendPolyData.h"
#include "vtkvmtkMath.h"
#include "vtkvmtkCenterlineSphereDistance.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointLocator.h"
#include "vtkConnectivityFilter.h"
#include "vtkLine.h"
#include "vtkXMLPolyDataWriter.h"
#include "vtkImplicitDataSet.h"
#include "vtkAppendFilter.h"
#include "vtkGeometryFilter.h"
#include "vtkUnstructuredGrid.h"
#include "vtkThreshold.h"
#include "vtkPointDataToCellData.h"
#include "vtkSortDataArray.h"

#include "vtkvmtkCenterlineUtilities.h"
#include "vtkvmtkPolyDataBranchUtilities.h"


vtkStandardNewMacro(vtkvmtkPolyDataCenterlineSections);

vtkvmtkPolyDataCenterlineSections::vtkvmtkPolyDataCenterlineSections()
{
    this->Centerlines = vtkPolyData::New();
    this->Surface = vtkPolyData::New();
    this->RadiusArrayName = NULL;
    this->GlobalNodeIdArrayName = NULL;
    this->BifurcationIdArrayNameTmp = NULL;
    this->BifurcationIdArrayName = NULL;
    this->BranchIdArrayNameTmp = NULL;
    this->BranchIdArrayName = NULL;
    this->PathArrayName = NULL;
    this->CenterlineSectionAreaArrayName = NULL;
    this->CenterlineSectionMinSizeArrayName = NULL;
    this->CenterlineSectionMaxSizeArrayName = NULL;
    this->CenterlineSectionShapeArrayName = NULL;
    this->CenterlineSectionNormalArrayName = NULL;
    this->CenterlineSectionClosedArrayName = NULL;
    this->CenterlineSectionBifurcationArrayName = NULL;
}

vtkvmtkPolyDataCenterlineSections::~vtkvmtkPolyDataCenterlineSections()
{
    if (this->Centerlines)
    {
        this->Centerlines->Delete();
        this->Centerlines = NULL;
    }

    if (this->Surface)
    {
        this->Surface->Delete();
        this->Surface = NULL;
    }

    if (this->RadiusArrayName)
    {
        delete[] this->RadiusArrayName;
        this->RadiusArrayName = NULL;
    }

    if (this->GlobalNodeIdArrayName)
    {
        delete[] this->GlobalNodeIdArrayName;
        this->GlobalNodeIdArrayName = NULL;
    }

    if (this->BifurcationIdArrayNameTmp)
    {
        delete[] this->BifurcationIdArrayNameTmp;
        this->BifurcationIdArrayNameTmp = NULL;
    }

    if (this->BifurcationIdArrayName)
    {
        delete[] this->BifurcationIdArrayName;
        this->BifurcationIdArrayName = NULL;
    }

    if (this->BranchIdArrayNameTmp)
    {
        delete[] this->BranchIdArrayNameTmp;
        this->BranchIdArrayNameTmp = NULL;
    }

    if (this->BranchIdArrayName)
    {
        delete[] this->BranchIdArrayName;
        this->BranchIdArrayName = NULL;
    }

    if (this->PathArrayName)
    {
        delete[] this->PathArrayName;
        this->PathArrayName = NULL;
    }
    
    if (this->CenterlineSectionAreaArrayName)
    {
        delete[] this->CenterlineSectionAreaArrayName;
        this->CenterlineSectionAreaArrayName = NULL;
    }

    if (this->CenterlineSectionMinSizeArrayName)
    {
        delete[] this->CenterlineSectionMinSizeArrayName;
        this->CenterlineSectionMinSizeArrayName = NULL;
    }

    if (this->CenterlineSectionMaxSizeArrayName)
    {
        delete[] this->CenterlineSectionMaxSizeArrayName;
        this->CenterlineSectionMaxSizeArrayName = NULL;
    }

    if (this->CenterlineSectionShapeArrayName)
    {
        delete[] this->CenterlineSectionShapeArrayName;
        this->CenterlineSectionShapeArrayName = NULL;
    }

    if (this->CenterlineSectionClosedArrayName)
    {
        delete[] this->CenterlineSectionClosedArrayName;
        this->CenterlineSectionClosedArrayName = NULL;
    }

    if (this->CenterlineSectionBifurcationArrayName)
    {
        delete[] this->CenterlineSectionBifurcationArrayName;
        this->CenterlineSectionBifurcationArrayName = NULL;
    }

    if (this->CenterlineSectionNormalArrayName)
    {
        delete[] this->CenterlineSectionNormalArrayName;
        this->CenterlineSectionNormalArrayName = NULL;
    }
}

int vtkvmtkPolyDataCenterlineSections::RequestData(
        vtkInformation *vtkNotUsed(request),
        vtkInformationVector **inputVector,
        vtkInformationVector *outputVector)
{
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    vtkPolyData *input = vtkPolyData::SafeDownCast(
            inInfo->Get(vtkDataObject::DATA_OBJECT()));
    vtkPolyData *output = vtkPolyData::SafeDownCast(
            outInfo->Get(vtkDataObject::DATA_OBJECT()));

    if (!this->Centerlines)
    {
        vtkErrorMacro(<<"Centerlines not set");
        return 1;
    }

    if (!this->CenterlineSectionAreaArrayName)
    {
        vtkErrorMacro(<<"CenterlineSectionAreaArrayName not specified");
        return 1;
    }

    if (!CenterlineSectionMinSizeArrayName)
    {
        vtkErrorMacro(<<"CenterlineSectionMinSizeArrayName not specified");
        return 1;
    }

    if (!CenterlineSectionMaxSizeArrayName)
    {
        vtkErrorMacro(<<"CenterlineSectionMaxSizeArrayName not specified");
        return 1;
    }

    if (!CenterlineSectionShapeArrayName)
    {
        vtkErrorMacro(<<"CenterlineSectionShapeArrayName not specified");
        return 1;
    }

    if (!CenterlineSectionClosedArrayName)
    {
        vtkErrorMacro(<<"CenterlineSectionClosedArrayName not specified");
        return 1;
    }

    if (!CenterlineSectionBifurcationArrayName)
    {
        vtkErrorMacro(<<"CenterlineSectionBifurcationArrayName not specified");
        return 1;
    }

    if (!CenterlineSectionNormalArrayName)
    {
        vtkErrorMacro(<<"CenterlineSectionNormalArrayName not specified");
        return 1;
    }

    vtkPoints* outputPoints = vtkPoints::New();
    vtkCellArray* outputPolys = vtkCellArray::New();

    output->SetPoints(outputPoints);
    output->SetPolys(outputPolys);

    vtkDoubleArray* centerlineSectionAreaArray = vtkDoubleArray::New();
    centerlineSectionAreaArray->SetName(this->CenterlineSectionAreaArrayName);

    vtkDoubleArray* centerlineSectionShapeArray = vtkDoubleArray::New();
    centerlineSectionShapeArray->SetName(this->CenterlineSectionShapeArrayName);

    vtkDoubleArray* centerlineSectionMinSizeArray = vtkDoubleArray::New();
    centerlineSectionMinSizeArray->SetName(this->CenterlineSectionMinSizeArrayName);

    vtkDoubleArray* centerlineSectionMaxSizeArray = vtkDoubleArray::New();
    centerlineSectionMaxSizeArray->SetName(this->CenterlineSectionMaxSizeArrayName);

    vtkIntArray* centerlineSectionClosedArray = vtkIntArray::New();
    centerlineSectionClosedArray->SetName(this->CenterlineSectionClosedArrayName);

    vtkIntArray* centerlineSectionBifurcationArray = vtkIntArray::New();
    centerlineSectionBifurcationArray->SetName(this->CenterlineSectionBifurcationArrayName);

    vtkIntArray* centerlineSectionGlobalNodeIdArray = vtkIntArray::New();
    centerlineSectionGlobalNodeIdArray->SetName(this->GlobalNodeIdArrayName);

    vtkDoubleArray* centerlineAreaArray = vtkDoubleArray::New();
    centerlineAreaArray->SetName(this->CenterlineSectionAreaArrayName);

    vtkDoubleArray* centerlineShapeArray = vtkDoubleArray::New();
    centerlineShapeArray->SetName(this->CenterlineSectionShapeArrayName);

    vtkDoubleArray* centerlineMinSizeArray = vtkDoubleArray::New();
    centerlineMinSizeArray->SetName(this->CenterlineSectionMinSizeArrayName);

    vtkDoubleArray* centerlineMaxSizeArray = vtkDoubleArray::New();
    centerlineMaxSizeArray->SetName(this->CenterlineSectionMaxSizeArrayName);

    vtkDoubleArray* centerlineNormalArray = vtkDoubleArray::New();
    centerlineNormalArray->SetName(this->CenterlineSectionNormalArrayName);
    centerlineNormalArray->SetNumberOfComponents(3);

    vtkIntArray* centerlineClosedArray = vtkIntArray::New();
    centerlineClosedArray->SetName(this->CenterlineSectionClosedArrayName);

    vtkIntArray* centerlineBifurcationArray = vtkIntArray::New();
    centerlineBifurcationArray->SetName(this->CenterlineSectionBifurcationArrayName);

    output->GetCellData()->AddArray(centerlineSectionAreaArray);
    output->GetCellData()->AddArray(centerlineSectionMinSizeArray);
    output->GetCellData()->AddArray(centerlineSectionMaxSizeArray);
    output->GetCellData()->AddArray(centerlineSectionShapeArray);
    output->GetCellData()->AddArray(centerlineSectionClosedArray);
    output->GetCellData()->AddArray(centerlineSectionBifurcationArray);
    output->GetCellData()->AddArray(centerlineSectionGlobalNodeIdArray);

    // generate surface normals
    vtkPolyDataNormals* surfaceNormals = vtkPolyDataNormals::New();
    surfaceNormals->SetInputData(input);
    surfaceNormals->SplittingOff();
    surfaceNormals->AutoOrientNormalsOn();
    surfaceNormals->ComputePointNormalsOn();
    surfaceNormals->ConsistencyOn();
    surfaceNormals->Update();

    this->Surface->DeepCopy(surfaceNormals->GetOutput());
    surfaceNormals->Delete();

    // generate a clean, simply connected centerline
    this->GenerateCleanCenterline();

    // initialize normal array
    this->Centerlines->GetPointData()->AddArray(centerlineNormalArray);
    int numberOfCenterlinePoints = this->Centerlines->GetNumberOfPoints();
    centerlineNormalArray->SetNumberOfTuples(numberOfCenterlinePoints);

    // calculate centerline tangent vectors (= section normal vectors)
    this->CalculateTangent();

    // add additional points at caps to prevent bifurcations at caps
    this->RefineCapPoints();

    // initialize centerline arrays
    this->Centerlines->GetPointData()->AddArray(centerlineAreaArray);
    this->Centerlines->GetPointData()->AddArray(centerlineMinSizeArray);
    this->Centerlines->GetPointData()->AddArray(centerlineMaxSizeArray);
    this->Centerlines->GetPointData()->AddArray(centerlineShapeArray);
    this->Centerlines->GetPointData()->AddArray(centerlineClosedArray);
    this->Centerlines->GetPointData()->AddArray(centerlineBifurcationArray);

    numberOfCenterlinePoints = this->Centerlines->GetNumberOfPoints();

    centerlineAreaArray->SetNumberOfTuples(numberOfCenterlinePoints);
    centerlineMinSizeArray->SetNumberOfTuples(numberOfCenterlinePoints);
    centerlineMaxSizeArray->SetNumberOfTuples(numberOfCenterlinePoints);
    centerlineShapeArray->SetNumberOfTuples(numberOfCenterlinePoints);
    centerlineClosedArray->SetNumberOfTuples(numberOfCenterlinePoints);
    centerlineBifurcationArray->SetNumberOfTuples(numberOfCenterlinePoints);

    // preliminary color surface according to centerline BranchIdTmp to allow bifurcation detection
    this->BranchSurface(this->BranchIdArrayNameTmp, this->BifurcationIdArrayNameTmp);

    // slice centerline to get cross-sectional area and detect bifurcation regions
    this->ComputeCenterlineSections(output);

    // clean up centerline bifurcation detection
    this->CleanBifurcation();

    // split into bifurcations and branches
    this->GroupCenterline();

    // final color surface according to centerline BranchId and BifurcationId
    this->BranchSurface(this->BranchIdArrayName, this->BifurcationIdArrayName);
    this->BranchSurface(this->BifurcationIdArrayName, this->BranchIdArrayName);

    // delete temporary arrays
    this->Centerlines->GetPointData()->RemoveArray(this->BifurcationIdArrayNameTmp);
    this->Centerlines->GetPointData()->RemoveArray(this->BranchIdArrayNameTmp);
    this->Surface->GetPointData()->RemoveArray(this->BranchIdArrayNameTmp);

    outputPoints->Delete();
    outputPolys->Delete();

    centerlineSectionAreaArray->Delete();
    centerlineSectionMinSizeArray->Delete();
    centerlineSectionMaxSizeArray->Delete();
    centerlineSectionShapeArray->Delete();
    centerlineSectionClosedArray->Delete();
    centerlineSectionBifurcationArray->Delete();

    centerlineAreaArray->Delete();
    centerlineMinSizeArray->Delete();
    centerlineMaxSizeArray->Delete();
    centerlineShapeArray->Delete();
    centerlineNormalArray->Delete();
    centerlineClosedArray->Delete();
    centerlineBifurcationArray->Delete();

    return 1;
}


void vtkvmtkPolyDataCenterlineSections::CleanBifurcation()
{
    // modify this array to ensure bifurcations are only where they should be
    vtkIntArray* isBifurcation = vtkIntArray::SafeDownCast(this->Centerlines->GetPointData()->GetArray(this->CenterlineSectionBifurcationArrayName));

    // make sure geometrical bifurcation points are included
    vtkIntArray* bf_id = vtkIntArray::SafeDownCast(this->Centerlines->GetPointData()->GetArray(this->BifurcationIdArrayNameTmp));
    for (int i = 0; i < this->Centerlines->GetNumberOfPoints(); i++)
        if (bf_id->GetValue(i) == 1)
            isBifurcation->SetValue(i, 1);

    // preliminary split of the centerline
    vtkPolyData* bifurcations = vtkPolyData::New();
    vtkPolyData* branches = vtkPolyData::New();
    this->SplitCenterline(bifurcations, branches);

    // connectivity filter on bifurcations
    vtkConnectivityFilter* connect = vtkConnectivityFilter::New();
    connect->SetExtractionModeToAllRegions();
    connect->ColorRegionsOn();
    connect->SetInputData(bifurcations);
    connect->Update();

    // threshold each bifurcation
    vtkThreshold* thresh = vtkThreshold::New();
    thresh->SetInputData(connect->GetOutput());
    thresh->SetInputArrayToProcess(0, 0, 0, 1, "RegionId");

    // exclude bifurcations that are somewhere in the middle of a branch
    for (int i = 0; i < connect->GetNumberOfExtractedRegions(); i++)
    {
        // extract bifurcation
        thresh->ThresholdBetween(i, i);
        thresh->Update();

        // not a real bifurcation if it contains only one BranchId
        vtkIdList* branchIds = vtkIdList::New();
        for (int j = 0; j < thresh->GetOutput()->GetNumberOfPoints(); j++)
            branchIds->InsertUniqueId(thresh->GetOutput()->GetPointData()->GetArray(this->BranchIdArrayNameTmp)->GetTuple1(j));

        // remove bifurcation (convert to branch)
        if (branchIds->GetNumberOfIds() == 1)
            for (int j = 0; j < thresh->GetOutput()->GetNumberOfPoints(); j++)
                isBifurcation->SetValue(thresh->GetOutput()->GetPointData()->GetArray(this->GlobalNodeIdArrayName)->GetTuple1(j), 0);
    }

    // exclude branches that consist of a single point
    vtkIdList* pointCells = vtkIdList::New();
    vtkIdList* cellPoints = vtkIdList::New();

    bool remove;
    int bifurcationThis, bifurcationOthers;
    for (int i = 0; i < this->Centerlines->GetNumberOfPoints(); i++)
    {
        bifurcationThis = isBifurcation->GetTuple1(i);
        if (bifurcationThis == 0)
        {
            remove = true;

            // loop all connected cells
            this->Centerlines->GetPointCells(i, pointCells);
            for (int j = 0; j < pointCells->GetNumberOfIds(); j++)
            {
                // loop all connected points
                this->Centerlines->GetCellPoints(pointCells->GetId(j), cellPoints);
                for (int k = 0; k < cellPoints->GetNumberOfIds(); k++)
                {
                    // don't remove if there is at least one other connected branch point
                    bifurcationOthers = isBifurcation->GetTuple1(cellPoints->GetId(k));
                    if ((cellPoints->GetId(k) != i) && (bifurcationOthers == 0))
                        remove = false;
                }
            }
            if (remove)
                isBifurcation->SetValue(i, 1);
        }
    }

    // force two non-bifurcation points at caps
    isBifurcation->SetValue(0, 0);
    isBifurcation->SetValue(1, 0);
    for (int i = 1; i < this->Centerlines->GetNumberOfPoints(); i++)
    {
        this->Centerlines->GetPointCells(i, pointCells);
        if (pointCells->GetNumberOfIds() == 1)
        {
            isBifurcation->SetValue(i - 1, 0);
            isBifurcation->SetValue(i, 0);
        }
    }

}


void vtkvmtkPolyDataCenterlineSections::GroupCenterline()
{
    // split centerline in bifurcations and branches
    vtkPolyData* bifurcations = vtkPolyData::New();
    vtkPolyData* branches = vtkPolyData::New();
    this->SplitCenterline(bifurcations, branches);

    // enumerate all bifurcations and branches
    this->ConnectivityCenterline(bifurcations, this->BifurcationIdArrayName, this->BranchIdArrayName);
    this->ConnectivityCenterline(branches, this->BranchIdArrayName, this->BifurcationIdArrayName);

    // bring bifurcations and branches back together
    vtkAppendFilter* append = vtkAppendFilter::New();
    append->AddInputData(bifurcations);
    append->AddInputData(branches);
    append->MergePointsOn();
    append->Update();

    // convert vtkUnstructerdGrid to vtkPolyData
    vtkGeometryFilter* geo = vtkGeometryFilter::New();
    geo->SetInputData(append->GetOutput());
    geo->Update();

    // check if geometry is one piece
    if (!(this->IsOnePiece(geo->GetOutput())))
    {
        vtkErrorMacro(<< "Input centerline consists of more than one piece");
        return;
    }

    this->Centerlines->DeepCopy(geo->GetOutput());
    geo->Delete();

    // order nodes according to GlobalNodeId
    vtkPoints* points = vtkPoints::New();
    vtkCellArray* lines = vtkCellArray::New();

    vtkIntArray* globalIdArray = vtkIntArray::SafeDownCast(this->Centerlines->GetPointData()->GetArray(this->GlobalNodeIdArrayName));
    vtkIntArray* globalIdArrayInverse = vtkIntArray::SafeDownCast(this->Centerlines->GetPointData()->GetArray(this->GlobalNodeIdArrayName));
    vtkIdList* globalMap = vtkIdList::New();
    vtkIdList* globalMapInverse = vtkIdList::New();

    int local_id;
    double point[3];

    // create map
    for (int i=0; i < this->Centerlines->GetNumberOfPoints(); i++)
        globalMap->InsertNextId(globalIdArray->GetValue(i));

    // create inverse map
    for (int i=0; i < this->Centerlines->GetNumberOfPoints(); i++)
        globalIdArrayInverse->InsertValue(i, globalMap->GetId(i));

    // create new points
    for (int i=0; i < this->Centerlines->GetNumberOfPoints(); i++)
    {
        this->Centerlines->GetPoint(globalMap->IsId(i), point);
        points->InsertNextPoint(point);
    }

    // create new elements
    for (int i=0; i < this->Centerlines->GetNumberOfCells(); i++)
    {
    	vtkCell* cell = this->Centerlines->GetCell(i);
        vtkLine* line = vtkLine::New();
        for (int j=0; j < cell->GetNumberOfPoints(); j++)
            line->GetPointIds()->SetId(j, globalMap->GetId(cell->GetPointId(j)));
        lines->InsertNextCell(line);
    }

    // create new vtkPolyData
    vtkPolyData* polydata_ordered = vtkPolyData::New();
    polydata_ordered->SetPoints(points);
    polydata_ordered->SetLines(lines);
    polydata_ordered->Modified();

    // add arrays
    polydata_ordered->GetPointData()->DeepCopy(this->Centerlines->GetPointData());

    // sort arrays
    vtkSortDataArray* sort = vtkSortDataArray::New();
    vtkIntArray* sortArray = vtkIntArray::New();

    for (int i=0; i < polydata_ordered->GetPointData()->GetNumberOfArrays(); i++)
    {
        sortArray->DeepCopy(globalIdArrayInverse);
        sort->Sort(sortArray, polydata_ordered->GetPointData()->GetAbstractArray(i));
    }
    this->Centerlines->DeepCopy(polydata_ordered);
    polydata_ordered->Delete();
}


void vtkvmtkPolyDataCenterlineSections::SplitCenterline(vtkPolyData* bifurcations, vtkPolyData* branches)
{
    // map point data to cell data
    vtkPointDataToCellData* map = vtkPointDataToCellData::New();
    map->SetInputData(this->Centerlines);
    map->PassPointDataOn();
    map->Update();

    // threshold according to bifurcation cell array
    vtkThreshold* thresh = vtkThreshold::New();
    thresh->SetInputData(map->GetOutput());
    thresh->SetInputArrayToProcess(0, 0, 0, 1, this->CenterlineSectionBifurcationArrayName);

    for (int i=0; i < 2; i++)
    {
        thresh->ThresholdBetween(i, i);
        thresh->Update();

        // convert vtkUnstructerdGrid to vtkPolyData
        vtkGeometryFilter* geo = vtkGeometryFilter::New();
        geo->SetInputData(thresh->GetOutput());
        geo->Update();

        switch (i)
        {
        case 0:
            branches->DeepCopy(geo->GetOutput());
            break;
        case 1:
            bifurcations->DeepCopy(geo->GetOutput());
            break;
        }
        geo->Delete();
    }
    thresh->Delete();
    map->Delete();
}


void vtkvmtkPolyDataCenterlineSections::ConnectivityCenterline(vtkPolyData* geo, char* nameThis, char* nameOther)
{
    // color geometry by connectivity
    vtkConnectivityFilter* connect = vtkConnectivityFilter::New();
    connect->SetInputData(geo);
    connect->SetExtractionModeToAllRegions();
    connect->ColorRegionsOn();
    connect->Update();
    vtkPolyData* connected = vtkPolyData::SafeDownCast(connect->GetOutput());

    // map connect points to global id
    vtkIntArray* connectGlobalId = vtkIntArray::SafeDownCast(connected->GetPointData()->GetArray(this->GlobalNodeIdArrayName));
    vtkIdList* connectToGlobal = vtkIdList::New();
    for (int j = 0; j < connected->GetNumberOfPoints(); j++)
        connectToGlobal->InsertNextId(connectGlobalId->GetValue(j));

    // add path array for each segment to geometry
    vtkDataArray* regionId = connected->GetPointData()->GetArray("RegionId");
    vtkDoubleArray* path = vtkDoubleArray::New();
    path->SetNumberOfValues(connected->GetNumberOfPoints());
    path->SetName(this->PathArrayName);
    path->Fill(-1);
    connected->GetPointData()->AddArray(path);

    // threshold according to RegionId cells
    vtkThreshold* thresh = vtkThreshold::New();
    thresh->SetInputData(connected);
    thresh->SetInputArrayToProcess(0, 0, 0, 1, "RegionId");

    int k;
    double dist = 0.0;
    double p0[3], p1[3], vec[3];
    bool found_first;

    for (int i = 0; i < connect->GetNumberOfExtractedRegions(); i++)
    {
        // extract segment
        thresh->ThresholdBetween(i, i);
        thresh->Update();

        // map thresh points to global id
        vtkIntArray* threshGlobalId = vtkIntArray::SafeDownCast(thresh->GetOutput()->GetPointData()->GetArray(this->GlobalNodeIdArrayName));
        vtkIdList* threshToGlobal = vtkIdList::New();
        for (int j = 0; j < thresh->GetOutput()->GetNumberOfPoints(); j++)
            threshToGlobal->InsertNextId(threshGlobalId->GetValue(j));

        // loop according to GlobalNodeId to ensure correct numbering of points
        dist = 0.0;
        found_first = false;
        for (int j = 0; j < int(threshGlobalId->GetMaxNorm() + 1.5); j++)
        {
            // convert to thresh id, skip if not present
            k = threshToGlobal->IsId(j);
            if (k == -1)
                continue;

            if (!found_first)
            {
                // get first point
                thresh->GetOutput()->GetPoint(k, p0);
                found_first = true;
            }
            else
            {
                // calculate distance to previous point
                thresh->GetOutput()->GetPoint(k, p1);
                vtkMath::Subtract(p1, p0, vec);
                dist += vtkMath::Norm(vec);

                for (int l = 0; l < 3; l++)
                    p0[l] = p1[l];
            }

            path->SetValue(connectToGlobal->IsId(j), dist);
        }
    }

    // rename RegionId
    geo->DeepCopy(connected);
    connect->Delete();
    geo->GetPointData()->GetArray("RegionId")->SetName(nameThis);
    geo->GetCellData()->GetArray("RegionId")->SetName(nameThis);

    // add empty array
    vtkIdTypeArray* idsPoints = vtkIdTypeArray::New();
    vtkIdTypeArray* idsCell = vtkIdTypeArray::New();
    idsPoints->SetNumberOfValues(geo->GetNumberOfPoints());
    idsCell->SetNumberOfValues(geo->GetNumberOfCells());
    idsPoints->SetName(nameOther);
    idsCell->SetName(nameOther);
    idsPoints->Fill(-1);
    idsCell->Fill(-1);
    geo->GetPointData()->AddArray(idsPoints);
    geo->GetCellData()->AddArray(idsCell);
}

void vtkvmtkPolyDataCenterlineSections::ComputeCenterlineSections(vtkPolyData* output)
{
    vtkPoints* centerlineSectionPoints = output->GetPoints();
    vtkCellArray* centerlineSectionPolys = output->GetPolys();

    vtkDoubleArray* centerlineSectionAreaArray = vtkDoubleArray::SafeDownCast(output->GetCellData()->GetArray(this->CenterlineSectionAreaArrayName));
    vtkDoubleArray* centerlineSectionMinSizeArray = vtkDoubleArray::SafeDownCast(output->GetCellData()->GetArray(this->CenterlineSectionMinSizeArrayName));
    vtkDoubleArray* centerlineSectionMaxSizeArray = vtkDoubleArray::SafeDownCast(output->GetCellData()->GetArray(this->CenterlineSectionMaxSizeArrayName));
    vtkDoubleArray* centerlineSectionShapeArray = vtkDoubleArray::SafeDownCast(output->GetCellData()->GetArray(this->CenterlineSectionShapeArrayName));
    vtkIntArray* centerlineSectionClosedArray = vtkIntArray::SafeDownCast(output->GetCellData()->GetArray(this->CenterlineSectionClosedArrayName));
    vtkIntArray* centerlineSectionBifurcationArray = vtkIntArray::SafeDownCast(output->GetCellData()->GetArray(this->CenterlineSectionBifurcationArrayName));
    vtkIntArray* centerlineSectionGlobalNodeIdArray = vtkIntArray::SafeDownCast(output->GetCellData()->GetArray(this->GlobalNodeIdArrayName));

    vtkDoubleArray* centerlineAreaArray = vtkDoubleArray::SafeDownCast(this->Centerlines->GetPointData()->GetArray(this->CenterlineSectionAreaArrayName));
    vtkDoubleArray* centerlineMinSizeArray = vtkDoubleArray::SafeDownCast(this->Centerlines->GetPointData()->GetArray(this->CenterlineSectionMinSizeArrayName));
    vtkDoubleArray* centerlineMaxSizeArray = vtkDoubleArray::SafeDownCast(this->Centerlines->GetPointData()->GetArray(this->CenterlineSectionMaxSizeArrayName));
    vtkDoubleArray* centerlineShapeArray = vtkDoubleArray::SafeDownCast(this->Centerlines->GetPointData()->GetArray(this->CenterlineSectionShapeArrayName));
    vtkDoubleArray* centerlineNormalArray = vtkDoubleArray::SafeDownCast(this->Centerlines->GetPointData()->GetArray(this->CenterlineSectionNormalArrayName));
    vtkIntArray* centerlineClosedArray = vtkIntArray::SafeDownCast(this->Centerlines->GetPointData()->GetArray(this->CenterlineSectionClosedArrayName));
    vtkIntArray* centerlineBifurcationArray = vtkIntArray::SafeDownCast(this->Centerlines->GetPointData()->GetArray(this->CenterlineSectionBifurcationArrayName));

    vtkIdList* cellIds = vtkIdList::New();
    double point[3], tangent[3];

    // loop all centerline points
    for (int p = 0; p < this->Centerlines->GetNumberOfPoints(); p++)
    {
        // get centerline point and tangent (= section normal)
        this->Centerlines->GetPoint(p, point);
        centerlineNormalArray->GetTuple(p, tangent);

        // get number of cells connected to this point
        this->Centerlines->GetPointCells(p, cellIds);

        // at caps, move point eps_norm inward to nicely cut the geometry
        if (cellIds->GetNumberOfIds() == 1)
        {
            double eps_norm = 1.0e-3;
            if (p == 0)
                eps_norm *= -1.0;
            for (int j=0; j<3; j++)
                point[j] -= eps_norm * tangent[j];
        }

        // slice surface geometry
        vtkPolyData* section = vtkPolyData::New();
        bool closed = false;
        vtkvmtkPolyDataBranchSections::ExtractCylinderSection(this->Surface,point,tangent,section,closed);

        // build slice geometry
        section->BuildCells();
        vtkPoints* sectionCellPoints = section->GetCell(0)->GetPoints();
        int numberOfSectionCellPoints = sectionCellPoints->GetNumberOfPoints();
        centerlineSectionPolys->InsertNextCell(numberOfSectionCellPoints);
        int k;
        for (k=0; k<numberOfSectionCellPoints; k++)
        {
            vtkIdType branchPointId = centerlineSectionPoints->InsertNextPoint(sectionCellPoints->GetPoint(k));
            centerlineSectionPolys->InsertCellPoint(branchPointId);
        }

        // read properties of slice
        double area, shape, sizeRange[2];
        int intersectCenterline, intersectSurface;
        int bifurcation = 1;

        area = vtkvmtkPolyDataBranchSections::ComputeBranchSectionArea(section);
        shape = vtkvmtkPolyDataBranchSections::ComputeBranchSectionShape(section,point,sizeRange);
        intersectCenterline = vtkvmtkPolyDataBranchSections::ComputeBranchCenterlineIntersections(section,this->Centerlines,point,tangent);
        intersectSurface = vtkvmtkPolyDataBranchSections::ComputeBranchSurfaceIntersections(section,this->BranchIdArrayNameTmp);
        if((intersectCenterline == 1) && (intersectSurface == 1))
            bifurcation = 0;

        // store in sections
        centerlineSectionAreaArray->InsertNextValue(area);
        centerlineSectionMinSizeArray->InsertNextValue(sizeRange[0]);
        centerlineSectionMaxSizeArray->InsertNextValue(sizeRange[1]);
        centerlineSectionShapeArray->InsertNextValue(shape);
        centerlineSectionClosedArray->InsertNextValue(closed);
        centerlineSectionBifurcationArray->InsertNextValue(bifurcation);
        centerlineSectionGlobalNodeIdArray->InsertNextValue(p);

        // store in centerline
        centerlineAreaArray->InsertValue(p,area);
        centerlineMinSizeArray->InsertValue(p,sizeRange[0]);
        centerlineMaxSizeArray->InsertValue(p,sizeRange[1]);
        centerlineShapeArray->InsertValue(p,shape);
        centerlineClosedArray->InsertValue(p,closed);
        centerlineBifurcationArray->InsertValue(p,bifurcation);
        section->Delete();
    }
}


void vtkvmtkPolyDataCenterlineSections::BranchSurface(char* nameThis, char* nameOther)
{
    // output id array
    vtkIntArray* thisSurf = vtkIntArray::New();
    thisSurf->SetName(nameThis);
    thisSurf->SetNumberOfValues(this->Surface->GetNumberOfPoints());
    thisSurf->Fill(-1);
    this->Surface->GetPointData()->AddArray(thisSurf);

    // output distance array
    vtkDoubleArray* surfDist = vtkDoubleArray::New();
    surfDist->SetNumberOfValues(this->Surface->GetNumberOfPoints());
    surfDist->Fill(-1);

    // build locator for surface points
    vtkPolyData *dataset = vtkPolyData::New();
    dataset->SetPoints(this->Surface->GetPoints());

    vtkPointLocator* locator = vtkPointLocator::New();
    locator->Initialize();
    locator->SetDataSet(dataset);
    locator->BuildLocator();

    // get arrays
    vtkDoubleArray* centRadius = vtkDoubleArray::SafeDownCast(this->Centerlines->GetPointData()->GetArray(this->RadiusArrayName));
    vtkDataArray* normals = vtkDataArray::SafeDownCast(this->Surface->GetPointData()->GetArray("Normals"));
    vtkDataArray* otherCent = vtkDataArray::SafeDownCast(this->Centerlines->GetPointData()->GetArray(nameOther));
    vtkDataArray* thisCent = vtkDataArray::SafeDownCast(this->Centerlines->GetPointData()->GetArray(nameThis));

    vtkIdList* cellIds = vtkIdList::New();
    vtkIdList* surfPointIds = vtkIdList::New();
    int thisId, otherId;
    double p_cent[3], p_surf[3], normal[3], diff[3];
    double radius, dist, dist_n;

    for (int i = 0; i < this->Centerlines->GetNumberOfPoints(); i++)
    {
        thisId = thisCent->GetTuple1(i);
        otherId = otherCent->GetTuple1(i);

        // skip bifurcation points
        if (otherId != -1)
            continue;

        // pre-select surface points within sphere (very fast)
        this->Centerlines->GetPoint(i, p_cent);
        radius = centRadius->GetValue(i);
        locator->FindPointsWithinRadius(10.0 * radius, p_cent, surfPointIds);

        // select surface points according to distances (slow)
        for (int j = 0; j < surfPointIds->GetNumberOfIds(); j++)
        {
            // get surface point and normal
            this->Surface->GetPoint(surfPointIds->GetId(j), p_surf);
            normals->GetTuple(surfPointIds->GetId(j), normal);

            // distance vector between surface and centerline points
            vtkMath::Subtract(p_surf, p_cent, diff);
            dist = vtkMath::Norm(diff);

            // signed distance in surface normal direction
            dist_n = vtkMath::Dot(diff, normal);

            // check if centerline is inside branch (allow small tolerance for caps)
            if (-1.0e-2 <= dist_n)
            {
                // if surface point already has an id from closer centerline, skip
                if (-1 < thisSurf->GetValue(surfPointIds->GetId(j)))
                    if (dist > surfDist->GetValue(surfPointIds->GetId(j)))
                        continue;

                // set BranchId and distance
                thisSurf->SetValue(surfPointIds->GetId(j), thisId);
                surfDist->SetValue(surfPointIds->GetId(j), dist);
            }
        }
    }
}


void vtkvmtkPolyDataCenterlineSections::GenerateCleanCenterline()
{
    // remove duplicate points
    vtkCleanPolyData* cleaner = vtkCleanPolyData::New();
    cleaner->SetInputData(this->Centerlines);
    cleaner->PointMergingOn();
    cleaner->Update();
    vtkPolyData* centerlines = cleaner->GetOutput();

    // check if geometry is one piece
    if (!(this->IsOnePiece(cleaner->GetOutput())))
    {
        vtkErrorMacro(<< "Input centerline consists of more than one piece");
        return;
    }

    // build connected centerline geometry
    vtkPoints* points = vtkPoints::New();
    vtkCellArray* lines = vtkCellArray::New();

    // keep track of added points
    vtkIdList* pointIds = vtkIdList::New();
    pointIds->Initialize();

    // copy radius array from centerline
    vtkDoubleArray* radius = vtkDoubleArray::New();
    radius->SetName(this->RadiusArrayName);
    radius->SetNumberOfValues(centerlines->GetNumberOfPoints());
    radius->Fill(0.0);

    // create unique id for each point
    vtkIntArray* nodeId = vtkIntArray::New();
    nodeId->SetName(this->GlobalNodeIdArrayName);
    nodeId->SetNumberOfValues(centerlines->GetNumberOfPoints());
    nodeId->Fill(0);

    // add inlet point
    points->InsertNextPoint(centerlines->GetPoint(0));
    pointIds->InsertNextId(0);
    radius->SetValue(0, centerlines->GetPointData()->GetArray(this->RadiusArrayName)->GetTuple1(0));

    // loop all centerlines
    for (int c = 0; c < centerlines->GetNumberOfCells(); c++)
    {
        // individual centerline
        vtkCell* cell = centerlines->GetCell(c);

        // loop through all points in centerline (assumes points are ordered consecutively)
        for (int p = 0; p < cell->GetNumberOfPoints(); p++)
        {
            int i = cell->GetPointId(p);

            // add point and line only if point hasn't been added yet
            if ((pointIds->IsId(i) == -1) && (i > 0))
            {
                // add point
                points->InsertNextPoint(centerlines->GetPoint(i));
                pointIds->InsertNextId(i);

                // add line connecting this point and previous point
                int id_prev = pointIds->IsId(cell->GetPointId(p - 1));
                int id_this = pointIds->IsId(cell->GetPointId(p));
                vtkLine* line = vtkLine::New();
                line->GetPointIds()->SetId(0, id_prev);
                line->GetPointIds()->SetId(1, id_this);
                lines->InsertNextCell(line);

                // copy radius
                radius->SetValue(id_this, centerlines->GetPointData()->GetArray(this->RadiusArrayName)->GetTuple1(i));

                // set id
                nodeId->SetValue(id_this, pointIds->GetNumberOfIds());
            }
        }
    }

    // create polydata
    vtkPolyData* polydata = vtkPolyData::New();
    polydata->SetPoints(points);
    polydata->SetLines(lines);
    polydata->Modified();

    // check mesh consistency
    if (polydata->GetNumberOfPoints() != cleaner->GetOutput()->GetNumberOfPoints())
    {
        vtkErrorMacro(<< "Number of points mismatch");
        return;
    }
    if (polydata->GetNumberOfPoints() != polydata->GetNumberOfCells() + 1)
    {
        vtkErrorMacro(<< "Number of cells mismatch");
        return;
    }

    // add preliminary arrays for branches and bifurcations based on element connectivity
    vtkIntArray* bifurcation = vtkIntArray::New();
    vtkIntArray* branch = vtkIntArray::New();
    bifurcation->SetName(this->BifurcationIdArrayNameTmp);
    branch->SetName(this->BranchIdArrayNameTmp);
    bifurcation->SetNumberOfValues(polydata->GetNumberOfPoints());
    branch->SetNumberOfValues(polydata->GetNumberOfPoints());
    bifurcation->Fill(-1);
    branch->Fill(-1);

    // add arrays to centerline
    polydata->GetPointData()->AddArray(bifurcation);
    polydata->GetPointData()->AddArray(branch);
    polydata->GetPointData()->AddArray(radius);
    polydata->GetPointData()->AddArray(nodeId);

    // go through tree and color each branch/bifurcation
    vtkIdList* cellIds = vtkIdList::New();
    int branchId = 0;
    for (int p = 0; p < polydata->GetNumberOfPoints(); p++)
    {
        // set BranchId
        branch->SetValue(p, branchId);

        // get number of cells connected to this point
        polydata->GetPointCells(p, cellIds);
        if ((cellIds->GetNumberOfIds() == 1) && (p != 0))
            // outlet
            branchId++;
        else if (cellIds->GetNumberOfIds() > 2)
        {
            // bifurcation point
            bifurcation->SetValue(p, 1);
            branchId++;
        }
    }

    // build locator for centerline points
    vtkPolyData *dataset = vtkPolyData::New();
    dataset->SetPoints(polydata->GetPoints());

    vtkPointLocator* locator = vtkPointLocator::New();
    locator->Initialize();
    locator->SetDataSet(dataset);
    locator->BuildLocator();

    vtkIdList* pointCells = vtkIdList::New();
    vtkIdList* cellPoints = vtkIdList::New();
    vtkIdList* closePoints = vtkIdList::New();

    double point[3];

    // mark points within one sphere radius downstream of bifurcation as bifurcation
    for (int i = 0; i < polydata->GetNumberOfPoints(); i++)
    {
        // check if point is a bifurcation
        polydata->GetPointCells(i, pointCells);
        if (pointCells->GetNumberOfIds() > 2)
        {
            // get radius of this bifurcation
            double radius = polydata->GetPointData()->GetArray(this->RadiusArrayName)->GetTuple1(i);

            // find centerline points within sphere-distance
            polydata->GetPoint(i, point);
            locator->FindPointsWithinRadius(radius, point, closePoints);

            // get upstream Branch Id
            int branchIdUpstream = branch->GetValue(i);

            // get downstream Branch Ids
            vtkIdList* branchIdsDownstream = vtkIdList::New();
            for (int j = 0; j < pointCells->GetNumberOfIds(); j++)
            {
                // loop all points attached to cell
                polydata->GetCellPoints(pointCells->GetId(j), cellPoints);
                for (int k = 0; k < cellPoints->GetNumberOfIds(); k++)
                {
                    // add branch id of down-stream branches
                    int branchId = branch->GetValue(cellPoints->GetId(k));
                    if (branchId != branchIdUpstream)
                        branchIdsDownstream->InsertUniqueId(branchId);
                }
            }

            // loop all downstream branches
            for (int j = 0; j < branchIdsDownstream->GetNumberOfIds(); j++)
                for (int k = 0; k < polydata->GetNumberOfPoints(); k++)
                    if ((branch->GetValue(k) == branchIdsDownstream->GetId(j)) && (closePoints->IsId(k) > -1))
                        bifurcation->SetValue(k, 2);
        }
    }

    // mark points within two 2 * sphere radius of caps for smoothing
    vtkIntArray* smoothing = vtkIntArray::New();
    smoothing->SetNumberOfValues(polydata->GetNumberOfPoints());
    smoothing->Fill(0);

    for (int i = 0; i < polydata->GetNumberOfPoints(); i++)
    {
        // check if point is a bifurcation
        polydata->GetPointCells(i, pointCells);
        if (pointCells->GetNumberOfIds() == 1)
        {
            // get radius of this cap
            double radius = polydata->GetPointData()->GetArray(this->RadiusArrayName)->GetTuple1(i);

            // find centerline points within sphere-distance
            polydata->GetPoint(i, point);
            locator->FindPointsWithinRadius(2.0 * radius, point, closePoints);

            // get cap Branch Id
            int branchIdCap = branch->GetValue(i);

            // loop all downstream branches
            for (int k = 0; k < polydata->GetNumberOfPoints(); k++)
                if ((branch->GetValue(k) == branchIdCap) && (closePoints->IsId(k) > -1))
                    smoothing->SetValue(k, 1);
        }
    }

    // apply moving average filter to individual branches
    const int numberOfIterations = 200;
    const double relaxation_caps = 1.0;
    const double relaxation_rest = 0.01;

    double point0[3], point1[3], point2[3], diff[3];
    double dist_t;

    for (int j = 0; j < numberOfIterations; j++)
        for (int i = 0; i < branchId; i++)
            for (int k = 1; k < polydata->GetNumberOfPoints() - 1; k++)
                // check if all three points are within branch
                if ((branch->GetValue(k - 1) == i) && (branch->GetValue(k) == i) && (branch->GetValue(k + 1) == i))
                {
                    // select point and surroundings
                    points->GetPoint(k - 1, point0);
                    points->GetPoint(k    , point1);
                    points->GetPoint(k + 1, point2);

                    // point displacement vector
                    for (int l = 0; l < 3; l++)
                        diff[l] = (0.5 * (point0[l] + point2[l]) - point1[l]);

                    // check if cap smoothing should be applied
                    if (smoothing->GetValue(k) > 0)
                    {
                        double tangent[3] = {0.0, 0.0, 0.0};

                        // tangent vector of centerline
                        double distance01 = sqrt(vtkMath::Distance2BetweenPoints(point0,point1));
                        double distance12 = sqrt(vtkMath::Distance2BetweenPoints(point1,point2));
                        for (int j=0; j<3; j++)
                        {
                            tangent[j] += (point1[j] - point0[j]) / distance01;
                            tangent[j] += (point2[j] - point1[j]) / distance12;
                        }
                        vtkMath::Normalize(tangent);

                        // signed distance in centerline normal direction
                        dist_t = vtkMath::Dot(diff, tangent);

                        // add displacement only in normal direction
                        for (int l = 0; l < 3; l++)
                            point1[l] += relaxation_caps * (diff[l] - dist_t * tangent[l]);
                    }
                    else
                    {
                        // add total displacement
                        for (int l = 0; l < 3; l++)
                            point1[l] += relaxation_rest * diff[l];
                    }

                    // replace point
                    points->SetPoint(k, point1);
                }

    this->Centerlines->DeepCopy(polydata);
    polydata->Delete();
}


void vtkvmtkPolyDataCenterlineSections::CalculateTangent()
{

    // build locator for surface points
    vtkPolyData *dataset = vtkPolyData::New();
    dataset->SetPoints(this->Surface->GetPoints());

    vtkPointLocator* locator = vtkPointLocator::New();
    locator->Initialize();
    locator->SetDataSet(dataset);
    locator->BuildLocator();

    // initialize
    vtkIdList* cellIds = vtkIdList::New();

    double point[3], point0[3], point1[3];
    double distance;
    int id;

    vtkDoubleArray* centerlineNormalArray = vtkDoubleArray::SafeDownCast(this->Centerlines->GetPointData()->GetArray(this->CenterlineSectionNormalArrayName));

    // loop all centerline points
    for (int p = 0; p < this->Centerlines->GetNumberOfPoints(); p++)
    {
        // get number of cells connected to this point
        this->Centerlines->GetPointCells(p, cellIds);
        this->Centerlines->GetPoint(p, point);

        double tangent[3] = {0.0, 0.0, 0.0};

        // cap point: normal from surface
        if (cellIds->GetNumberOfIds() == 1)
        {
            // get corresponding surface point
            id = locator->FindClosestPoint(point);

            // tangent = cap normal
            this->Surface->GetPointData()->GetArray("Normals")->GetTuple(id, tangent);

            // move point eps_norm inward to nicely cut the geometry
            const double eps_norm = 1.0e-3;
            for (int j=0; j<3; j++)
                point[j] -= eps_norm * tangent[j];

            // flip inlet tangent for consistency
            if (p == 0)
                vtkMath::MultiplyScalar(tangent, -1.0);
        }
        // interior point: normal from finite difference on centerline
        else
        {
            // calculate tangent on each connected centerline cell
            for (int c = 0; c < cellIds->GetNumberOfIds(); c++)
            {
            	vtkCell* cell = this->Centerlines->GetCell(cellIds->GetId(c));
                this->Centerlines->GetPoint(cell->GetPointId(0), point0);
                this->Centerlines->GetPoint(cell->GetPointId(1), point1);
                distance = sqrt(vtkMath::Distance2BetweenPoints(point0,point1));
                for (int j=0; j<3; j++)
                    tangent[j] += (point1[j] - point0[j]) / distance;
            }
            vtkMath::Normalize(tangent);
        }
        centerlineNormalArray->InsertTuple(p, tangent);
    }
}

void vtkvmtkPolyDataCenterlineSections::RefineCapPoints()
{
    vtkPolyData* polydata = this->Centerlines;

    // count number of caps
    vtkIdList* cellIds = vtkIdList::New();
    int n_cap = 0;
    for (int i = 0; i < polydata->GetNumberOfPoints(); i++)
    {
        polydata->GetPointCells(i, cellIds);
        if (cellIds->GetNumberOfIds() == 1)
            n_cap++;
    }

    // get old arrays
    vtkDoubleArray* radius = vtkDoubleArray::SafeDownCast(polydata->GetPointData()->GetArray(this->RadiusArrayName));
    vtkDoubleArray* normals = vtkDoubleArray::SafeDownCast(polydata->GetPointData()->GetArray(this->CenterlineSectionNormalArrayName));
    vtkIntArray* bifurcation = vtkIntArray::SafeDownCast(polydata->GetPointData()->GetArray(this->BifurcationIdArrayNameTmp));
    vtkIntArray* branch = vtkIntArray::SafeDownCast(polydata->GetPointData()->GetArray(this->BranchIdArrayNameTmp));

    // create new arrays
    vtkDoubleArray* radius_new = vtkDoubleArray::New();
    vtkDoubleArray* normals_new = vtkDoubleArray::New();
    vtkIntArray* bifurcation_new = vtkIntArray::New();
    vtkIntArray* branch_new = vtkIntArray::New();
    vtkIntArray* nodeId_new = vtkIntArray::New();

    radius_new->SetName(this->RadiusArrayName);
    normals_new->SetName(this->CenterlineSectionNormalArrayName);
    bifurcation_new->SetName(this->BifurcationIdArrayNameTmp);
    branch_new->SetName(this->BranchIdArrayNameTmp);
    nodeId_new->SetName(this->GlobalNodeIdArrayName);

    radius_new->SetNumberOfValues(polydata->GetNumberOfPoints() + n_cap);
    normals_new->SetNumberOfValues(polydata->GetNumberOfPoints() + n_cap);
    bifurcation_new->SetNumberOfValues(polydata->GetNumberOfPoints() + n_cap);
    branch_new->SetNumberOfValues(polydata->GetNumberOfPoints() + n_cap);
    nodeId_new->SetNumberOfValues(polydata->GetNumberOfPoints() + n_cap);

    normals_new->SetNumberOfComponents(3);

    radius_new->Fill(0.0);
    bifurcation_new->Fill(-1);
    branch_new->Fill(-1);
    nodeId_new->Fill(0);

    // create new points
    vtkPoints* points = vtkPoints::New();
    vtkCellArray* lines = vtkCellArray::New();
    vtkIdList* inserted = vtkIdList::New();

    double point_c[3], point_i[3], point_new[3], tangent_c[3], tangent_i[3], tangent_new[3];
    double radius_c, radius_i;
    int sign, i_new, i_cap;

    // loop all centerline points
    n_cap = 0;
    for (int i = 0; i < polydata->GetNumberOfPoints(); i++)
    {
        // get number of cells connected to this point
        polydata->GetPointCells(i, cellIds);

        // get cap point and tangent
        polydata->GetPoint(i, point_c);
        normals->GetTuple(i, tangent_c);

        // cap point: insert new point and cell
        if (cellIds->GetNumberOfIds() == 1)
        {
            // select indices depending on if cap is inlet or outlet
            if (i == 0)
            {
                sign = -1;
                i_new = i + n_cap + 1;
                i_cap = i + n_cap;
            }
            else
            {
                sign = 1;
                i_new = i + n_cap;
                i_cap = i + n_cap + 1;
            }

            // get interior point and tangent
            polydata->GetPoint(i - sign, point_i);
            normals->GetTuple(i - sign, tangent_i);

            // create new point and tangent
            for (int j=0; j<3; j++)
            {
                // linear interpolation
                point_new[j] = 0.5 * (point_c[j] + point_i[j]);
                tangent_new[j] = 0.5 * (tangent_c[j] + tangent_i[j]);
            }
            vtkMath::Normalize(tangent_new);

            // insert array values
            points->InsertPoint(i_cap, point_c);
            points->InsertPoint(i_new, point_new);

            normals_new->InsertTuple(i_cap, tangent_c);
            normals_new->InsertTuple(i_new, tangent_new);

            radius_new->SetValue(i_cap, radius->GetTuple1(i));
            radius_new->SetValue(i_new, 0.5 * (radius->GetTuple1(i) + radius->GetTuple1(i - sign)));

            // insert new element
            vtkLine* line = vtkLine::New();
            if (i == 0)
            {
                line->GetPointIds()->SetId(0, 0);
                line->GetPointIds()->SetId(1, 1);
            }
            else
            {
                for (int j=0; j<2; j++)
                    line->GetPointIds()->SetId(j, polydata->GetCell(i - 1)->GetPointId(j) + n_cap);

                inserted->InsertId(i - 1, i + n_cap);
            }
            lines->InsertNextCell(line);

            // set new arrays
            bifurcation_new->SetValue(i + n_cap, bifurcation->GetValue(i));
            branch_new->SetValue(i + n_cap, branch->GetValue(i));
            nodeId_new->SetValue(i + n_cap, i + n_cap);

            n_cap++;
        }
        // insert old point
        else
        {
            points->InsertNextPoint(point_c);
            radius_new->SetValue(i + n_cap, radius->GetTuple1(i));
            normals_new->InsertTuple(i + n_cap, tangent_c);
        }

        // insert old cell
        inserted->InsertNextId(i + n_cap);
        if (i > 0)
        {
            vtkLine* line = vtkLine::New();
            for (int j=0; j<2; j++)
                line->GetPointIds()->SetId(j, inserted->GetId(polydata->GetCell(i - 1)->GetPointId(j)));
            lines->InsertNextCell(line);
        }

        // copy to new arrays
        bifurcation_new->SetValue(i + n_cap, bifurcation->GetValue(i));
        branch_new->SetValue(i + n_cap, branch->GetValue(i));
        nodeId_new->SetValue(i + n_cap, i + n_cap);
    }

    // create new polydata
    vtkPolyData* polydata_new = vtkPolyData::New();
    polydata_new->SetPoints(points);
    polydata_new->SetLines(lines);
    polydata_new->Modified();

    polydata_new->GetPointData()->AddArray(bifurcation_new);
    polydata_new->GetPointData()->AddArray(branch_new);
    polydata_new->GetPointData()->AddArray(radius_new);
    polydata_new->GetPointData()->AddArray(nodeId_new);
    polydata_new->GetPointData()->AddArray(normals_new);

    polydata->DeepCopy(polydata_new);
    polydata_new->Delete();
}

bool vtkvmtkPolyDataCenterlineSections::IsOnePiece(vtkPolyData* inp)
{
    // check if geometry is one piece
    vtkConnectivityFilter* connectivity = vtkConnectivityFilter::New();
    connectivity->SetInputData(inp);
    connectivity->SetExtractionModeToAllRegions();
    connectivity->ColorRegionsOn();
    connectivity->Update();

    return connectivity->GetNumberOfExtractedRegions() == 1;
}


void vtkvmtkPolyDataCenterlineSections::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);
}

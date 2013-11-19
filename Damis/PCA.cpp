///////////////////////////////////////////////////////////
//  PCA.cpp
//  Implementation of the Class PCA
//  Created on:      07-Lie-2013 20:07:31
//  Original author: Povilas
///////////////////////////////////////////////////////////

#include "PCA.h"
#include "Statistics.h"
#include "alglib/dataanalysis.h"

PCA::PCA(){

}

PCA::~PCA(){

}

/**
 * Constructor that sets desired projection d
 */
PCA::PCA(int dim){
    setProjectionDimension(dim);
    initializeProjectionMatrix();
    ProjectXMatrix();
}

/**
 * Overloaded constructor when the part of retaining dispersion is passed to PCA
 * object
 */
PCA::PCA(double disp){
    dispPart = disp;
    int n = X.getObjectCount();
    int m = X.getObjectAt(0).getFeatureCount();
    int dd = 0;
    PCA pca(X, m);
    ObjectMatrix Y_visi = pca.getProjection();
    std::vector<double> sum;
    sum.reserve(m);
    double wholeSum = 0.0, tempSum = 0.0;
    
    for (int i = 0; i < m; i++)
        sum.push_back(Statistics::getSum(Y_visi, i));
    
    for (int i = 0; i < m; i++)
        wholeSum += sum.at(i);
    
    for (int i = 0; i < m; i++)
    {
        tempSum += sum.at(i);
        dd++;
        if ((tempSum / wholeSum) > disp)
            break;           
    }
    
    setProjectionDimension(dd);
    initializeProjectionMatrix();

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < d; j++)
            Y.updateDataObject(i, j, Y_visi.getObjectAt(i).getFeatureAt(j));       
    } 
}

/**
 * Overloaded constructor that accept object matrix that must be projected to d
 * dimension
 */
PCA::PCA(ObjectMatrix objMatrix, int dim){ 
    setProjectionDimension(dim);
    X = objMatrix;
    initializeProjectionMatrix();
    ProjectXMatrix();
}

ObjectMatrix PCA::getProjection(){
    
    return Y; 
}

/**
 * Converts data object to datatype required by external method
 */
void PCA::toDataType(){
    int m = X.getObjectCount();
    int n = X.getObjectAt(0).getFeatureCount();
    alglibX.setlength(m, n);

    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            alglibX(i,j) = X.getObjectAt(i).getFeatureAt(j);
}

void PCA::ProjectXMatrix()
{
    PCA::toDataType();
    int m = X.getObjectCount();
    int n = X.getObjectAt(0).getFeatureCount();
    double X_vid[n], tmp = 0.0;
    double wholeDisp = 0.0, tarpDisp = 0.0;
    for (int i = 0; i < n; i++)
        X_vid[i] = Statistics::getAverage(X, i);
    alglib::ae_int_t info;
    alglib::real_1d_array eigValues;
    alglib::real_2d_array eigVectors;
    pcabuildbasis(alglibX, m, n, info, eigValues, eigVectors);
    
    if (info == 1)
    {
        for (int i = 0; i < m; i++)
            for (int j = 0; j < d; j++)
            {
                tmp = 0.0;
                for (int k = 0; k < n; k++)
                    tmp += (alglibX(i,k) - X_vid[k]) * eigVectors[k][j];
                Y.updateDataObject(i, j, tmp);
            }  
        
        for (int i = 0; i < d; i++)
            tarpDisp += eigValues[i];
        
        for (int i = 0; i < n; i++)
            wholeDisp += eigValues[i];
        
        dispPart = tarpDisp / wholeDisp;
    }
}

/**
 * Converts to ObjectMatrix result that is provided by external library.
 */
void PCA::fromDataType(){
    int m = X.getObjectCount();
    int n = X.getObjectAt(0).getFeatureCount();

    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            X.updateDataObject(i, j, alglibX(i,j));

}

int PCA::getDimension()
{
    return d;
}

double PCA::getDispersionPart()
{
    return dispPart;
}

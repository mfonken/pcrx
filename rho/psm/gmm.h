//
//  gmm.h
//  GaussianMixtureModel
//
//  Created by Matthew Fonken on 2/9/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#ifdef __PSM__

#ifndef gmm_h
#define gmm_h

#ifdef __cplusplus
extern "C" {
#endif
    
#include "fsm.h"

/* Cluster functions */
void InitializeGaussianMixtureCluster(                 gaussian_mixture_cluster_t *, observation_t *, vec2 * );
void UpdateGaussianMixtureCluster(                     gaussian_mixture_cluster_t *, observation_t *, vec2 * );
void GetScoreOfGaussianMixtureCluster(                 gaussian_mixture_cluster_t *, vec2 *);
void UpdateNormalOfGaussianMixtureCluster(             gaussian_mixture_cluster_t * );
void UpdateInputProbabilityOfGaussianMixtureCluster(   gaussian_mixture_cluster_t *, double );
void ContributeToOutputOfGaussianMixtureCluster(       gaussian_mixture_cluster_t *, vec2 *, vec2 * );
void UpdateLimitsOfGaussianMixtureCluster(             gaussian_mixture_cluster_t * );
void WeighGaussianMixtureCluster(                      gaussian_mixture_cluster_t * );

/* Model functions */
void InitializeGaussianMixtureModel(                   gaussian_mixture_model_t * );
double GetScoreSumOfClustersInGaussianMixtureModel(    gaussian_mixture_model_t *, vec2 * );
double GetOutputAndBestDistanceOfGaussianMixtureModel( gaussian_mixture_model_t *, double, vec2 *, vec2 * );
double GetMaxErrorOfGaussianMixtureModel(              gaussian_mixture_model_t *, vec2 *, vec2 *, vec2 * );
void AddClusterToGaussianMixtureModel(                 gaussian_mixture_model_t *, observation_t *, vec2 * );
void UpdateGaussianMixtureModel(                       gaussian_mixture_model_t *, observation_t *, vec2 * );
void AddValueToGaussianMixtureModel(                   gaussian_mixture_model_t *, observation_t *, vec2 * );
void RemovClusterFromGaussianMixtureModel(             gaussian_mixture_model_t *, uint16_t );

typedef struct
{
    void (*Initialize)(                 gaussian_mixture_cluster_t *, observation_t *, vec2 * );
    void (*Update)(                     gaussian_mixture_cluster_t *, observation_t *, vec2 * );
    void (*GetScore)(                   gaussian_mixture_cluster_t *, vec2 *);
    void (*UpdateNormal)(               gaussian_mixture_cluster_t * );
    void (*UpdateInputProbability)(     gaussian_mixture_cluster_t *, double );
    void (*ContributeToOutput)(         gaussian_mixture_cluster_t *, vec2 *, vec2 * );
    void (*UpdateLimits)(               gaussian_mixture_cluster_t * );
    void (*Weigh)(                      gaussian_mixture_cluster_t * );
} gaussian_mixture_cluster_functions;

typedef struct
{
    void   (*Initialize)(               gaussian_mixture_model_t * );
    double (*GetScoreSumOfClusters)(    gaussian_mixture_model_t *, vec2 * );
    double (*GetOutputAndBestDistance)( gaussian_mixture_model_t *, double, vec2 *, vec2 * );
    double (*GetMaxError)(              gaussian_mixture_model_t *, vec2 *, vec2 *, vec2 * );
    void   (*AddCluster)(               gaussian_mixture_model_t *, observation_t *, vec2 * );
    void   (*Update)(                   gaussian_mixture_model_t *, observation_t *, vec2 * );
    void   (*AddValue)(                 gaussian_mixture_model_t *, observation_t *, vec2 * );
    void   (*RemoveCluster)(            gaussian_mixture_model_t *, uint16_t );
} gaussian_mixture_model_functions;

typedef struct
{
    gaussian_mixture_cluster_functions Cluster;
    gaussian_mixture_model_functions Model;
} gaussian_mixture_functions;

static gaussian_mixture_functions GMMFunctions =
{
    .Cluster.Initialize             = InitializeGaussianMixtureCluster,
    .Cluster.Update                 = UpdateGaussianMixtureCluster,
    .Cluster.GetScore               = GetScoreOfGaussianMixtureCluster,
    .Cluster.UpdateNormal           = UpdateNormalOfGaussianMixtureCluster,
    .Cluster.UpdateInputProbability = UpdateInputProbabilityOfGaussianMixtureCluster,
    .Cluster.ContributeToOutput     = ContributeToOutputOfGaussianMixtureCluster,
    .Cluster.UpdateLimits           = UpdateLimitsOfGaussianMixtureCluster,
    .Cluster.Weigh                  = WeighGaussianMixtureCluster,
    
    .Model.Initialize               = InitializeGaussianMixtureModel,
    .Model.GetScoreSumOfClusters    = GetScoreSumOfClustersInGaussianMixtureModel,
    .Model.GetOutputAndBestDistance = GetOutputAndBestDistanceOfGaussianMixtureModel,
    .Model.GetMaxError              = GetMaxErrorOfGaussianMixtureModel,
    .Model.AddCluster               = AddClusterToGaussianMixtureModel,
    .Model.Update                   = UpdateGaussianMixtureModel,
    .Model.AddValue                 = AddValueToGaussianMixtureModel,
    .Model.RemoveCluster            = RemovClusterFromGaussianMixtureModel
};

#ifdef __cplusplus
}
#endif
    
#endif /* gmm_h */

#endif

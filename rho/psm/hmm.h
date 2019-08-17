//
//  hmm.h
//  hmmtest
//
//  Created by Matthew Fonken on 2/10/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#ifdef __PSM__

#ifndef hmm_h
#define hmm_h

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "gmm.h"

void InitializeHiddenMarkovModel(               hidden_markov_model_t *, observation_symbol_t );
void UpdateObservationMatrixHiddenMarkovModel(  hidden_markov_model_t * );
void ReportObservationToHiddenMarkovModel(      hidden_markov_model_t *, observation_symbol_t );
void BaumWelchGammaSolveHiddenMarkovModel(      hidden_markov_model_t * );
void PrintObservationMatrixHiddenMarkovModel(   hidden_markov_model_t * );

typedef struct
{
    void   (*Initialize)(               hidden_markov_model_t *, observation_symbol_t );
    void   (*UpdateObservationMatrix)(  hidden_markov_model_t * );
    void   (*ReportObservation)(        hidden_markov_model_t *, observation_symbol_t );
    void   (*BaumWelchGammaSolve)(      hidden_markov_model_t * );
    void   (*PrintObservationMatrix)(   hidden_markov_model_t * );
} hidden_markov_model_function_t;

static const hidden_markov_model_function_t HMMFunctions =
{
    .Initialize                 = InitializeHiddenMarkovModel,
    .UpdateObservationMatrix    = UpdateObservationMatrixHiddenMarkovModel,
    .ReportObservation          = ReportObservationToHiddenMarkovModel,
    .BaumWelchGammaSolve        = BaumWelchGammaSolveHiddenMarkovModel,
    .PrintObservationMatrix     = PrintObservationMatrixHiddenMarkovModel
};
    
#ifdef __cplusplus
}
#endif

#endif /* hmm_h */

#endif

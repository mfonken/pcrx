/************************************************************************
 *  File: rho_utility.c
 *  Group: Rho Core
 ***********************************************************************/

/************************************************************************
 *                             Includes                                 *
 ***********************************************************************/
#include "rho_utility.h"

const rho_utility_functions RhoUtility =
{
    {
        InitializeDataRhoUtility,
        InitializeFiltersRhoUtility,
        InitializePredictionRhoUtility,
        InitializeDensityMapRhoUtility
    },
    
    {
        ResetForDetectRhoUtility,
        ResetForPredictionRhoUtility,
        ResetDensityMapPairKalmansRhoUtility
    },
    
    {
        PredictPeakFilterRhoUtility,
        PredictTrackingFiltersRhoUtility,
        CalculateValidTracksRhoUtility,
        SortTrackingFiltersRhoUtility,
        PredictTrackingProbabilitiesRhoUtility,
        CorrectPredictionAmbiguityRhoUtility,
        CombineAxisProbabilitesRhoUtility,
        RedistributeDensitiesRhoUtility,
        UpdateCorePredictionDataRhoUtility,
        GenerateObservationListFromPredictionsRhoUtility,
        GenerateObservationListsFromPredictionsRhoUtility,
        UpdatePredictiveStateModelPairRhoUtility
    },
    
    {
        PerformDetectRhoUtility,
        CalculateBandLowerBoundRhoUtility,
        DetectRegionsRhoUtility,
        DetectRegionRhoUtility,
        SubtractBackgroundForDetectionRhoUtility,
        CalculateChaosRhoUtility,
        ScoreRegionsRhoUtility,
        SortRegionsRhoUtility,
        CalculatedFrameStatisticsRhoUtility
    },
    
    {
        CalculateTuneRhoUtility,
        CalculateBackgroundTuneFactorRhoUtility,
        CalculateStateTuneFactorRhoUtility,
        CalculateTargetTuneFactorRhoUtility,
        CalculateTargetCoverageFactorRhoUtility
    },
    
    {
        GenerateCumulativeMomentsStatistics,
        GenerateRegionScoreRhoUtility,
        GenerateCentroidRhoUtility,
        GeneratePacketRhoUtility,
        GenerateBackgroundRhoUtility
    },
    
    {
        PrintPacketRhoUtility
    }
};

/************************************************************************
 *                       Function Definitions                           *
 ***********************************************************************/
void InitializeDataRhoUtility( rho_core_t * core, index_t width, index_t height )
{
    /* reset entire structure */
    memset(core, 0, sizeof(rho_core_t));

    /* Core frame */
    core->Width = width;
    core->Height = height;

    /* Centroid */
    core->Centroid.x   = width/2.;
    core->Centroid.y   = height/2.;
    core->Primary.x   = core->Centroid.x;
    core->Primary.y   = core->Centroid.y;
    core->Secondary.x   = core->Centroid.x;
    core->Secondary.y   = core->Centroid.y;

    core->TargetCoverageFactor = FILTERED_COVERAGE_TARGET;

    /* Packet */
    core->Packet.header.ID       = PACKET_HEADER_ID;
    core->Packet.header.includes = PACKET_INCLUDES;
    memset(core->Packet.data, 0, sizeof(packet_offset_lookup_t));

    /* Background */
    core->BackgroundCounter = 0;
    core->BackgroundPeriod = BACKGROUNDING_PERIOD;

    core->Thresh = (double)AVG2( MAX_THRESH, MIN_THRESH );
    core->ThreshByte = (byte_t)core->Thresh;

    core->DensityMapPair.x.map          = FOREGROUND_DENSITY_MAP_X;
    core->DensityMapPair.x.background   = BACKGROUND_DENSITY_MAP_X;
    core->DensityMapPair.y.map          = FOREGROUND_DENSITY_MAP_Y;
    core->DensityMapPair.y.background   = BACKGROUND_DENSITY_MAP_Y;
    
#ifndef __PSM__
    KumaraswamyFunctions.Initialize( &core->Kumaraswamy, NUM_STATES + 1 );
    FSMFunctions.Sys.Initialize( &core->StateMachine, CHAOTIC );
#endif
}

void InitializeFiltersRhoUtility( rho_core_t * core )
{
    /* Threshold Filter */
    RhoPID.Initialize( &core->ThreshFilter, DEFAULT_PID_GAIN );

    /* Coverage Filter */
    core->TargetCoverageFactor  = (floating_t)FILTERED_COVERAGE_TARGET;
    RhoKalman.Initialize(&core->TargetFilter, core->TargetCoverageFactor, RHO_TARGET_LS, 0, RHO_TARGET_FILTER_MAX, DEFAULT_TARGET_UNCERTAINTY );
}

void InitializePredictionRhoUtility( prediction_t * prediction, index_t length )
{
    /* Prediction probabilities */
    memset( &prediction->Probabilities, 0, sizeof(floating_t) * 4 );
    for(uint8_t i = 0; i < MAX_TRACKING_FILTERS; i++)
    {
        RhoKalman.Initialize( &prediction->TrackingFilters[i], 0., RHO_PREDICTION_LS, 0, length, DEFAULT_PREDICTION_UNCERTAINTY );
        prediction->TrackingFiltersOrder[i] = i;
    }
    /* predictionegions */
    for(uint8_t i = 0; i < MAX_REGIONS; i++)
    {
        memset(&prediction->Regions[i], 0, sizeof(region_t));
        prediction->RegionsOrder[i] = i;
    }
}

void InitializeDensityMapRhoUtility( density_map_t * density_map, index_t length, index_t centroid )
{
    size_t size = sizeof(density_map_unit_t)*length;
    memset(density_map->map, 0, size);
    memset(density_map->background, 0, size);
    density_map->length = length;
    density_map->max[0] = 0;
    density_map->max[1] = 0;
    density_map->centroid = centroid;
    RhoKalman.Initialize( &density_map->kalmans[0],  0, RHO_DEFAULT_LS, 0, length, DEFAULT_KALMAN_UNCERTAINTY );
    RhoKalman.Initialize( &density_map->kalmans[1], 0, RHO_DEFAULT_LS, 0, length, DEFAULT_KALMAN_UNCERTAINTY );
}

void ResetForDetectRhoUtility( rho_detection_variables * _, density_map_t * density_map, prediction_t * prediction )
{
    memset( _, 0, sizeof(rho_detection_variables) );

    _->len          = density_map->length;
    _->range[0]     = density_map->length;
    _->range[1]     = density_map->centroid;
    _->range[2]     = 0;

    memset( &prediction->Probabilities, 0, sizeof(floating_t)*NUM_STATES );
    memset( &prediction->Probabilities, 0, sizeof(floating_t)*NUM_STATES );

    prediction->NuRegions    = 0;
    prediction->TotalDensity = 0;
    prediction->NumRegions   = 0;

    for( index_t i = 0; i < MAX_REGIONS; i++ )
    {
        memset( &prediction->Regions[i], 0, sizeof(region_t) );
        prediction->RegionsOrder[i] = i;
    }
    for( index_t i = 0; i < MAX_TRACKING_FILTERS; i++ )
        prediction->TrackingFiltersOrder[i] = i;
}

void ResetDensityMapPairKalmansRhoUtility( rho_core_t * core )
{
    RhoKalman.Reset( &core->DensityMapPair.x.kalmans[0], core->PredictionPair.x.PreviousPeak[0] );
    RhoKalman.Reset( &core->DensityMapPair.x.kalmans[1], core->PredictionPair.x.PreviousPeak[1] );
    RhoKalman.Reset( &core->DensityMapPair.y.kalmans[0], core->PredictionPair.y.PreviousPeak[0] );
    RhoKalman.Reset( &core->DensityMapPair.y.kalmans[1], core->PredictionPair.y.PreviousPeak[1] );
}

void PerformDetectRhoUtility( rho_detection_variables * _, density_map_t * density_map, prediction_t * prediction )
{
    DUAL_FILTER_CYCLE(_->cycle)
    {
        _->maximum  = 0;
        _->start    = _->range[_->cycle];
        _->end      = _->range[_->cycle_];

        RhoUtility.Predict.PeakFilter( _, density_map, prediction );
        if( RhoUtility.Detect.LowerBound( _ ) )
        {
            do
            {
                RhoUtility.Detect.Regions( _, density_map, prediction );
                RhoUtility.Detect.CalculateChaos( _, prediction );
                RhoUtility.Detect.ScoreRegions( _, density_map, prediction );
                if(_->recalculation_counter == 0) _->first_filtered_density = _->filtered_density;
            } while( _->recalculate && ++_->recalculation_counter < MAX_RHO_RECALCULATION_LEVEL );

            RhoUtility.Detect.SortRegions( _, prediction );
        }

        prediction->PreviousPeak[_->cycle] = BOUNDU( _->maximum, _->len );
        prediction->PreviousDensity[_->cycle] = _->first_filtered_density;
        density_map->max[_->cycle] = _->maximum;
    }
    LOG_RHO(RHO_DEBUG, "Regions: %d\n", _->region_total);
}

void PredictPeakFilterRhoUtility( rho_detection_variables * _, density_map_t * density_map, prediction_t * prediction )
{
    _->filter_peak      = (index_t)RhoKalman.Step( &density_map->kalmans[_->cycle], prediction->PreviousPeak[_->cycle], density_map->kalmans[_->cycle].velocity );
    _->filter_peak_2    = _->filter_peak << 1;
    _->filter_variance  = BOUND((index_t)(RHO_VARIANCE( density_map->kalmans[_->cycle].K[0]) ), MIN_VARIANCE, MAX_VARIANCE);
    density_map->kalmans[_->cycle].variance = _->filter_variance;
}

inline bool CalculateBandLowerBoundRhoUtility( rho_detection_variables * _ )
{
    if( _->filter_variance > 0 )// && (_->filter_peak == 0 || _->filter_peak > _->filter_variance ))
    {
        if(_->filter_peak > _->filter_variance)
            _->filter_band_lower = _->filter_peak - _->filter_variance;
        else
            _->filter_band_lower = 0;
        return true;
    }
    return false;
}

inline void DetectRegionsRhoUtility( rho_detection_variables * _, density_map_t * density_map, prediction_t * prediction )
{ /* Detect regions - START */
    
    if( !_->recalculate )
    {
        _->total_density = 0;
        _->filtered_density = 0;
    }

    BOUNDED_CYCLE_DUAL(_->x, _->start, _->end, _->curr, density_map->map, _->background_curr, density_map->background)
    {
        if( !_->recalculate )
            RhoUtility.Detect.SubtractBackground( _ );
        RhoUtility.Detect.Region( _, density_map, prediction );
    }
} /* Detect regions - END */

inline void SubtractBackgroundForDetectionRhoUtility( rho_detection_variables * _ )
{
    if( _->curr > _->background_curr )
    {
        _->total_density += _->curr;
        
#ifdef USE_BACKGROUNDING
        /* Subtract background */
        _->curr -= _->background_curr;

        /* Punish values above the filter peak */
        if( ( _->curr > _->filter_peak )
           && ( _->filter_peak_2 > _->curr ) )
            _->curr = _->filter_peak_2 - _->curr;
#endif
    }
    else
        _->curr = 0;
}

inline void DetectRegionRhoUtility( rho_detection_variables * _, density_map_t * density_map, prediction_t * prediction )
{
    /* Check if CMA value is in band */
    if( _->curr > _->filter_band_lower )
    {
        /* Update max */
        if(_->curr > _->maximum) _->maximum = _->curr;
        
        /* De-offset valid values */
        _->curr -= _->filter_band_lower;

        /* Process new values into region */
        RhoUtility.Generate.CumulativeMoments( (floating_t)_->curr, (floating_t)_->x, &_->average_curr, &_->average_moment, &_->average_counter );

        /* Increment width */
        _->width++;

        /* reset flag and counter */
        _->has_region = 1; _->gap_counter = 0;
    }

    /* Process completed regions and increment count */
    else if( ++_->gap_counter > RHO_GAP_MAX && _->has_region
#ifdef __USE_RUNNING_AVERAGE__
    )
    {
        _->current_density = (density_2d_t)_->average_curr;
#else
    && _->average_counter )
    {
        _->current_density = (density_2d_t)( _->average_curr / _->average_counter );
#endif
        _->filtered_density += _->current_density;

        /* Check if new region is dense enough to be saved */
        if( _->current_density > MIN_REGION_DENSITY)
        {
            /* Create new region at secondary */
            index_t loc = (index_t)ZDIV( _->average_moment, _->average_curr );
            prediction->Regions[_->region_total] = (region_t){ density_map->map[loc], _->current_density, loc, _->width };
            prediction->RegionsOrder[_->region_total] = _->region_total;
            _->region_total++;
        }

        /* reset variables */
        _->average_moment = 0.; _->average_curr = 0.; _->average_counter = 0.;
        _->has_region = 0; _->gap_counter = 0;
    }

    else if (!_->has_region )
    {
        /* reset width */
        _->width = 0;
    }
}

void CalculateChaosRhoUtility( rho_detection_variables * _, prediction_t * prediction )
{
    _->chaos = (floating_t)prediction->PreviousDensity[_->cycle] / (floating_t)_->filtered_density;
    LOG_RHO(RHO_DEBUG_3, "chaos:%.4f\n", _->chaos);
}

void ScoreRegionsRhoUtility( rho_detection_variables * _, density_map_t * density_map, prediction_t * prediction )
{
    /* Assume no recalculations are needed */
    _->recalculate = false;
    
    /* predictioneturn on empty frames */
    if(_->filtered_density > 0 )
    {
        /* Cycle regions */
        for(uint8_t i = 0; i < _->region_total; i++)
        {
            /* Get region at index from order array and check validity */
            uint8_t j = prediction->RegionsOrder[i];
            if(j >= MAX_REGIONS) continue;
            region_t * curr = &prediction->Regions[j];

            /* Score current region */
            RhoUtility.Generate.RegionScore( curr, _->filtered_density, _->maximum );

            /* predictionecalculate regions with chaos */
            if(curr->score > _->chaos)
            {
                /* predictionecalculate around chaotic region */
                _->recalculate = true;

                /* predictionemove this index */
                prediction->RegionsOrder[i] = MAX_REGIONS;

                /* Get centroid peak and update filter peak double */
                sdensity_t cpeak = (sdensity_t)density_map->map[curr->location];
                _->filter_peak_2 = cpeak << 1;

                /* predictionaise lower band half of region centroid to band center when below */
                if( cpeak < _->filter_peak)
                    _->filter_band_lower += abs( (sdensity_t)_->filter_peak - cpeak) >> 1;

                /* Otherwise raise to centroid peak */
                else _->filter_band_lower = cpeak;

                /* predictionecalculate _->cycle width */
                index_t half_region_width = curr->width >> 1;
                if(curr->location - half_region_width > _->range[_->cycle_])
                    _->end = curr->location - half_region_width;
                else _->end = _->range[_->cycle_];
                if(curr->location + half_region_width < _->range[_->cycle])
                    _->start = curr->location + half_region_width;
                else _->start = _->range[_->cycle];
                break;
            }
        }
    }
}

void SortRegionsRhoUtility( rho_detection_variables * _, prediction_t * prediction )
{
    /// NOTE: Smaller scores are better
    /* Assume all regions are valid */
    index_t i, io, j, jo, best_index;
    floating_t best_score;
    region_t *curr, *check;

    /* Cycle through found regions */
    for( i = 0; i < _->region_total; i++)
    {
        io = prediction->RegionsOrder[i];
        curr = &prediction->Regions[io];
        if( curr->sort ) continue;

        best_score = curr->score;
        best_index = i;
        /* Cycle through other regions */
        for( j = i+1; j < _->region_total; j++ )
        {
            jo = prediction->RegionsOrder[j];
            check = &prediction->Regions[jo];

            /* If unscored and less than min, set as new min */
            if( check->score < best_score )
            {
                best_score = curr->score;
                best_index = j;
            }
        }

        prediction->RegionsOrder[i] = best_index;
        prediction->RegionsOrder[best_index] = i;

        prediction->Regions[i].sort = true;
        LOG_RHO(RHO_DEBUG, "R%d:{%d,%d,%d,%d} = %.4f\n",
            i,
            curr->maximum,
            curr->density,
            curr->location,
            curr->width,
            curr->score
            );
    }
    for(; i < MAX_REGIONS; i++ )
    {
        prediction->RegionsOrder[i] = i;
    }
}

void CalculatedFrameStatisticsRhoUtility( rho_detection_variables * _, prediction_t * prediction )
{
    /* Update frame statistics */
    _->assumed_regions = (floating_t)_->region_total;
    if( _->assumed_regions == 0 ) _->assumed_regions = 1;

    prediction->NuRegions = BOUNDU( ZDIV( (floating_t)_->total_density * (floating_t)_->assumed_regions, _->target_density ), MAX_NU_REGIONS );
    prediction->NumRegions = _->region_total;
    prediction->TotalDensity = _->total_density;
    prediction->AverageDensity = ZDIV( (floating_t)prediction->TotalDensity, prediction->NuRegions );

    /* reset sort flags */
    for( uint8_t i = 0; i < MAX_REGIONS; i++ )
        prediction->Regions[i].sort = false;
}

void PredictTrackingFiltersRhoUtility( prediction_t * prediction )
{
    index_t valid_tracks = RhoUtility.Predict.CalculateValidTracks( prediction );
    LOG_RHO( RHO_DEBUG, "Found %d valid/active tracking filter(s)\n", valid_tracks);

    /* Declare essential variables */
    floating_t aa, bb, ab, ba, total_difference = 0., average_difference = 0.;

    /* Match regions to Kalmans */
    index_t m = 0, n = 0, updated = 0;
    for( ; m < ( valid_tracks - 1 ); m ++ )
    {
        for( ; n < ( prediction->NumRegions - 1 ); n++ )
        { /* Update tracking filters in pairs following determinant */

            /* predictionetreive current region pair */
            region_t
            *regionA = &prediction->Regions[prediction->RegionsOrder[n]],
            *regionB = &prediction->Regions[prediction->RegionsOrder[n+1]];

            /* predictionetreive tracking filters pair */
            byte_t
            fAi = prediction->TrackingFiltersOrder[m],
            fBi = prediction->TrackingFiltersOrder[m+1];

            rho_kalman_t
            *filterA = &prediction->TrackingFilters[fAi],
            *filterB = &prediction->TrackingFilters[fBi];

            bool swapped = false;

            /* Calculate distances between filters and regions */
            aa = fabs(filterA->value - regionA->location );
            bb = fabs(filterB->value - regionB->location );
            ab = fabs(filterA->value - regionB->location );
            ba = fabs(filterB->value - regionA->location );

            if( aa > MAX_TRACKING_MATCH_DIFFERNCE )
            {
                aa = MAX_TRACKING_MATCH_DIFFERNCE;
                RhoKalman.Punish(filterA);
            }
            if( ab > MAX_TRACKING_MATCH_DIFFERNCE )
            {
                ab = MAX_TRACKING_MATCH_DIFFERNCE;
                RhoKalman.Punish(filterA);
            }
            if( bb > MAX_TRACKING_MATCH_DIFFERNCE )
            {
                bb = MAX_TRACKING_MATCH_DIFFERNCE;
                RhoKalman.Punish(filterB);
            }
            if( ba > MAX_TRACKING_MATCH_DIFFERNCE )
            {
                ba = MAX_TRACKING_MATCH_DIFFERNCE;
                RhoKalman.Punish(filterB);
            }

            /* Swap on upward determinant */
            if( aa * bb > ab * ba )
            {
                swapped = true;
                total_difference += aa + bb;
            }
            else
                total_difference += ab + ba;
            updated += 2;

            /* Update filters */
            if(swapped)
            {
                RhoKalman.Update(filterA, regionB->location );
                RhoKalman.Update(filterB, regionA->location );
                regionB->tracking_id = fAi;
                regionA->tracking_id = fBi;
            }
            else
            {
                RhoKalman.Update(filterA, regionA->location );
                RhoKalman.Update(filterB, regionB->location );
                regionA->tracking_id = fAi;
                regionB->tracking_id = fBi;
            }

            m++; n++;
            
            LOG_RHO(RHO_DEBUG, "Updating %d: %.4f\n", fAi, filterA->value);
            LOG_RHO(RHO_DEBUG, "Updating %d: %.4f\n", fBi, filterB->value);
        }

        /* Account for odd number of and spare regions */
        if( m < valid_tracks && n < prediction->NumRegions )
        {
            rho_kalman_t
            *filter = &prediction->TrackingFilters[prediction->TrackingFiltersOrder[m]];
            floating_t
            bloc = (floating_t)prediction->Regions[prediction->RegionsOrder[n]].location;
            RhoKalman.Update(filter, bloc);

            aa = fabs(filter->value - bloc);
            total_difference += aa;
            updated++;
        }
    }

    /* Activate new filters */
    for( ; n < prediction->NumRegions && n < MAX_REGIONS; n++ )
    {
        LOG_RHO( RHO_DEBUG, "Activating filter at index %d[%d]\n", prediction->TrackingFiltersOrder[n], n );
        RhoKalman.Step( &prediction->TrackingFilters[prediction->TrackingFiltersOrder[n]], prediction->Regions[prediction->RegionsOrder[n]].location, 0. );
    }

    /* Punish unused ones */
    for( ; m < MAX_TRACKING_FILTERS; m++ )
    {
//        LOG_RHO( RHO_DEBUG, "Punishing filter at index %d[%d]\n", prediction->TrackingFiltersOrder[m], m );
        RhoKalman.Punish(&prediction->TrackingFilters[prediction->TrackingFiltersOrder[m]]);
    }

    /* Calculate confidence */
    if( updated )
    {
        average_difference = total_difference / (floating_t)updated;
        if( average_difference > MAX_TRACKING_MATCH_DIFFERNCE ) prediction->Probabilities.confidence = 0.;
        else prediction->Probabilities.confidence = TRACKING_MATCH_TRUST * ( 1 - ( average_difference / MAX_TRACKING_MATCH_DIFFERNCE ) );
    }

    RhoUtility.Predict.SortFilters( prediction );
}

void SortTrackingFiltersRhoUtility( prediction_t * prediction )
{
    rho_kalman_t *curr, *check;
    floating_t best_score;
    index_t i, io, j, jo, best_index = 0;
    /* Score all filters */
    for( i = 0; i < MAX_TRACKING_FILTERS; i++ )
        RhoKalman.Score( &prediction->TrackingFilters[i] );

    /* Swap sort - Cycle through found regions */
    for( i = 0; i < MAX_TRACKING_FILTERS; i++)
    {
        io = prediction->TrackingFiltersOrder[i];
        curr = &prediction->TrackingFilters[io];
        if( curr->sorted == true) continue;

        best_score = curr->score;
        best_index = i;
        /* Cycle through other regions */
        for( j = i+1; j < MAX_TRACKING_FILTERS; j++ )
        {
            jo = prediction->TrackingFiltersOrder[j];
            check = &prediction->TrackingFilters[jo];

            /* If unscored and less than min, set as new min */
            if( check->score > best_score )
            {
                best_score = curr->score;
                best_index = j;
            }
        }
        prediction->TrackingFiltersOrder[i] = best_index;
        prediction->TrackingFiltersOrder[best_index] = i;
        prediction->TrackingFilters[i].sorted = true;
    }

    for( i = 0; i < MAX_TRACKING_FILTERS; i++ )
        prediction->TrackingFilters[i].sorted = false;
}

index_t CalculateValidTracksRhoUtility( prediction_t * prediction )
{
    index_t valid_tracks = 0, i, io;
    for( i = 0; i < MAX_TRACKING_FILTERS; i++ )
    {
        io = prediction->TrackingFiltersOrder[i];
        rho_kalman_t *curr = &prediction->TrackingFilters[io];
        curr->valid = false;
        floating_t score = RhoKalman.Score( curr );
        if( RhoKalman.IsExpired(curr)
           || ( score < MIN_TRACKING_KALMAN_SCORE ) ) break;
        RhoKalman.Predict(curr, curr->velocity);
        curr->valid = true;
        valid_tracks++;
    }
    return valid_tracks;
}

void PredictTrackingProbabilitiesRhoUtility( prediction_t * prediction )
{
    floating_t a = prediction->NuRegions+1, b = (floating_t)NUM_STATE_GROUPS+1, curr_CDF, prev_CDF = 0.,
    interval[4] = STATE_KUMARASWAMY_INTERVALS;
    for( uint8_t i = 0; i < NUM_STATE_GROUPS; i++ )
    {
        curr_CDF = KUMARASWAMY_CDF(interval[i],a,b);
        prediction->Probabilities.P[i] = curr_CDF - prev_CDF;
        prev_CDF = curr_CDF;
    }
    
    
    prediction->Primary   = prediction->TrackingFilters[0].value;
    prediction->Secondary = prediction->TrackingFilters[1].value;
}

void ResetForPredictionRhoUtility( prediction_predict_variables * _, prediction_pair_t * prediction, index_pair_t centroid )
{
    _->Primary   = (index_pair_t){ prediction->y.Primary,   (index_t)prediction->x.Primary   };
    _->Secondary = (index_pair_t){ prediction->y.Secondary, (index_t)prediction->x.Secondary };
    _->Centroid  = (index_pair_t){ centroid.x, centroid.y };
}

void CorrectPredictionAmbiguityRhoUtility( prediction_predict_variables * _, rho_core_t * core )
{
    /* Check if X or Y are ambiguous */
    if(   !( ( _->Primary.x < _->Centroid.x ) ^ ( _->Secondary.x > _->Centroid.x ) )
       || !( ( _->Primary.y < _->Centroid.y ) ^ ( _->Secondary.y > _->Centroid.y ) ) )
    {
        RhoUtility.Predict.RedistributeDensities( core );
        _->quadrant_check = (  core->Qf[0] > core->Qf[1] ) + ( core->Qf[2] < core->Qf[3] ) - 1;
        if( ( _->Primary.x > _->Secondary.x ) ^ ( ( _->quadrant_check > 0 ) ^ ( _->Primary.y < _->Secondary.y ) ) )
            SWAP(_->Primary.x, _->Secondary.x);
    }
}

void CombineAxisProbabilitesRhoUtility( prediction_pair_t * prediction )
{
    /* Combine X & Y probabilities with confidence factor */
    double x_confidence, y_confidence;
    for( uint8_t i = 0; i < NUM_STATE_GROUPS; i++ )
    {
        x_confidence = prediction->x.Probabilities.confidence * prediction->x.Probabilities.P[i];
        y_confidence = prediction->y.Probabilities.confidence * prediction->y.Probabilities.P[i];
        prediction->Probabilities.P[i] = AVG2( x_confidence, y_confidence );
    }
    prediction->Probabilities.confidence = AVG2( prediction->x.Probabilities.confidence, prediction->y.Probabilities.confidence );
}

void UpdateCorePredictionDataRhoUtility( prediction_predict_variables * _, rho_core_t * core )
{
    _->Centroid.x = BOUNDU((index_t)(_->Primary.x + _->Secondary.x) >> 1, core->Width );
    _->Centroid.y = BOUNDU((index_t)(_->Primary.y + _->Secondary.y) >> 1, core->Height);

    LOG_RHO(RHO_DEBUG, "Centroid.x>%d | Centroid.y>%d\n", _->Centroid.x, _->Centroid.y);

    core->Primary   = _->Primary;
    core->Secondary = _->Secondary;
    core->Centroid  = _->Centroid;

    /* NOTE: density maps invert axes */
    core->DensityMapPair.y.centroid         = _->Centroid.x;
    core->DensityMapPair.x.centroid         = _->Centroid.y;
    core->PredictionPair.y.PreviousCentroid = _->Centroid.x;
    core->PredictionPair.x.PreviousCentroid = _->Centroid.y;
}

void CalculateTuneRhoUtility( rho_core_t * core )
{
    /* Background-Tune on significant background */
    RhoUtility.Calculate.BackgroundTuneFactor( core );

    /* State-Tune by FSM state */
    RhoUtility.Calculate.StateTuneFactor( core );

    /* Filtered-Tune on target difference */
    RhoUtility.Calculate.TargetTuneFactor( core );

    core->Tune.proposed = BOUND( core->Tune.background + core->Tune.state + core->Tune.target, -THRESH_STEP_MAX, THRESH_STEP_MAX);
    
    core->Thresh = BOUND(core->Thresh + core->Tune.proposed, THRESH_MIN, THRESH_MAX);
    core->ThreshByte = (byte_t)core->Thresh;
}

void CalculateBackgroundTuneFactorRhoUtility( rho_core_t * core )
{
    floating_t background_tune_factor = 0.;
    if( core->QbT > BACKGROUND_COVERAGE_MIN )
    {
        floating_t background_coverage_factor = 1 - ZDIV( BACKGROUND_COVERAGE_MIN, core->QbT );
        background_tune_factor = -pow( BOUND(background_coverage_factor, -BACKGROUND_TUNE_MAX, BACKGROUND_TUNE_MAX), BACKGROUND_TUNE_EXPONENT);
    }
    core->Tune.background = background_tune_factor;
}
    
void CalculateStateTuneFactorRhoUtility( rho_core_t * core )
{
    core->TargetCoverageFactor = core->TargetFilter.value;
    core->PredictionPair.AverageDensity = MAX( core->PredictionPair.x.AverageDensity, core->PredictionPair.y.AverageDensity );
#ifdef __PSM__
    switch(core->PredictiveStateModelPair.current_state)
#else
    switch(core->StateMachine.state)
#endif
    {
        case CHAOTIC:
            RhoUtility.Reset.DensityMapPairKalmans( core );
        case TARGET_POPULATED:
            RhoKalman.Step( &core->TargetFilter, core->FilteredPercentage, 0. );
        case OVER_POPULATED:
        case UNDER_POPULATED:
            RhoUtility.Calculate.TargetCoverageFactor( core );
            break;
        default:
            break;
    }
    RhoPID.Update( &core->ThreshFilter, core->TargetCoverageFactor, core->TargetFilter.value );
    core->Tune.state = core->ThreshFilter.Value;
    
    LOG_RHO(RHO_DEBUG, "TCF:%.4f | TFV:%.4f\n", core->TargetCoverageFactor, core->ThreshFilter.Value);
}

void CalculateTargetTuneFactorRhoUtility( rho_core_t * core )
{
    core->Tune.target = TARGET_TUNE_FACTOR * ZDIV( core->ThreshFilter.Value, core->TargetFilter.value );
}
    
void CalculateTargetCoverageFactorRhoUtility( rho_core_t * core )
{
    floating_t TotalPixels = (floating_t)TOTAL_RHO_PIXELS;
#ifdef __PSM__
    if( core->PredictiveStateModelPair.best_confidence > MIN_STATE_CONFIDENCE )
        core->TargetCoverageFactor = ZDIV( core->PredictiveStateModelPair.proposed_nu * core->PredictiveStateModelPair.proposed_avg_den, TotalPixels );
#else
    if( core->PredictionPair.Probabilities.confidence > MIN_STATE_CONFIDENCE )
        core->TargetCoverageFactor = ZDIV( core->PredictionPair.NuRegions * core->PredictionPair.AverageDensity, TotalPixels );
#endif
    else
        core->TargetCoverageFactor = ZDIV( core->TotalCoverage, TotalPixels );
}


/* Perform density redistribution from combining current frame and background */
void RedistributeDensitiesRhoUtility( rho_core_t * core )
{
    /* Quadrant density redistribution lookup table */
    static const density_redistribution_lookup_t rlookup =
    {
        {
            {
                { { 0, 1, 3, 4 }, { 2, 5 }, { 6, 7 }, { 8 } },
                { { 0 }, { 1, 2 }, { 3, 6 }, { 4, 5, 7, 8 } },
                { { 0, 1, 2, 3 }, { 1, 3 }, { 2, 3 }, { 3 } },
                { 4, 2, 2, 1 }
            },
            {
                { { 0, 3 }, { 1, 2, 4, 5 }, { 6 }, { 7, 8 } },
                { { 0, 1 }, { 2 }, { 3, 4, 6, 7 }, { 5, 8 } },
                { { 0, 2 }, { 0, 1, 2, 3 }, { 2 }, { 2, 3 } },
                { 2, 4, 1, 2 }
            },
            {
                { { 0, 1 }, { 2 }, { 3, 4, 6, 7 }, { 5, 8 } },
                { { 0, 3 }, { 1, 2, 4, 5 }, { 6 }, { 7, 8 } },
                { { 0, 1 }, { 1 }, { 0, 1, 2, 3 }, { 1, 3 } },
                { 2, 1, 4, 2 }
            },
            {
                { { 0 }, { 1, 2 }, { 3, 6 }, { 4, 5, 7, 8 } },
                { { 0, 1, 3, 4 }, { 2, 5 }, { 6, 7 }, { 8 } },
                { { 0 }, { 0, 1 }, { 0, 2 }, { 0, 1, 2, 3 } },
                { 1, 2, 2, 4 }
            }
        }
    };

    LOG_RHO(RHO_DEBUG, "Redistributing densities.\n");
    redistribution_variables _ =
    {
        { core->Secondary.x, abs(core->Centroid.x-core->Secondary.x), core->Width - core->Centroid.x  },
        { core->Secondary.y, abs(core->Centroid.y-core->Secondary.y), core->Height - core->Centroid.y },
        { 0 }, 0
    };
    if( core->Centroid.x < core->Secondary.x )
    {
        _.xl[0] = core->Centroid.x;
        _.xl[2] = core->Width - core->Secondary.x;
        _.c |= 0x01;
    }
    if( core->Centroid.y < core->Secondary.y )
    {
        _.yl[0] = core->Centroid.y;
        _.yl[2] = core->Width - core->Secondary.y;
        _.c |= 0x02;
    }
    while( _.y < 3 )
        for( _.x = 0; _.x < 3; )
            _.area[_.p++] = _.xl[_.x++]* _.yl[_.y++];
    for( ; _.q < 4; _.q++ )
    {
        _.l  = rlookup.config[_.c].length[    _.q];
        _.l_ = rlookup.config[_.c].length[3 - _.q];
        for( _.x = 0, _.b = 0; _.x < _.l; _.x++ )
        {
            _.a = _.area[rlookup.config[_.c].current[_.q][_.x]];
            for( _.y = 0; _.y < _.l_; _.y++ )
                _.b += _.area[rlookup.config[_.c].background[rlookup.config[_.c].factor[_.q][_.x]][_.y]];
            _.d += ZDIV( _.a, _.b ) * core->Qb[_.q];
        }
#ifndef ALLOW_NEGATIVE_REDISTRIBUTION
        if( _.d > core->Q[_.q] ) core->Qf[_.q] = 0;
        else
#endif
            core->Qf[_.q] = core->Q[_.q] - _.d;
    }
}

void GenerateRegionScoreRhoUtility( region_t * region, density_t total_density, byte_t peak )
{
    floating_t
        delta_d = ( (floating_t)region->density / (floating_t)total_density) - 0.5,
        delta_p = (floating_t)peak / (floating_t)region->maximum;
    region->score = REGION_SCORE_FACTOR * ( ( delta_d * delta_d ) + ( delta_p * delta_p ) );
}

/* Generic centroid and mass calculator */
density_2d_t GenerateCentroidRhoUtility( density_map_unit_t * density_map, index_t length, index_t * centroid, register density_t thresh )
{
    floating_t avg = 0, average_moment = 0, count = 0, total = 0;
    for( index_t i = 0; i < length; i++ )
    {
        density_map_unit_t curr = density_map[i];
        if( curr > thresh )
        {
            /* Note only fraction m1/m0 is needed so either average method works*/
            RhoUtility.Generate.CumulativeMoments((floating_t)curr, (floating_t)i, &avg, &average_moment, &count);
            total += curr;
        }
    }
    *centroid = (index_t)(average_moment/avg);
    return(density_2d_t)count;
}

void PrintPacketRhoUtility( packet_t * packet, index_t length )
{
#ifdef PACKET_DEBUG
    LOG_PACKET(DEBUG_0,"Packet Size - %lubytes\n", sizeof(packet_t));
    for(int i = 0; i < sizeof(packet_t); )
    {
        LOG_PACKET(DEBUG_0,"(%02d)", i);
        for( int j = 0; j < length && i < sizeof(packet_t); j++, i++ )
            LOG_PACKET(DEBUG_0," 0x%02x", *(byte_t*)(&((byte_t*)&packet->header)[i]));
        LOG_PACKET(DEBUG_0,"\n");
    }
    uint32_t px = *(uint32_t*)packet->data;
    floating_t pxf = *(floating_t*)packet->data;
    uint8_t * ptr = (uint8_t*)&px;
    
    uint8_t pxr[4] = {ptr[3],ptr[2],ptr[1],ptr[0]};
    floating_t pxfr = *(floating_t*)&pxr;
    LOG_PACKET(DEBUG_0,"{%02x}{%02x}{%02x}{%02x} %f:%f\n",ptr[0],ptr[1],ptr[2],ptr[3],pxf,pxfr);
#endif
}
    
void GenerateBackgroundRhoUtility( rho_core_t * core )
{
    density_2d_t xt = RhoUtility.Generate.Centroid( core->DensityMapPair.x.background, core->DensityMapPair.x.length, &core->Secondary.x, BACKGROUND_CENTROID_CALC_THRESH );
    density_2d_t yt = RhoUtility.Generate.Centroid( core->DensityMapPair.y.background, core->DensityMapPair.y.length, &core->Secondary.y, BACKGROUND_CENTROID_CALC_THRESH );
    core->QbT = MAX(xt, yt);
}

void GeneratePacketRhoUtility( rho_core_t * core )
{
    packet_value_lookup_t  packet_value_lookup  = PACKET_ADDRESS_INITIALIZER(core->PredictionPair);
    packet_offset_lookup_t packet_offset_lookup = PACKET_OFFSETS;
    packet_generation_variables _ =
    {
        &core->Packet,
        (address_t)&core->Packet.data,
        (address_t)&packet_offset_lookup,
        (address_t*)&packet_value_lookup,
        0
    };
    _.packet->header.timestamp = TIMESTAMP();
    while( _.i++ < NUM_PACKET_ELEMENTS )
    {
        if( _.packet->header.includes & 0x01 )
        {
            if(!_.t) _.l = (*(packing_template_t*)_.llPtr).a;
            else          _.l = (*(packing_template_t*)_.llPtr).b;
            for( _.j = 0; _.j < _.l; _.j++)
                ((byte_t*)_.pdPtr)[_.j] = *(((byte_t*)* _.alPtr)+_.j);
            _.pdPtr += _.l;
        }
        _.alPtr++;
        _.includes >>= 1;
        if((_.t=!_.t )) ++_.llPtr;
    }
}

void GenerateObservationListFromPredictionsRhoUtility( prediction_t * prediction, uint8_t thresh )
{
    index_t i = 0;
    for( ; i < prediction->NumRegions && i < MAX_OBSERVATIONS; i++ )
    {
        index_t io = prediction->TrackingFiltersOrder[i];
        if( io >= MAX_TRACKING_FILTERS ) continue;
        floating_t x = prediction->TrackingFilters[io].value;
        io = prediction->RegionsOrder[i];
        region_t * region = &prediction->Regions[io];
        bool below_centroid = (density_t)x < prediction->PreviousCentroid;

        index_t density =  (index_t)region->density + (index_t)prediction->PreviousPeak[(uint8_t)below_centroid];
        density = BOUNDU( density, MAX_REGION_HEIGHT );
        LOG_RHO_BARE(RHO_DEBUG_2, "\t\t(%d) <%d %d %d>\n", i, density, thresh, io);
        prediction->ObservationList.observations[i] = (observation_t){ density, thresh, io };
    }
    prediction->ObservationList.length = i;
}
    
void GenerateObservationListsFromPredictionsRhoUtility( rho_core_t * core )
{
//    if(core->PredictionPair.x.NumRegions > 0)
//    {
//        LOG_RHO(RHO_DEBUG_2, "Creating observation list for X:\n");
//        RhoUtility.Predict.GenerateObservationList( &core->PredictionPair.x, core->ThreshByte );
//    }
    if(core->PredictionPair.y.NumRegions > 0)
    {
        LOG_RHO(RHO_DEBUG_2, "Creating observation list for Y:\n");
        RhoUtility.Predict.GenerateObservationList( &core->PredictionPair.y, core->ThreshByte );
    }
}

void UpdatePredictiveStateModelPairRhoUtility( rho_core_t * core )
{
#ifdef __PSM__
//    PSMFunctions.Update( &core->PredictiveStateModelPair.x, &core->PredictionPair.x.ObservationList, core->PredictionPair.x.NuRegions );
    PSMFunctions.Update( &core->PredictiveStateModelPair.y, &core->PredictionPair.y.ObservationList, core->PredictionPair.y.NuRegions );
    core->PredictiveStateModelPair.current_state = MAX( core->PredictiveStateModelPair.x.current_state, core->PredictiveStateModelPair.y.current_state );
    core->PredictiveStateModelPair.best_confidence = AVG2( core->PredictiveStateModelPair.x.best_confidence, core->PredictiveStateModelPair.y.best_confidence );
    core->PredictiveStateModelPair.proposed_nu = MAX( core->PredictiveStateModelPair.x.proposed_nu, core->PredictiveStateModelPair.y.proposed_nu );
    core->PredictiveStateModelPair.proposed_avg_den = AVG2( core->PredictiveStateModelPair.x.proposed_avg_den, core->PredictiveStateModelPair.y.proposed_avg_den );
#endif
}

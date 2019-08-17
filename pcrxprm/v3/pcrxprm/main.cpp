#include "tau_master.h"

int run( char instructions[] = {}, int num_instructions = 0, bool end_after_instructions = false, int width = FRAME_WIDTH, int height = FRAME_HEIGHT)
{
    TauDrawer tau("Tau", width, height
#ifndef HAS_CAMERA
      , FRAME_IMAGE_IMAGE_SOURCE_PATH, FRAME_IMAGE_SOURCE_NUM_FRAMES
#endif
      );
    Environment env(&tau, TAU_FPS);
    env.start();
    env.pause();
    sleep(0.1);
    env.resume();
    Mat local_frame;
    
    struct timeval a,b;
    int instruction_index = 0;
    while(1)
    {
        local_frame = tau.GetDensitiesFrame(tau.frame);
        if(local_frame.data != nullptr)
        {
//          imshow("Thresh Frame", tau.utility.outframe);
//          imshow(TITLE_STRING, local_frame);
//          imshow("Detection Map", tau.rho_drawer.GetDetectionMapFrame());
//          imshow("Rho Frame", local_frame);
//          imshow("X Detection", tau.DrawRhoDetection(X_DIMENSION));
//          imshow("Y Detection", tau.DrawRhoDetection(Y_DIMENSION));
        }

        
        char c = waitKey(KEY_DELAY);
        if(num_instructions)
        {
            c = instructions[instruction_index];
            if(instruction_index >= num_instructions)
            {
                if(end_after_instructions) return 0;
            }
            else instruction_index++;
        }
        ofstream file;
        switch(c)
        {
            case ' ':
                if(env.status != LIVE) env.resume();
                else env.pause();
                break;
            default:
                if(tau.utility.loop(c))
                {
                    tau.utility.trigger();
                    tau.trigger();
                }
                break;
            case 's':
                env.pause();
                sleep(0.01);
                tau.avg = 0;
                tau.count = 0;
                tau.stddev_sum = 0;
                env.start();
                gettimeofday( &a, NULL);
                sleep(10);
                gettimeofday( &b, NULL);
                env.pause();
                tau.stddev = sqrt( tau.stddev_sum / (double)(tau.count));
                //                printf("%d\n", tau.count);
                
                char buf[128];
                sprintf(buf, "PCR (Seg. Only),%d,%d,%lf,%f,%f,%f\n", width, tau.count, timeDiff(a,b), tau.avg * 1000., tau.accuracy / (double)width * 100., tau.stddev / (double)width * 100.);
                printf("%s", buf);
                
                file.open(PERF_FILENAME, ofstream::app | ofstream::out);
                file << string(buf);
                file.close();
                break;
            case 'p':
                struct timeval tp;
                gettimeofday(&tp, NULL);
                long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000; //get current timestamp in milliseconds
                imwrite(FRAME_SAVE_ROOT + to_string(ms) + ".png", local_frame);
                break;
        }
    }
}

int main(int argc, const char * argv[])
{
#ifdef AUTOMATION_RUN
//    ofstream file(PERF_FILENAME);
//    file << "Algorithm,Dimension(px),Iterations,Total Time(s),Avg. Time(ms), Avg. Diff.(%), Std. Dev. Diff. (%)\n";
//    file.close();
#ifdef AUTOMATION_INSTRUCTIONS
    char instructions[] = AUTOMATION_INSTRUCTIONS;
    int num_instructions = sizeof(instructions)/sizeof(instructions[0]);
#else
    char instructions[] = { '\0' };
    int num_instructions = 0;
#endif
    
#ifdef AUTOMATION_END_AFTER_INSTRUCTIONS
    bool end_after_instructions = true;
#else
    bool end_after_instructions = false;
#endif
#endif
    
#ifdef AUTOMATION_RUN
#ifdef AUTOMATION_SIZES
    int sizes[] = AUTOMATION_SIZES;
    int num_sizes = sizeof(sizes)/sizeof(sizes[0]);
    for(int i = 0; i < num_sizes; i++)
    {
        run( instructions, num_instructions, end_after_instructions, sizes[i], sizes[i] );
    }
    return 0;
#else
    return run( instructions, num_instructions );
#endif
#else
    return run();
#endif
    
}
/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <ctype.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <utils/threads.h>
#include <gui/Sensor.h>
#include <gui/SensorManager.h>
#include <gui/SensorEventQueue.h>
#include "psh_cht_spi_util.h"
#include <string.h>


static struct timestamp_item_data g_profile_data;


static double calc_frequency_ms(long count, long duration)
{
    return (1000000000.0 * count)/duration;
}


static void parse_distribution_data_by_range(struct timestamp_item_data* lp_profile_data,
                                             int start_index,int range,
                                             int total_data,
                                             int& range_data_sum, float& percentage)
{
    int loop_i;
    int data_sum = 0;

    for (loop_i = 0; loop_i < range; ++loop_i)
    {
        data_sum += atomic64_read(lp_profile_data->dist_array[start_index + loop_i]);
    }

    range_data_sum = data_sum;
    
    percentage = 0.0;
    if (total_data)
    {
        percentage = (100.0 * data_sum) / total_data;
    }

    return;
}

static void parse_distribution_data(struct timestamp_item_data* lp_profile_data, int range)
{
    int loop_i;
    long calced_count = 0;

    int max_index = 0;
    int max_index_end = 0;
    int min_index = 0;
    int min_index_end = 0;

    int max_data = 0;
    int min_data = INT_MAX;

    int remain_count;

    for (loop_i = 0; loop_i <= MAX_DIST_INDEX; ++loop_i)
    {
        calced_count += atomic64_read(lp_profile_data->dist_array[loop_i]);
    }

    loop_i = 0;
    while(loop_i <= MAX_DIST_INDEX)
    {
        int each_data = 0;
        float each_percentage = 0.0;

        int min_max_index = 0;
        int min_max_index_end = 0;
    
        remain_count = MAX_DIST_INDEX - loop_i;

        if (remain_count >= range)
        {
            parse_distribution_data_by_range(lp_profile_data,
                                             loop_i,range,
                                             calced_count,
                                             each_data, each_percentage);
            
            if (1 == range)
            {
                printf("[SS] %08d hz :  %8d       (%f%%)\n", loop_i,
                       each_data,
                       each_percentage);
            }
            else
            {
                printf("[SS] %08d hz - %08d hz :  %8d       (%f%%)\n",
                       loop_i, loop_i + range - 1,
                       each_data,
                       each_percentage);
            }
            
            min_max_index = loop_i;
            min_max_index_end = loop_i + range - 1;

            loop_i += range;
        }
        else
        {
            /*remain part if necessary*/
            each_data = atomic64_read(lp_profile_data->dist_array[loop_i]);

            each_percentage = 0.0;
            if (calced_count)
            {
                each_percentage = (100.0 * each_data) / calced_count;
            }

            printf("[SS] %08d hz :  %8d       (%f%%)\n", loop_i,
                   each_data,
                   each_percentage);

            min_max_index = loop_i;            
            min_max_index_end = loop_i;

            ++loop_i;
        }

        if (each_data > max_data)                             
        {                                                     
            max_data = each_data;                             
            max_index = min_max_index;                               
            max_index_end = min_max_index_end;
        }                                                     
        if (each_data < min_data && 0 != each_data)           
        {                                                     
            min_data = each_data;                             
            min_index = min_max_index;
            min_index_end = min_max_index_end;
        }
    }

    printf("[SS] distribution sample count = %lld\n", (int64_t)calced_count);

    if (1 == range)
    {
        printf("[SS] maximum freq in distribution: %d hz \n", max_index);
        printf("[SS] minimum freq in distribution: %d hz \n", min_index);
    }
    else
    {
        printf("[SS] maximum freq range in distribution: %d hz - %d hz\n",
               max_index, max_index_end);

        printf("[SS] minimum freq range in distribution: %d hz - %d hz\n",
               min_index, min_index_end);
    }

}


static void parse_profile_data(struct timestamp_item_data* lp_profile_data,
                               int distribution_range)
{
    float freq_max;
    float freq_min;
    float freq_avg;
    float freq_now;

    freq_avg = 0.0;
    if (atomic64_read(lp_profile_data->total_delta) 
        && atomic64_read(lp_profile_data->count))
    {
        freq_avg = calc_frequency_ms(atomic64_read(lp_profile_data->count),
                                     atomic64_read(lp_profile_data->total_delta));
    }
    
    freq_max = 0.0;
    if (atomic64_read(lp_profile_data->delta_min))
    {
        freq_max = calc_frequency_ms(1, atomic64_read(lp_profile_data->delta_min));
    }

    freq_min = 0.0;
    if (atomic64_read(lp_profile_data->delta_max))
    {
        freq_min = calc_frequency_ms(1, atomic64_read(lp_profile_data->delta_max));
    }

    freq_now = 0.0;
    if (atomic64_read(lp_profile_data->delta_now))
    {
        freq_now = calc_frequency_ms(1, atomic64_read(lp_profile_data->delta_now));
    }
    
    printf("[SS] frequency distribution data:\n");

    parse_distribution_data(lp_profile_data, distribution_range);

    printf("[SS] frequency distribution data end here\n\n");

    printf("[SS] sample count = %lld.\nnow_freq = %f | avg_freq = %f | min_freq = %f | max_freq = %f\n",
           atomic64_read(lp_profile_data->count),
           freq_now, freq_avg, freq_min, freq_max);

    printf("\n");
 
    return;
}

static void usage(void)
{
    printf("usage: sensor_profiler [profile file path] [distribution range] allow range: (1 - %d)\n",
           MAX_DIST_INDEX);
}

int main(int argc, char* argv[])
{
    char* lp_profile_file_path = NULL;

    FILE* lp_profile;
    int read_count;
    int require_read_count = sizeof(struct timestamp_item_data);
    int distribution_range = 0;

    if (argc < 3)
    {
        usage();
        return -1;
    }

    lp_profile_file_path = argv[1];
    distribution_range = atoi(argv[2]);

    if (0 == distribution_range
        || distribution_range > MAX_DIST_INDEX
        || !lp_profile_file_path)
    {
        printf("wrong parameter: %d\n", distribution_range);
        usage();
        return -1;
    }

    lp_profile = fopen(lp_profile_file_path, "rb");
    if (!lp_profile)
    {
        printf("can't open profile binary file, path =%s, ret = %d, errno = %d\n",
               lp_profile_file_path,
               0, errno);

        return -1;
    }

    read_count = fread(&g_profile_data,
                       require_read_count, 1,
                       lp_profile);

    printf("read profile file name: %s\ndata count = %d, require = %d.\n",
           lp_profile_file_path,
           read_count, require_read_count);

    if (read_count > 0)
    {
        parse_profile_data(&g_profile_data, distribution_range);
    }
    
    fclose(lp_profile);

    return 0;
}


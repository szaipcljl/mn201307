#include <linux/device.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>
#include <linux/pci.h>
#include <linux/circ_buf.h>
#include <linux/completion.h>
#include <linux/module.h>
#include <linux/dma-mapping.h>
#include <linux/string.h>
#include <linux/kthread.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/firmware.h>
#include <linux/spi/spi.h>
#include <linux/gpio.h>
#include <linux/sched.h>
#include <linux/pm_runtime.h>
#include <linux/workqueue.h>

#include "psh_ia_common.h"
#include "psh_cht_spi_extern.h"
#include "psh_cht_spi.h"
#include "psh_cht_spi_test.h"



static int stress_test_data_check(struct psh_ext_if* psh_if_info,
                                  struct cmd_resp* lp_resp,
                                  u8* lp_compare_buffer)
{
    int ret_value = TRUE;
    
    if (sizeof(struct ia_cmd) != lp_resp->data_len)
    {
        dev_err(&psh_if_info->pshc->dev, "STRESS_STEP_3_WAIT_DATA: wrong cmd resp->data_len: %d", (int)lp_resp->data_len);
        ret_value = FALSE;
    }
    
    if (RESP_LOW_TRANSFER_TEST != lp_resp->type)
    {
        dev_err(&psh_if_info->pshc->dev, "STRESS_STEP_3_WAIT_DATA: wrong cmd resp->type: %d", (int)lp_resp->type);
        ret_value = FALSE;
    }
    
    if (ret_value)
    {
        if (0 == memcmp(lp_compare_buffer, lp_resp->buf,
                        lp_resp->data_len))
        {    
            dev_err(&psh_if_info->pshc->dev, "STRESS_STEP_3_WAIT_DATA: OK\n");
        }                        
        else
        {
            dev_err(&psh_if_info->pshc->dev, "STRESS_STEP_3_WAIT_DATA compare received data wrong, dump:\n");
            dump_buffer(lp_resp->buf, lp_resp->data_len);
            ret_value = FALSE;
        }
    }

    return ret_value;
}




void poll_sensor_data_debug(struct psh_ext_if *psh_if_info)
{
    u8* lp_send;
    int send_size;
    int receved_size;
    int ret_value;
    static int data_seed;

	struct psh_ia_priv *ia_data = psh_if_info->ia_data;

    /*for debug only*/
    if (0 == strcmp(ia_data->debug_scheme, "stream_start"))
    {
        
    }
    else if (0 == strcmp(ia_data->debug_scheme, "xfer"))
    {        
        if (0 != ia_data->debug_send_count)
        {
            --ia_data->debug_send_count;

            ret_value = ia_send_cmd(ia_data,
                                    &ia_data->test_cmd, CMD_PARAM_MAX_SIZE,
                                    SEND_ASYNC);

            /*send the data directly here, because no normal receive when in debug*/
            {
                struct send_list_entry* lp_send_entry = NULL;
                
                lp_send_entry = remove_send_data_entry_from_list(psh_if_info);    
                
                if (lp_send_entry)
                {
                    INIT_SPI_IO_CONTEXT(&psh_if_info->send_data_list.low_spi_io_context);
                                        
                    lp_send = lp_send_entry->data;
                    send_size = lp_send_entry->used_size;
                    
                    ret_value = do_io_transaction(psh_if_info->pshc, 
                                                  &psh_if_info->send_data_list.low_spi_io_context,
                                                  lp_send, send_size,
                                                  psh_if_info->psh_frame, sizeof(psh_if_info->psh_frame),
                                                  &receved_size);
                    
                    /*send data logic*/
                    dev_info(&psh_if_info->pshc->dev,"send real data, size = %d ret_value = %d\n",
                             send_size, ret_value);                        
                }
            }
        }
        else
        {
            //stop it
            psh_if_info->task_flag &= (~TASK_FLAG_REQUEST_LOOP);
            ia_data->is_in_debug = FALSE;
        }
    }
    else if (0 == strcmp(ia_data->debug_scheme, "stress"))
    {
        
        if (STRESS_STEP_0_PREAPRE_DATA == ia_data->stress_test_step)
        {
            int loop_i = 0;
            
            ia_data->test_cmd.tran_id = loop_i;
            ia_data->test_cmd.cmd_id = CMD_LOW_TRANSFER_TEST;
            ia_data->test_cmd.sensor_id = 0;
            for (loop_i = 0; loop_i < CMD_PARAM_MAX_SIZE; ++ loop_i)
            {
                ia_data->test_cmd.param[loop_i] = data_seed + loop_i;
            }

            data_seed = (data_seed + 1) % 255;
            
            ret_value = ia_send_cmd(ia_data,
                                    &ia_data->test_cmd, CMD_PARAM_MAX_SIZE,
                                    SEND_ASYNC);
            if (IS_SUCCESS(ret_value))
            {
                ia_data->stress_test_step = STRESS_STEP_2_WAIT_ACK;

                /*send the data directly here, because no normal receive when in debug*/
                {
                    struct send_list_entry* lp_send_entry = NULL;
                    
                    lp_send_entry = remove_send_data_entry_from_list(psh_if_info);    
                    
                    if (lp_send_entry)
                    {
                        INIT_SPI_IO_CONTEXT(&psh_if_info->send_data_list.low_spi_io_context);
                        
                        
                        lp_send = lp_send_entry->data;
                        send_size = lp_send_entry->used_size;

                        ret_value = do_io_transaction(psh_if_info->pshc, 
                                                      &psh_if_info->send_data_list.low_spi_io_context,
                                                      lp_send, send_size,
                                                      psh_if_info->psh_frame, sizeof(psh_if_info->psh_frame),
                                                      &receved_size);

                        /*send data logic*/
                        dev_err(&psh_if_info->pshc->dev,"send real data, size = %d ret_value = %d\n",
                                 send_size, ret_value);                        
                    }
                }
            }
            else
            {
                dev_err(&psh_if_info->pshc->dev, "STRESS_STEP_0_PREPARE_DATA: send data error: %d \n", ret_value);
                psh_if_info->task_flag &= (~TASK_FLAG_REQUEST_LOOP);
            }            
        }
        else if (STRESS_STEP_2_WAIT_ACK == ia_data->stress_test_step)
        {
            INIT_SPI_IO_CONTEXT(&psh_if_info->send_data_list.low_spi_io_context);
            
            lp_send = psh_if_info->zero_send_buffer;
            send_size = sizeof(psh_if_info->zero_send_buffer);
            ret_value = do_io_transaction(psh_if_info->pshc, 
                                          &psh_if_info->send_data_list.low_spi_io_context,
                                          lp_send, send_size,
                                          psh_if_info->psh_frame, sizeof(psh_if_info->psh_frame),
                                          &receved_size);
            
            if (IS_SUCCESS(ret_value))
            {
                if (receved_size > SIZE_OF_FRAME_HEAD)
                {
                    union
                    {
                        struct cmd_resp* lp_resp;
                        u8* lp_u8;
                    }seeker;
                    struct resp_cmd_ack* lp_ack;

                    

                    seeker.lp_u8 = psh_if_info->psh_frame + SIZE_OF_FRAME_HEAD;
                    lp_ack = (struct resp_cmd_ack*)seeker.lp_resp->buf;

                    dev_info(&psh_if_info->pshc->dev, "sizeof(resp_cmd): %ld\n", sizeof(struct cmd_resp));
                    dev_info(&psh_if_info->pshc->dev, "sizeof(resp_cmd_ack): %ld\n", sizeof(struct resp_cmd_ack));

                    dev_info(&psh_if_info->pshc->dev, "resp_cmd start offset: %ld\n", seeker.lp_u8 - (u8*)psh_if_info->psh_frame);
                    dev_info(&psh_if_info->pshc->dev, "resp_cmd_ack start offset: %ld\n", (u8*)lp_ack - (u8*)psh_if_info->psh_frame);

                    if (lp_ack->cmd_id == CMD_LOW_TRANSFER_TEST)
                    {
                        if (receved_size > (SIZE_OF_FRAME_HEAD + seeker.lp_resp->data_len))
                        {
                            /*need move sizeof cmd_resp + it's payload size*/
                            seeker.lp_u8  = ((u8*)lp_ack) + seeker.lp_resp->data_len;
 
                            dev_info(&psh_if_info->pshc->dev, "resp_cmd for check received data start offset: %ld\n", seeker.lp_u8 - (u8*)psh_if_info->psh_frame);

                            ret_value = stress_test_data_check(psh_if_info,
                                                               seeker.lp_resp,
                                                               (u8*)&ia_data->test_cmd);
                            if (ret_value)
                            {
                                ia_data->stress_test_step = STRESS_STEP_0_PREAPRE_DATA;
                            }
                            else
                            {
                                ia_data->stress_test_step = STRESS_STEP_3_WAIT_DATA;
                            }
                        }
                        else
                        {
                            ia_data->stress_test_step = STRESS_STEP_3_WAIT_DATA;
                        }
                    }
                    else
                    {
                        psh_if_info->task_flag &= (~TASK_FLAG_REQUEST_LOOP);
                        dev_err(&psh_if_info->pshc->dev, "wrong ack cmd id: %d \n", (int)lp_ack->cmd_id);
                    }
                }
                else
                {
                    dev_err(&psh_if_info->pshc->dev, "STRESS_STEP_2_WAIT_ACK: recv wrong size: %d!\n", receved_size);
                    /*psh_if_info->task_flag &= (~TASK_FLAG_REQUEST_LOOP);*/
                }
            }
            else
            {
                dev_err(&psh_if_info->pshc->dev, "STRESS_STEP_2_WAIT_ACK: io error: %d\n", ret_value);
                psh_if_info->task_flag &= (~TASK_FLAG_REQUEST_LOOP);
            }
        }
        else if (STRESS_STEP_3_WAIT_DATA == ia_data->stress_test_step)
        {
            INIT_SPI_IO_CONTEXT(&psh_if_info->send_data_list.low_spi_io_context);

            lp_send = psh_if_info->zero_send_buffer;
            send_size = sizeof(psh_if_info->zero_send_buffer);            

            ret_value = do_io_transaction(psh_if_info->pshc, 
                                          &psh_if_info->send_data_list.low_spi_io_context,
                                          lp_send, send_size,
                                          psh_if_info->psh_frame, sizeof(psh_if_info->psh_frame),
                                          &receved_size);

            if (IS_SUCCESS(ret_value))
            {
                if (receved_size > 4)
                {
                    struct cmd_resp* lp_resp = (struct cmd_resp*)(psh_if_info->psh_frame + SIZE_OF_FRAME_HEAD);
                    
                    ret_value = stress_test_data_check(psh_if_info,
                                                       lp_resp,
                                                       (u8*)&ia_data->test_cmd);
                    if (ret_value)
                    {
                        ia_data->stress_test_step = STRESS_STEP_0_PREAPRE_DATA;
                    }
                    else
                    {
                        psh_if_info->task_flag &= (~TASK_FLAG_REQUEST_LOOP);
                    }
                }
                else
                {
                    dev_err(&psh_if_info->pshc->dev, "STRESS_STEP_3_WAIT_DATA: recv wrong size: %d!\n", receved_size);
                    psh_if_info->task_flag &= (~TASK_FLAG_REQUEST_LOOP);
                }
            }
            else
            {
                dev_err(&psh_if_info->pshc->dev, "STRESS_STEP_3_WAIT_DATA: io error: %d", ret_value);
                psh_if_info->task_flag &= (~TASK_FLAG_REQUEST_LOOP);
            }
        }
        else if (STRESS_STEP_QUIT == ia_data->stress_test_step)
        {
            psh_if_info->task_flag &= (~TASK_FLAG_REQUEST_LOOP);
        }
    }
}



void dump_cmd_resp(char *ptr, int len)
{
	struct cmd_resp *resp = (struct cmd_resp *)ptr;
	u32 size = sizeof(struct cmd_resp) + resp->data_len;
	int i;

	printk(KERN_DEBUG"%s, tran_id=%d, type=%d, sensor_id=%d, data_len=%d\n", __func__, resp->tran_id, resp->type, resp->sensor_id, size);

	for(i=0 ; i<len ; i++)
	{
		printk(" %d", ptr[i]);
	}
	printk(KERN_DEBUG"\n");	
}

void dump_buffer(void* lp_dumpbuffer, int size)
{
#if 0
    int loop_end = size / 8;
    int remain_end = size % 8;
    int index = 0;
    int loop_i;
    u8* lp_buffer = (u8*)lp_dumpbuffer;

    if (0 == size)
    {
        return;
    }

    printk(KERN_ERR"spi dump: size = %d\n", size); 
    for (loop_i = 0; loop_i < loop_end; ++loop_i)
    {
        printk(KERN_ERR"spi dump: [%.2d] 0x%.2x, 0x%.2x, 0x%.2x, 0x%.2x, 0x%.2x, 0x%.2x, 0x%.2x, 0x%.2x \n", 
               8 * loop_i,
               lp_buffer[index + 8 * loop_i + 0],
               lp_buffer[index + 8 * loop_i + 1],
               lp_buffer[index + 8 * loop_i + 2],
               lp_buffer[index + 8 * loop_i + 3],
               lp_buffer[index + 8 * loop_i + 4],
               lp_buffer[index + 8 * loop_i + 5],
               lp_buffer[index + 8 * loop_i + 6],
               lp_buffer[index + 8 * loop_i + 7]);
    }

    lp_buffer += 8 * loop_end;
    if (0 != remain_end)
    {
        //printk(KERN_ERR"[%0.2d]:", index);
        for (loop_i = 0; loop_i < remain_end; ++loop_i)
        {
            printk(KERN_ERR"spi dump: [%.2d] 0x%.2x \n", 
                   8 * loop_end + loop_i,
                   lp_buffer[loop_i]);
        }
    }
#endif

    return;
}

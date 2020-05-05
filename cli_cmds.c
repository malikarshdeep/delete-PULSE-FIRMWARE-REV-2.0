/**
 * Copyright (c) 2018 - 2018, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
#include  "nrf_cli.h"
#include "nrf_log.h"
#include <ctype.h>

#include "ble.h"
#include "pulse_uicr.h"
#include "FreeRTOS.h"
#include "task.h"

#define CLI_EXAMPLE_MAX_CMD_CNT (20u)
#define CLI_EXAMPLE_MAX_CMD_LEN (33u)

/* buffer holding dynamicly created user commands */
static char m_dynamic_cmd_buffer[CLI_EXAMPLE_MAX_CMD_CNT][CLI_EXAMPLE_MAX_CMD_LEN];
/* commands counter */
static uint8_t m_dynamic_cmd_cnt;

uint32_t m_counter;
bool     m_counter_active = false;

/* Command handlers */
static void cmd_print_param(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    for (size_t i = 1; i < argc; i++)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "argv[%d] = %s\r\n", i, argv[i]);
    }
}

static void cmd_print_all(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    for (size_t i = 1; i < argc; i++)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "%s ", argv[i]);
    }
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "\r\n");
}

static void cmd_print(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc != 2)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: bad parameter count\r\n", argv[0]);
        return;
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s: unknown parameter: %s\r\n", argv[0], argv[1]);
}


static void cmd_serial_program(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
  uint32_t sn;
  uint32_t i;
  if (argc>2)
  {
    nrf_cli_fprintf (p_cli, NRF_CLI_NORMAL, "%s: too many arguments\r\n", argv[0]);
    return;
  }
  if (argc<2)
  {
    nrf_cli_fprintf (p_cli, NRF_CLI_NORMAL, "%s: argument is expected\r\n", argv[0]);
    return;
  }
  i =0;
  //check if argument is less than 8 digit hex
  while(argv[1][i] != '\0')
  {
 
    if ( (i<8) && (((argv[1][i]>='0') && (argv[1][i]<='9')) || ((argv[1][i]>='A') && (argv[1][i]<='F')) || ((argv[1][i]>='a') && (argv[1][i]<='f'))))
    {
      ++i;
    }
    else
    {        
      nrf_cli_fprintf (p_cli, NRF_CLI_NORMAL, "%s: invalid argument: %s\r\n", argv[0], argv[0]);
      return;
    }        
  }
  sscanf(argv[1], "%x", &sn);
  nrf_cli_fprintf (p_cli, NRF_CLI_NORMAL, "sn is %08x\r\n", sn);
  if (pulse_serial_number == 0xFFFFFFFF)
  {
     nrf_cli_fprintf (p_cli, NRF_CLI_NORMAL, "%s: programming serial number: %08x and reboot\r\n", argv[0], sn); 
     ble_deinit();
     taskENTER_CRITICAL();
     NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;
     while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
     pulse_serial_number = sn;
     NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;
     while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
     NVIC_SystemReset();
  }
  else
  {
     nrf_cli_fprintf (p_cli, NRF_CLI_NORMAL, "serial number is already programmed\r\n", argv[0]);
     return;
  }
}


static void cmd_serial(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
  nrf_cli_fprintf (p_cli, NRF_CLI_NORMAL, "device serial number is %08x\r\n", pulse_serial_number);
}



/* function required by qsort */
static int string_cmp(const void * p_a, const void * p_b)
{
    ASSERT(p_a);
    ASSERT(p_b);
    return strcmp((const char *)p_a, (const char *)p_b);
}



NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_serial)
{
    NRF_CLI_CMD(program, NULL, "Flashes the serial number into memory", cmd_serial_program),
    NRF_CLI_SUBCMD_SET_END
};

NRF_CLI_CMD_REGISTER(serial, &m_sub_serial, "serial", cmd_serial);


/**
 * @brief Command set array
 * */

NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_print)
{
    NRF_CLI_CMD(all,   NULL, "Print all entered parameters.", cmd_print_all),
    NRF_CLI_CMD(param, NULL, "Print each parameter in new line.", cmd_print_param),
    NRF_CLI_SUBCMD_SET_END
};
NRF_CLI_CMD_REGISTER(print, &m_sub_print, "print", cmd_print);

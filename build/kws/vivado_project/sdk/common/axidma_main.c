#include "xaxidma.h"
#include "xparameters.h"
#include "unistd.h"      /* sleep */
#include "xil_printf.h"  /* UART debug print functions */
#include "stdio.h"       /* printf (w/ float) support */
#include "platform.h"    /* platform init/cleanup functions */
#include "xtime_l.h"     /* to measure performance of the system */

#include "src.h"
#include "dst.h"

#ifdef __USE_OCM__
#define ITER_N 505100
#define CMD_CNT (1)
#ifndef CMD_N
#define CMD_N (1)
#endif
#else
#define ITER_N 1
#define CMD_CNT (10102)
#ifndef CMD_N
#define CMD_N (10102)
#endif
#endif
#define INPUT_N_FEATURES (490)
#define OUTPUT_N_FEATURES (1)

//#define __DEBUG__
#define MAX_PRINT_ELEMENTS 16

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
    (byte & 0x80 ? '1' : '0'), \
    (byte & 0x40 ? '1' : '0'), \
    (byte & 0x20 ? '1' : '0'), \
    (byte & 0x10 ? '1' : '0'), \
    (byte & 0x08 ? '1' : '0'), \
    (byte & 0x04 ? '1' : '0'), \
    (byte & 0x02 ? '1' : '0'), \
    (byte & 0x01 ? '1' : '0')

#define SHORT_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"
#define SHORT_TO_BINARY(byte)  \
    (byte & 0x8000 ? '1' : '0'), \
    (byte & 0x4000 ? '1' : '0'), \
    (byte & 0x2000 ? '1' : '0'), \
    (byte & 0x1000 ? '1' : '0'), \
    (byte & 0x800 ? '1' : '0'), \
    (byte & 0x400 ? '1' : '0'), \
    (byte & 0x200 ? '1' : '0'), \
    (byte & 0x100 ? '1' : '0'), \
    (byte & 0x80 ? '1' : '0'), \
    (byte & 0x40 ? '1' : '0'), \
    (byte & 0x20 ? '1' : '0'), \
    (byte & 0x10 ? '1' : '0'), \
    (byte & 0x08 ? '1' : '0'), \
    (byte & 0x04 ? '1' : '0'), \
    (byte & 0x02 ? '1' : '0'), \
    (byte & 0x01 ? '1' : '0')

/* Base address for the AXI DMA */
#define DMA_DEV_ID XPAR_AXIDMA_0_DEVICE_ID

/* AXI DMA instance */
XAxiDma axi_dma;
XAxiDma_Config *axi_dma_cfg;

/* AXI DMA initialization routine */
int init_axi_dma(u16 device_id) {
    int status;

    xil_printf("INFO: Initializing AXI DMA\n\r");
    axi_dma_cfg = XAxiDma_LookupConfig(device_id);
    if (!axi_dma_cfg) {
        xil_printf("No config found for %d\r\n", device_id);
        return XST_FAILURE;
    }

    status = XAxiDma_CfgInitialize(&axi_dma, axi_dma_cfg);
    if (status != XST_SUCCESS) {
        xil_printf("Initialization failed %d\r\n", status);
        return XST_FAILURE;
    }

    if(XAxiDma_HasSg(&axi_dma)){
        xil_printf("Device configured as SG mode \r\n");
        return XST_FAILURE;
    }

    /* Disable interrupts, we use polling mode */
    XAxiDma_IntrDisable(&axi_dma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
    XAxiDma_IntrDisable(&axi_dma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);

    return XST_SUCCESS;
}

/* Golden model of the accelerator in software */
int finn_cnv_w1a1_sw(u8 *rx, u8 *tx, unsigned img_n, unsigned input_n_features, unsigned output_n_features) {
    xil_printf("INFO: Golden results are pre-compiled\r\n");
    xil_printf("INFO: It would be nice to run a software model\r\n");
    // See src.h and dst.h for input and golden output respectively.
    for (u32 i = 0; i < (img_n * output_n_features); i++) {
        rx[i] = dst_data[i];
    }
    return 0;
}

/* Profiling utility */
double get_elapsed_time(XTime start, XTime stop) {
    return 1.0 * (stop - start) / (COUNTS_PER_SECOND);
}

/* Dump data to the console */
void dump_data(const char* label, u8* data, unsigned sample_count, unsigned feature_count, unsigned print_hex, unsigned print_bin) {
    xil_printf("INFO:   %s[%u][%u]:\n\r", label, sample_count, feature_count);
    /* print at most MAX_PRINT_ELEMENTS */
    for (unsigned i = 0; i < sample_count && i < MAX_PRINT_ELEMENTS; i++)
    {
        xil_printf("INFO:     [%u] ", i);
        if (print_hex)
            for (unsigned j = 0; j < feature_count; j++)
            {
                unsigned index = i * feature_count + j;
                xil_printf("%02X ", data[index]);
            }
        if (print_bin)
            for (unsigned j = 0; j < feature_count; j++)
            {
                unsigned index = i * feature_count + j;
                xil_printf(""SHORT_TO_BINARY_PATTERN, SHORT_TO_BINARY(data[index]));
                xil_printf(" ");
            }
        xil_printf("\n\r");
    }
}

/* The top of the hill :-) */
int main(int argc, char** argv) {

    int status;
    XTime start, stop;
    double calibration_time;
    double sw_elapsed;
    char __attribute__ ((unused)) dummy; /* dummy input */

    int hw_errors;
    double hw_elapsed = 0;
    double cache_elapsed = 0;

    /* Initialize platform (uart and caches) */
    init_platform();

    xil_printf("\n\r");
    xil_printf("INFO: =========================================================================\n\r");
    xil_printf("INFO: FINN cnv-w1a1 (w/ polling, AXI DMA, batch = 1)\n\r");
    xil_printf("INFO: =========================================================================\n\r");

    init_axi_dma(DMA_DEV_ID);

    /* Calibration */
    XTime_GetTime(&start);
    sleep(1);
    XTime_GetTime(&stop);
    calibration_time = get_elapsed_time(start, stop);
    printf("INFO: Time calibration for one second (%lf sec)\n\r", calibration_time);

    /* Initialize memory */
    xil_printf("INFO: Initialize memory\r\n");
    xil_printf("INFO:   - Total inputs: %u\r\n", CMD_CNT);
    xil_printf("INFO:   - Batch size: %u\r\n", CMD_N);
    xil_printf("INFO:   - Iterations: %u\r\n", ITER_N);
    xil_printf("INFO:   - Input features: %u\r\n", INPUT_N_FEATURES);
    xil_printf("INFO:   - Output features: %u\r\n", OUTPUT_N_FEATURES);
    xil_printf("INFO:   - Single feature data size: %u B\r\n", sizeof(u8));
    printf("INFO:   - Total input data size: %uB, %.2fKB\r\n", INPUT_N_FEATURES * CMD_CNT * sizeof(u8), (INPUT_N_FEATURES * CMD_CNT * sizeof(u8))/1024.0);
    printf("INFO:   - Total output data size: %uB, %.2fKB\r\n", OUTPUT_N_FEATURES * CMD_CNT * sizeof(u8), (OUTPUT_N_FEATURES * CMD_CNT * sizeof(u8))/1024.0);

    /* Set Heap Size in ldscript.ld to 0x1000000 (16MB) */
    u8 *tx_buffer_ptr = (u8*)malloc(INPUT_N_FEATURES * CMD_CNT * sizeof(u8));
    u8 *rx_buffer_ptr = (u8*)malloc(OUTPUT_N_FEATURES * CMD_CNT * sizeof(u8));
    u8 *rx_buffer_gld_ptr = (u8*)malloc(OUTPUT_N_FEATURES * CMD_CNT * sizeof(u8));
    /* malloc_stats(); */

    /* Load data in the TX/RX buffers */
    for (int i = 0; i < INPUT_N_FEATURES * CMD_CNT; i++) {
        tx_buffer_ptr[i] = src_data[i];
    }
    for (int i = 0; i < OUTPUT_N_FEATURES * CMD_CNT; i++) {
        rx_buffer_gld_ptr[i] = 0x0;
        rx_buffer_ptr[i] = 0x0;
    }

    /* ****** SOFTWARE REFERENCE ****** */
    xil_printf("INFO: Run SW reference model\r\n");
    XTime_GetTime(&start);
    finn_cnv_w1a1_sw(rx_buffer_gld_ptr, tx_buffer_ptr, CMD_CNT, INPUT_N_FEATURES, OUTPUT_N_FEATURES);
    XTime_GetTime(&stop);
    sw_elapsed = get_elapsed_time(start, stop);

    /* ****** ACCELERATOR ****** */
    xil_printf("INFO: Press any key to start the HW accelerator: ");
    dummy = inbyte();
    xil_printf("\n\rINFO: \n\r");

    xil_printf("INFO: Configure and start HW accelerator\n\r");

    for (u32 b = 0; b < ITER_N; b++)
    for (u32 idx = 0; idx < CMD_CNT; idx++) {

        XTime_GetTime(&start);
        Xil_DCacheFlushRange((UINTPTR)(tx_buffer_ptr + idx * INPUT_N_FEATURES), INPUT_N_FEATURES * sizeof(u8));
        Xil_DCacheFlushRange((UINTPTR)(rx_buffer_ptr + idx * OUTPUT_N_FEATURES), OUTPUT_N_FEATURES * sizeof(u8));
        XTime_GetTime(&stop);
        cache_elapsed = get_elapsed_time(start, stop);

        XTime_GetTime(&start);

        status = XAxiDma_SimpleTransfer(&axi_dma,(UINTPTR)(tx_buffer_ptr + idx * INPUT_N_FEATURES), INPUT_N_FEATURES, XAXIDMA_DMA_TO_DEVICE);
        if (status != XST_SUCCESS) {
            xil_printf("ERROR: DMA_TO_DEVICE failed, status %u\n\r", status);
            return XST_FAILURE;
        }

        status = XAxiDma_SimpleTransfer(&axi_dma, (UINTPTR)(rx_buffer_ptr + idx * OUTPUT_N_FEATURES), OUTPUT_N_FEATURES, XAXIDMA_DEVICE_TO_DMA);
        if (status != XST_SUCCESS) {
            xil_printf("ERROR: DEVICE_TO_DMA failed, status %u\n\r", status);
            return XST_FAILURE;
        }

        while ((XAxiDma_Busy(&axi_dma, XAXIDMA_DEVICE_TO_DMA))) {
            ; /* Wait */
        }

        while ((XAxiDma_Busy(&axi_dma, XAXIDMA_DMA_TO_DEVICE))) {
            ; /* Wait */
        }

        XTime_GetTime(&stop);
        hw_elapsed += get_elapsed_time(start, stop);

        XTime_GetTime(&start);
        //Xil_DCacheFlushRange((UINTPTR)(rx_buffer_ptr + idx * OUTPUT_N_FEATURES), OUTPUT_N_FEATURES * sizeof(data_t));
        Xil_DCacheInvalidateRange((UINTPTR)(rx_buffer_ptr + idx * OUTPUT_N_FEATURES), OUTPUT_N_FEATURES * sizeof(u8));
        XTime_GetTime(&stop);
        cache_elapsed += get_elapsed_time(start, stop);

    }

    /* ****** VALIDATION ****** */
    xil_printf("INFO: ============================== Validation ===============================\n\r");
#ifdef __DEBUG__
    xil_printf("INFO: Dump data\n\r");
    //dump_data("src", (data_t*)(tx_buffer_ptr + IMG_ID * INPUT_N_FEATURES), 1, INPUT_N_FEATURES, 1, 0);
    dump_data("sw_dst", (u8*)(rx_buffer_gld_ptr), IMG_N, OUTPUT_N_FEATURES, 1, 0);
    dump_data("hw_dst", (u8*)(rx_buffer_ptr), IMG_N, OUTPUT_N_FEATURES, 1, 0);
#endif

    printf("INFO: Total SW reference model execution time: %f sec\r\n", sw_elapsed);
    printf("INFO: Total HW accelerator execution time: %f sec\r\n", hw_elapsed);
    printf("INFO: Average HW accelerator execution time: %f sec\r\n", hw_elapsed / (CMD_N * ITER_N));
    printf("INFO: Total cache flush time: %f sec\r\n", cache_elapsed);
    //printf("INFO: Accelerator/software speedup (the sofware is fake so this does not count...): %.2f X\r\n", (sw_elapsed >= (hw_elapsed+cache_elapsed))?(sw_elapsed/(hw_elapsed+cache_elapsed)):-((hw_elapsed+cache_elapsed)/sw_elapsed));

    /* Accelerator validation */
    hw_errors = 0;
    for (int i = 0; i < OUTPUT_N_FEATURES * CMD_CNT; i++) {
        if ((rx_buffer_ptr)[i] != (rx_buffer_gld_ptr)[i]) {
#ifdef __DEBUG__
            xil_printf("ERROR: [%d]: Accelerator hw %02X != sw %02X !!!\n\r", i, (rx_buffer_ptr)[i], (rx_buffer_gld_ptr)[i]);
#endif
            hw_errors++;
        }
    }

    float error_rate = hw_errors / ((float) OUTPUT_N_FEATURES * CMD_CNT);
    printf("INFO: Total errors = %d (out of %d elements)\r\n", hw_errors, OUTPUT_N_FEATURES * CMD_N);
    printf("INFO: Error rate = %.1f%%\r\n", error_rate * 100);
    printf("INFO: Model accuracy = %.1f%%\r\n", 100 - (error_rate*100));
    if (hw_errors > 0)
        xil_printf("INFO: Accelerator validation: FAIL\n\r");
    else
        xil_printf("INFO: Accelerator validation: PASS!\n\r");


    xil_printf("INFO: Done!\n\r");
    xil_printf("INFO: =========================================================================\n\r");
    cleanup_platform();

    return 0;
}



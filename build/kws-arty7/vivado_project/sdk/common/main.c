#include "xaxidma.h"
#include "xparameters.h"
#include "unistd.h"      /* sleep */
#include "xil_printf.h"  /* UART debug print functions */
#include "stdio.h"       /* printf (w/ double) support */
#include "platform.h"    /* platform init/cleanup functions */
#include "xtmrctr.h"     /* timer, Xilinx IP Timer Counter */

#include "src.h"
#include "dst.h"

#define CMD_CNT (10102)
#ifndef CMD_N
#define CMD_N (10102)
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

#define DDR_BASE_ADDR XPAR_MIG_7SERIES_0_BASEADDR
#define MEM_BASE_ADDR (DDR_BASE_ADDR + 0x1000000)

/* AXI DMA instance */
XAxiDma axi_dma;
XAxiDma_Config *axi_dma_cfg;

/* Profiling utilities */
static XTmrCtr TimerCounterInst;
#define TMRCTR_DEVICE_ID    XPAR_TMRCTR_0_DEVICE_ID
#define TIMER_CNTR_0        0
#define TIMER_CNTR_1        1

u64 get_64b_counter_value() {
    u64 counter = 0, lo_counter = 0, hi_counter = 0; 
    lo_counter = XTmrCtr_GetValue(&TimerCounterInst, TIMER_CNTR_0);
    //hi_counter = XTmrCtr_GetValue(&TimerCounterInst, TIMER_CNTR_1);
    counter = (hi_counter << 32ull) | lo_counter;
    return counter;
}

u64 start_64b_counter() {
    XTmrCtr_Start(&TimerCounterInst, TIMER_CNTR_0);
    //XTmrCtr_Start(&TimerCounterInst, TIMER_CNTR_1);
    u64 value = get_64b_counter_value();
    return value;
}

u64 stop_64b_counter() {
    XTmrCtr_Stop(&TimerCounterInst, TIMER_CNTR_0);
    //XTmrCtr_Stop(&TimerCounterInst, TIMER_CNTR_1);
    u64 value = get_64b_counter_value();
    return value;
}

double get_elapsed_time(u64 clk_start, u64 clk_stop) {
    return ((clk_stop-clk_start) * (1.0/XPAR_AXI_TIMER_MCU_CLOCK_FREQ_HZ));
}

/* AXI DMA initialization routine */
int init_axi_dma(u16 device_id) {
    int status;

    xil_printf("INFO: Initializing AXI DMA\r\n");
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
    return 0;
}

/* Dump data to the console */
void dump_data(const char* label, u8* data, unsigned sample_count, unsigned feature_count, unsigned print_dec, unsigned print_hex, unsigned print_bin) {
    xil_printf("INFO:   %s[%u][%u]:\r\n", label, sample_count, feature_count);
    /* print at most MAX_PRINT_ELEMENTS */
    for (unsigned i = 0; i < sample_count && i < MAX_PRINT_ELEMENTS; i++)
    {
        xil_printf("INFO:     [%u]\r\n", i);
        xil_printf("INFO:       ");
        if (print_dec)
            for (unsigned j = 0; j < feature_count; j++)
            {
                unsigned index = i * feature_count + j;
                xil_printf("%d ", (int)((s8*)data)[index]);
                if (j % 9 == 0) xil_printf("\r\nINFO:       ");
            }
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
        xil_printf("\r\n");
    }
}

/* The top of the hill :-) */
int main(int argc, char** argv) {

    int status;
    u64 start = 0, stop = 0;
    double calibration_time = 0;
    double sw_elapsed = 0;
    char __attribute__ ((unused)) dummy; /* dummy input */

    int hw_errors;
    double hw_elapsed = 0;
    double cache_elapsed = 0;

    xil_printf("\r\n");
    xil_printf("INFO: =========================================================================\r\n");
    xil_printf("INFO: FINN KWS-MLP-W3A3 (w/ polling, AXI DMA, batch = 1)\r\n");
    xil_printf("INFO: =========================================================================\r\n");

    /* Initialize platform (uart and caches) */
    init_platform();

    init_axi_dma(DMA_DEV_ID);

    status = XTmrCtr_Initialize(&TimerCounterInst, TMRCTR_DEVICE_ID);
    if (status != XST_SUCCESS){
      xil_printf("ERROR: Timer counter initialization failed!\r\n");
      return XST_FAILURE;
    }

    /* Calibration */
    start = start_64b_counter();
    sleep(1);
    stop = stop_64b_counter();
    calibration_time = get_elapsed_time(start, stop);
    printf("INFO: Time calibration for one second (%f sec)\r\n", calibration_time);

    /* Initialize memory */
    xil_printf("INFO: Initialize memory\r\n");
    xil_printf("INFO:   - Total image count: %u\r\n", CMD_CNT);
    xil_printf("INFO:   - Image batch size: %u\r\n", CMD_N);
    xil_printf("INFO:   - Input-feature count: %u\r\n", INPUT_N_FEATURES);
    xil_printf("INFO:   - Output-feature count: %u\r\n", OUTPUT_N_FEATURES);
    xil_printf("INFO:   - Data size: %u B\r\n", sizeof(u8));

    /* Set Heap Size in ldscript.ld to 0x1000000 (16MB) */
    u8 *tx_buffer_ptr = (u8*)malloc(INPUT_N_FEATURES * CMD_CNT * sizeof(u8));
    u8 *rx_buffer_ptr = (u8*)malloc(OUTPUT_N_FEATURES * CMD_CNT * sizeof(u8));
    u8 *rx_buffer_gld_ptr = (u8*)malloc(OUTPUT_N_FEATURES * CMD_CNT * sizeof(u8));
    //malloc_stats();

    /* Load data in the TX/RX buffers */
    for (int i = 0; i < INPUT_N_FEATURES * CMD_CNT; i++) {
        tx_buffer_ptr[i] = src_data[i];
    }
    for (int i = 0; i < OUTPUT_N_FEATURES * CMD_CNT; i++) {
        rx_buffer_gld_ptr[i] = dst_data[i];
        rx_buffer_ptr[i] = 0x0;
    }

    /* ****** SOFTWARE REFERENCE ****** */
#ifdef __DEBUG__
    xil_printf("INFO: Start SW accelerator\r\n");
#endif
    start = start_64b_counter();
    finn_cnv_w1a1_sw(tx_buffer_ptr, rx_buffer_gld_ptr, CMD_CNT, INPUT_N_FEATURES, OUTPUT_N_FEATURES);
    stop = stop_64b_counter();
    sw_elapsed = get_elapsed_time(start, stop);


    /* ****** ACCELERATOR ****** */
    xil_printf("INFO: Press any key to start the accelerator: ");
    dummy = inbyte();
    xil_printf("\r\nINFO: \r\n");

#ifdef __DEBUG__
    xil_printf("INFO: Configure and start accelerator\r\n");
#endif

#ifdef __DEBUG__
    xil_printf("INFO: ");
#endif

#if 1
    for (int idx = 0; idx < CMD_N; idx++) {
#ifdef __DEBUG__
        /* xil_printf("INFO: Image #%u\r\n", idx); */
        xil_printf(".");
#endif

        start = start_64b_counter();
        Xil_DCacheFlushRange((UINTPTR)(tx_buffer_ptr + idx * INPUT_N_FEATURES), INPUT_N_FEATURES * sizeof(u8));
        Xil_DCacheFlushRange((UINTPTR)(rx_buffer_ptr + idx * OUTPUT_N_FEATURES), OUTPUT_N_FEATURES * sizeof(u8));
        stop = stop_64b_counter();
        cache_elapsed = get_elapsed_time(start, stop);

        start = start_64b_counter();
        status = XAxiDma_SimpleTransfer(&axi_dma, (UINTPTR)(tx_buffer_ptr + idx * INPUT_N_FEATURES), INPUT_N_FEATURES, XAXIDMA_DMA_TO_DEVICE);
        if (status != XST_SUCCESS) {
            xil_printf("ERROR: DMA_TO_DEVICE failed, status %u\r\n", status);
            return XST_FAILURE;
        }

        status = XAxiDma_SimpleTransfer(&axi_dma, (UINTPTR)(rx_buffer_ptr + idx * OUTPUT_N_FEATURES), OUTPUT_N_FEATURES, XAXIDMA_DEVICE_TO_DMA);
        if (status != XST_SUCCESS) {
            xil_printf("ERROR: DEVICE_TO_DMA failed, status %u\r\n", status);
            return XST_FAILURE;
        }

        while (XAxiDma_Busy(&axi_dma, XAXIDMA_DEVICE_TO_DMA)) {
            ; /* Wait */
        }

        while (XAxiDma_Busy(&axi_dma, XAXIDMA_DMA_TO_DEVICE)) {
            ; /* Wait */
        }
        stop = stop_64b_counter();
        hw_elapsed += get_elapsed_time(start, stop);

        start = start_64b_counter();
        //Xil_DCacheFlushRange((UINTPTR)(rx_buffer_ptr + idx * OUTPUT_N_FEATURES), OUTPUT_N_FEATURES * sizeof(data_t));
        Xil_DCacheInvalidateRange((UINTPTR)(rx_buffer_ptr + idx * OUTPUT_N_FEATURES), OUTPUT_N_FEATURES * sizeof(u8));
        stop = stop_64b_counter();
        cache_elapsed += get_elapsed_time(start, stop);
    }

#ifdef __DEBUG__
    xil_printf("\r\n");
#endif
#endif /* __BATCH_MODE__ */

    /* ****** VALIDATION ****** */

#ifdef __DEBUG__
    xil_printf("INFO: ============================== Validation ===============================\r\n");
    xil_printf("INFO: Dump data\r\n");
    dump_data("tx_buffer", (u8*)(tx_buffer_ptr), 1, INPUT_N_FEATURES, 1, 0, 0);
    //dump_data("sw_dst", (u8*)(rx_buffer_gld_ptr), IMG_N, OUTPUT_N_FEATURES, 1, 0);
    //dump_data("hw_dst", (u8*)(rx_buffer_ptr), IMG_N, OUTPUT_N_FEATURES, 1, 0);
#endif

    printf("INFO: Software execution time: %f sec\r\n", sw_elapsed);

    printf("INFO: Total accelerator execution time: %f sec\r\n", hw_elapsed);
    printf("INFO: Average accelerator execution time: %f sec\r\n", hw_elapsed / CMD_N);
    printf("INFO: Cache flush time: %f sec\r\n", cache_elapsed);
    printf("INFO: Accelerator/software speedup (the sofware is fake so this does not count...): %.2f X\r\n", (sw_elapsed >= (hw_elapsed+cache_elapsed))?(sw_elapsed/(hw_elapsed+cache_elapsed)):-((hw_elapsed+cache_elapsed)/sw_elapsed));

    /* Accelerator validation */
    hw_errors = 0;
    for (int i = 0; i < OUTPUT_N_FEATURES * CMD_N; i++) {
        if ((rx_buffer_ptr)[i] != (rx_buffer_gld_ptr)[i]) {
#ifdef __DEBUG__
            xil_printf("ERROR: [%d]: Accelerator hw %d != sw %d !!!\r\n", i, (rx_buffer_ptr)[i], (rx_buffer_gld_ptr)[i]);
#endif
            hw_errors++;
        }
    }

    double error_rate = hw_errors / ((double) OUTPUT_N_FEATURES * CMD_N);
    printf("INFO: Total errors = %d (out of %d elements, error rate = %.1f%%)\r\n", hw_errors, OUTPUT_N_FEATURES * CMD_N, error_rate * 100);
    if (hw_errors > 0)
        xil_printf("INFO: Accelerator validation: FAIL\r\n");
    else
        xil_printf("INFO: Accelerator validation: PASS!\r\n");


    xil_printf("INFO: Done!\r\n");
    xil_printf("INFO: =========================================================================\r\n");

    cleanup_platform();

    return 0;
}


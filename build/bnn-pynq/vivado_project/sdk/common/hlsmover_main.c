//
//#include "stdlib.h"
//#include "malloc.h"
//#include "xil_io.h"      /* peripheral read/write wrappers */
//
//
////#include "xil_cache.h"   /* enable/disable caches etc */
//#include "xparameters.h" /* peripherals base addresses */

//#include "xaxidma.h"

#include "xstreamingdataflowpartition_0_iodma_0_streamingdataflowpartition_0_iodma_0.h"

#include "xparameters.h"
#include "unistd.h"      /* sleep */
#include "xil_printf.h"  /* UART debug print functions */
#include "stdio.h"       /* printf (w/ float) support */
#include "platform.h"    /* platform init/cleanup functions */
#include "xtime_l.h"     /* to measure performance of the system */

#include "src.h"
#include "dst.h"

#define IMG_ID 5
#define IMG_H 32
#define IMG_W 32
#define IMG_C 3
#define INPUT_N_FEATURES (IMG_H * IMG_W * IMG_C)
#define OUTPUT_N_FEATURES (1)

#define __DEBUG__
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
//#define DMA_DEV_ID XPAR_AXIDMA_0_DEVICE_ID

#define DDR_BASE_ADDR XPAR_PS7_DDR_0_S_AXI_BASEADDR
#define MEM_BASE_ADDR (DDR_BASE_ADDR + 0x1000000)

/* Data offsets and pointers */
#define TX_BUFFER_BASE (MEM_BASE_ADDR + 0x01000000)
#define RX_BUFFER_BASE (MEM_BASE_ADDR + 0x03000000)
#define RX_BUFFER_GLD_BASE (MEM_BASE_ADDR + 0x05000000)

static u8 *tx_buffer_ptr = (u8*)TX_BUFFER_BASE;
static u8 *rx_buffer_ptr = (u8*)RX_BUFFER_BASE;
static u8 *rx_buffer_gld_ptr = (u8*)RX_BUFFER_GLD_BASE;

/* AXI DMA instance */
//XAxiDma axi_dma;
//XAxiDma_Config *axi_dma_cfg;

XStreamingdataflowpartition_0_iodma_0_streamingdataflowpartition_0_iodma_0 hlsmover_in;
XStreamingdataflowpartition_0_iodma_0_streamingdataflowpartition_0_iodma_0_Config *hlsmover_in_cfg;


int init_hls_mover(u16 in_device_id) {
    int status;

    xil_printf("INFO: Initializing HLS Mover\n\r");
    hlsmover_in_cfg = XStreamingdataflowpartition_0_iodma_0_streamingdataflowpartition_0_iodma_0_LookupConfig(in_device_id);
    if (!hlsmover_in_cfg) {
        xil_printf("No config found for %d\r\n", in_device_id);
        return XST_FAILURE;
    }

    status = XStreamingdataflowpartition_0_iodma_0_streamingdataflowpartition_0_iodma_0_CfgInitialize(&hlsmover_in, hlsmover_in_cfg);
    if (status != XST_SUCCESS) {
        xil_printf("Initialization failed %d\r\n", status);
        return XST_FAILURE;
    }

#if 0
    if(XAxiDma_HasSg(&axi_dma)){
        xil_printf("Device configured as SG mode \r\n");
        return XST_FAILURE;
    }


    /* Disable interrupts, we use polling mode */
    XStreamingdataflowpartition_0_iodma_0_streamingdataflowpartition_0_iodma_0_InterruptDisable(&hlsmover_in, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
    XAxiDma_IntrDisable(&axi_dma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);
#endif

    return XST_SUCCESS;
}

#if 0
/* HLS mover initialization routine */
int init_hls_mover(u16 device_id) {
    int status;

    xil_printf("INFO: Initializing HLS Mover\n\r");
    hlsmover_in_cfg = XAxiDma_LookupConfig(device_id);
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
#endif

/* Golden model of the accelerator in software */
int finn_cnv_w1a1_sw(u8 *rx, u8 *tx, unsigned img_n, unsigned input_n_features, unsigned output_n_features) {
    xil_printf("INFO: Golden results are pre-compiled\r\n");
    xil_printf("INFO: It would be nice to run a software model\r\n");
    // See src.h and dst.h for input and golden output respectively.
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
    for (unsigned i = sample_count - MAX_PRINT_ELEMENTS; i < sample_count; i++)
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

    xil_printf("\n\r");
    xil_printf("INFO: ===============================================\n\r");
    xil_printf("INFO: FINN cnv-w1a1 (w/ polling)\n\r");
    xil_printf("INFO: ===============================================\n\r");

    /* Initialize platform (uart and caches) */
    init_platform();

    //init_axi_dma(DMA_DEV_ID);

    /* Calibration */
    XTime_GetTime(&start);
    sleep(1);
    XTime_GetTime(&stop);
    calibration_time = get_elapsed_time(start, stop);
    printf("INFO: Time calibration for one second (%lf sec)\n\r", calibration_time);

    /* Initialize memory */
    xil_printf("INFO: Initialize memory\n\r");
    xil_printf("INFO:   - Image id: %u\n\r", IMG_ID);
    xil_printf("INFO:   - Input-feature count: %u\n\r", INPUT_N_FEATURES);
    xil_printf("INFO:   - Output-feature count: %u\n\r", OUTPUT_N_FEATURES);
    xil_printf("INFO:   - Data size: %u B\n\r", sizeof(u8));
    //    printf("INFO:   - Total input size: %u B, %.2f KB, %.2f MB\n\r", src_FEATURE_COUNT * src_SAMPLE_COUNT * sizeof(unsigned short), (src_FEATURE_COUNT * src_SAMPLE_COUNT * sizeof(unsigned short)) / (float)1024, (src_FEATURE_COUNT * src_SAMPLE_COUNT * sizeof(unsigned short)) / (float)(1024*1024));
    //    printf("INFO:   - Total output size: %u B, %.2f KB, %.2f MB\n\r", dst_FEATURE_COUNT * dst_SAMPLE_COUNT * sizeof(unsigned short), (dst_FEATURE_COUNT * dst_SAMPLE_COUNT * sizeof(unsigned short)) / (float)1024, (dst_FEATURE_COUNT * dst_SAMPLE_COUNT * sizeof(unsigned short)) / (float)(1024*1024));
    //
    //    // Set Heap Size in ldscript.ld to 0x1000000 (16MB)
    //    //malloc_stats();
    //
    for (int i = 0; i < INPUT_N_FEATURES; i++) {
        tx_buffer_ptr[i] = (src_data + IMG_ID * INPUT_N_FEATURES)[i];
    }
    for (int i = 0; i < OUTPUT_N_FEATURES; i++) {
        rx_buffer_gld_ptr[i] = (dst_data + IMG_ID * OUTPUT_N_FEATURES)[i];
        rx_buffer_ptr[i] = 0x0;
    }

    /* ****** SOFTWARE REFERENCE ****** */
#ifdef __DEBUG__
    xil_printf("INFO: Start SW accelerator\n\r");
#endif
    XTime_GetTime(&start);
    finn_cnv_w1a1_sw(tx_buffer_ptr, rx_buffer_gld_ptr, IMG_ID, INPUT_N_FEATURES, OUTPUT_N_FEATURES);
    XTime_GetTime(&stop);
    sw_elapsed = get_elapsed_time(start, stop);

    /* ****** ACCELERATOR ****** */
    xil_printf("INFO: Press any key to start the accelerator: ");
    dummy = inbyte();
    xil_printf("\n\rINFO: \n\r");

#ifdef __DEBUG__
    xil_printf("INFO: Configure and start accelerator\n\r");
#endif

    //for (int idx = 0; idx < IMG_ID; idx++) {
#ifdef __DEBUG__
        xil_printf("INFO: Image #%u\n\r", IMG_ID);
#endif

        XTime_GetTime(&start);
        Xil_DCacheFlushRange((UINTPTR)(tx_buffer_ptr), INPUT_N_FEATURES * sizeof(u8));
        Xil_DCacheFlushRange((UINTPTR)(rx_buffer_ptr), OUTPUT_N_FEATURES * sizeof(u8));
        XTime_GetTime(&stop);
        cache_elapsed = get_elapsed_time(start, stop);

        XTime_GetTime(&start);
#if 0
        status = XAxiDma_SimpleTransfer(&axi_dma, (UINTPTR)(rx_buffer_ptr), OUTPUT_N_FEATURES, XAXIDMA_DEVICE_TO_DMA);
        if (status != XST_SUCCESS)
            return XST_FAILURE;

        status = XAxiDma_SimpleTransfer(&axi_dma,(UINTPTR)(tx_buffer_ptr), INPUT_N_FEATURES, XAXIDMA_DMA_TO_DEVICE);
        if (status != XST_SUCCESS)
            return XST_FAILURE;

        while ((XAxiDma_Busy(&axi_dma, XAXIDMA_DEVICE_TO_DMA)) || (XAxiDma_Busy(&axi_dma,XAXIDMA_DMA_TO_DEVICE))) {
            ; /* Wait */
        }
#endif
        XTime_GetTime(&stop);
        hw_elapsed += get_elapsed_time(start, stop);

        XTime_GetTime(&start);
        //Xil_DCacheFlushRange((UINTPTR)rx_buffer_ptr, OUTPUT_N_FEATURES * sizeof(u8));
        Xil_DCacheInvalidateRange((UINTPTR)(rx_buffer_ptr), OUTPUT_N_FEATURES * sizeof(u8));
        XTime_GetTime(&stop);
        cache_elapsed += get_elapsed_time(start, stop);
    //}

    /* ****** VALIDATION ****** */

#ifdef __DEBUG__
    xil_printf("INFO: ================== Validation =================\n\r");
    xil_printf("INFO: Dump data\n\r");
    //dump_data("src", (u8*)(tx_buffer_ptr + IMG_ID * INPUT_N_FEATURES), 1, INPUT_N_FEATURES, 1, 0);
    dump_data("sw_dst", (u8*)(rx_buffer_gld_ptr), 1, OUTPUT_N_FEATURES, 1, 0);
    dump_data("hw_dst", (u8*)(rx_buffer_ptr), 1, OUTPUT_N_FEATURES, 1, 0);
#endif

    printf("INFO: Software execution time: %f sec\n\r", sw_elapsed);

    printf("INFO: Accelerator execution time: %f sec\n\r", hw_elapsed);
    printf("INFO: Cache flush time: %f sec\n\r", cache_elapsed);
    printf("INFO: Accelerator/software speedup (the sofware is fake so this does not count...): %.2f X\n\r", (sw_elapsed >= (hw_elapsed+cache_elapsed))?(sw_elapsed/(hw_elapsed+cache_elapsed)):-((hw_elapsed+cache_elapsed)/sw_elapsed));

    /* Accelerator validation */
    hw_errors = 0;
    for (int i = 0; i < OUTPUT_N_FEATURES; i++) {
        if ((rx_buffer_ptr)[i] != (rx_buffer_gld_ptr)[i]) {
            xil_printf("ERROR: [%d]: Accelerator hw %02X != sw %02X !!!\n\r", i, (rx_buffer_ptr)[i], (rx_buffer_gld_ptr)[i]);
            hw_errors++;
        }
        xil_printf("INFO: [%d]: Accelerator hw %02X == sw %02X\n\r", i, (rx_buffer_ptr)[i], (rx_buffer_gld_ptr)[i]);
    }

    xil_printf("INFO: Total errors = %d (out of %d elements)\n\r", hw_errors, OUTPUT_N_FEATURES);
    if (hw_errors > 0)
        xil_printf("INFO: Accelerator validation: FAIL\n\r");
    else
        xil_printf("INFO: Accelerator validation: PASS!\n\r");

    xil_printf("INFO: Done!\n\r");
    xil_printf("INFO: ===============================================\n\r");

    cleanup_platform();

    return 0;
}



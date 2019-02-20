`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 03/04/2017 08:24:33 PM
// Design Name: 
// Module Name: RISCV_PROCESSOR
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////

module RISCV_PROCESSOR#(
    parameter  ram_addr_width= 20                                         ,
    localparam ram_depth     = 2**ram_addr_width                            ,
    parameter data_width     = 32                                          ,
    parameter address_width  = 64                                           ,
    parameter block_size     = 4                                           ,
    parameter cache_depth    = 256                                          ,
    parameter l2_delay_read  = 10                                           ,
    localparam line_width    = $clog2(cache_depth)                          ,
    localparam offset_width  = $clog2(data_width*block_size/8 )               ,
    localparam tag_width     = address_width - line_width -  offset_width   ,
    localparam cache_width   = block_size*data_width                        ,
    parameter  C_M00_AXI_TARGET_SLAVE_BASE_ADDR    = 32'h00000000,
    parameter integer C_M00_AXI_BURST_LEN    = block_size,
    parameter integer C_M00_AXI_ID_WIDTH    = 1,
    parameter integer C_M00_AXI_ADDR_WIDTH    = 32,
    parameter integer C_M00_AXI_DATA_WIDTH    = 32,
    parameter integer C_M00_AXI_AWUSER_WIDTH    = 0,
    parameter integer C_M00_AXI_ARUSER_WIDTH    = 0,
    parameter integer C_M00_AXI_WUSER_WIDTH    = 0,
    parameter integer C_M00_AXI_RUSER_WIDTH    = 0,
    parameter integer C_M00_AXI_BUSER_WIDTH    = 0,
    parameter integer C_S00_AXI_ID_WIDTH    = 1,
    parameter integer C_S00_AXI_DATA_WIDTH    = 32,
    parameter integer C_S00_AXI_ADDR_WIDTH    = 32,
    parameter integer C_S00_AXI_AWUSER_WIDTH    = 0,
    parameter integer C_S00_AXI_ARUSER_WIDTH    = 0,
    parameter integer C_S00_AXI_WUSER_WIDTH    = 0,
    parameter integer C_S00_AXI_RUSER_WIDTH    = 0,
    parameter integer C_S00_AXI_BUSER_WIDTH    = 0,
          parameter  C_Peripheral_Interface_START_DATA_VALUE              = 32'h00000000,
        parameter  C_Peripheral_Interface_TARGET_SLAVE_BASE_ADDR        = 32'h00000000,
        parameter integer C_Peripheral_Interface_ADDR_WIDTH             = 32,
        parameter integer C_Peripheral_Interface_DATA_WIDTH             = 32,
        parameter integer C_Peripheral_Interface_TRANSACTIONS_NUM       = 4,
        // Fixed parameters
        localparam ADDR_WIDTH           = 39,
        localparam DATA_WIDTH           = 64,
        localparam EXT_FIFO_ADDRESS     = 32'he0001030,
   //     localparam EXT_FIFO_ADDRESS     = 32'h10150,
        parameter  PERIPHERAL_BASE_ADDR = 32'h10000000,
                              // Delay of the second level of cache
        parameter HISTORY_DEPTH         = 32
    ) (
        // Standard inputs
        input                               CLK,
        input                               RSTN,
        input  P0_INIT_AXI_TXN,
                   
       output reg                                            EXT_FIFO_WR_ENB,
       output reg [DATA_WIDTH     - 1 : 0]                   EXT_FIFO_WR_DATA=32,

        ///////////////////////////
        //    DATA PERIPHERAL    //
        ///////////////////////////
        
        // output      [31:0]          RD_ADDR_TO_PERI,
        // output                      RD_ADDR_TO_PERI_VALID,
        // input                       RD_ADDR_TO_PERI_READY,
        // output      [31:0]          WR_ADDR_TO_PERI,
        // output                      WR_TO_PERI_VALID,
        // input                       WR_TO_PERI_READY,
        // output      [31:0]          DATA_TO_PERI,
        // input       [31:0]          DATA_FROM_PERI,
        // output                      DATA_FROM_PERI_READY,
        // input                       DATA_FROM_PERI_VALID,
        // input                       TRANSACTION_COMPLETE_PERI,
        // output  [3:0]                      WSTRB_OUT,
        input                                           MEIP                            ,   //machine external interupt pending
        input                                           MTIP                            ,   //machine timer interupt pending
        input                                           MSIP                   ,
        input wire  m00_axi_init_axi_txn,
        output wire  m00_axi_txn_done,
        output wire  m00_axi_error,
        input wire  m00_axi_aclk,
        input wire  m00_axi_aresetn,
        output wire [C_M00_AXI_ID_WIDTH-1 : 0] m00_axi_awid,
        output wire [C_M00_AXI_ADDR_WIDTH-1 : 0] m00_axi_awaddr,
        output wire [7 : 0] m00_axi_awlen,
        output wire [2 : 0] m00_axi_awsize,
        output wire [1 : 0] m00_axi_awburst,
        output wire  m00_axi_awlock,
        output wire [3 : 0] m00_axi_awcache,
        output wire [2 : 0] m00_axi_awprot,
        output wire [3 : 0] m00_axi_awqos,
        output wire [C_M00_AXI_AWUSER_WIDTH-1 : 0] m00_axi_awuser,
        output wire  m00_axi_awvalid,
        input wire  m00_axi_awready,
        output wire [C_M00_AXI_DATA_WIDTH-1 : 0] m00_axi_wdata,
        output wire [C_M00_AXI_DATA_WIDTH/8-1 : 0] m00_axi_wstrb,
        output wire  m00_axi_wlast,
        output wire [C_M00_AXI_WUSER_WIDTH-1 : 0] m00_axi_wuser,
        output wire  m00_axi_wvalid,
        input wire  m00_axi_wready,
        input wire [C_M00_AXI_ID_WIDTH-1 : 0] m00_axi_bid,
        input wire [1 : 0] m00_axi_bresp,
        input wire [C_M00_AXI_BUSER_WIDTH-1 : 0] m00_axi_buser,
        input wire  m00_axi_bvalid,
        output wire  m00_axi_bready,
        output wire [C_M00_AXI_ID_WIDTH-1 : 0] m00_axi_arid,
        output wire [C_M00_AXI_ADDR_WIDTH-1 : 0] m00_axi_araddr,
        output wire [7 : 0] m00_axi_arlen,
        output wire [2 : 0] m00_axi_arsize,
        output wire [1 : 0] m00_axi_arburst,
        output wire  m00_axi_arlock,
        output wire [3 : 0] m00_axi_arcache,
        output wire [2 : 0] m00_axi_arprot,
        output wire [3 : 0] m00_axi_arqos,
        output wire [C_M00_AXI_ARUSER_WIDTH-1 : 0] m00_axi_aruser,
        output wire  m00_axi_arvalid,
        input wire  m00_axi_arready,
        input wire [C_M00_AXI_ID_WIDTH-1 : 0] m00_axi_rid,
        input wire [C_M00_AXI_DATA_WIDTH-1 : 0] m00_axi_rdata,
        input wire [1 : 0] m00_axi_rresp,
        input wire  m00_axi_rlast,
        input wire [C_M00_AXI_RUSER_WIDTH-1 : 0] m00_axi_ruser,
        input wire  m00_axi_rvalid,
        output wire  m00_axi_rready,


        input wire  m01_axi_init_axi_txn,
        output wire  m01_axi_txn_done,
        output wire  m01_axi_error,
        input wire  m01_axi_aclk,
        input wire  m01_axi_aresetn,
        output wire [C_M00_AXI_ID_WIDTH-1 : 0] m01_axi_awid,
        output wire [C_M00_AXI_ADDR_WIDTH-1 : 0] m01_axi_awaddr,
        output wire [7 : 0] m01_axi_awlen,
        output wire [2 : 0] m01_axi_awsize,
        output wire [1 : 0] m01_axi_awburst,
        output wire  m01_axi_awlock,
        output wire [3 : 0] m01_axi_awcache,
        output wire [2 : 0] m01_axi_awprot,
        output wire [3 : 0] m01_axi_awqos,
        output wire [C_M00_AXI_AWUSER_WIDTH-1 : 0] m01_axi_awuser,
        output wire  m01_axi_awvalid,
        input wire  m01_axi_awready,
        output wire [C_M00_AXI_DATA_WIDTH-1 : 0] m01_axi_wdata,
        output wire [C_M00_AXI_DATA_WIDTH/8-1 : 0] m01_axi_wstrb,
        output wire  m01_axi_wlast,
        output wire [C_M00_AXI_WUSER_WIDTH-1 : 0] m01_axi_wuser,
        output wire  m01_axi_wvalid,
        input wire  m01_axi_wready,
        input wire [C_M00_AXI_ID_WIDTH-1 : 0] m01_axi_bid,
        input wire [1 : 0] m01_axi_bresp,
        input wire [C_M00_AXI_BUSER_WIDTH-1 : 0] m01_axi_buser,
        input wire  m01_axi_bvalid,
        output wire  m01_axi_bready,
        output wire [C_M00_AXI_ID_WIDTH-1 : 0] m01_axi_arid,
        output wire [C_M00_AXI_ADDR_WIDTH-1 : 0] m01_axi_araddr,
        output wire [7 : 0] m01_axi_arlen,
        output wire [2 : 0] m01_axi_arsize,
        output wire [1 : 0] m01_axi_arburst,
        output wire  m01_axi_arlock,
        output wire [3 : 0] m01_axi_arcache,
        output wire [2 : 0] m01_axi_arprot,
        output wire [3 : 0] m01_axi_arqos,
        output wire [C_M00_AXI_ARUSER_WIDTH-1 : 0] m01_axi_aruser,
        output wire  m01_axi_arvalid,
        input wire  m01_axi_arready,
        input wire [C_M00_AXI_ID_WIDTH-1 : 0] m01_axi_rid,
        input wire [C_M00_AXI_DATA_WIDTH-1 : 0] m01_axi_rdata,
        input wire [1 : 0] m01_axi_rresp,
        input wire  m01_axi_rlast,
        input wire [C_M00_AXI_RUSER_WIDTH-1 : 0] m01_axi_ruser,
        input wire  m01_axi_rvalid,
        output wire  m01_axi_rready,
        input  wire                                             peripheral_interface_init_axi_txn,
        output wire                                             peripheral_interface_error,
        output wire                                             peripheral_interface_txn_done,
        input  wire                                             peripheral_interface_aclk,
        input  wire                                             peripheral_interface_aresetn,
        output wire [C_Peripheral_Interface_ADDR_WIDTH-1 : 0]   peripheral_interface_awaddr,
        output wire [2 : 0]                                     peripheral_interface_awprot,
        output wire                                             peripheral_interface_awvalid,
        input  wire                                             peripheral_interface_awready,
        output wire [C_Peripheral_Interface_DATA_WIDTH-1 : 0]   peripheral_interface_wdata,
        output wire [C_Peripheral_Interface_DATA_WIDTH/8-1 : 0] peripheral_interface_wstrb,
        output wire                                             peripheral_interface_wvalid,
        input  wire                                             peripheral_interface_wready,
        input  wire [1 : 0]                                     peripheral_interface_bresp,
        input  wire                                             peripheral_interface_bvalid,
        output wire                                             peripheral_interface_bready,
        output wire [C_Peripheral_Interface_ADDR_WIDTH-1 : 0]   peripheral_interface_araddr,
        output wire [2 : 0]                                     peripheral_interface_arprot,
        output wire                                             peripheral_interface_arvalid,
        input  wire                                             peripheral_interface_arready,
        input  wire [C_Peripheral_Interface_DATA_WIDTH-1 : 0]   peripheral_interface_rdata,
        input  wire [1 : 0]                                     peripheral_interface_rresp,
        input  wire                                             peripheral_interface_rvalid,
        output wire                                             peripheral_interface_rready 
        
        
    );
    wire [ADDR_WIDTH      - 1 : 0]  RD_ADDR_TO_PERI;
    wire                            RD_ADDR_TO_PERI_VALID;
    wire                            RD_ADDR_TO_PERI_READY;
    wire [ADDR_WIDTH      - 1 : 0]  WR_ADDR_TO_PERI;
    wire                            WR_TO_PERI_VALID;
    wire                            WR_TO_PERI_READY;
    wire [DATA_WIDTH/2      - 1 : 0]  DATA_TO_PERI;
    wire [DATA_WIDTH/2      - 1 : 0]  DATA_FROM_PERI;
    wire                            DATA_FROM_PERI_READY;
    wire                            DATA_FROM_PERI_VALID;
    wire                            TRANSACTION_COMPLETE_PERI;
    wire [3:0]                      wstrb;
     // Peripheral signals
     reg                                p_flag ;
     wire                               tlb_ready;
     reg                                tlb_ready_d1;
     reg                                tlb_ready_d2;
     reg                                tlb_ready_d3;
     reg                                stop_dat_cache ;
     reg                                stop_ins_cache ;
     integer                            counter ;
     reg                                peri_start;
     reg        [31:0]                  addr_to_peri;
     reg                                control_to_peri;
     reg        [31:0]                  data_to_peri;
     wire       [31:0]                  data_from_peri;
     wire                               peri_complete;
     reg                                peri_done ;
     reg        [3 :0]                  wstrb_to_peri;
     wire       [4:0]                   amo_op;
             
        wire fence;
     
     // Status signals between processor and instruction cache
     wire                               proc_ready_ins;
     wire                               cache_ready_ins;
     
     // Input address bus from the processor to instruction cache
     wire                               branch_taken;
     wire   [ADDR_WIDTH - 1 : 0]        branch_address;
           
     // Output data bus from instruction cache to the processor
     wire   [32 - 1 : 0]        data_to_proc_ins;
     wire   [ADDR_WIDTH - 1 : 0]        pc_to_proc_ins;
     
     
     //Status signals between processor and data cache
     wire                               cache_ready_dat;
        
     //Input address bus from the processor to data cache     
     wire   [2          - 1 : 0]        control_from_proc_dat;
     wire   [ADDR_WIDTH - 1 : 0]        addr_from_proc_dat;
     wire   [DATA_WIDTH - 1 : 0]        data_from_proc_dat;
     wire   [8         - 1 : 0]        byte_enb_proc;
               
     //Output data from data cache to processor
     wire   [DATA_WIDTH - 1 : 0]        data_to_proc_dat;
     
     /////////////////////////////////////////////////////////
     //ijaz maathu
     wire predicted;
     wire [31:0] pc;
     wire [31:0] ex_pc;
     wire branch;
     wire prd_valid;
     wire [31:0] prd_addr;
     wire flush_w;
     wire [31:0] return_addr;
     wire  return_w;
     wire [31:0] ins_id_ex;
         reg                      flush                           ;
        reg  [address_width-1:0]    addr                            ;
        wire  [address_width-1:0]    addr_out                            ;
        wire  [address_width-1:0]    curr_addr                            ;

        reg                      addr_valid                      ;
        // wire [data_width-1:0]    data                            ;
        wire                     cache_ready                     ;
        wire                     addr_to_l2_valid                ;
        wire [cache_width-1:0]  datas                           ;
        wire [address_width-offset_width-1:0]     addr_to_l2       ;
        wire  [cache_width-1:0]   data_from_l2                    ;
        wire                      data_from_l2_valid              ;        

        wire [address_width-5-1:0]     addr_to_l2_dat       ;
        wire  [block_size*64-1:0]   data_from_l2_dat                    ;
        wire                      data_from_l2_valid_dat              ;
              wire                      data_to_l2_valid              ;
        wire  [address_width-5-1:0] waddr_to_l2      ;
        wire                      write_done;
        wire  [block_size*64-1:0]   data_to_l2                    ;
        wire addr_to_l2_valid_dat;
        reg DATA_FROM_AXIM_VALID;
        reg [31:0] VIRT_ADDR,DATA_FROM_AXIM;
        wire [31:0] PHY_ADDR,ADDR_TO_AXIM;
        wire PHY_ADDR_VALID,ADDR_TO_AXIM_VALID;
        wire [3:0] WSTRB_OUT;
     //reg [63:0]     mtime;
     //reg [63:0]     mtimecmp;
     //reg            timer_interrupt;
     //reg            reg_done = 0;
     //reg [31:0]     data_from_reg;
     //reg            reg_flag = 0;
     
    ///////////////////////////////////////////////////// 
    
    //Simulation

     
    wire    exstage_stalled ;
     

    wire op32;
     PIPELINE pipeline(
    .CLK(CLK),
    .RST(~RSTN),
    // Towards instruction cache
    .CACHE_READY(cache_ready_ins & !exstage_stalled  & !stop_ins_cache & PHY_ADDR_VALID & tlb_ready_d3),
    .PIPELINE_STALL(proc_ready_ins),
    .BRANCH_TAKEN(branch_taken),
    .BYTE_ENB_TO_CACHE( byte_enb_proc),
    .BRANCH_ADDRESS(branch_address),
    .PC_IF_ID(pc_to_proc_ins), 
    .INS_IF_ID(data_to_proc_ins),
    //Data cache busses
    .CONTROL_DATA_CACHE(control_from_proc_dat), 
    .ADDR_TO_DATA_CACHE(addr_from_proc_dat),
    .DATA_TO_DATA_CACHE(data_from_proc_dat),
    .DATA_TO_PROC((p_flag)? data_from_peri: data_to_proc_dat),
    .CACHE_READY_DATA(cache_ready_dat & !stop_dat_cache),
    .EX_PC(ex_pc),
    .BRANCH(branch),
    .FLUSH(flush_w),
    .RETURN_ADDR(return_addr),
    .RETURN(return_w),
    .PREDICTED(predicted),
    .EXSTAGE_STALLED(exstage_stalled),
    .INS_ID_EX(ins_id_ex),
    .MEIP(MEIP),
    .MSIP(MSIP),
    .MTIP(MTIP),
    .FENCE(fence),
    .AMO_TO_CACHE(amo_op),
    .OP_32_out(op32)
    );
    

    
    
    BHT bht (
    .CLK(CLK)                              ,
    .RST(~RSTN)                            ,
    .PC(pc)                                ,
    .EX_PC(ex_pc)                          ,
    .BRANCH(branch)                        ,
    .BRANCH_TAKEN(branch_taken)            ,
    .BRANCH_ADDR (branch_address)          ,
    .PRD_VALID (prd_valid)                 ,
    .PRD_ADDR  (prd_addr)                  ,
    .FLUSH(flush_w)                          ,
    .CACHE_READY(cache_ready_ins & !exstage_stalled  & !stop_ins_cache & PHY_ADDR_VALID & tlb_ready_d3)          ,
    .CACHE_READY_DATA(cache_ready_dat & !stop_dat_cache),
    .RETURN_ADDR(return_addr),
    .RETURN(return_w),
    .PREDICTED(predicted)
    );
    
    Peripheral peripheral
    (
    .CLK(CLK),
    .RSTN(RSTN),
    .START(peri_start),
    .ADDRESS(addr_to_peri),
    .WRITE(control_to_peri),
    .DATA_IN(data_to_peri),
    .DATA_OUT(data_from_peri),
    .DONE(peri_complete),
    .WSTRB(wstrb_to_peri),
    .RD_ADDR_TO_PERI(RD_ADDR_TO_PERI),
    .RD_ADDR_TO_PERI_VALID(RD_ADDR_TO_PERI_VALID),
    .RD_ADDR_TO_PERI_READY(RD_ADDR_TO_PERI_READY),
    .WR_ADDR_TO_PERI(WR_ADDR_TO_PERI),
    .WR_TO_PERI_VALID(WR_TO_PERI_VALID),
    .WR_TO_PERI_READY(WR_TO_PERI_READY),
    .DATA_TO_PERI(DATA_TO_PERI),
    .DATA_FROM_PERI(DATA_FROM_PERI),
    .DATA_FROM_PERI_READY(DATA_FROM_PERI_READY),
    .DATA_FROM_PERI_VALID(DATA_FROM_PERI_VALID),
    .TRANSACTION_COMPLETE_PERI(TRANSACTION_COMPLETE_PERI),
    .CACHE_READY_DAT(cache_ready_dat),
    .WSTRB_OUT(WSTRB_OUT)
    );
    
    
    // Intercepting and extracting certain data writes
    reg [2          - 1 : 0] control;
    reg [ADDR_WIDTH - 1 : 0] address;

    
    always@(posedge CLK)
    begin
        if (~RSTN)
        begin
            peri_start<=0;
            control_to_peri <=0;
            counter <=0;
            stop_ins_cache<=0;
            stop_dat_cache<=0;
            peri_done <=0;
            p_flag <=0;
            counter <=0;

            
        end
        else if  (addr_from_proc_dat >= PERIPHERAL_BASE_ADDR &&  control_from_proc_dat != 0 && !stop_dat_cache && cache_ready_ins && cache_ready_dat && counter==0 )
        begin
            data_to_peri <= data_from_proc_dat;
            addr_to_peri <= addr_from_proc_dat;
            control_to_peri <= (control_from_proc_dat == 2) ? 1'b1:1'b0;
            wstrb_to_peri <= byte_enb_proc;
            peri_start <=1;
            stop_ins_cache <=1;
            counter <= counter +1;
            peri_done <= 0;
        end
        else if (cache_ready_dat && counter>0)
        begin
            if(peri_complete)
            begin
                peri_done <= 1;
            end
                        
            if (counter == 3 && !peri_done)
            begin
                stop_dat_cache <=1;
                p_flag         <=0;
                stop_ins_cache <=0;     
            end
            else if (counter == 3 && peri_done)
            begin
                stop_ins_cache <=0;  
                p_flag         <=1;
                counter        <=0;
                stop_dat_cache <=0;
            end
            else if (counter!=0)
            begin
                counter       <=counter + 1;
                peri_start    <=0;
            end    
        end
        
        if (p_flag && peri_done & peri_start)
        begin
            peri_start <=0;
        end
        if (p_flag & cache_ready_dat & !stop_dat_cache)
        begin
            p_flag <= 0;
        end
    end
    

    function integer logb2;
        input integer depth;
        for (logb2 = 0; depth > 1; logb2 = logb2 + 1)
            depth = depth >> 1;
    endfunction
    

myip_v1_0_M00_AXI # ( 
        .C_M_TARGET_SLAVE_BASE_ADDR(C_M00_AXI_TARGET_SLAVE_BASE_ADDR),
        .C_M_AXI_BURST_LEN(C_M00_AXI_BURST_LEN),
        .C_M_AXI_ID_WIDTH(C_M00_AXI_ID_WIDTH),
        .C_M_AXI_ADDR_WIDTH(C_M00_AXI_ADDR_WIDTH),
        .C_M_AXI_DATA_WIDTH(C_M00_AXI_DATA_WIDTH),
        .C_M_AXI_AWUSER_WIDTH(C_M00_AXI_AWUSER_WIDTH),
        .C_M_AXI_ARUSER_WIDTH(C_M00_AXI_ARUSER_WIDTH),
        .C_M_AXI_WUSER_WIDTH(C_M00_AXI_WUSER_WIDTH),
        .C_M_AXI_RUSER_WIDTH(C_M00_AXI_RUSER_WIDTH),
        .C_M_AXI_BUSER_WIDTH(C_M00_AXI_BUSER_WIDTH),
        .cache_width(cache_width)
    ) myip_v1_0_M00_AXI_inst (
        .RSTN(RSTN),
        .INIT_AXI_TXN(m00_axi_init_axi_txn),
        .TXN_DONE(m00_axi_txn_done),
        .ERROR(m00_axi_error),
        .M_AXI_ACLK(m00_axi_aclk),
        .M_AXI_ARESETN(m00_axi_aresetn),
        .M_AXI_AWID(m00_axi_awid),
        .M_AXI_AWADDR(m00_axi_awaddr),
        .M_AXI_AWLEN(m00_axi_awlen),
        .M_AXI_AWSIZE(m00_axi_awsize),
        .M_AXI_AWBURST(m00_axi_awburst),
        .M_AXI_AWLOCK(m00_axi_awlock),
        .M_AXI_AWCACHE(m00_axi_awcache),
        .M_AXI_AWPROT(m00_axi_awprot),
        .M_AXI_AWQOS(m00_axi_awqos),
        .M_AXI_AWUSER(m00_axi_awuser),
        .M_AXI_AWVALID(m00_axi_awvalid),
        .M_AXI_AWREADY(m00_axi_awready),
        .M_AXI_WDATA(m00_axi_wdata),
        .M_AXI_WSTRB(m00_axi_wstrb),
        .M_AXI_WLAST(m00_axi_wlast),
        .M_AXI_WUSER(m00_axi_wuser),
        .M_AXI_WVALID(m00_axi_wvalid),
        .M_AXI_WREADY(m00_axi_wready),
        .M_AXI_BID(m00_axi_bid),
        .M_AXI_BRESP(m00_axi_bresp),
        .M_AXI_BUSER(m00_axi_buser),
        .M_AXI_BVALID(m00_axi_bvalid),
        .M_AXI_BREADY(m00_axi_bready),
        .M_AXI_ARID(m00_axi_arid),
        .M_AXI_ARADDR(m00_axi_araddr),
        .M_AXI_ARLEN(m00_axi_arlen),
        .M_AXI_ARSIZE(m00_axi_arsize),
        .M_AXI_ARBURST(m00_axi_arburst),
        .M_AXI_ARLOCK(m00_axi_arlock),
        .M_AXI_ARCACHE(m00_axi_arcache),
        .M_AXI_ARPROT(m00_axi_arprot),
        .M_AXI_ARQOS(m00_axi_arqos),
        .M_AXI_ARUSER(m00_axi_aruser),
        .M_AXI_ARVALID(m00_axi_arvalid),
        .M_AXI_ARREADY(m00_axi_arready),
        .M_AXI_RID(m00_axi_rid),
        .M_AXI_RDATA(m00_axi_rdata),
        .M_AXI_RRESP(m00_axi_rresp),
        .M_AXI_RLAST(m00_axi_rlast),
        .M_AXI_RUSER(m00_axi_ruser),
        .M_AXI_RVALID(m00_axi_rvalid),
        .M_AXI_RREADY(m00_axi_rready),
        // user defined ports
        .data_from_l2(data_from_l2)         ,
        .addr_to_l2_valid(addr_to_l2_valid) ,
        .addr_to_l2({addr_to_l2,{offset_width{1'b0}}})              ,
        .data_from_l2_valid(data_from_l2_valid)
 
    );
myip_v1_0_M00_AXI # ( 
        .C_M_TARGET_SLAVE_BASE_ADDR(C_M00_AXI_TARGET_SLAVE_BASE_ADDR),
        .C_M_AXI_BURST_LEN(8),
        .C_M_AXI_ID_WIDTH(C_M00_AXI_ID_WIDTH),
        .C_M_AXI_ADDR_WIDTH(C_M00_AXI_ADDR_WIDTH),
        .C_M_AXI_DATA_WIDTH(C_M00_AXI_DATA_WIDTH),
        .C_M_AXI_AWUSER_WIDTH(C_M00_AXI_AWUSER_WIDTH),
        .C_M_AXI_ARUSER_WIDTH(C_M00_AXI_ARUSER_WIDTH),
        .C_M_AXI_WUSER_WIDTH(C_M00_AXI_WUSER_WIDTH),
        .C_M_AXI_RUSER_WIDTH(C_M00_AXI_RUSER_WIDTH),
        .C_M_AXI_BUSER_WIDTH(C_M00_AXI_BUSER_WIDTH),
        .cache_width(block_size*64)
    ) myip_v1_0_M01_AXI_inst (
        .RSTN(RSTN),
        .INIT_AXI_TXN(m01_axi_init_axi_txn),
        .TXN_DONE(m01_axi_txn_done),
        .ERROR(m01_axi_error),
        .M_AXI_ACLK(m01_axi_aclk),
        .M_AXI_ARESETN(m01_axi_aresetn),
        .M_AXI_AWID(m01_axi_awid),
        .M_AXI_AWADDR(m01_axi_awaddr),
        .M_AXI_AWLEN(m01_axi_awlen),
        .M_AXI_AWSIZE(m01_axi_awsize),
        .M_AXI_AWBURST(m01_axi_awburst),
        .M_AXI_AWLOCK(m01_axi_awlock),
        .M_AXI_AWCACHE(m01_axi_awcache),
        .M_AXI_AWPROT(m01_axi_awprot),
        .M_AXI_AWQOS(m01_axi_awqos),
        .M_AXI_AWUSER(m01_axi_awuser),
        .M_AXI_AWVALID(m01_axi_awvalid),
        .M_AXI_AWREADY(m01_axi_awready),
        .M_AXI_WDATA(m01_axi_wdata),
        .M_AXI_WSTRB(m01_axi_wstrb),
        .M_AXI_WLAST(m01_axi_wlast),
        .M_AXI_WUSER(m01_axi_wuser),
        .M_AXI_WVALID(m01_axi_wvalid),
        .M_AXI_WREADY(m01_axi_wready),
        .M_AXI_BID(m01_axi_bid),
        .M_AXI_BRESP(m01_axi_bresp),
        .M_AXI_BUSER(m01_axi_buser),
        .M_AXI_BVALID(m01_axi_bvalid),
        .M_AXI_BREADY(m01_axi_bready),
        .M_AXI_ARID(m01_axi_arid),
        .M_AXI_ARADDR(m01_axi_araddr),
        .M_AXI_ARLEN(m01_axi_arlen),
        .M_AXI_ARSIZE(m01_axi_arsize),
        .M_AXI_ARBURST(m01_axi_arburst),
        .M_AXI_ARLOCK(m01_axi_arlock),
        .M_AXI_ARCACHE(m01_axi_arcache),
        .M_AXI_ARPROT(m01_axi_arprot),
        .M_AXI_ARQOS(m01_axi_arqos),
        .M_AXI_ARUSER(m01_axi_aruser),
        .M_AXI_ARVALID(m01_axi_arvalid),
        .M_AXI_ARREADY(m01_axi_arready),
        .M_AXI_RID(m01_axi_rid),
        .M_AXI_RDATA(m01_axi_rdata),
        .M_AXI_RRESP(m01_axi_rresp),
        .M_AXI_RLAST(m01_axi_rlast),
        .M_AXI_RUSER(m01_axi_ruser),
        .M_AXI_RVALID(m01_axi_rvalid),
        .M_AXI_RREADY(m01_axi_rready),
        // user defined ports
        .data_from_l2(data_from_l2_dat)         ,
        .addr_to_l2_valid(addr_to_l2_valid_dat) ,
        .addr_to_l2({addr_to_l2_dat,{5{1'b0}}})              ,

        .data_from_l2_valid(data_from_l2_valid_dat),
        
        .waddr_to_l2({waddr_to_l2,{5{1'b0}}}),
        .data_to_l2(data_to_l2),
        .data_to_l2_valid(data_to_l2_valid),
        .data_written(write_done)
    );


        Icache
    #(
        .data_width     (data_width)                                         ,
        .address_width  (address_width)                                     ,
        .block_size     (block_size)                                        ,
        .cache_depth    ( cache_depth)                                      
        
        )
    cache
    (
        .CLK(CLK)                                   ,
        .RST(~RSTN)                                   ,
        .FLUSH(fence)                               ,
        .ADDR(PHY_ADDR)                                 ,
        .ADDR_vir(pc)                             ,
        .ADDR_VALID(PHY_ADDR_VALID& proc_ready_ins & !exstage_stalled  & !stop_ins_cache)                     ,
        .DATA (data_to_proc_ins)                                ,
        .CACHE_READY(cache_ready_ins )                   ,
        .ADDR_TO_L2_VALID(addr_to_l2_valid)         ,
        .ADDR_TO_L2 (addr_to_l2)                    ,
        .DATA_FROM_L2 (data_from_l2)                ,
        .DATA_FROM_L2_VALID (data_from_l2_valid)     ,
//        .CURR_ADDR(pc)        ,
        .ADDR_OUT(pc_to_proc_ins)
        

    ); 
   
   Dcache
    #(
        .data_width     (64)                                        ,
        .address_width  (address_width)                                     ,
        .block_size     (block_size)                                        ,
        .cache_depth    ( cache_depth)                                      
        
        )
   dcache (
           .CLK(CLK)                                   ,
        .RST(~RSTN)                                   ,
        .FLUSH(fence)                               ,
        .ADDR(addr_from_proc_dat)                                 ,
        .ADDR_VALID(1)                     ,
        .DATA (data_to_proc_dat)                                ,
        .CACHE_READY(cache_ready_dat)                   ,
        .ADDR_TO_L2_VALID(addr_to_l2_valid_dat)         ,
        .ADDR_TO_L2 (addr_to_l2_dat)                    ,
        .DATA_FROM_L2 (data_from_l2_dat)                ,
        .DATA_FROM_L2_VALID (data_from_l2_valid_dat)    ,
         .CONTROL ((addr_from_proc_dat != EXT_FIFO_ADDRESS & addr_from_proc_dat < PERIPHERAL_BASE_ADDR & cache_ready_ins & PHY_ADDR_VALID & tlb_ready_d3)? control_from_proc_dat : 2'b0)                         ,   
        .WSTRB (byte_enb_proc)                           ,
        .DATA_in(data_from_proc_dat),
        .DATA_TO_L2_VALID(data_to_l2_valid)                ,
        .WADDR_TO_L2(waddr_to_l2)    ,
        .WRITE_DONE(write_done),
        .DATA_TO_L2(data_to_l2),
        .AMO(amo_op),
        .OP32(op32)

    );
    Itlb
    #(.virt_addr_init(32'h0000_0000) )                                                      
    dut (
        .CLK(CLK),
        .RST(~RSTN),
    .TLB_FLUSH(0),
    .VIRT_ADDR(prd_addr),
    .VIRT_ADDR_VALID(proc_ready_ins & !exstage_stalled  & !stop_ins_cache),
    .CURR_ADDR(pc),
    .PHY_ADDR_VALID(PHY_ADDR_VALID),
    .PHY_ADDR(PHY_ADDR),
    .ADDR_TO_AXIM_VALID(ADDR_TO_AXIM_VALID),
    .ADDR_TO_AXIM(ADDR_TO_AXIM),
    .DATA_FROM_AXIM_VALID(DATA_FROM_AXIM_VALID),
    .DATA_FROM_AXIM(DATA_FROM_AXIM),
    .CACHE_READY(cache_ready_ins)
            
            
    );
      PERIPHERAL_INTERFACE # ( 

       ) peripheral_interface (
       .INIT_AXI_TXN(RSTN),
       .ERROR                           (peripheral_interface_error),
       .TXN_DONE                        (peripheral_interface_txn_done),
       .M_AXI_ACLK                      (peripheral_interface_aclk),
       .M_AXI_ARESETN                   (RSTN),
       .M_AXI_AWADDR                    (peripheral_interface_awaddr),
       .M_AXI_AWPROT                    (peripheral_interface_awprot),
       .M_AXI_AWVALID                   (peripheral_interface_awvalid),
       .M_AXI_AWREADY                   (peripheral_interface_awready),
       .M_AXI_WDATA                     (peripheral_interface_wdata),
       .M_AXI_WSTRB                     (peripheral_interface_wstrb),
       .M_AXI_WVALID                    (peripheral_interface_wvalid),
       .M_AXI_WREADY                    (peripheral_interface_wready),
       .M_AXI_BRESP                     (peripheral_interface_bresp),
       .M_AXI_BVALID                    (peripheral_interface_bvalid),
       .M_AXI_BREADY                    (peripheral_interface_bready),
       .M_AXI_ARADDR                    (peripheral_interface_araddr),
       .M_AXI_ARPROT                    (peripheral_interface_arprot),
       .M_AXI_ARVALID                   (peripheral_interface_arvalid),
       .M_AXI_ARREADY                   (peripheral_interface_arready),
       .M_AXI_RDATA                     (peripheral_interface_rdata),
       .M_AXI_RRESP                     (peripheral_interface_rresp),
       .M_AXI_RVALID                    (peripheral_interface_rvalid),
       .M_AXI_RREADY                    (peripheral_interface_rready),
       .wstrb(          WSTRB_OUT                                       ),
       //Peripheral Side Ports
       .dout_ra                         (RD_ADDR_TO_PERI),
       .valid_ra                        (RD_ADDR_TO_PERI_VALID),
       .ready_ra                        (RD_ADDR_TO_PERI_READY),
       .dout_wa                         (WR_ADDR_TO_PERI),
       .valid_wa                        (WR_TO_PERI_VALID),
       .ready_wa                        (WR_TO_PERI_READY),
       .dout_wd                         (DATA_TO_PERI),
       .dout                            (DATA_FROM_PERI),
       .ready_rd                        (DATA_FROM_PERI_READY),
       .valid_rd                        (DATA_FROM_PERI_VALID),
       .ack                             (TRANSACTION_COMPLETE_PERI)
       );

    always@(posedge CLK)
    begin
        if(~RSTN)
        begin
            tlb_ready_d1    <=0;
            tlb_ready_d2    <=0;
            tlb_ready_d3    <=0;
        end
        else if(cache_ready_ins & !exstage_stalled  & !stop_ins_cache & PHY_ADDR_VALID )
        begin
            tlb_ready_d1  <=1;
            tlb_ready_d2  <= tlb_ready_d1;
            tlb_ready_d3  <= tlb_ready_d2;
        end
    end
    always @(posedge CLK) begin
        if (~RSTN)
        begin
            DATA_FROM_AXIM_VALID <=0;
            DATA_FROM_AXIM<=0;
        end
        else if(ADDR_TO_AXIM_VALID)begin
            DATA_FROM_AXIM_VALID <= 1;
            DATA_FROM_AXIM       <= (ADDR_TO_AXIM) >> 2;
        end
        else DATA_FROM_AXIM_VALID <= 0;
    end
endmodule
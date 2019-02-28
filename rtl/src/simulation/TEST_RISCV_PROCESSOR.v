`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: Yasas Senevitatne
// 
// Create Date: 03/05/2017 10:37:35 AM
// Design Name: 
// Module Name: Test_RISCV_PROCESSOR
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

module Test_RISCV_PROCESSOR
#(
   parameter  ram_addr_width= 20                                         ,
 localparam ram_depth     = 2**ram_addr_width                            ,
 parameter data_width     = 32                                          ,
 parameter address_width  = 32                                           ,
 parameter block_size     = 8                                           ,
 parameter cache_depth    = 512                                          ,
 parameter l2_delay_read  = 10                                           ,
 localparam line_width    = $clog2(cache_depth)                          ,
 localparam offset_width  = $clog2(data_width*block_size/8 )               ,
 localparam tag_width     = address_width - line_width -  offset_width   ,
 localparam cache_width   = block_size*data_width                        ,
 parameter  C_M00_AXI_TARGET_SLAVE_BASE_ADDR    = 32'h00000000,
           parameter  C_Peripheral_Interface_START_DATA_VALUE              = 32'h00000000,
parameter  C_Peripheral_Interface_TARGET_SLAVE_BASE_ADDR        = 32'h00000000,
parameter integer C_Peripheral_Interface_ADDR_WIDTH             = 32,
parameter integer C_Peripheral_Interface_DATA_WIDTH             = 32,
parameter integer C_Peripheral_Interface_TRANSACTIONS_NUM       = 4,
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
 parameter integer C_S00_AXI_BUSER_WIDTH    = 0
) ();
    // Fixed parameters
    localparam ADDR_WIDTH        = 32;
    localparam DATA_WIDTH        = 32;
    
    // Primary parameters for processor instantiation

                
    // Constants
    reg TRUE  = 1;
    reg FALSE = 0;    
    
    // Standard inputs    
    reg                           CLK;
    reg                           RSTN;             
             

 
    wire  m00_axi_init_axi_txn;                                
    wire  m00_axi_txn_done;                                    
    wire  m00_axi_error;                                     
    wire  m00_axi_aclk=CLK;                                         
    wire  m00_axi_aresetn=RSTN;                                      
    wire [C_M00_AXI_ID_WIDTH-1 : 0] m00_axi_awid;             
    wire [C_M00_AXI_ADDR_WIDTH-1 : 0] m00_axi_awaddr;          
    wire [7 : 0] m00_axi_awlen;                                
    wire [2 : 0] m00_axi_awsize;                               
    wire [1 : 0] m00_axi_awburst;                              
    wire  m00_axi_awlock;                                    
    wire [3 : 0] m00_axi_awcache;                             
    wire [2 : 0] m00_axi_awprot;                               
    wire [3 : 0] m00_axi_awqos;                               
    wire [C_M00_AXI_AWUSER_WIDTH-1 : 0] m00_axi_awuser;        
    wire  m00_axi_awvalid;                                     
    wire  m00_axi_awready;                                      
    wire [C_M00_AXI_DATA_WIDTH-1 : 0] m00_axi_wdata;           
    wire [C_M00_AXI_DATA_WIDTH/8-1 : 0] m00_axi_wstrb;         
    wire  m00_axi_wlast;                                       
    wire [C_M00_AXI_WUSER_WIDTH-1 : 0] m00_axi_wuser;          
    wire  m00_axi_wvalid;                                      
    wire  m00_axi_wready;                                       
    wire [C_M00_AXI_ID_WIDTH-1 : 0] m00_axi_bid;                
    wire [1 : 0] m00_axi_bresp;                                 
    wire [C_M00_AXI_BUSER_WIDTH-1 : 0] m00_axi_buser;          
    wire  m00_axi_bvalid;                                       
    wire  m00_axi_bready;                                      
    wire [C_M00_AXI_ID_WIDTH-1 : 0] m00_axi_arid;             
    wire [C_M00_AXI_ADDR_WIDTH-1 : 0] m00_axi_araddr;          
    wire [7 : 0] m00_axi_arlen;                                
    wire [2 : 0] m00_axi_arsize;                               
    wire [1 : 0] m00_axi_arburst;                              
    wire  m00_axi_arlock;                                      
    wire [3 : 0] m00_axi_arcache;                              
    wire [2 : 0] m00_axi_arprot;                               
    wire [3 : 0] m00_axi_arqos;                                
    wire [C_M00_AXI_ARUSER_WIDTH-1 : 0] m00_axi_aruser;        
    wire  m00_axi_arvalid;                                     
    wire  m00_axi_arready;                                      
    wire [C_M00_AXI_ID_WIDTH-1 : 0] m00_axi_rid;                
    wire [C_M00_AXI_DATA_WIDTH-1 : 0] m00_axi_rdata;            
    wire [1 : 0] m00_axi_rresp;                                 
    wire  m00_axi_rlast;                                        
    wire [C_M00_AXI_RUSER_WIDTH-1 : 0] m00_axi_ruser;           
    wire  m00_axi_rvalid;                                       
    wire  m00_axi_rready;   
    ////////////////////////////////////////////////////////////////////////////////                                   
    wire  m01_axi_init_axi_txn;                                
    wire  m01_axi_txn_done;                                    
    wire  m01_axi_error;                                     
    wire  m01_axi_aclk=CLK;                                         
    wire  m01_axi_aresetn=RSTN;                                      
    wire [C_M00_AXI_ID_WIDTH-1 : 0] m01_axi_awid;             
    wire [C_M00_AXI_ADDR_WIDTH-1 : 0] m01_axi_awaddr;          
    wire [7 : 0] m01_axi_awlen;                                
    wire [2 : 0] m01_axi_awsize;                               
    wire [1 : 0] m01_axi_awburst;                              
    wire  m01_axi_awlock;                                    
    wire [3 : 0] m01_axi_awcache;                             
    wire [2 : 0] m01_axi_awprot;                               
    wire [3 : 0] m01_axi_awqos;                               
    wire [C_M00_AXI_AWUSER_WIDTH-1 : 0] m01_axi_awuser;        
    wire  m01_axi_awvalid;                                     
    wire  m01_axi_awready;                                      
    wire [C_M00_AXI_DATA_WIDTH-1 : 0] m01_axi_wdata;           
    wire [C_M00_AXI_DATA_WIDTH/8-1 : 0] m01_axi_wstrb;         
    wire  m01_axi_wlast;                                       
    wire [C_M00_AXI_WUSER_WIDTH-1 : 0] m01_axi_wuser;          
    wire  m01_axi_wvalid;                                      
    wire  m01_axi_wready;                                       
    wire [C_M00_AXI_ID_WIDTH-1 : 0] m01_axi_bid;                
    wire [1 : 0] m01_axi_bresp;                                 
    wire [C_M00_AXI_BUSER_WIDTH-1 : 0] m01_axi_buser;          
    wire  m01_axi_bvalid;                                       
    wire  m01_axi_bready;                                      
    wire [C_M00_AXI_ID_WIDTH-1 : 0] m01_axi_arid;             
    wire [C_M00_AXI_ADDR_WIDTH-1 : 0] m01_axi_araddr;          
    wire [7 : 0] m01_axi_arlen;                                
    wire [2 : 0] m01_axi_arsize;                               
    wire [1 : 0] m01_axi_arburst;                              
    wire  m01_axi_arlock;                                      
    wire [3 : 0] m01_axi_arcache;                              
    wire [2 : 0] m01_axi_arprot;                               
    wire [3 : 0] m01_axi_arqos;                                
    wire [C_M00_AXI_ARUSER_WIDTH-1 : 0] m01_axi_aruser;        
    wire  m01_axi_arvalid;                                     
    wire  m01_axi_arready;                                      
    wire [C_M00_AXI_ID_WIDTH-1 : 0] m01_axi_rid;                
    wire [C_M00_AXI_DATA_WIDTH-1 : 0] m01_axi_rdata;            
    wire [1 : 0] m01_axi_rresp;                                 
    wire  m01_axi_rlast;                                        
    wire [C_M00_AXI_RUSER_WIDTH-1 : 0] m01_axi_ruser;           
    wire  m01_axi_rvalid;                                       
    wire  m01_axi_rready; 
    //////////////////////////////////////////////////////////////////////////
     wire                                             peripheral_interface_init_axi_txn;       
    wire                                             peripheral_interface_error;             
    wire                                             peripheral_interface_txn_done;           
    wire                                             peripheral_interface_aclk;               
    wire                                             peripheral_interface_aresetn;         
    wire [C_Peripheral_Interface_ADDR_WIDTH-1 : 0]   peripheral_interface_awaddr;             
    wire [2 : 0]                                     peripheral_interface_awprot;             
    wire                                             peripheral_interface_awvalid;            
    wire                                             peripheral_interface_awready;            
    wire [C_Peripheral_Interface_DATA_WIDTH-1 : 0]   peripheral_interface_wdata;              
    wire [C_Peripheral_Interface_DATA_WIDTH/8-1 : 0] peripheral_interface_wstrb;              
    wire                                             peripheral_interface_wvalid;             
    wire                                             peripheral_interface_wready;             
    wire [1 : 0]                                     peripheral_interface_bresp;              
    wire                                             peripheral_interface_bvalid;             
    wire                                             peripheral_interface_bready;             
    wire [C_Peripheral_Interface_ADDR_WIDTH-1 : 0]   peripheral_interface_araddr;             
    wire [2 : 0]                                     peripheral_interface_arprot;             
    wire                                             peripheral_interface_arvalid;            
    wire                                             peripheral_interface_arready;            
    wire [C_Peripheral_Interface_DATA_WIDTH-1 : 0]   peripheral_interface_rdata;            
    wire [1 : 0]                                     peripheral_interface_rresp;              
    wire                                             peripheral_interface_rvalid;             
    wire                                             peripheral_interface_rready; 
    //////////////////////////////////////////////////////////////////////////////////             
     wire  itlb_axi_init_axi_txn;                                
    wire  itlb_axi_txn_done;                                    
    wire  itlb_axi_error;                                     
    wire  itlb_axi_aclk=CLK;                                         
    wire  itlb_axi_aresetn=RSTN;                                      
    wire [C_M00_AXI_ID_WIDTH-1 : 0] itlb_axi_awid;             
    wire [C_M00_AXI_ADDR_WIDTH-1 : 0] itlb_axi_awaddr;          
    wire [7 : 0] itlb_axi_awlen;                                
    wire [2 : 0] itlb_axi_awsize;                               
    wire [1 : 0] itlb_axi_awburst;                              
    wire  itlb_axi_awlock;                                    
    wire [3 : 0] itlb_axi_awcache;                             
    wire [2 : 0] itlb_axi_awprot;                               
    wire [3 : 0] itlb_axi_awqos;                               
    wire [C_M00_AXI_AWUSER_WIDTH-1 : 0] itlb_axi_awuser;        
    wire  itlb_axi_awvalid;                                     
    wire  itlb_axi_awready;                                      
    wire [C_M00_AXI_DATA_WIDTH-1 : 0] itlb_axi_wdata;           
    wire [C_M00_AXI_DATA_WIDTH/8-1 : 0] itlb_axi_wstrb;         
    wire  itlb_axi_wlast;                                       
    wire [C_M00_AXI_WUSER_WIDTH-1 : 0] itlb_axi_wuser;          
    wire  itlb_axi_wvalid;                                      
    wire  itlb_axi_wready;                                       
    wire [C_M00_AXI_ID_WIDTH-1 : 0] itlb_axi_bid;                
    wire [1 : 0] itlb_axi_bresp;                                 
    wire [C_M00_AXI_BUSER_WIDTH-1 : 0] itlb_axi_buser;          
    wire  itlb_axi_bvalid;                                       
    wire  itlb_axi_bready;                                      
    wire [C_M00_AXI_ID_WIDTH-1 : 0] itlb_axi_arid;             
    wire [C_M00_AXI_ADDR_WIDTH-1 : 0] itlb_axi_araddr;          
    wire [7 : 0] itlb_axi_arlen;                                
    wire [2 : 0] itlb_axi_arsize;                               
    wire [1 : 0] itlb_axi_arburst;                              
    wire  itlb_axi_arlock;                                      
    wire [3 : 0] itlb_axi_arcache;                              
    wire [2 : 0] itlb_axi_arprot;                               
    wire [3 : 0] itlb_axi_arqos;                                
    wire [C_M00_AXI_ARUSER_WIDTH-1 : 0] itlb_axi_aruser;        
    wire  itlb_axi_arvalid;                                     
    wire  itlb_axi_arready;                                      
    wire [C_M00_AXI_ID_WIDTH-1 : 0] itlb_axi_rid;                
    wire [C_M00_AXI_DATA_WIDTH-1 : 0] itlb_axi_rdata;            
    wire [1 : 0] itlb_axi_rresp;                                 
    wire  itlb_axi_rlast;                                        
    wire [C_M00_AXI_RUSER_WIDTH-1 : 0] itlb_axi_ruser;           
    wire  itlb_axi_rvalid;                                       
    wire  itlb_axi_rready;     

////////////////////////////////////////////////////////////////////////////////
    wire  dtlb_axi_init_axi_txn;                                
    wire  dtlb_axi_txn_done;                                    
    wire  dtlb_axi_error;                                     
    wire  dtlb_axi_aclk=CLK;                                         
    wire  dtlb_axi_aresetn=RSTN;                                      
    wire [C_M00_AXI_ID_WIDTH-1 : 0] dtlb_axi_awid;             
    wire [C_M00_AXI_ADDR_WIDTH-1 : 0] dtlb_axi_awaddr;          
    wire [7 : 0] dtlb_axi_awlen;                                
    wire [2 : 0] dtlb_axi_awsize;                               
    wire [1 : 0] dtlb_axi_awburst;                              
    wire  dtlb_axi_awlock;                                    
    wire [3 : 0] dtlb_axi_awcache;                             
    wire [2 : 0] dtlb_axi_awprot;                               
    wire [3 : 0] dtlb_axi_awqos;                               
    wire [C_M00_AXI_AWUSER_WIDTH-1 : 0] dtlb_axi_awuser;        
    wire  dtlb_axi_awvalid;                                     
    wire  dtlb_axi_awready;                                      
    wire [C_M00_AXI_DATA_WIDTH-1 : 0] dtlb_axi_wdata;           
    wire [C_M00_AXI_DATA_WIDTH/8-1 : 0] dtlb_axi_wstrb;         
    wire  dtlb_axi_wlast;                                       
    wire [C_M00_AXI_WUSER_WIDTH-1 : 0] dtlb_axi_wuser;          
    wire  dtlb_axi_wvalid;                                      
    wire  dtlb_axi_wready;                                       
    wire [C_M00_AXI_ID_WIDTH-1 : 0] dtlb_axi_bid;                
    wire [1 : 0] dtlb_axi_bresp;                                 
    wire [C_M00_AXI_BUSER_WIDTH-1 : 0] dtlb_axi_buser;          
    wire  dtlb_axi_bvalid;                                       
    wire  dtlb_axi_bready;                                      
    wire [C_M00_AXI_ID_WIDTH-1 : 0] dtlb_axi_arid;             
    wire [C_M00_AXI_ADDR_WIDTH-1 : 0] dtlb_axi_araddr;          
    wire [7 : 0] dtlb_axi_arlen;                                
    wire [2 : 0] dtlb_axi_arsize;                               
    wire [1 : 0] dtlb_axi_arburst;                              
    wire  dtlb_axi_arlock;                                      
    wire [3 : 0] dtlb_axi_arcache;                              
    wire [2 : 0] dtlb_axi_arprot;                               
    wire [3 : 0] dtlb_axi_arqos;                                
    wire [C_M00_AXI_ARUSER_WIDTH-1 : 0] dtlb_axi_aruser;        
    wire  dtlb_axi_arvalid;                                     
    wire  dtlb_axi_arready;                                      
    wire [C_M00_AXI_ID_WIDTH-1 : 0] dtlb_axi_rid;                
    wire [C_M00_AXI_DATA_WIDTH-1 : 0] dtlb_axi_rdata;            
    wire [1 : 0] dtlb_axi_rresp;                                 
    wire  dtlb_axi_rlast;                                        
    wire [C_M00_AXI_RUSER_WIDTH-1 : 0] dtlb_axi_ruser;           
    wire  dtlb_axi_rvalid;                                       
    wire  dtlb_axi_rready; 


   RISCV_PROCESSOR # (
   ) uut (
       // Standard inputs
       .CLK(CLK),
       .RSTN(RSTN),
       .peripheral_interface_aclk(CLK),
       // Output address bus from Instruction Cache to Memory               
       //axi interface
       .m00_axi_aclk(m00_axi_aclk),
       .m00_axi_aresetn(m00_axi_aresetn),
       .m00_axi_awid(m00_axi_awid),
       .m00_axi_awaddr(m00_axi_awaddr),
        .m00_axi_awlen(m00_axi_awlen),
        .m00_axi_awsize(m00_axi_awsize),
        .m00_axi_awburst(m00_axi_awburst),
        .m00_axi_awlock(m00_axi_awlock),
        .m00_axi_awcache(m00_axi_awcache),
        .m00_axi_awprot(m00_axi_awprot),
        .m00_axi_awqos(m00_axi_awqos),
        .m00_axi_awuser(m00_axi_awuser),
        .m00_axi_awvalid(m00_axi_awvalid),
        .m00_axi_awready(m00_axi_awready),
        .m00_axi_wdata(m00_axi_wdata),
        .m00_axi_wstrb(m00_axi_wstrb),
        .m00_axi_wlast(m00_axi_wlast),
        .m00_axi_wuser(m00_axi_wuser),
        .m00_axi_wvalid(m00_axi_wvalid),
        .m00_axi_wready(m00_axi_wready),
        .m00_axi_bid(m00_axi_bid),
        .m00_axi_bresp(m00_axi_bresp),
        .m00_axi_buser(m00_axi_buser),
        .m00_axi_bvalid(m00_axi_bvalid),
        .m00_axi_bready(m00_axi_bready),
        .m00_axi_arid(m00_axi_arid),
        .m00_axi_araddr(m00_axi_araddr),
        .m00_axi_arlen(m00_axi_arlen),
        .m00_axi_arsize(m00_axi_arsize),
        .m00_axi_arburst(m00_axi_arburst),
        .m00_axi_arlock(m00_axi_arlock),
        .m00_axi_arcache(m00_axi_arcache),
        .m00_axi_arprot(m00_axi_arprot),
        .m00_axi_arqos(m00_axi_arqos),
        .m00_axi_aruser(m00_axi_aruser),
        .m00_axi_arvalid(m00_axi_arvalid),
        .m00_axi_arready(m00_axi_arready),
        .m00_axi_rid(m00_axi_rid),
        .m00_axi_rdata(m00_axi_rdata),
        .m00_axi_rresp(m00_axi_rresp),
        .m00_axi_rlast(m00_axi_rlast),
        .m00_axi_ruser(m00_axi_ruser),
        .m00_axi_rvalid(m00_axi_rvalid),
        .m00_axi_rready(m00_axi_rready),
        //////////////////////////////////////////////////////////////
               .m01_axi_aclk(m01_axi_aclk),
       .m01_axi_aresetn(m01_axi_aresetn),
       .m01_axi_awid(m01_axi_awid),
       .m01_axi_awaddr(m01_axi_awaddr),
        .m01_axi_awlen(m01_axi_awlen),
        .m01_axi_awsize(m01_axi_awsize),
        .m01_axi_awburst(m01_axi_awburst),
        .m01_axi_awlock(m01_axi_awlock),
        .m01_axi_awcache(m01_axi_awcache),
        .m01_axi_awprot(m01_axi_awprot),
        .m01_axi_awqos(m01_axi_awqos),
        .m01_axi_awuser(m01_axi_awuser),
        .m01_axi_awvalid(m01_axi_awvalid),
        .m01_axi_awready(m01_axi_awready),
        .m01_axi_wdata(m01_axi_wdata),
        .m01_axi_wstrb(m01_axi_wstrb),
        .m01_axi_wlast(m01_axi_wlast),
        .m01_axi_wuser(m01_axi_wuser),
        .m01_axi_wvalid(m01_axi_wvalid),
        .m01_axi_wready(m01_axi_wready),
        .m01_axi_bid(m01_axi_bid),
        .m01_axi_bresp(m01_axi_bresp),
        .m01_axi_buser(m01_axi_buser),
        .m01_axi_bvalid(m01_axi_bvalid),
        .m01_axi_bready(m01_axi_bready),
        .m01_axi_arid(m01_axi_arid),
        .m01_axi_araddr(m01_axi_araddr),
        .m01_axi_arlen(m01_axi_arlen),
        .m01_axi_arsize(m01_axi_arsize),
        .m01_axi_arburst(m01_axi_arburst),
        .m01_axi_arlock(m01_axi_arlock),
        .m01_axi_arcache(m01_axi_arcache),
        .m01_axi_arprot(m01_axi_arprot),
        .m01_axi_arqos(m01_axi_arqos),
        .m01_axi_aruser(m01_axi_aruser),
        .m01_axi_arvalid(m01_axi_arvalid),
        .m01_axi_arready(m01_axi_arready),
        .m01_axi_rid(m01_axi_rid),
        .m01_axi_rdata(m01_axi_rdata),
        .m01_axi_rresp(m01_axi_rresp),
        .m01_axi_rlast(m01_axi_rlast),
        .m01_axi_ruser(m01_axi_ruser),
        .m01_axi_rvalid(m01_axi_rvalid),
        .m01_axi_rready(m01_axi_rready),
/////////////////////////////////////////////////////
               .itlb_axi_aclk(itlb_axi_aclk),
       .itlb_axi_aresetn(itlb_axi_aresetn),
       .itlb_axi_awid(itlb_axi_awid),
       .itlb_axi_awaddr(itlb_axi_awaddr),
        .itlb_axi_awlen(itlb_axi_awlen),
        .itlb_axi_awsize(itlb_axi_awsize),
        .itlb_axi_awburst(itlb_axi_awburst),
        .itlb_axi_awlock(itlb_axi_awlock),
        .itlb_axi_awcache(itlb_axi_awcache),
        .itlb_axi_awprot(itlb_axi_awprot),
        .itlb_axi_awqos(itlb_axi_awqos),
        .itlb_axi_awuser(itlb_axi_awuser),
        .itlb_axi_awvalid(itlb_axi_awvalid),
        .itlb_axi_awready(itlb_axi_awready),
        .itlb_axi_wdata(itlb_axi_wdata),
        .itlb_axi_wstrb(itlb_axi_wstrb),
        .itlb_axi_wlast(itlb_axi_wlast),
        .itlb_axi_wuser(itlb_axi_wuser),
        .itlb_axi_wvalid(itlb_axi_wvalid),
        .itlb_axi_wready(itlb_axi_wready),
        .itlb_axi_bid(itlb_axi_bid),
        .itlb_axi_bresp(itlb_axi_bresp),
        .itlb_axi_buser(itlb_axi_buser),
        .itlb_axi_bvalid(itlb_axi_bvalid),
        .itlb_axi_bready(itlb_axi_bready),
        .itlb_axi_arid(itlb_axi_arid),
        .itlb_axi_araddr(itlb_axi_araddr),
        .itlb_axi_arlen(itlb_axi_arlen),
        .itlb_axi_arsize(itlb_axi_arsize),
        .itlb_axi_arburst(itlb_axi_arburst),
        .itlb_axi_arlock(itlb_axi_arlock),
        .itlb_axi_arcache(itlb_axi_arcache),
        .itlb_axi_arprot(itlb_axi_arprot),
        .itlb_axi_arqos(itlb_axi_arqos),
        .itlb_axi_aruser(itlb_axi_aruser),
        .itlb_axi_arvalid(itlb_axi_arvalid),
        .itlb_axi_arready(itlb_axi_arready),
        .itlb_axi_rid(itlb_axi_rid),
        .itlb_axi_rdata(itlb_axi_rdata),
        .itlb_axi_rresp(itlb_axi_rresp),
        .itlb_axi_rlast(itlb_axi_rlast),
        .itlb_axi_ruser(itlb_axi_ruser),
        .itlb_axi_rvalid(itlb_axi_rvalid),
        .itlb_axi_rready(itlb_axi_rready),

//////////////////////////////////////////////////////////////////////
      .peripheral_interface_error    (peripheral_interface_error),   
      .peripheral_interface_txn_done(peripheral_interface_txn_done),
      .peripheral_interface_awaddr  (peripheral_interface_awaddr),  
      .peripheral_interface_awprot  (peripheral_interface_awprot),  
      .peripheral_interface_awvalid(peripheral_interface_awvalid), 
      .peripheral_interface_awready(peripheral_interface_awready), 
      .peripheral_interface_wdata   (peripheral_interface_wdata),   
      .peripheral_interface_wstrb   (peripheral_interface_wstrb),   
      .peripheral_interface_wvalid  (peripheral_interface_wvalid),  
      .peripheral_interface_wready (peripheral_interface_wready),  
      .peripheral_interface_bresp   (peripheral_interface_bresp),   
      .peripheral_interface_bvalid  (peripheral_interface_bvalid),  
      .peripheral_interface_bready  (peripheral_interface_bready),  
      .peripheral_interface_araddr  (peripheral_interface_araddr),  
      .peripheral_interface_arprot  (peripheral_interface_arprot),  
      .peripheral_interface_arvalid (peripheral_interface_arvalid), 
      .peripheral_interface_arready (peripheral_interface_arready), 
      .peripheral_interface_rdata  (peripheral_interface_rdata),   
      .peripheral_interface_rresp   (peripheral_interface_rresp),   
      .peripheral_interface_rvalid  (peripheral_interface_rvalid),  
      .peripheral_interface_rready  (peripheral_interface_rready),
///////////////////////////////////////////////////////////////////////
                .dtlb_axi_aclk(dtlb_axi_aclk),
       .dtlb_axi_aresetn(dtlb_axi_aresetn),
       .dtlb_axi_awid(dtlb_axi_awid),
       .dtlb_axi_awaddr(dtlb_axi_awaddr),
        .dtlb_axi_awlen(dtlb_axi_awlen),
        .dtlb_axi_awsize(dtlb_axi_awsize),
        .dtlb_axi_awburst(dtlb_axi_awburst),
        .dtlb_axi_awlock(dtlb_axi_awlock),
        .dtlb_axi_awcache(dtlb_axi_awcache),
        .dtlb_axi_awprot(dtlb_axi_awprot),
        .dtlb_axi_awqos(dtlb_axi_awqos),
        .dtlb_axi_awuser(dtlb_axi_awuser),
        .dtlb_axi_awvalid(dtlb_axi_awvalid),
        .dtlb_axi_awready(dtlb_axi_awready),
        .dtlb_axi_wdata(dtlb_axi_wdata),
        .dtlb_axi_wstrb(dtlb_axi_wstrb),
        .dtlb_axi_wlast(dtlb_axi_wlast),
        .dtlb_axi_wuser(dtlb_axi_wuser),
        .dtlb_axi_wvalid(dtlb_axi_wvalid),
        .dtlb_axi_wready(dtlb_axi_wready),
        .dtlb_axi_bid(dtlb_axi_bid),
        .dtlb_axi_bresp(dtlb_axi_bresp),
        .dtlb_axi_buser(dtlb_axi_buser),
        .dtlb_axi_bvalid(dtlb_axi_bvalid),
        .dtlb_axi_bready(dtlb_axi_bready),
        .dtlb_axi_arid(dtlb_axi_arid),
        .dtlb_axi_araddr(dtlb_axi_araddr),
        .dtlb_axi_arlen(dtlb_axi_arlen),
        .dtlb_axi_arsize(dtlb_axi_arsize),
        .dtlb_axi_arburst(dtlb_axi_arburst),
        .dtlb_axi_arlock(dtlb_axi_arlock),
        .dtlb_axi_arcache(dtlb_axi_arcache),
        .dtlb_axi_arprot(dtlb_axi_arprot),
        .dtlb_axi_arqos(dtlb_axi_arqos),
        .dtlb_axi_aruser(dtlb_axi_aruser),
        .dtlb_axi_arvalid(dtlb_axi_arvalid),
        .dtlb_axi_arready(dtlb_axi_arready),
        .dtlb_axi_rid(dtlb_axi_rid),
        .dtlb_axi_rdata(dtlb_axi_rdata),
        .dtlb_axi_rresp(dtlb_axi_rresp),
        .dtlb_axi_rlast(dtlb_axi_rlast),
        .dtlb_axi_ruser(dtlb_axi_ruser),
        .dtlb_axi_rvalid(dtlb_axi_rvalid),
        .dtlb_axi_rready(dtlb_axi_rready)

    );
            

    
  
   
   
      
   
    initial begin
        CLK  = 0;
        RSTN = 0;
        #20;
        RSTN = 1;
        

 
    end


    always begin
        #5;
        CLK = !CLK;
    end
    
    myip_v1_0_S00_AXI # ( 
        .C_S_AXI_ID_WIDTH   (C_S00_AXI_ID_WIDTH),
        .C_S_AXI_DATA_WIDTH (C_S00_AXI_DATA_WIDTH),
        .C_S_AXI_ADDR_WIDTH (C_S00_AXI_ADDR_WIDTH),
        .C_S_AXI_AWUSER_WIDTH(C_S00_AXI_AWUSER_WIDTH),
        .C_S_AXI_ARUSER_WIDTH(C_S00_AXI_ARUSER_WIDTH),
        .C_S_AXI_WUSER_WIDTH(C_S00_AXI_WUSER_WIDTH),
        .C_S_AXI_RUSER_WIDTH(C_S00_AXI_RUSER_WIDTH),
        .C_S_AXI_BUSER_WIDTH(C_S00_AXI_BUSER_WIDTH)
    ) myip_v1_0_S00_AXI_inst (
        .S_AXI_ACLK(m00_axi_aclk),
        .S_AXI_ARESETN(m00_axi_aresetn),
        .S_AXI_AWID(m00_axi_awid),
        .S_AXI_AWADDR(m00_axi_awaddr),
        .S_AXI_AWLEN(m00_axi_awlen),
        .S_AXI_AWSIZE(m00_axi_awsize),
        .S_AXI_AWBURST(m00_axi_awburst),
        .S_AXI_AWLOCK(m00_axi_awlock),
        .S_AXI_AWCACHE(m00_axi_awcache),
        .S_AXI_AWPROT(m00_axi_awprot),
        .S_AXI_AWQOS(m00_axi_awqos),
        .S_AXI_AWREGION(m00_axi_awregion),
        .S_AXI_AWUSER(m00_axi_awuser),
        .S_AXI_AWVALID(m00_axi_awvalid),
        .S_AXI_AWREADY(m00_axi_awready),
        .S_AXI_WDATA(m00_axi_wdata),
        .S_AXI_WSTRB(m00_axi_wstrb),
        .S_AXI_WLAST(m00_axi_wlast),
        .S_AXI_WUSER(m00_axi_wuser),
        .S_AXI_WVALID(m00_axi_wvalid),
        .S_AXI_WREADY(m00_axi_wready),
        .S_AXI_BID(m00_axi_bid),
        .S_AXI_BRESP(m00_axi_bresp),
        .S_AXI_BUSER(m00_axi_buser),
        .S_AXI_BVALID(m00_axi_bvalid),
        .S_AXI_BREADY(m00_axi_bready),
        .S_AXI_ARID(m00_axi_arid),
        .S_AXI_ARADDR(m00_axi_araddr),
        .S_AXI_ARLEN(m00_axi_arlen),
        .S_AXI_ARSIZE(m00_axi_arsize),
        .S_AXI_ARBURST(m00_axi_arburst),
        .S_AXI_ARLOCK(m00_axi_arlock),
        .S_AXI_ARCACHE(m00_axi_arcache),
        .S_AXI_ARPROT(m00_axi_arprot),
        .S_AXI_ARQOS(m00_axi_arqos),
        .S_AXI_ARREGION(m00_axi_arregion),
        .S_AXI_ARUSER(m00_axi_aruser),
        .S_AXI_ARVALID(m00_axi_arvalid),
        .S_AXI_ARREADY(m00_axi_arready),
        .S_AXI_RID(m00_axi_rid),
        .S_AXI_RDATA(m00_axi_rdata),
        .S_AXI_RRESP(m00_axi_rresp),
        .S_AXI_RLAST(m00_axi_rlast),
        .S_AXI_RUSER(m00_axi_ruser),
        .S_AXI_RVALID(m00_axi_rvalid),
        .S_AXI_RREADY(m00_axi_rready)
    );
        myip_v1_0_S00_AXI # ( 
        .C_S_AXI_ID_WIDTH   (C_S00_AXI_ID_WIDTH),
        .C_S_AXI_DATA_WIDTH (C_S00_AXI_DATA_WIDTH),
        .C_S_AXI_ADDR_WIDTH (C_S00_AXI_ADDR_WIDTH),
        .C_S_AXI_AWUSER_WIDTH(C_S00_AXI_AWUSER_WIDTH),
        .C_S_AXI_ARUSER_WIDTH(C_S00_AXI_ARUSER_WIDTH),
        .C_S_AXI_WUSER_WIDTH(C_S00_AXI_WUSER_WIDTH),
        .C_S_AXI_RUSER_WIDTH(C_S00_AXI_RUSER_WIDTH),
        .C_S_AXI_BUSER_WIDTH(C_S00_AXI_BUSER_WIDTH)
    ) myip_v1_0_S01_AXI_inst (
        .S_AXI_ACLK(m01_axi_aclk),
        .S_AXI_ARESETN(m01_axi_aresetn),
        .S_AXI_AWID(m01_axi_awid),
        .S_AXI_AWADDR(m01_axi_awaddr),
        .S_AXI_AWLEN(m01_axi_awlen),
        .S_AXI_AWSIZE(m01_axi_awsize),
        .S_AXI_AWBURST(m01_axi_awburst),
        .S_AXI_AWLOCK(m01_axi_awlock),
        .S_AXI_AWCACHE(m01_axi_awcache),
        .S_AXI_AWPROT(m01_axi_awprot),
        .S_AXI_AWQOS(m01_axi_awqos),
        .S_AXI_AWREGION(m01_axi_awregion),
        .S_AXI_AWUSER(m01_axi_awuser),
        .S_AXI_AWVALID(m01_axi_awvalid),
        .S_AXI_AWREADY(m01_axi_awready),
        .S_AXI_WDATA(m01_axi_wdata),
        .S_AXI_WSTRB(m01_axi_wstrb),
        .S_AXI_WLAST(m01_axi_wlast),
        .S_AXI_WUSER(m01_axi_wuser),
        .S_AXI_WVALID(m01_axi_wvalid),
        .S_AXI_WREADY(m01_axi_wready),
        .S_AXI_BID(m01_axi_bid),
        .S_AXI_BRESP(m01_axi_bresp),
        .S_AXI_BUSER(m01_axi_buser),
        .S_AXI_BVALID(m01_axi_bvalid),
        .S_AXI_BREADY(m01_axi_bready),
        .S_AXI_ARID(m01_axi_arid),
        .S_AXI_ARADDR(m01_axi_araddr),
        .S_AXI_ARLEN(m01_axi_arlen),
        .S_AXI_ARSIZE(m01_axi_arsize),
        .S_AXI_ARBURST(m01_axi_arburst),
        .S_AXI_ARLOCK(m01_axi_arlock),
        .S_AXI_ARCACHE(m01_axi_arcache),
        .S_AXI_ARPROT(m01_axi_arprot),
        .S_AXI_ARQOS(m01_axi_arqos),
        .S_AXI_ARREGION(m01_axi_arregion),
        .S_AXI_ARUSER(m01_axi_aruser),
        .S_AXI_ARVALID(m01_axi_arvalid),
        .S_AXI_ARREADY(m01_axi_arready),
        .S_AXI_RID(m01_axi_rid),
        .S_AXI_RDATA(m01_axi_rdata),
        .S_AXI_RRESP(m01_axi_rresp),
        .S_AXI_RLAST(m01_axi_rlast),
        .S_AXI_RUSER(m01_axi_ruser),
        .S_AXI_RVALID(m01_axi_rvalid),
        .S_AXI_RREADY(m01_axi_rready)
    );
    // A
    reg [7:0] byte_ram[0: (1<<24)-1][0:3];

    bit [31:0] word_ram[0: (1<<24)-1];

    always@(negedge RSTN)
    begin
        $readmemh("data_hex.txt",word_ram);
        for (int j=0; j < (1<<24); j=j+1)
        begin
            for (int i=0; i<4; i=i+1)
            begin
                byte_ram[j][i]=  word_ram[j][8*i +: 8];
            end
        end
    end
    
    myip11_v1_0_S00_AXI # ( 
            .C_S_AXI_DATA_WIDTH(C_S00_AXI_DATA_WIDTH),
            .C_S_AXI_ADDR_WIDTH(C_S00_AXI_ADDR_WIDTH)
        ) myip11_v1_0_S00_AXI_inst (
            .S_AXI_ACLK(CLK),
            .S_AXI_ARESETN(RSTN),
            .S_AXI_AWADDR(peripheral_interface_awaddr),
            .S_AXI_AWPROT(peripheral_interface_awprot),
            .S_AXI_AWVALID(peripheral_interface_awvalid),
            .S_AXI_AWREADY(peripheral_interface_awready),
            .S_AXI_WDATA(peripheral_interface_wdata),
            .S_AXI_WSTRB(peripheral_interface_wstrb),
            .S_AXI_WVALID(peripheral_interface_wvalid),
            .S_AXI_WREADY(peripheral_interface_wready),
            .S_AXI_BRESP(peripheral_interface_bresp),
            .S_AXI_BVALID(peripheral_interface_bvalid),
            .S_AXI_BREADY(peripheral_interface_bready),
            .S_AXI_ARADDR(peripheral_interface_araddr),
            .S_AXI_ARPROT(peripheral_interface_arprot),
            .S_AXI_ARVALID(peripheral_interface_arvalid),
            .S_AXI_ARREADY(peripheral_interface_arready),
            .S_AXI_RDATA(peripheral_interface_rdata),
            .S_AXI_RRESP(peripheral_interface_rresp),
            .S_AXI_RVALID(peripheral_interface_rvalid),
            .S_AXI_RREADY(peripheral_interface_rready)
        );
        myip_v1_0_S00_AXI # ( 
        .C_S_AXI_ID_WIDTH   (C_S00_AXI_ID_WIDTH),
        .C_S_AXI_DATA_WIDTH (C_S00_AXI_DATA_WIDTH),
        .C_S_AXI_ADDR_WIDTH (C_S00_AXI_ADDR_WIDTH),
        .C_S_AXI_AWUSER_WIDTH(C_S00_AXI_AWUSER_WIDTH),
        .C_S_AXI_ARUSER_WIDTH(C_S00_AXI_ARUSER_WIDTH),
        .C_S_AXI_WUSER_WIDTH(C_S00_AXI_WUSER_WIDTH),
        .C_S_AXI_RUSER_WIDTH(C_S00_AXI_RUSER_WIDTH),
        .C_S_AXI_BUSER_WIDTH(C_S00_AXI_BUSER_WIDTH)
    ) dtlb_slave_AXI_inst (
        .S_AXI_ACLK(dtlb_axi_aclk),
        .S_AXI_ARESETN(dtlb_axi_aresetn),
        .S_AXI_AWID(dtlb_axi_awid),
        .S_AXI_AWADDR(dtlb_axi_awaddr),
        .S_AXI_AWLEN(dtlb_axi_awlen),
        .S_AXI_AWSIZE(dtlb_axi_awsize),
        .S_AXI_AWBURST(dtlb_axi_awburst),
        .S_AXI_AWLOCK(dtlb_axi_awlock),
        .S_AXI_AWCACHE(dtlb_axi_awcache),
        .S_AXI_AWPROT(dtlb_axi_awprot),
        .S_AXI_AWQOS(dtlb_axi_awqos),
        .S_AXI_AWREGION(dtlb_axi_awregion),
        .S_AXI_AWUSER(dtlb_axi_awuser),
        .S_AXI_AWVALID(dtlb_axi_awvalid),
        .S_AXI_AWREADY(dtlb_axi_awready),
        .S_AXI_WDATA(dtlb_axi_wdata),
        .S_AXI_WSTRB(dtlb_axi_wstrb),
        .S_AXI_WLAST(dtlb_axi_wlast),
        .S_AXI_WUSER(dtlb_axi_wuser),
        .S_AXI_WVALID(dtlb_axi_wvalid),
        .S_AXI_WREADY(dtlb_axi_wready),
        .S_AXI_BID(dtlb_axi_bid),
        .S_AXI_BRESP(dtlb_axi_bresp),
        .S_AXI_BUSER(dtlb_axi_buser),
        .S_AXI_BVALID(dtlb_axi_bvalid),
        .S_AXI_BREADY(dtlb_axi_bready),
        .S_AXI_ARID(dtlb_axi_arid),
        .S_AXI_ARADDR(dtlb_axi_araddr),
        .S_AXI_ARLEN(dtlb_axi_arlen),
        .S_AXI_ARSIZE(dtlb_axi_arsize),
        .S_AXI_ARBURST(dtlb_axi_arburst),
        .S_AXI_ARLOCK(dtlb_axi_arlock),
        .S_AXI_ARCACHE(dtlb_axi_arcache),
        .S_AXI_ARPROT(dtlb_axi_arprot),
        .S_AXI_ARQOS(dtlb_axi_arqos),
        .S_AXI_ARREGION(dtlb_axi_arregion),
        .S_AXI_ARUSER(dtlb_axi_aruser),
        .S_AXI_ARVALID(dtlb_axi_arvalid),
        .S_AXI_ARREADY(dtlb_axi_arready),
        .S_AXI_RID(dtlb_axi_rid),
        .S_AXI_RDATA(dtlb_axi_rdata),
        .S_AXI_RRESP(dtlb_axi_rresp),
        .S_AXI_RLAST(dtlb_axi_rlast),
        .S_AXI_RUSER(dtlb_axi_ruser),
        .S_AXI_RVALID(dtlb_axi_rvalid),
        .S_AXI_RREADY(dtlb_axi_rready)
    );
            myip_v1_0_S00_AXI # ( 
        .C_S_AXI_ID_WIDTH   (C_S00_AXI_ID_WIDTH),
        .C_S_AXI_DATA_WIDTH (C_S00_AXI_DATA_WIDTH),
        .C_S_AXI_ADDR_WIDTH (C_S00_AXI_ADDR_WIDTH),
        .C_S_AXI_AWUSER_WIDTH(C_S00_AXI_AWUSER_WIDTH),
        .C_S_AXI_ARUSER_WIDTH(C_S00_AXI_ARUSER_WIDTH),
        .C_S_AXI_WUSER_WIDTH(C_S00_AXI_WUSER_WIDTH),
        .C_S_AXI_RUSER_WIDTH(C_S00_AXI_RUSER_WIDTH),
        .C_S_AXI_BUSER_WIDTH(C_S00_AXI_BUSER_WIDTH)
    ) itlb_slave_AXI_inst (
        .S_AXI_ACLK(itlb_axi_aclk),
        .S_AXI_ARESETN(itlb_axi_aresetn),
        .S_AXI_AWID(itlb_axi_awid),
        .S_AXI_AWADDR(itlb_axi_awaddr),
        .S_AXI_AWLEN(itlb_axi_awlen),
        .S_AXI_AWSIZE(itlb_axi_awsize),
        .S_AXI_AWBURST(itlb_axi_awburst),
        .S_AXI_AWLOCK(itlb_axi_awlock),
        .S_AXI_AWCACHE(itlb_axi_awcache),
        .S_AXI_AWPROT(itlb_axi_awprot),
        .S_AXI_AWQOS(itlb_axi_awqos),
        .S_AXI_AWREGION(itlb_axi_awregion),
        .S_AXI_AWUSER(itlb_axi_awuser),
        .S_AXI_AWVALID(itlb_axi_awvalid),
        .S_AXI_AWREADY(itlb_axi_awready),
        .S_AXI_WDATA(itlb_axi_wdata),
        .S_AXI_WSTRB(itlb_axi_wstrb),
        .S_AXI_WLAST(itlb_axi_wlast),
        .S_AXI_WUSER(itlb_axi_wuser),
        .S_AXI_WVALID(itlb_axi_wvalid),
        .S_AXI_WREADY(itlb_axi_wready),
        .S_AXI_BID(itlb_axi_bid),
        .S_AXI_BRESP(itlb_axi_bresp),
        .S_AXI_BUSER(itlb_axi_buser),
        .S_AXI_BVALID(itlb_axi_bvalid),
        .S_AXI_BREADY(itlb_axi_bready),
        .S_AXI_ARID(itlb_axi_arid),
        .S_AXI_ARADDR(itlb_axi_araddr),
        .S_AXI_ARLEN(itlb_axi_arlen),
        .S_AXI_ARSIZE(itlb_axi_arsize),
        .S_AXI_ARBURST(itlb_axi_arburst),
        .S_AXI_ARLOCK(itlb_axi_arlock),
        .S_AXI_ARCACHE(itlb_axi_arcache),
        .S_AXI_ARPROT(itlb_axi_arprot),
        .S_AXI_ARQOS(itlb_axi_arqos),
        .S_AXI_ARREGION(itlb_axi_arregion),
        .S_AXI_ARUSER(itlb_axi_aruser),
        .S_AXI_ARVALID(itlb_axi_arvalid),
        .S_AXI_ARREADY(itlb_axi_arready),
        .S_AXI_RID(itlb_axi_rid),
        .S_AXI_RDATA(itlb_axi_rdata),
        .S_AXI_RRESP(itlb_axi_rresp),
        .S_AXI_RLAST(itlb_axi_rlast),
        .S_AXI_RUSER(itlb_axi_ruser),
        .S_AXI_RVALID(itlb_axi_rvalid),
        .S_AXI_RREADY(itlb_axi_rready)
    );
endmodule
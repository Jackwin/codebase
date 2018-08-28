`timescale 1ns/1ps

// A reset bridge to sync the async reset
module sync_reset_bridge (
    input clk,
    input async_rst_in,

    output sync_rst_out,
    output sync_rst_n_out
);
localparam DELAY_CYCLE = 10;
logic rst_meta;
logic [DELAY_CYCLE-1:0] rst_sync;
always_ff @(posedge clk or posedge async_rst_in) begin : proc_sync_reset
    if(async_rst_in) begin
        rst_meta <= 1;
        rst_sync <= {DELAY_CYCLE{1'b1}};
    end else begin
        rst_meta <= 0;
        rst_sync[0] <= rst_meta;
       // rst_sync[1] <= rst_sync[0];
        //rst_sync[2] <= rst_sync[1];
        for (int i = 0; i < (DELAY_CYCLE - 1); i++) begin
            rst_sync[i + 1] <= rst_sync[i];
        end
    end
end

logic rst_sync_buf[2:0];

always_ff @(posedge clk) begin
    rst_sync_buf[2:0] <= {rst_sync_buf[1:0], rst_sync[DELAY_CYCLE-1]};
end

assign sync_rst_out = rst_sync_buf[2];
assign sync_rst_n_out = ~rst_sync_buf[2];

endmodule